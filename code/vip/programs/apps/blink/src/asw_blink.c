
/**
 * ============================================================================
 * FILE: asw_blink.c
 * ============================================================================
 * DESCRIPTION:
 *   Application Software (ASW) blink component. 
 *
 * AUTHOR: Hari
 * ============================================================================
 */

/* --- INCLUDES --- */
#include "pico/stdlib.h"
#include "mcal_dio.h"
#include "vip_linker_usage_helper.h"

/* --- MACROS --- */
/* On-board LED (GP25), set up as a DIO output by Port_Init() */
#define LED_CHANNEL DioConf_DioChannel_LED_Onboard

/* --- CONSTANTS --- */
/* None */

/* --- VARIABLES --- */
static bool basw_led_state = false;
GENESIS_NOINIT uint32_t g_reset_count;

/* --- PRIVATE CODE --- */
/* None */

/* --- PUBLIC CODE --- */
/**
 * asw_blink_init()
 * The LED pin (on-board LED, GPIO 25) is configured by Port_Init().
 * Called once during ECU initialization.
 */
void asw_blink_init(void)
{
    g_reset_count += 1u;
}

/**
 * asw_blink_mainfunction()
 * Periodic mainfunction (100ms) called by RTE task.
 * Toggles LED on GPIO 15.
 */
void asw_blink_mainfunction(void)
{
    Dio_WriteChannel(LED_CHANNEL, basw_led_state ? (Dio_LevelType)STD_HIGH : (Dio_LevelType)STD_LOW);
    basw_led_state = !basw_led_state;
}

/* --- REVISION HISTORY --- */
/* v1.0 - Initial commit - Hari */


