#ifndef MCAL_SPI_CFG_H
#define MCAL_SPI_CFG_H

/* Header file inclusions */
#include "mcal_spi_types.h"

#define MCAL_SPI_CH_MAX_LENGTH (256u)
#define MCAL_SPI_DEFAULT_TX_DATA (0xFFu)

/* 2 Physical SPI Hardwares SPI0 and SPI1 */
/* Base addresses - 0x4003c000 and 0x40040000 respectively 
   Defined as SPI0_BASE and SPI1_BASE in SDK*/
typedef enum {
    MCAL_SPI_HW_SPI0 = 0,
    MCAL_SPI_HW_SPI1,
    MCAL_SPI_HW_COUNT
} Mcal_SpiHwIdType;

/* Channel configuration - Motorola format */
typedef struct {
    Mcal_SpiHwIdType HWchannel_id;  /* SPI0 or SPI1 */
    uint8          sck_pin;     /* Serial clock pin */
    uint8          tx_pin;      /* MOSI pin */
    uint8          rx_pin;      /* MISO pin */
    uint8          cs_pin;      /* software-GPIO CS, not HW SS */
    uint32         baud_hz;     /* Clock frequency in Hz */
    /* CPOL and CPHA - motorola SPI modes */
    uint8          cpol;        /* 0 = idle low, 1 = idle high */
    uint8          cpha;        /* 0 = sample leading edge, 1 = sample trailing edge */
    uint8          data_bits;   /* 4..16, per RP2040 SPI block */
} Mcal_SpiChannelCfgType;

extern const Mcal_SpiChannelCfgType Mcal_SpiChannelCfg[MCAL_SPI_HW_COUNT];

typedef enum
{
    MCAL_SPI_CH_Test = 0,
    MCAL_SPI_CH_COUNT
} Mcal_SpiChannelIdType;

/* Logical SPI Job IDs */
typedef enum
{
    MCAL_SPI_JOB_0 = 0,  /* Write command */
    MCAL_SPI_JOB_1,      /* Read data */
    MCAL_SPI_JOB_COUNT
} Mcal_SpiJobIdType;

/* Logical SPI Sequence IDs */
typedef enum
{
    MCAL_SPI_SEQ_0 = 0,
    MCAL_SPI_SEQ_COUNT
} Mcal_SpiSeqIdType;

/* Job configuration */
typedef struct
{
    Mcal_SpiChannelIdType channel_id;
    uint8 priority;
} Mcal_SpiJobCfgType;

/* Sequence configuration */
typedef struct
{
    const Mcal_SpiJobIdType *job_list;
    uint8 job_count;
} Mcal_SpiSeqCfgType;

typedef struct
{
    const Spi_DataBufferType *tx_buffer;
    Spi_DataBufferType *rx_buffer;
    Spi_NumberOfDataType length;
    boolean is_configured;
} Mcal_SpiEbRuntimeType;


extern const Mcal_SpiJobCfgType Mcal_SpiJobCfg[MCAL_SPI_JOB_COUNT];

extern const Mcal_SpiSeqCfgType Mcal_SpiSeqCfg[MCAL_SPI_SEQ_COUNT];


#endif /* MCAL_SPI_CFG_H */