/**
 * ============================================================================
 * FILE: ecum.c
 * ============================================================================
 * DESCRIPTION:
 *   ECU Manager (EcuM) component. Responsible for initializing and managing
 *   all platform software components (CDD servo, ASW blink, etc.).
 *
 * AUTHOR: Hari
 * ============================================================================
 */

/* --- INCLUDES --- */
#include <FreeRTOS.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
/* User files */
#include "ecum.h"
#include "os.h"
#include "rte.h"

/* Forward declarations/external symbols from components */
extern void cdd_servo_init(void);
extern void asw_blink_init(void);
extern void sys_nvm_init(void);

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
 * EcuM_Init()
 * Initialize all ECU software components (CDD, ASW, etc.).
 * Called once during system startup before scheduler begins.
 */
void EcuM_Init(void)
{
    /* Run driver init sequence; OS will create and schedule SYS_NVM on core1 */
    Driver_init0();
    Driver_init1();
    Driver_init2();
}

void Driver_init0(void)
{
    OS_Init();
    RTE_Init();
    sys_nvm_init();
}

void Driver_init1(void)
{
    cdd_servo_init();
}

void Driver_init2(void)
{
    asw_blink_init();
    vTaskStartScheduler();
}


/* --- REVISION HISTORY --- */
/* v1.0 - Initial commit - Hari */