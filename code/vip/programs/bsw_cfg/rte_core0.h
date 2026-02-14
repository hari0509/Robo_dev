#ifndef BSW_RTE_CORE0_H
#define BSW_RTE_CORE0_H

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
void asw_task_100ms(void *pvParameters);
void cdd_task_100ms(void *pvParameters);
void Test_task_100ms(void *pvParameters);

/* Application/linkage points: mainfunctions implemented in component source */
extern void asw_checkpointreached(void);
extern void asw_blink_mainfunction(void);
extern void cdd_checkpointreached(void);
extern void cdd_servo_mainfunction(void);
extern void TestMain_mainfunction(void);

#endif /* BSW_RTE_CORE0_H */

/* --- REVISION HISTORY --- */
/* v1.0 - Initial commit - Hari */
