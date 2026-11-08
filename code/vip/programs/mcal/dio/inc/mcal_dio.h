#ifndef MCAL_DIO_H
#define MCAL_DIO_H

/**
 * AUTOSAR DIO Driver public interface (RP2040 / Pico SDK hardware_gpio).
 *
 * Every service is reentrant and lock-free: reads are one SIO register read,
 * writes are single atomic SIO operations, so both cores may call any
 * service at any time. A group / port / masked write changes all its bits
 * with ONE store, i.e. simultaneously, as the SWS requires.
 *
 * Writing to a channel that is configured as input has no effect on the pin
 * (SWS_Dio_00028): output-only bits are filtered through the SIO output
 * enable register.
 *
 * Service IDs and development error codes follow the SWS_Dio numbering.
 */

#include "mcal_dio_types.h"
#include "mcal_dio_cfg.h"

/* ---------------------------------- */
/* Service IDs */
#define DIO_SID_READCHANNEL           (0x00u)
#define DIO_SID_WRITECHANNEL          (0x01u)
#define DIO_SID_READPORT              (0x02u)
#define DIO_SID_WRITEPORT             (0x03u)
#define DIO_SID_READCHANNELGROUP      (0x04u)
#define DIO_SID_WRITECHANNELGROUP     (0x05u)
#define DIO_SID_GETVERSIONINFO        (0x12u)
#define DIO_SID_FLIPCHANNEL           (0x11u)
#define DIO_SID_MASKEDWRITEPORT       (0x13u)

/* ---------------------------------- */
/* Development error codes */
#define DIO_E_NO_ERROR                (0x00u)
#define DIO_E_PARAM_INVALID_CHANNEL_ID (0x0Au)
#define DIO_E_PARAM_INVALID_PORT_ID   (0x14u)
#define DIO_E_PARAM_INVALID_GROUP     (0x1Fu)
#define DIO_E_PARAM_POINTER           (0x20u)

/* ---------------------------------- */
/* Channel services */

/* Level of the pad (input and output channels alike) */
Dio_LevelType Dio_ReadChannel(Dio_ChannelType ChannelId);

/* No effect when the channel is an input */
void Dio_WriteChannel(Dio_ChannelType ChannelId, Dio_LevelType Level);

/* Inverts an output channel and returns the new level. For an input channel
 * nothing is written and the current pad level is returned. */
#if (DIO_FLIP_CHANNEL_API == STD_ON)
Dio_LevelType Dio_FlipChannel(Dio_ChannelType ChannelId);
#endif

/* ---------------------------------- */
/* Port services */

/* Levels of all configured channels of the port, others read as 0 */
Dio_PortLevelType Dio_ReadPort(Dio_PortType PortId);

/* Writes all configured output channels, inputs are ignored */
void Dio_WritePort(Dio_PortType PortId, Dio_PortLevelType Level);

/* Writes only the bits set in Mask (and configured as output) */
#if (DIO_MASKED_WRITE_PORT_API == STD_ON)
void Dio_MaskedWritePort(Dio_PortType PortId, Dio_PortLevelType Level, Dio_PortLevelType Mask);
#endif

/* ---------------------------------- */
/* Channel group services. The result / argument is right-aligned: bit 0 is
 * the lowest pin of the group, whatever its position in the port. */
Dio_PortLevelType Dio_ReadChannelGroup(const Dio_ChannelGroupType *ChannelGroupIdPtr);
void Dio_WriteChannelGroup(const Dio_ChannelGroupType *ChannelGroupIdPtr, Dio_PortLevelType Level);

/* ---------------------------------- */
#if (DIO_VERSION_INFO_API == STD_ON)
void Dio_GetVersionInfo(Std_VersionInfoType *VersionInfo);
#endif

#endif /* MCAL_DIO_H */
