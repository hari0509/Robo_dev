#ifndef MCAL_SPI_TYPE_H
#define MCAL_SPI_TYPE_H

/* Header file inclusions */
#include "Std_Types.h"

typedef uint8  Spi_DataBufferType;
typedef uint16 Spi_NumberOfDataType;

/* SPI Driver Status */
typedef enum
{
    SPI_UNINIT = 0,
    SPI_IDLE,
    SPI_BUSY
} Mcal_SpiStatusType;

/* SPI Sequence Result */
typedef enum
{
    SPI_SEQ_OK = 0,
    SPI_SEQ_PENDING,
    SPI_SEQ_FAILED,
    SPI_SEQ_CANCELLED
} Mcal_SpiSeqResultType;

/* SPI Job Result */
typedef enum
{
    SPI_JOB_OK = 0,
    SPI_JOB_PENDING,
    SPI_JOB_FAILED,
    SPI_JOB_QUEUED
} Mcal_SpiJobResultType;

#endif /* MCAL_SPI_TYPE_H */