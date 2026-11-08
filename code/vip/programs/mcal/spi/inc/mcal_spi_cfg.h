#ifndef MCAL_SPI_CFG_H
#define MCAL_SPI_CFG_H

/**
 * AUTOSAR SPI Handler/Driver - configuration.
 */

/* Header file inclusions */
#include "mcal_spi_types.h"
#include "mcal_dio_types.h"

/* ================================================================ */
/* SpiGeneral - pre-compile configuration                           */
/* ================================================================ */

/* Module identification */
#define SPI_VENDOR_ID             (0xFFFFu)  /* Not a registered vendor ID */
#define SPI_MODULE_ID             (83u)
#define SPI_INSTANCE_ID           (0u)
#define SPI_SW_MAJOR_VERSION      (1u)
#define SPI_SW_MINOR_VERSION      (0u)
#define SPI_SW_PATCH_VERSION      (0u)

/* SpiLevelDelivered: 0 = sync only, 1 = async only, 2 = sync + async */
#define SPI_LEVEL_DELIVERED       (2u)

/* SpiChannelBuffersAllowed: 0 = IB only, 1 = EB only, 2 = IB and EB */
#define SPI_CHANNEL_BUFFERS_ALLOWED (2u)

/* SpiDevErrorDetect */
#define SPI_DEV_ERROR_DETECT      (STD_ON)
#ifndef SPI_DET_REPORT_ERROR
#define SPI_DET_REPORT_ERROR(ApiId, ErrorId) ((void)(ApiId), (void)(ErrorId))
#endif

/* Optional API groups */
#define SPI_VERSION_INFO_API      (STD_ON)  /* Spi_GetVersionInfo   */
#define SPI_CANCEL_API            (STD_ON)  /* Spi_Cancel           */
#define SPI_HW_STATUS_API         (STD_ON)  /* Spi_GetHWUnitStatus  */

/* Baud rate the HW unit is brought up with; each job re-programs its own */
#define SPI_HW_INIT_BAUDRATE_HZ   (1000000u) //1MHz

/* cs_channel of a device that has no CS line (0xFF is no valid Dio channel) */
#define SPI_CS_NOT_USED           ((Dio_ChannelType)0xFFu)

/* ================================================================ */
/* Generated sizes and symbolic names                               */
/* ================================================================ */

#define SPI_MAX_HWUNIT            (1u) //currently SPI0 is used; SPI1 is not configured
#define SPI_MAX_CHANNEL           (1u)
#define SPI_MAX_JOB               (2u)
#define SPI_MAX_SEQUENCE          (1u)

/* HW units (RP2040: SPI0 = 0, SPI1 = 1) */
#define SpiConf_SpiHwUnit_SPI0    ((Spi_HWUnitType)0u)
#define SpiConf_SpiHwUnit_SPI1    ((Spi_HWUnitType)1u)

/* Channels */
#define SpiConf_SpiChannel_Test   ((Spi_ChannelType)0u)

/* Jobs */
#define SpiConf_SpiJob_Write      ((Spi_JobType)0u)   /* Write command */
#define SpiConf_SpiJob_Read       ((Spi_JobType)1u)   /* Read data     */

/* Sequences */
#define SpiConf_SpiSequence_Test  ((Spi_SequenceType)0u)

/* ================================================================ */
/* SpiDriver - post-build configuration types                       */
/* ================================================================ */

/* SpiChannelType */
typedef enum
{
    SPI_CHANNEL_IB = 0,  /* Internal buffer, driver owns the memory      */
    SPI_CHANNEL_EB       /* External buffer, memory set by Spi_SetupEB() */
} Spi_ChannelKindType;

/* SpiShiftClockIdleLevel */
typedef enum
{
    SPI_CLK_IDLE_LOW = 0,
    SPI_CLK_IDLE_HIGH
} Spi_ShiftClockIdleType;

/* SpiDataShiftEdge - the edge on which data is shifted out.
 * Motorola SPI modes (PL022 SPO/SPH):
 * +------+-------+-------+-------------+----------------+
 * | Mode | CPOL  | CPHA  | Clock Idle  | Data shift edge|
 * +------+-------+-------+-------------+----------------+
 * |  0   |   0   |   0   | LOW         | TRAILING       |
 * |  1   |   0   |   1   | LOW         | LEADING        |
 * |  2   |   1   |   0   | HIGH        | TRAILING       |
 * |  3   |   1   |   1   | HIGH        | LEADING        |
 * +------+-------+-------+-------------+----------------+
 */
typedef enum
{
    SPI_EDGE_LEADING = 0,
    SPI_EDGE_TRAILING
} Spi_DataShiftEdgeType;

/* SpiCsPolarity */
typedef enum
{
    SPI_CS_POLARITY_LOW = 0,   /* CS is active low  */
    SPI_CS_POLARITY_HIGH       /* CS is active high */
} Spi_CsPolarityType;

/* SpiHwUnit - one PL022 block and its pins */
typedef struct
{
    uint8 hw_instance;        /* 0 = SPI0, 1 = SPI1 */
    uint8 sck_pin;
    uint8 tx_pin;             /* MOSI */
    uint8 rx_pin;             /* MISO */
} Spi_HwUnitConfigType;

/* SpiExternalDevice - what a job talks to */
typedef struct
{
    Spi_HWUnitType         hw_unit;
    uint32                 baudrate_hz;
    Spi_ShiftClockIdleType clk_idle;
    Spi_DataShiftEdgeType  shift_edge;
    Dio_ChannelType        cs_channel;     /* software CS (Dio), or SPI_CS_NOT_USED */
    Spi_CsPolarityType     cs_polarity;
    uint32                 time_cs2clk_us; /* SpiTimeCs2Clk */
    uint32                 time_clk2cs_us; /* SpiTimeClk2Cs */
} Spi_ExternalDeviceConfigType;

/* SpiChannel */
typedef struct
{
    Spi_ChannelKindType  channel_type;
    uint8                data_width;       /* SpiDataWidth, 4..8 bit    */
    Spi_DataBufferType   default_data;     /* SpiDefaultData            */
    Spi_NumberOfDataType max_length;       /* SpiIbNBuffers/SpiEbMaxLength */
    Spi_DataBufferType  *ib_tx_buffer;     /* IB only, max_length frames */
    Spi_DataBufferType  *ib_rx_buffer;     /* IB only, max_length frames */
} Spi_ChannelConfigType;

/* SpiJob - ordered channel list sharing one chip select */
typedef struct
{
    uint8                               priority;       /* 0 (lowest) .. 3 */
    const Spi_ChannelType              *channel_list;
    uint8                               channel_count;
    const Spi_ExternalDeviceConfigType *device;
    Spi_NotificationType                end_notification; /* may be NULL */
} Spi_JobConfigType;

/* SpiSequence - ordered job list */
typedef struct
{
    const Spi_JobType   *job_list;
    uint8                job_count;
    Spi_NotificationType end_notification;                /* may be NULL */
} Spi_SequenceConfigType;

/* Root passed to Spi_Init() */
struct Spi_ConfigType_Tag
{
    const Spi_HwUnitConfigType   *hw_units;
    const Spi_ChannelConfigType  *channels;
    const Spi_JobConfigType      *jobs;
    const Spi_SequenceConfigType *sequences;
};

extern const Spi_ConfigType Spi_Config;

#endif /* MCAL_SPI_CFG_H */