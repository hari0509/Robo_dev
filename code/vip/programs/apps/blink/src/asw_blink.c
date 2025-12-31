
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

/* --- MACROS --- */
#define LED_PIN 25

/* --- CONSTANTS --- */
/* None */

/* --- VARIABLES --- */
static bool state = false;

/* --- PRIVATE CODE --- */
/* None */

/* --- PUBLIC CODE --- */
/**
 * asw_blink_init()
 * Initialize GPIO pins for LED control (pins 15).
 * Called once during ECU initialization.
 */
void asw_blink_init(void)
{
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
}

/**
 * asw_blink_mainfunction()
 * Periodic mainfunction (100ms) called by RTE task.
 * Toggles LED on GPIO 15.
 */
void asw_blink_mainfunction(void)
{
    gpio_put(LED_PIN, state);
    state = !state;
}

/* --- REVISION HISTORY --- */
/* v1.0 - Initial commit - Hari */


