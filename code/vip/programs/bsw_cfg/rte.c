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
#include "os.h"
#include <FreeRTOS.h>
#include <task.h>

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
    for (;;)
    {
        asw_blink_mainfunction();
        vTaskDelay(pdMS_TO_TICKS(100));
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
    for (;;)
    {
        cdd_servo_mainfunction();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/**
 * RTE_Init()
 * RTE-specific initialization. Currently a placeholder for future
 * component-level configuration. Task creation is performed by OS_Init
 * using the task_table.
 */
void RTE_Init(void)
{
    /* RTE-specific initialisation, component-level inits could go
     * here. Task creation is performed by OS_Init using the
     * `task_table` so RTE_Init can be empty or used for future work.
     */
}

/* --- REVISION HISTORY --- */
/* v1.0 - Initial commit - Hari */
