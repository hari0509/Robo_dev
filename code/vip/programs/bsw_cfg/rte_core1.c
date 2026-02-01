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
#include "rte_core1.h"

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
 * sys_task_100ms()
 * 100ms periodic task for System NVM component.
 * Calls the sys_nvm_mainfunction which handles NVM operations.
 */
void sys_task_100ms(void *pvParameters)
{
    (void)pvParameters;
    const TickType_t xFrequency = pdMS_TO_TICKS(1000);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for (;;)
    {
        sys_nvm_mainfunction();
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

/* --- REVISION HISTORY --- */
/* v1.0 - Initial commit - Hari */
