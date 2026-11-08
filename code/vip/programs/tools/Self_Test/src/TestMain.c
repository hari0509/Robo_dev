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
#include "mcal_dio.h"

/* --- MACROS --- */
/* None */

/* --- CONSTANTS --- */
/* None */

/* --- VARIABLES --- */
/* None */

/* --- PRIVATE CODE --- */
/* None */
/*STUB code*/
/* Test pin (GP20) is configured as a DIO output by Port_Init() */
static bool Teststate = false;
/* --- PUBLIC CODE --- */
void TestMain_init(void)
{
}

/*
 */
void TestMain_mainfunction(void)
{
    (void) TestScheduler_mainfunction();
    Dio_WriteChannel(DioConf_DioChannel_TestPin, Teststate ? (Dio_LevelType)STD_HIGH : (Dio_LevelType)STD_LOW);
    Teststate = !Teststate;
    printf("\ntestpin%d",Teststate);
}

/* --- REVISION HISTORY --- */
/* v1.0 - Initial commit - Hari */