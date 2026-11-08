#ifndef MCAL_DIO_TYPES_H
#define MCAL_DIO_TYPES_H

/**
 * AUTOSAR DIO Driver - general types (SWS_Dio, "Type definitions").
 *
 * RP2040 has one GPIO bank of 30 pins, so there is a single DIO port
 * (32 bit wide, GP0 = bit 0). A channel ID is (port << 5) | bit, which for
 * port 0 is simply the GPIO number.
 */

/* Header file inclusions */
#include "Std_Types.h"

typedef uint8  Dio_ChannelType;      /* one pin                            */
typedef uint8  Dio_PortType;         /* group of up to 32 pins             */
typedef uint8  Dio_LevelType;        /* STD_HIGH / STD_LOW                 */
typedef uint32 Dio_PortLevelType;    /* all pin levels of a port, bit n = pin n */

/* Dio_ChannelGroupType - adjacent or scattered pins of one port that are
 * read and written together in a single operation */
typedef struct
{
    Dio_PortLevelType mask;     /* bits of the port that belong to the group */
    uint8             offset;   /* position of the lowest group bit          */
    Dio_PortType      port;
} Dio_ChannelGroupType;

/* DioPort - which channels of the port are configured (usable) */
typedef struct
{
    Dio_PortLevelType valid_mask;
} Dio_PortConfigType;

#endif /* MCAL_DIO_TYPES_H */
