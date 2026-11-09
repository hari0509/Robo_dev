#include "TestMain.h"
#include "mcal_spi.h"

/* Set TM01_Test from the debugger to pick the case that runs each cycle */
enum
{   TM01_Setup_EB_TEST = 0u,
    TM01_SpiSync_TEST = 1u,
    TM01_SpiAsync_TEST = 2u,
    TM01_TEST_COUNT
};

/* ---- MCP2515 SPI instructions and registers ---- */
#define MCP_CMD_RESET    (0xC0u)
#define MCP_CMD_READ     (0x03u)
#define MCP_CMD_WRITE    (0x02u)
#define MCP_REG_CNF3     (0x28u)   /* CNF2 = 0x29, CNF1 = 0x2A:  */
#define MCP_REG_CANSTAT  (0x0Eu)   /* reads 0x80 after reset (configuration mode) */
#define MCP_REG_CANCTRL  (0x0Fu)   /* reads 0x87 after reset */

/* WRTest channel: WRITE CNF3, CNF2, CNF1 in one CS window */
uint8 TM01_WrTxBuf[5] = {MCP_CMD_WRITE, MCP_REG_CNF3, 0x02u, 0x90u, 0x00u};
uint8 TM01_WrRxBuf[5] = {0u};      /* MISO is not driven during a write: ignore */

/* RDTest channel: READ the same three registers; bytes 2..4 come back */
uint8 TM01_RdTxBuf[5] = {MCP_CMD_READ, MCP_REG_CNF3, 0x00u, 0x00u, 0x00u};
uint8 TM01_RdRxBuf[5] = {0u};      /* expect {xx, xx, 0x02, 0x90, 0x00} */

uint8 TM01_McpOk = 0u;             /* 1 = read-back matches the write */

uint8 TM01_Test = 0u;
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
        TM01_SetupEBret = Spi_SetupEB(SpiConf_SpiChannel_WRTest, TM01_WrTxBuf, TM01_WrRxBuf, (Spi_NumberOfDataType)sizeof(TM01_WrTxBuf));
        TM01_SetupEBret |= Spi_SetupEB(SpiConf_SpiChannel_RDTest, TM01_RdTxBuf, TM01_RdRxBuf, (Spi_NumberOfDataType)sizeof(TM01_RdTxBuf));
        break;

    case TM01_SpiSync_TEST:
        TM01_SpiSyncret = Spi_SyncTransmit(SpiConf_SpiSequence_Test);
        TM01_McpOk = ((TM01_SpiSyncret == E_OK) &&
                    (TM01_RdRxBuf[2] == 0x02u) &&
                    (TM01_RdRxBuf[3] == 0x90u) &&
                    (TM01_RdRxBuf[4] == 0x00u)) ? 1u : 0u;
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