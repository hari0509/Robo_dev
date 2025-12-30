
/**
 * ============================================================================
 * FILE: asw_blink.c
 * ============================================================================
 * DESCRIPTION:
 *   Application Software (ASW) blink component. Manages GPIO-based LED
 *   blinking for system visibility and debug purposes.
 *
 * AUTHOR: Hari
 * ============================================================================
 */

/* --- INCLUDES --- */
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <FreeRTOS.h>
#include <task.h>

/* --- MACROS --- */
#define LED_PIN 25

/* --- CONSTANTS --- */
/* None */

/* --- VARIABLES --- */
/* None */

/* --- PRIVATE CODE --- */
/* None */

/* --- PUBLIC CODE --- */
/**
 * asw_blink_init()
 * Initialize GPIO pins for LED control (pins 25, 15, 16).
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
 * Toggles LED on GPIO 25.
 */
void asw_blink_mainfunction(void)
{
    static bool state = false;
    gpio_put(LED_PIN, state);
    state = !state;
}

/* --- REVISION HISTORY --- */
/* v1.0 - Initial commit - Hari */


