/**
 * ============================================================================
 * FILE: main.c
 * ============================================================================
 * DESCRIPTION:
 *   RTOS entry point. Initializes platform (GPIO, stdio) and starts the
 *   OS/RTE scheduler with ECU Manager (EcuM) component initialization.
 *
 * AUTHOR: Hari
 * ============================================================================
 */

/* --- INCLUDES --- */
#include <FreeRTOS.h>
#include <task.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "os.h"
#include "rte.h"
#include "ecum.h"

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
 * main()
 * Entry point for the application. Initializes stdio, OS, RTE, and ECU
 * manager, then starts the FreeRTOS scheduler.
 */
int main(void)
{
    stdio_init_all();

    /* Initialise OS helpers (if any) and create tasks via RTE */
    OS_Init();
    RTE_Init();
    EcuM_Init();

    vTaskStartScheduler();

    while (true)
    {
        /* Should never reach here if scheduler started */
    }
    return 0;
}

/* --- REVISION HISTORY --- */
/* v1.0 - Initial commit - Hari */