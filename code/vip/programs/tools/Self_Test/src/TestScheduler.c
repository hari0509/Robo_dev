/**
 * ============================================================================
 * FILE: TestMain.c
 * ============================================================================
 * DESCRIPTION:
 *    
 *
 * AUTHOR: Hari
 * ============================================================================
 */

/* --- INCLUDES --- */
#include "TestMain.h"

/* --- MACROS --- */
/* None */

/* --- CONSTANTS --- */
/* None */

/* --- VARIABLES --- */
/* None */

/* --- PRIVATE CODE --- */
/* None */

/* --- PUBLIC CODE --- */
void TestScheduler_mainfunction(void)
{

}

void asw_checkpointreached()
{
    printf("ASW core=%u -- Time=%llu\n", get_core_num(), gpt_get_timervalue(GPT_TIMER_0)/1000);
}

void cdd_checkpointreached()
{
    printf("cdd core=%u -- Time=%llu\n", get_core_num(), gpt_get_timervalue(GPT_TIMER_0)/1000);
}

void sys_checkpointreached()
{
    printf("cdd core=%u -- Time=%llu\n", get_core_num(), gpt_get_timervalue(GPT_TIMER_0)/1000);
}
/* --- REVISION HISTORY --- */
/* v1.0 - Initial commit - Hari */