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
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "os.h"
#include "rte.h"
#include "ecum.h"
#include "mcal_gpt.h"
#include "pico/multicore.h"

/* --- MACROS --- */
/* None */

/* --- CONSTANTS --- */
/* None */

/* --- VARIABLES --- */
static uint64_t startup_KPI_time_us = 0u;

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
    printf("Hello from VIP SW!! \n");
    /* GPT should be initialized very before in order to measure start-up time */
    gpt_init();
    /* Start-up KPI */
    gpt_start_timer(GPT_TIMER_0);
    EcuM_Init();
    startup_KPI_time_us = gpt_get_timervalue(GPT_TIMER_0);
    printf("Start-up KPI(ms): %llu\r\n", startup_KPI_time_us/1000);

    while (true)
    {
        /* Should never reach here if scheduler started */
    }
    return 0;
}

/* --- REVISION HISTORY --- */
/* v1.0 - Initial commit - Hari */