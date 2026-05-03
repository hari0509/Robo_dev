
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
#include "hardware/gpio.h"
#include "vip_linker_usage_helper.h"

/* --- MACROS --- */
#define LED_PIN 25

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
 * Initialize GPIO pin for LED control (on-board LED, GPIO 25).
 * Called once during ECU initialization.
 */
void asw_blink_init(void)
{
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    g_reset_count += 1u;
}

/**
 * asw_blink_mainfunction()
 * Periodic mainfunction (100ms) called by RTE task.
 * Toggles LED on GPIO 15.
 */
void asw_blink_mainfunction(void)
{
    gpio_put(LED_PIN, basw_led_state);
    basw_led_state = !basw_led_state;
}

/* --- REVISION HISTORY --- */
/* v1.0 - Initial commit - Hari */


