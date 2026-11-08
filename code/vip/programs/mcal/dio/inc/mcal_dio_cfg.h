#ifndef MCAL_DIO_CFG_H
#define MCAL_DIO_CFG_H

/**
 * AUTOSAR DIO Driver - configuration.
 *
 * DioGeneral is macros, DioConfig (ports, channels, channel groups) is the
 * pair of const tables defined in mcal_dio_cfg.c. A channel is usable only
 * if its bit is in the valid mask of its port, and the pin itself must have
 * been set up in Port (PORT_PIN_MODE_DIO) - Dio does not touch pin muxing.
 */

/* Header file inclusions */
#include "mcal_dio_types.h"

/* ================================================================ */
/* DioGeneral - pre-compile configuration                           */
/* ================================================================ */

/* Module identification */
#define DIO_VENDOR_ID             (0xFFFFu)  /* Not a registered vendor ID */
#define DIO_MODULE_ID             (120u)
#define DIO_INSTANCE_ID           (0u)
#define DIO_SW_MAJOR_VERSION      (1u)
#define DIO_SW_MINOR_VERSION      (0u)
#define DIO_SW_PATCH_VERSION      (0u)

/* DioDevErrorDetect: parameter checks, reported through DIO_DET_REPORT_ERROR.
 * Wire the macro to Det_ReportError() once a Det module exists. With
 * STD_OFF the checks vanish (hot path), as the SWS intends. */
#define DIO_DEV_ERROR_DETECT      (STD_ON)
#ifndef DIO_DET_REPORT_ERROR
#define DIO_DET_REPORT_ERROR(ApiId, ErrorId) ((void)(ApiId), (void)(ErrorId))
#endif

/* Optional API groups */
#define DIO_VERSION_INFO_API      (STD_ON)  /* Dio_GetVersionInfo */
#define DIO_FLIP_CHANNEL_API      (STD_ON)  /* Dio_FlipChannel    */
#define DIO_MASKED_WRITE_PORT_API (STD_ON)  /* Dio_MaskedWritePort */

/* ================================================================ */
/* Channel arithmetic                                               */
/* ================================================================ */

#define DIO_CHANNEL(port, pin)    ((Dio_ChannelType)(((port) << 5u) | (pin)))
#define DIO_CHANNEL_PORT(ch)      ((Dio_PortType)((ch) >> 5u))
#define DIO_CHANNEL_PIN(ch)       ((uint8)((ch) & 31u))
#define DIO_CHANNEL_BIT(ch)       (((uint32)1u) << DIO_CHANNEL_PIN(ch))

/* ================================================================ */
/* Generated sizes and symbolic names                               */
/* ================================================================ */

#define DIO_MAX_PORT              (1u)   /* RP2040: a single GPIO bank */
#define DIO_MAX_CHANNEL_GROUP     (1u)

/* Ports */
#define DioConf_DioPort_GPIO0     ((Dio_PortType)0u)

/* Channels (pin set-up lives in Port) */
#define DioConf_DioChannel_SPI_CS       DIO_CHANNEL(0u, 5u)   /* SPI0 chip select, active low */
#define DioConf_DioChannel_TestPin      DIO_CHANNEL(0u, 20u)
#define DioConf_DioChannel_LED_Onboard  DIO_CHANNEL(0u, 25u)

/* Channel groups: pointers into the const table, used directly as the
 * ChannelGroupIdPtr argument of the group services */
#define DioConf_DioChannelGroup_TestPin (&Dio_ChannelGroupCfg[0])

extern const Dio_PortConfigType    Dio_PortCfg[DIO_MAX_PORT];
extern const Dio_ChannelGroupType  Dio_ChannelGroupCfg[DIO_MAX_CHANNEL_GROUP];

#endif /* MCAL_DIO_CFG_H */
