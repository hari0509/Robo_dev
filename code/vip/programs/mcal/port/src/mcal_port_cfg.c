#include "mcal_port_cfg.h"

/* Pad defaults match the RP2040 reset state (4 mA, slew limited, hysteresis
 * on) so a pin only changes electrically where this table says so. */

static const Port_PinConfigType Port_PinCfg[] =
{
    /* SPI0 SCK (GP2): peripheral drives it */
    {
        .pin                  = PortConf_PortPin_SPI0_SCK,
        .mode                 = PORT_PIN_MODE_SPI,
        .direction            = PORT_PIN_OUT,
        .direction_changeable = FALSE,
        .mode_changeable      = FALSE,
        .level                = STD_LOW,
        .pull                 = PORT_PULL_NONE,
        .drive                = PORT_DRIVE_4MA,
        .slew_fast            = FALSE,
        .schmitt              = TRUE
    },
    /* SPI0 TX / MOSI (GP3) */
    {
        .pin                  = PortConf_PortPin_SPI0_TX,
        .mode                 = PORT_PIN_MODE_SPI,
        .direction            = PORT_PIN_OUT,
        .direction_changeable = FALSE,
        .mode_changeable      = FALSE,
        .level                = STD_LOW,
        .pull                 = PORT_PULL_NONE,
        .drive                = PORT_DRIVE_4MA,
        .slew_fast            = FALSE,
        .schmitt              = TRUE
    },
    /* SPI0 RX / MISO (GP4): pull-down kept from the reset state so the line
     * does not float while no device answers */
    {
        .pin                  = PortConf_PortPin_SPI0_RX,
        .mode                 = PORT_PIN_MODE_SPI,
        .direction            = PORT_PIN_IN,
        .direction_changeable = FALSE,
        .mode_changeable      = FALSE,
        .level                = STD_LOW,
        .pull                 = PORT_PULL_DOWN,
        .drive                = PORT_DRIVE_4MA,
        .slew_fast            = FALSE,
        .schmitt              = TRUE
    },
    /* SPI0 chip select (GP5): software GPIO, active low, idle HIGH from the
     * first cycle on. Driven through Dio by the Spi driver. */
    {
        .pin                  = PortConf_PortPin_SPI0_CS,
        .mode                 = PORT_PIN_MODE_DIO,
        .direction            = PORT_PIN_OUT,
        .direction_changeable = FALSE,
        .mode_changeable      = FALSE,
        .level                = STD_HIGH,
        .pull                 = PORT_PULL_NONE,
        .drive                = PORT_DRIVE_4MA,
        .slew_fast            = FALSE,
        .schmitt              = TRUE
    },
    /* Self-test pin (GP20) */
    {
        .pin                  = PortConf_PortPin_TestPin,
        .mode                 = PORT_PIN_MODE_DIO,
        .direction            = PORT_PIN_OUT,
        .direction_changeable = FALSE,
        .mode_changeable      = FALSE,
        .level                = STD_LOW,
        .pull                 = PORT_PULL_NONE,
        .drive                = PORT_DRIVE_4MA,
        .slew_fast            = FALSE,
        .schmitt              = TRUE
    },
    /* On-board LED (GP25) */
    {
        .pin                  = PortConf_PortPin_LED_Onboard,
        .mode                 = PORT_PIN_MODE_DIO,
        .direction            = PORT_PIN_OUT,
        .direction_changeable = FALSE,
        .mode_changeable      = FALSE,
        .level                = STD_LOW,
        .pull                 = PORT_PULL_NONE,
        .drive                = PORT_DRIVE_4MA,
        .slew_fast            = FALSE,
        .schmitt              = TRUE
    }
};

const Port_ConfigType Port_Config =
{
    .pin_count = (uint8)(sizeof(Port_PinCfg) / sizeof(Port_PinCfg[0])),
    .pins      = Port_PinCfg
};
