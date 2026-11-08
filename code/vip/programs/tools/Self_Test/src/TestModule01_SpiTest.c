#include "TestMain.h"
#include "mcal_spi.h"

/* Set TM01_Test from the debugger to pick the case that runs each cycle */
enum
{   TM01_Setup_EB_TEST = 0u,
    TM01_SpiSync_TEST = 1u,
    TM01_SpiAsync_TEST = 2u,
    TM01_TEST_COUNT
};

uint8 TM01_Test = 0u;
uint8 TM01_TxBuf = 5u;
uint8 TM01_RxBuf = 5u;
uint8 TM01_SetupEBret = 5u;
uint8 TM01_SpiSyncret = 5u;
uint8 TM01_SpiAsyncret = 5u;
uint8 TM01_SeqResult = 0xFFu;   /* Spi_SeqResultType of the test sequence */
uint8 TM01_Job0Result = 0xFFu;  /* Spi_JobResultType of SpiJob_Write      */
uint8 TM01_Job1Result = 0xFFu;  /* Spi_JobResultType of SpiJob_Read       */


void TM01_SpiTest(void)
{
    /* Stand-in for the RTE cyclic call that drives async sequences */
    Spi_MainFunction_Handling();

    switch (TM01_Test)
    {
    case TM01_Setup_EB_TEST:
        TM01_SetupEBret = Spi_SetupEB(SpiConf_SpiChannel_Test, &TM01_TxBuf, &TM01_RxBuf, 1u);
        break;

    case TM01_SpiSync_TEST:
        TM01_SpiSyncret = Spi_SyncTransmit(SpiConf_SpiSequence_Test);
        break;

    case TM01_SpiAsync_TEST:
        /* Start only if the previous run is done; results are read below */
        if (Spi_GetSequenceResult(SpiConf_SpiSequence_Test) != SPI_SEQ_PENDING)
        {
            TM01_SpiAsyncret = Spi_AsyncTransmit(SpiConf_SpiSequence_Test);
        }
        break;

    default:
        break;
    }

    TM01_SeqResult  = (uint8)Spi_GetSequenceResult(SpiConf_SpiSequence_Test);
    TM01_Job0Result = (uint8)Spi_GetJobResult(SpiConf_SpiJob_Write);
    TM01_Job1Result = (uint8)Spi_GetJobResult(SpiConf_SpiJob_Read);
}

/* --- REVISION HISTORY --- */
/* v1.0 - Initial commit - Hari */