#include "TestMain.h"
#include "mcal_spi.h"
#include "mcal_spi_cfg.h"

enum 
{   TM01_Setup_EB_TEST = 0u, 
    TM01_SpiSync_TEST = 1u,
    TM01_TEST_COUNT
};

uint8 TM01_Test = 0u;
uint8 TM01_TxBuf = 5u;
uint8 TM01_RxBuf = 5u;
uint8 TM01_SetupEBret = 5u;
uint8 TM01_SpiSyncret = 5u;


void TM01_SpiTest(void)
{
    switch (TM01_Test)
    {
    case TM01_Setup_EB_TEST:
        TM01_SetupEBret = Mcal_Spi_SetupEB(MCAL_SPI_CH_Test, &TM01_TxBuf, &TM01_RxBuf, 1u);
        break;
    
    case TM01_SpiSync_TEST:
        TM01_SpiSyncret = Mcal_Spi_SyncTransmit(MCAL_SPI_CH_Test);
        break;

    default:
        break;
    }
}

/* --- REVISION HISTORY --- */
/* v1.0 - Initial commit - Hari */