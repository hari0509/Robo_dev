#ifndef MCAL_SPI_H
#define MCAL_SPI_H

#include "mcal_spi_types.h"
#include "mcal_spi_cfg.h"

/* API list */
/* Requirements:
SWS_Spi_00298 - Non Re-entrant and Synchronous API
SWS_Spi_00299 - SPI HW init
SWS_Spi_00013 - SPI HW init will be done via config param
SWS_Spi_00015 - SPI Global Variables initialization */
void Mcal_Spi_Init(void);

/* Tear down every channel */
void Mcal_Spi_DeInit(void);

/* External buffer API */
Std_ReturnType Mcal_Spi_SetupEB(Mcal_SpiChannelIdType Channel, const Spi_DataBufferType *SrcDataBufferPtr, Spi_DataBufferType *DesDataBufferPtr, Spi_NumberOfDataType Length);

/* Synchronous transmission API */
Std_ReturnType Mcal_Spi_SyncTransmit(Mcal_SpiSeqIdType Sequence);

#endif /* MCAL_SPI_H */