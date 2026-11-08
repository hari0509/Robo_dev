/**
 * AUTOSAR Port Driver for RP2040, built on the Pico SDK hardware_gpio.
 *
 * Every pin of the configuration table is brought into a defined state in
 * Port_Init(). For a DIO output the initial level is written to the SIO
 * output latch and the direction is set BEFORE the function select hands the
 * pad to SIO, so the pin goes straight to its configured level. (gpio_init()
 * would drive the pin low first, which is not acceptable for a chip select.)
 *
 * Concurrency: all pad and SIO writes used here are single-register atomic
 * operations (set/clear/xor aliases), so calls for different pins are safe
 * from both cores. Two cores changing the SAME pin at the same time is a
 * last-writer-wins race, as in any AUTOSAR driver; Port_Init() must complete
 * before the scheduler starts.
 *
 * Range and state checks stay active with PORT_DEV_ERROR_DETECT == STD_OFF
 * because they guard the driver's own tables; only the Det reporting goes.
 */

#include "mcal_port.h"
#include "hardware/gpio.h"

/* ---------------------------------- */
/* Private types / constants */
#define PORT_NOT_CONFIGURED   (0xFFu)

typedef struct
{
    uint8                 cfg_index;   /* index into the config table       */
    Port_PinDirectionType direction;   /* current direction (changeable)    */
    Port_PinModeType      mode;        /* current mode (changeable)         */
} Port_PinRtType;

/* PORT_PIN_MODE_* -> RP2040 FUNCSEL. ADC uses NULL plus an analog pad. */
static const gpio_function_t Port_FuncSel[PORT_PIN_MODE_COUNT] =
{
    GPIO_FUNC_SIO,    /* PORT_PIN_MODE_DIO   */
    GPIO_FUNC_NULL,   /* PORT_PIN_MODE_ADC   */
    GPIO_FUNC_SPI,    /* PORT_PIN_MODE_SPI   */
    GPIO_FUNC_UART,   /* PORT_PIN_MODE_UART  */
    GPIO_FUNC_I2C,    /* PORT_PIN_MODE_I2C   */
    GPIO_FUNC_PWM,    /* PORT_PIN_MODE_PWM   */
    GPIO_FUNC_PIO0,   /* PORT_PIN_MODE_PIO0  */
    GPIO_FUNC_PIO1,   /* PORT_PIN_MODE_PIO1  */
    GPIO_FUNC_GPCK,   /* PORT_PIN_MODE_CLOCK */
    GPIO_FUNC_USB     /* PORT_PIN_MODE_USB   */
};

static const enum gpio_drive_strength Port_DriveSel[4] =
{
    GPIO_DRIVE_STRENGTH_2MA,
    GPIO_DRIVE_STRENGTH_4MA,
    GPIO_DRIVE_STRENGTH_8MA,
    GPIO_DRIVE_STRENGTH_12MA
};

/* ---------------------------------- */
/* Global variables */
static const Port_ConfigType *Port_CfgPtr = NULL;
static volatile boolean       Port_Initialized = FALSE;
static Port_PinRtType         Port_PinRt[PORT_NUM_GPIO];

/* ---------------------------------- */
/* Development error handling */
#if (PORT_DEV_ERROR_DETECT == STD_ON)
#define PORT_REPORT(ApiId, ErrorId)  PORT_DET_REPORT_ERROR((ApiId), (ErrorId))
#else
#define PORT_REPORT(ApiId, ErrorId)  ((void)0)
#endif

/* UNINIT / PARAM_PIN (not in the table). Only the run-time pin services use it. */
#if ((PORT_SET_PIN_DIRECTION_API == STD_ON) || (PORT_SET_PIN_MODE_API == STD_ON))
static uint8 Port_CheckPin(Port_PinType pin)
{
    uint8 err = PORT_E_NO_ERROR;

    if (Port_Initialized == FALSE)
    {
        err = PORT_E_UNINIT;
    }
    else if ((pin >= PORT_NUM_GPIO) || (Port_PinRt[pin].cfg_index == PORT_NOT_CONFIGURED))
    {
        err = PORT_E_PARAM_PIN;
    }
    else
    {
        /* valid */
    }

    return err;
}
#endif /* run-time pin services */

static boolean Port_ModeValid(Port_PinType pin, Port_PinModeType mode)
{
    boolean ok = (mode < PORT_PIN_MODE_COUNT) ? TRUE : FALSE;

    if ((ok == TRUE) && (mode == PORT_PIN_MODE_ADC) && (pin < PORT_ADC_FIRST_PIN))
    {
        ok = FALSE;
    }
    return ok;
}

/* A table that would index outside the driver's arrays or contradict itself
 * is refused as a whole before any hardware is touched. */
static boolean Port_ValidateConfig(const Port_ConfigType *cfg)
{
    boolean ok = ((cfg != NULL) && (cfg->pins != NULL) &&
                  (cfg->pin_count <= PORT_NUM_GPIO)) ? TRUE : FALSE;
    uint32  seen = 0u;
    uint32  i;

    for (i = 0u; (ok == TRUE) && (i < cfg->pin_count); i++)
    {
        const Port_PinConfigType *p = &cfg->pins[i];

        if ((p->pin >= PORT_NUM_GPIO) ||
            ((seen & ((uint32)1u << p->pin)) != 0u) ||
            (Port_ModeValid(p->pin, p->mode) == FALSE) ||
            (p->direction > PORT_PIN_OUT) ||
            (p->pull > PORT_PULL_BUS_KEEP) ||
            (p->drive > PORT_DRIVE_12MA) ||
            (p->level > STD_HIGH))
        {
            ok = FALSE;
        }
        else
        {
            seen |= ((uint32)1u << p->pin);
        }
    }
    return ok;
}

/* ---------------------------------- */
/* Hardware access helpers */

static void Port_ApplyPad(const Port_PinConfigType *p)
{
    const uint gpio = (uint)p->pin;
    const bool up   = (p->pull == PORT_PULL_UP)   || (p->pull == PORT_PULL_BUS_KEEP);
    const bool down = (p->pull == PORT_PULL_DOWN) || (p->pull == PORT_PULL_BUS_KEEP);

    gpio_set_pulls(gpio, up, down);
    gpio_set_drive_strength(gpio, Port_DriveSel[p->drive]);
    gpio_set_slew_rate(gpio, (p->slew_fast != FALSE) ? GPIO_SLEW_RATE_FAST : GPIO_SLEW_RATE_SLOW);
    gpio_set_input_hysteresis_enabled(gpio, (p->schmitt != FALSE));
}

/* Put the pin into `mode`. apply_level is TRUE only from Port_Init(): a later
 * switch to DIO keeps whatever the output latch holds. */
static void Port_Activate(const Port_PinConfigType *p,
                          Port_PinModeType mode,
                          Port_PinDirectionType dir,
                          boolean apply_level)
{
    const uint gpio = (uint)p->pin;

    if (mode == PORT_PIN_MODE_ADC)
    {
        gpio_set_function(gpio, GPIO_FUNC_NULL);
        gpio_disable_pulls(gpio);
        gpio_set_input_enabled(gpio, false);
    }
    else
    {
        Port_ApplyPad(p);

        if (mode == PORT_PIN_MODE_DIO)
        {
            if ((apply_level != FALSE) && (dir == PORT_PIN_OUT))
            {
                gpio_put(gpio, (p->level != STD_LOW));
            }
            gpio_set_dir(gpio, (dir == PORT_PIN_OUT));
        }

        /* Last step: the pad now follows the level / peripheral set above */
        gpio_set_function(gpio, Port_FuncSel[mode]);
    }
}

/* ---------------------------------- */
/* Initialization */

void Port_Init(const Port_ConfigType *ConfigPtr)
{
    uint8  err = PORT_E_NO_ERROR;
    uint32 i;

    if (Port_Initialized != FALSE)
    {
        err = PORT_E_ALREADY_INITIALIZED;
    }
    else if (Port_ValidateConfig(ConfigPtr) == FALSE)
    {
        err = PORT_E_INIT_FAILED;
    }
    else
    {
        /* valid */
    }

    if (err != PORT_E_NO_ERROR)
    {
        PORT_REPORT(PORT_SID_INIT, err);
    }
    else
    {
        Port_CfgPtr = ConfigPtr;

        for (i = 0u; i < PORT_NUM_GPIO; i++)
        {
            Port_PinRt[i].cfg_index = PORT_NOT_CONFIGURED;
            Port_PinRt[i].direction = PORT_PIN_IN;
            Port_PinRt[i].mode      = PORT_PIN_MODE_DIO;
        }

        for (i = 0u; i < ConfigPtr->pin_count; i++)
        {
            const Port_PinConfigType *p = &ConfigPtr->pins[i];

            Port_PinRt[p->pin].cfg_index = (uint8)i;
            Port_PinRt[p->pin].direction = p->direction;
            Port_PinRt[p->pin].mode      = p->mode;

            Port_Activate(p, p->mode, p->direction, TRUE);
        }

        Port_Initialized = TRUE;
    }
}

/* ---------------------------------- */
/* Run-time changes */
#if (PORT_SET_PIN_DIRECTION_API == STD_ON)

void Port_SetPinDirection(Port_PinType Pin, Port_PinDirectionType Direction)
{
    uint8 err = Port_CheckPin(Pin);

    if ((err == PORT_E_NO_ERROR) &&
        (Port_CfgPtr->pins[Port_PinRt[Pin].cfg_index].direction_changeable == FALSE))
    {
        err = PORT_E_DIRECTION_UNCHANGEABLE;
    }

    if (err != PORT_E_NO_ERROR)
    {
        PORT_REPORT(PORT_SID_SETPINDIRECTION, err);
    }
    else
    {
        Port_PinRt[Pin].direction = Direction;

        /* The SIO output enable only matters while the pad follows SIO */
        if (Port_PinRt[Pin].mode == PORT_PIN_MODE_DIO)
        {
            gpio_set_dir((uint)Pin, (Direction == PORT_PIN_OUT));
        }
    }
}

#endif /* PORT_SET_PIN_DIRECTION_API */

void Port_RefreshPortDirection(void)
{
    uint32 i;

    if (Port_Initialized == FALSE)
    {
        PORT_REPORT(PORT_SID_REFRESHPORTDIRECTION, PORT_E_UNINIT);
    }
    else
    {
        for (i = 0u; i < Port_CfgPtr->pin_count; i++)
        {
            const Port_PinConfigType *p = &Port_CfgPtr->pins[i];

            if ((p->direction_changeable == FALSE) && (Port_PinRt[p->pin].mode == PORT_PIN_MODE_DIO))
            {
                Port_PinRt[p->pin].direction = p->direction;
                gpio_set_dir((uint)p->pin, (p->direction == PORT_PIN_OUT));
            }
        }
    }
}

#if (PORT_SET_PIN_MODE_API == STD_ON)

void Port_SetPinMode(Port_PinType Pin, Port_PinModeType Mode)
{
    uint8 err = Port_CheckPin(Pin);
    const Port_PinConfigType *p = NULL;

    if (err == PORT_E_NO_ERROR)
    {
        p = &Port_CfgPtr->pins[Port_PinRt[Pin].cfg_index];

        if (p->mode_changeable == FALSE)
        {
            err = PORT_E_MODE_UNCHANGEABLE;
        }
        else if (Port_ModeValid(Pin, Mode) == FALSE)
        {
            err = PORT_E_PARAM_INVALID_MODE;
        }
        else
        {
            /* valid */
        }
    }

    if (err != PORT_E_NO_ERROR)
    {
        PORT_REPORT(PORT_SID_SETPINMODE, err);
    }
    else
    {
        Port_PinRt[Pin].mode = Mode;
        Port_Activate(p, Mode, Port_PinRt[Pin].direction, FALSE);
    }
}

#endif /* PORT_SET_PIN_MODE_API */

#if (PORT_VERSION_INFO_API == STD_ON)

void Port_GetVersionInfo(Std_VersionInfoType *versioninfo)
{
    if (versioninfo == NULL)
    {
        PORT_REPORT(PORT_SID_GETVERSIONINFO, PORT_E_PARAM_POINTER);
    }
    else
    {
        versioninfo->vendorID         = PORT_VENDOR_ID;
        versioninfo->moduleID         = PORT_MODULE_ID;
        versioninfo->sw_major_version = PORT_SW_MAJOR_VERSION;
        versioninfo->sw_minor_version = PORT_SW_MINOR_VERSION;
        versioninfo->sw_patch_version = PORT_SW_PATCH_VERSION;
    }
}

#endif /* PORT_VERSION_INFO_API */
