#ifndef BSW_RTE_CORE1_H
#define BSW_RTE_CORE1_H

/**
 * ============================================================================
 * FILE: rte.h
 * ============================================================================
 * DESCRIPTION:
 *   Runtime Environment (RTE) header. Defines task scheduling, periodic
 *   task entry points, and component mainfunction linkage points.
 *
 * AUTHOR: Hari
 * ============================================================================
 */

/* --- INCLUDES --- */
/* None */

/* --- MACROS --- */
/* None */

/* --- CONSTANTS --- */
/* None */

/* --- PUBLIC FUNCTION DECLARATIONS --- */
/* None */

/* Task functions provided by RTE (FreeRTOS task entry points) */
void sys_task_100ms(void *pvParameters);

/* Application/linkage points: mainfunctions implemented in component source */
void sys_nvm_mainfunction(void);

#endif /* BSW_RTE_CORE1_H */

/* --- REVISION HISTORY --- */
/* v1.0 - Initial commit - Hari */
