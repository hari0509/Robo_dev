#ifndef MCAL_PORT_CFG_H
#define MCAL_PORT_CFG_H

/**
 * AUTOSAR Port Driver - configuration.
 *
 * Pre-compile parameters (PortGeneral) are macros, the pin table
 * (PortConfigSet) is defined in mcal_port_cfg.c.
 */

/* Header file inclusions */
#include "mcal_port_types.h"

/* ================================================================ */
/* PortGeneral - pre-compile configuration                          */
/* ================================================================ */

/* Module identification */
#define PORT_VENDOR_ID            (0xFFFFu)  /* Not a registered vendor ID */
#define PORT_MODULE_ID            (124u)
#define PORT_INSTANCE_ID          (0u)
#define PORT_SW_MAJOR_VERSION     (1u)
#define PORT_SW_MINOR_VERSION     (0u)
#define PORT_SW_PATCH_VERSION     (0u)

/* PortDevErrorDetect: reported through PORT_DET_REPORT_ERROR. Wire the macro
 * to Det_ReportError() once a Det module exists in the project. The range and
 * state checks themselves stay active when this is STD_OFF (they protect the
 * driver's own tables); only the reporting is removed. */
#define PORT_DEV_ERROR_DETECT     (STD_ON)
#ifndef PORT_DET_REPORT_ERROR
#define PORT_DET_REPORT_ERROR(ApiId, ErrorId) ((void)(ApiId), (void)(ErrorId))
#endif

/* Optional API groups */
#define PORT_SET_PIN_DIRECTION_API (STD_ON)  /* Port_SetPinDirection */
#define PORT_SET_PIN_MODE_API      (STD_ON)  /* Port_SetPinMode      */
#define PORT_VERSION_INFO_API      (STD_ON)  /* Port_GetVersionInfo  */

/* ================================================================ */
/* Hardware constants (RP2040)                                      */
/* ================================================================ */

#define PORT_NUM_GPIO             (30u)   /* GP0..GP29                 */
#define PORT_ADC_FIRST_PIN        (26u)   /* GP26..GP29 have an ADC input */

/* ================================================================ */
/* Symbolic names                                                   */
/* ================================================================ */

#define PortConf_PortPin_SPI0_SCK   ((Port_PinType)2u)
#define PortConf_PortPin_SPI0_TX    ((Port_PinType)3u)   /* MOSI */
#define PortConf_PortPin_SPI0_RX    ((Port_PinType)4u)   /* MISO */
#define PortConf_PortPin_SPI0_CS    ((Port_PinType)5u)   /* software CS, Dio channel */
#define PortConf_PortPin_TestPin    ((Port_PinType)20u)
#define PortConf_PortPin_LED_Onboard ((Port_PinType)25u)

extern const Port_ConfigType Port_Config;

#endif /* MCAL_PORT_CFG_H */
