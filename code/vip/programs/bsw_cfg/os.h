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
/* Max no. of Tasks supported */
#define TASK_TABLE_SIZE     (4u)

/* Core affinity masks for RP2040 */
#define OS_CORE_0      (1 << 0)
#define OS_CORE_1      (1 << 1)
#define OS_CORE_BOTH   (OS_CORE_0 | OS_CORE_1)

/* --- DATA TYPES --- */
typedef struct {
    TaskFunction_t taskFn;
    const char *name;
    uint16_t stackDepth;
    void *params;
    UBaseType_t priority;
    UBaseType_t coreAffinity;   /* NEW */
} OS_TaskEntry;

/* --- VARIABLES --- */
extern const OS_TaskEntry task_table[TASK_TABLE_SIZE];

/* --- PUBLIC FUNCTION DECLARATIONS --- */
void OS_Init(void);

#endif /* BSW_OS_H */

/* --- REVISION HISTORY --- */
/* v1.0 - Initial commit - Hari */
