#ifndef BSW_OS_H
#define BSW_OS_H

/**
 * ============================================================================
 * FILE: os.h
 * ============================================================================
 * DESCRIPTION:
 *   Operating System abstraction layer header. Defines OS task management
 *   structures and initialization interface.
 *
 * AUTHOR: Hari
 * ============================================================================
 */

/* --- INCLUDES --- */
#include <FreeRTOS.h>
#include <task.h>

/* --- MACROS --- */
/* Size of the static task table defined in os.c */
#define TASK_TABLE_SIZE 2

/* --- CONSTANTS --- */
/* None */

/* --- DATA TYPES --- */
typedef struct {
    TaskFunction_t taskFn;
    const char *name;
    uint16_t stackDepth;
    void *params;
    UBaseType_t priority;
} OS_TaskEntry;

/* --- VARIABLES --- */
extern const OS_TaskEntry task_table[TASK_TABLE_SIZE];

/* --- PUBLIC FUNCTION DECLARATIONS --- */
void OS_Init(void);
BaseType_t OS_CreateTask(TaskFunction_t pxTaskCode, const char * const pcName, const uint16_t usStackDepth, void *pvParameters, UBaseType_t uxPriority, TaskHandle_t *pxCreatedTask);

#endif /* BSW_OS_H */

/* --- REVISION HISTORY --- */
/* v1.0 - Initial commit - Hari */
