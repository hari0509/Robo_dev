#include "mcal_spi_cfg.h"
#include "mcal_dio_cfg.h"

/* ---------------------------------- */
/* HW unit 0: SPI0 on GP2 (SCK), GP3 (MOSI), GP4 (MISO) */
#define MCAL_SPI_HW0_SCK_PIN      (2u)
#define MCAL_SPI_HW0_TX_PIN       (3u)
#define MCAL_SPI_HW0_RX_PIN       (4u)


#define MCAL_SPI_1MHZ             (1000000u)

/* Channel 0 */
#define MCAL_SPI_CH0_DATA_WIDTH   (8u)
#define MCAL_SPI_CH0_MAX_LENGTH   (256u)
#define MCAL_SPI_CH0_DEFAULT_DATA (0xFFu)

static const Spi_HwUnitConfigType Spi_HwUnitCfg[SPI_MAX_HWUNIT] =
{
    {
        .hw_instance = 0u,
        .sck_pin     = MCAL_SPI_HW0_SCK_PIN,
        .tx_pin      = MCAL_SPI_HW0_TX_PIN,
        .rx_pin      = MCAL_SPI_HW0_RX_PIN
    }
};

/* Mode 0 = idle low, data shifted on the trailing edge (see mcal_spi_cfg.h) */
static const Spi_ExternalDeviceConfigType Spi_ExtDev0 =
{
    .hw_unit        = SpiConf_SpiHwUnit_SPI0,
    .baudrate_hz    = MCAL_SPI_1MHZ,
    .clk_idle       = SPI_CLK_IDLE_LOW,
    .shift_edge     = SPI_EDGE_TRAILING,
    .cs_channel     = DioConf_DioChannel_SPI_CS,
    .cs_polarity    = SPI_CS_POLARITY_LOW,
    .time_cs2clk_us = 0u,
    .time_clk2cs_us = 0u
};

/* Channel 0 is an external-buffer channel, so it needs no IB memory */
static const Spi_ChannelConfigType Spi_ChannelCfg[SPI_MAX_CHANNEL] =
{
    {
        .channel_type = SPI_CHANNEL_EB,
        .data_width   = MCAL_SPI_CH0_DATA_WIDTH,
        .default_data = MCAL_SPI_CH0_DEFAULT_DATA,
        .max_length   = MCAL_SPI_CH0_MAX_LENGTH,
        .ib_tx_buffer = NULL,
        .ib_rx_buffer = NULL
    }
};

/* Both jobs run the test channel on the same device (write then read) */
static const Spi_ChannelType Spi_JobChannels[] =
{
    SpiConf_SpiChannel_Test
};

static const Spi_JobConfigType Spi_JobCfg[SPI_MAX_JOB] =
{
    {
        .priority         = 1u,
        .channel_list     = Spi_JobChannels,
        .channel_count    = 1u,
        .device           = &Spi_ExtDev0,
        .end_notification = NULL
    },
    {
        .priority         = 1u,
        .channel_list     = Spi_JobChannels,
        .channel_count    = 1u,
        .device           = &Spi_ExtDev0,
        .end_notification = NULL
    }
};

static const Spi_JobType Spi_SeqTestJobs[] =
{
    SpiConf_SpiJob_Write,
    SpiConf_SpiJob_Read
};

static const Spi_SequenceConfigType Spi_SequenceCfg[SPI_MAX_SEQUENCE] =
{
    {
        .job_list         = Spi_SeqTestJobs,
        .job_count        = 2u,
        .end_notification = NULL
    }
};

const Spi_ConfigType Spi_Config =
{
    .hw_units  = Spi_HwUnitCfg,
    .channels  = Spi_ChannelCfg,
    .jobs      = Spi_JobCfg,
    .sequences = Spi_SequenceCfg
};
