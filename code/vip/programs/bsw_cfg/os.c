/**
 * ============================================================================
 * FILE: os.c
 * ============================================================================
 * DESCRIPTION:
 *   Operating System abstraction layer. Manages FreeRTOS task creation
 *   and initialization via a static task table.
 *
 * AUTHOR: Hari
 * ============================================================================
 */

/* --- INCLUDES --- */
#include "os.h"
#include "rte.h"
#include <FreeRTOS.h>
#include <task.h>

/* --- MACROS --- */
/* None */

/* --- CONSTANTS --- */
/* None */

/* --- VARIABLES --- */
/**
 * Static task table used to create periodic threads. The table size is
 * defined by TASK_TABLE_SIZE in os.h. Each entry specifies a task function,
 * name, stack depth, parameters, and priority.
 */
const OS_TaskEntry task_table[TASK_TABLE_SIZE] = {
    { asw_task_100ms, "ASW_Task", 256, NULL, 2 },
    { cdd_task_100ms, "CDD_Task", 256, NULL, 2 },
};

/* --- PRIVATE CODE --- */
/* None */

/* --- PUBLIC CODE --- */
/**
 * OS_Init()
 * Create tasks from the static task table during system startup.
 */
void OS_Init(void)
{
    /* Create tasks from the static task table */
    for (int i = 0; i < TASK_TABLE_SIZE; ++i)
    {
        const OS_TaskEntry *e = &task_table[i];
        xTaskCreate(e->taskFn, e->name, e->stackDepth, e->params, e->priority, NULL);
    }
}

/**
 * OS_CreateTask()
 * Wrapper function for xTaskCreate, providing a consistent OS abstraction.
 */
BaseType_t OS_CreateTask(TaskFunction_t pxTaskCode, const char * const pcName, const uint16_t usStackDepth, void *pvParameters, UBaseType_t uxPriority, TaskHandle_t *pxCreatedTask)
{
    return xTaskCreate(pxTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask);
}

/* --- REVISION HISTORY --- */
/* v1.0 - Initial commit - Hari */
