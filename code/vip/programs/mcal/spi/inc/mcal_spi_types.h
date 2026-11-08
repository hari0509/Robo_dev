#ifndef MCAL_SPI_TYPE_H
#define MCAL_SPI_TYPE_H

/* Header file inclusions */
#include "Std_Types.h"

/* ---------------------------------- */
/* Identifier / size types */
typedef uint8  Spi_DataBufferType;      /* One frame per element, DataWidth 4..8 bit */
typedef uint16 Spi_NumberOfDataType;    /* Number of frames in a buffer */
typedef uint8  Spi_ChannelType;
typedef uint8  Spi_JobType;
typedef uint8  Spi_SequenceType;
typedef uint8  Spi_HWUnitType;

/* SPI Driver and HW unit status (Spi_StatusType) */
typedef enum
{
    SPI_UNINIT = 0,
    SPI_IDLE,
    SPI_BUSY
} Spi_StatusType;

/* Job result (Spi_JobResultType) */
typedef enum
{
    SPI_JOB_OK = 0,
    SPI_JOB_PENDING,
    SPI_JOB_FAILED,
    SPI_JOB_QUEUED
} Spi_JobResultType;

/* Sequence result (Spi_SeqResultType) */
typedef enum
{
    SPI_SEQ_OK = 0,
    SPI_SEQ_PENDING,
    SPI_SEQ_FAILED,
    SPI_SEQ_CANCELED
} Spi_SeqResultType;

/* Asynchronous handling mode (Spi_AsyncModeType) */
typedef enum
{
    SPI_POLLING_MODE = 0,
    SPI_INTERRUPT_MODE
} Spi_AsyncModeType;

/* Job / sequence end notification */
typedef void (*Spi_NotificationType)(void);

/* Post-build configuration root, defined in mcal_spi_cfg.h */
typedef struct Spi_ConfigType_Tag Spi_ConfigType;

#endif /* MCAL_SPI_TYPE_H */