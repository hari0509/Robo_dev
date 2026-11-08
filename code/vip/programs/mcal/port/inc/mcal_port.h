#ifndef MCAL_PORT_H
#define MCAL_PORT_H

/**
 * AUTOSAR Port Driver public interface (RP2040 / Pico SDK hardware_gpio).
 *
 * Port owns pin muxing, pads and the initial state of every configured pin.
 * Run-time access to the pin level belongs to Dio.
 *
 * Service IDs and development error codes follow the SWS_Port numbering.
 */

#include "mcal_port_types.h"
#include "mcal_port_cfg.h"

/* ---------------------------------- */
/* Service IDs */
#define PORT_SID_INIT                 (0x00u)
#define PORT_SID_SETPINDIRECTION      (0x01u)
#define PORT_SID_REFRESHPORTDIRECTION (0x02u)
#define PORT_SID_GETVERSIONINFO       (0x03u)
#define PORT_SID_SETPINMODE           (0x04u)

/* ---------------------------------- */
/* Development error codes */
#define PORT_E_NO_ERROR               (0x00u)
#define PORT_E_PARAM_PIN              (0x0Au)
#define PORT_E_DIRECTION_UNCHANGEABLE (0x0Bu)
#define PORT_E_INIT_FAILED            (0x0Cu)
#define PORT_E_PARAM_INVALID_MODE     (0x0Du)
#define PORT_E_MODE_UNCHANGEABLE      (0x0Eu)
#define PORT_E_UNINIT                 (0x0Fu)
#define PORT_E_PARAM_POINTER          (0x10u)
#define PORT_E_ALREADY_INITIALIZED    (0x11u)

/* ---------------------------------- */
/* Initialization */

/* Configures every pin in the table: pad (pull, drive, slew, hysteresis),
 * initial level, direction and mode. Output pins in DIO mode get their level
 * written before the pin starts driving, so they never glitch. Pins that are
 * not in the table are left untouched. Reports PORT_E_INIT_FAILED for an
 * invalid table (nothing is configured then) and PORT_E_ALREADY_INITIALIZED
 * on a second call. Call before any driver that uses the pins. */
void Port_Init(const Port_ConfigType *ConfigPtr);

/* ---------------------------------- */
/* Run-time changes */

/* Only for pins with direction_changeable. The direction is stored for every
 * mode; it is applied to the hardware while the pin is in DIO mode. */
#if (PORT_SET_PIN_DIRECTION_API == STD_ON)
void Port_SetPinDirection(Port_PinType Pin, Port_PinDirectionType Direction);
#endif

/* Re-applies the configured direction of every pin that is NOT
 * direction_changeable (and in DIO mode). Use it to repair after a fault. */
void Port_RefreshPortDirection(void);

/* Only for pins with mode_changeable. Switching to DIO keeps the output
 * latch and applies the stored direction; ADC switches the pad to analog
 * (no pulls, no digital input) and needs GP26..GP29. */
#if (PORT_SET_PIN_MODE_API == STD_ON)
void Port_SetPinMode(Port_PinType Pin, Port_PinModeType Mode);
#endif

#if (PORT_VERSION_INFO_API == STD_ON)
void Port_GetVersionInfo(Std_VersionInfoType *versioninfo);
#endif

#endif /* MCAL_PORT_H */
