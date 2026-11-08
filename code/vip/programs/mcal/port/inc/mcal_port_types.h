#ifndef MCAL_PORT_TYPES_H
#define MCAL_PORT_TYPES_H

/**
 * AUTOSAR Port Driver - general types (SWS_Port, "Type definitions").
 * RP2040: one GPIO bank (GP0..GP29), every pin has its own pad control.
 */

/* Header file inclusions */
#include "Std_Types.h"

/* ---------------------------------- */
/* Identifier types */
typedef uint8 Port_PinType;        /* GPIO number                        */
typedef uint8 Port_PinModeType;    /* one of the PORT_PIN_MODE_* values  */

/* Port_PinDirectionType */
typedef enum
{
    PORT_PIN_IN = 0,
    PORT_PIN_OUT
} Port_PinDirectionType;

/* ---------------------------------- */
/* Pin modes. Each maps to one RP2040 FUNCSEL value inside the driver,
 * so a project never has to deal with raw function numbers. */
#define PORT_PIN_MODE_DIO     ((Port_PinModeType)0u)   /* SIO: software controlled via Dio */
#define PORT_PIN_MODE_ADC     ((Port_PinModeType)1u)   /* analog: GP26..GP29 only          */
#define PORT_PIN_MODE_SPI     ((Port_PinModeType)2u)
#define PORT_PIN_MODE_UART    ((Port_PinModeType)3u)
#define PORT_PIN_MODE_I2C     ((Port_PinModeType)4u)
#define PORT_PIN_MODE_PWM     ((Port_PinModeType)5u)
#define PORT_PIN_MODE_PIO0    ((Port_PinModeType)6u)
#define PORT_PIN_MODE_PIO1    ((Port_PinModeType)7u)
#define PORT_PIN_MODE_CLOCK   ((Port_PinModeType)8u)   /* GPCK clock in / out */
#define PORT_PIN_MODE_USB     ((Port_PinModeType)9u)
#define PORT_PIN_MODE_COUNT   ((Port_PinModeType)10u)

/* ---------------------------------- */
/* Pad configuration (vendor specific part of PortPin) */

/* Internal resistors. BUS_KEEP enables both, which makes the RP2040 pad
 * hold its last driven level. */
typedef enum
{
    PORT_PULL_NONE = 0,
    PORT_PULL_UP,
    PORT_PULL_DOWN,
    PORT_PULL_BUS_KEEP
} Port_PinPullType;

typedef enum
{
    PORT_DRIVE_2MA = 0,
    PORT_DRIVE_4MA,
    PORT_DRIVE_8MA,
    PORT_DRIVE_12MA
} Port_PinDriveType;

/* ---------------------------------- */
/* Post-build configuration */

/* PortPin - one configured pin */
typedef struct
{
    Port_PinType          pin;
    Port_PinModeType      mode;                  /* PortPinInitialMode        */
    Port_PinDirectionType direction;             /* PortPinDirection          */
    boolean               direction_changeable;  /* PortPinDirectionChangeable */
    boolean               mode_changeable;       /* PortPinModeChangeable     */
    uint8                 level;                 /* PortPinLevelValue, STD_HIGH / STD_LOW */
    Port_PinPullType      pull;
    Port_PinDriveType     drive;
    boolean               slew_fast;             /* FALSE = slew rate limited */
    boolean               schmitt;               /* input hysteresis enabled  */
} Port_PinConfigType;

/* Root passed to Port_Init() */
typedef struct
{
    uint8                      pin_count;
    const Port_PinConfigType  *pins;
} Port_ConfigType;

#endif /* MCAL_PORT_TYPES_H */
