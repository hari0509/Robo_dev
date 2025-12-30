/**
 * ============================================================================
 * FILE: cdd_servo.c
 * ============================================================================
 * DESCRIPTION:
 *   Calibration Data and Device (CDD) servo component. Manages servo
 *   control and GPIO-based debug LED toggling for system visibility.
 *
 * AUTHOR: Hari
 * ============================================================================
 */

/* --- INCLUDES --- */
#include "pico/stdlib.h"
#include "hardware/gpio.h"

/* --- MACROS --- */
#define CDD_DEBUG_LED_PIN 15

/* --- CONSTANTS --- */
/* None */

/* --- VARIABLES --- */
/* None */

/* --- PRIVATE CODE --- */
/* None */

/* --- PUBLIC CODE --- */
/**
 * cdd_servo_init()
 * Initialize GPIO pins for servo debug/control.
 * Sets up GPIO 15 as output for visibility.
 */
void cdd_servo_init(void)
{
    gpio_init(CDD_DEBUG_LED_PIN);
    gpio_set_dir(CDD_DEBUG_LED_PIN, GPIO_OUT);
}

/**
 * cdd_servo_mainfunction()
 * Periodic mainfunction (100ms) called by RTE task.
 * Toggles debug LED on GPIO 15.
 */
void cdd_servo_mainfunction(void)
{
    static bool state = false;
    gpio_put(CDD_DEBUG_LED_PIN, state);
    state = !state;
}

/* --- REVISION HISTORY --- */
/* v1.0 - Initial commit - Hari */
