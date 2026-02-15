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
/*STUB code*/
#define TEST_PIN 20
static bool Teststate = false;
/* --- PUBLIC CODE --- */
void TestMain_init(void)
{
    gpio_init(TEST_PIN);
    gpio_set_dir(TEST_PIN, GPIO_OUT);
}

/*
 */
void TestMain_mainfunction(void)
{
    (void) TestScheduler_mainfunction();
    gpio_put(TEST_PIN, Teststate);
    Teststate = !Teststate;
    printf("\ntestpin%d",Teststate);
}

/* --- REVISION HISTORY --- */
/* v1.0 - Initial commit - Hari */