/**
 * ============================================================================
 * FILE: rte.c
 * ============================================================================
 * DESCRIPTION:
 *   Runtime Environment (RTE). Implements periodic task scheduling and
 *   defines task entry points that call component-specific mainfunctions.
 *
 * AUTHOR: Hari
 * ============================================================================
 */

/* --- INCLUDES --- */
#include "rte.h"
#include "rte_core0.h"

/* --- MACROS --- */
/* None */

/* --- CONSTANTS --- */
/* None */

/* --- VARIABLES --- */
/* None */

/* --- PRIVATE CODE --- */
/* None */

/* --- PUBLIC CODE --- */
/**
 * asw_task_100ms()
 * 100ms periodic task for Application Software (ASW) blink component.
 * Calls the ASW mainfunction which toggles LED on GPIO 25.
 */
void asw_task_100ms(void *pvParameters)
{
    (void)pvParameters;
    const TickType_t xFrequency = pdMS_TO_TICKS(1000);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for (;;)
    {
        asw_blink_mainfunction();
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

/**
 * cdd_task_100ms()
 * 100ms periodic task for CDD servo component.
 * Calls the CDD mainfunction which toggles LED on GPIO 15.
 */
void cdd_task_100ms(void *pvParameters)
{
    (void)pvParameters;
    const TickType_t xFrequency = pdMS_TO_TICKS(1000);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for (;;)
    {
        cdd_servo_mainfunction();
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

/* --- REVISION HISTORY --- */
/* v1.0 - Initial commit - Hari */
