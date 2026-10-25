#include "mcal_spi_cfg.h"

/* GPIO Configuration */
#define MCAL_SPI_CH0_SCK_PIN      (2u)
#define MCAL_SPI_CH0_TX_PIN       (3u)
#define MCAL_SPI_CH0_RX_PIN       (4u)
#define MCAL_SPI_CH0_CS_PIN       (5u)

/* Communication Configuration 
 * Motorola SPI Modes:
 * +------+-------+-------+-------------+-------------+-------------+
 * | Mode | CPOL  | CPHA  | Clock Idle  | Sample Edge | Shift Edge  |
 * +------+-------+-------+-------------+-------------+-------------+
 * |  0   |   0   |   0   | LOW         | Rising      | Falling     |
 * |  1   |   0   |   1   | LOW         | Falling     | Rising      |
 * |  2   |   1   |   0   | HIGH        | Falling     | Rising      |
 * |  3   |   1   |   1   | HIGH        | Rising      | Falling     |
 * +------+-------+-------+-------------+-------------+-------------+
*/
#define MCAL_SPI_1MHZ             (1000000u)
#define MCAL_SPI_CH0_CPOL         (0u)
#define MCAL_SPI_CH0_CPHA         (0u)
#define MCAL_SPI_CH0_DATA_BITS    (8u)

const Mcal_SpiChannelCfgType Mcal_SpiChannelCfg[MCAL_SPI_HW_COUNT] = {
    /* Logical channel 0 */
    {
        .HWchannel_id = MCAL_SPI_HW_SPI0,
        .sck_pin    = MCAL_SPI_CH0_SCK_PIN,
        .tx_pin     = MCAL_SPI_CH0_TX_PIN,
        .rx_pin     = MCAL_SPI_CH0_RX_PIN,
        .cs_pin     = MCAL_SPI_CH0_CS_PIN,
        .baud_hz    = MCAL_SPI_1MHZ,
        .cpol       = MCAL_SPI_CH0_CPOL,
        .cpha       = MCAL_SPI_CH0_CPHA,
        .data_bits  = MCAL_SPI_CH0_DATA_BITS,
    },
};

const Mcal_SpiJobCfgType Mcal_SpiJobCfg[MCAL_SPI_JOB_COUNT] =
{
    {
        .channel_id = MCAL_SPI_CH_Test,
        .priority   = 1u
    },
    {
        .channel_id = MCAL_SPI_CH_Test,
        .priority   = 1u
    }
};

static const Mcal_SpiJobIdType Mcal_SpiSeq0JobList[] =
{
    MCAL_SPI_JOB_0,
    MCAL_SPI_JOB_1
};

const Mcal_SpiSeqCfgType Mcal_SpiSeqCfg[MCAL_SPI_SEQ_COUNT] =
{
    {
        .job_list  = Mcal_SpiSeq0JobList,
        .job_count = 2u
    }
};

