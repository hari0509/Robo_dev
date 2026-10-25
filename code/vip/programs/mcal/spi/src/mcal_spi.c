#include "mcal_spi.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"

/* ---------------------------------- */
/* Global variables */
/* SPI Driver State */
static Mcal_SpiStatusType Mcal_Spi_Status = SPI_UNINIT;

/* SPI Sequence Results */
static Mcal_SpiSeqResultType Mcal_Spi_SeqResult[MCAL_SPI_SEQ_COUNT];

/* SPI Job Results */
static Mcal_SpiJobResultType Mcal_Spi_JobResult[MCAL_SPI_JOB_COUNT];

/* EB runtime configuration */
static Mcal_SpiEbRuntimeType Mcal_Spi_EbRuntime[MCAL_SPI_CH_COUNT];

/* ---------------------------------- */
/* Private functions */
static inline spi_inst_t *Mcal_Spi_GetHw(Mcal_SpiHwIdType hw)
{
    return (hw == MCAL_SPI_HW_SPI1) ? spi1 : spi0;
}

static inline spi_cpol_t Mcal_Spi_MapCpol(uint8_t cpol)
{
    return cpol ? SPI_CPOL_1 : SPI_CPOL_0;
}

static inline spi_cpha_t Mcal_Spi_MapCpha(uint8_t cpha)
{
    return cpha ? SPI_CPHA_1 : SPI_CPHA_0;
}

/* ---------------------------------- */
/* Public functions */

void Mcal_Spi_Init(void)
{
    if (Mcal_Spi_Status == SPI_UNINIT)
    {
        /* Initialize configured SPI channels */
        for (uint32 channel = 0u; channel < MCAL_SPI_CH_COUNT; channel++)
        {
            const Mcal_SpiChannelCfgType *cfg = &Mcal_SpiChannelCfg[channel];

            spi_inst_t *hw = Mcal_Spi_GetHw(cfg->HWchannel_id);

            spi_init(hw, cfg->baud_hz);

            spi_set_format(
                hw,
                cfg->data_bits,
                Mcal_Spi_MapCpol(cfg->cpol),
                Mcal_Spi_MapCpha(cfg->cpha),
                SPI_MSB_FIRST
            );

            gpio_set_function(cfg->sck_pin, GPIO_FUNC_SPI);
            gpio_set_function(cfg->tx_pin, GPIO_FUNC_SPI);
            gpio_set_function(cfg->rx_pin, GPIO_FUNC_SPI);

            /* Software-controlled chip select */
            gpio_init(cfg->cs_pin);
            gpio_set_dir(cfg->cs_pin, GPIO_OUT);
            gpio_put(cfg->cs_pin, 1u);

            /* Initialize EB runtime */
            Mcal_Spi_EbRuntime[channel].tx_buffer = NULL;
            Mcal_Spi_EbRuntime[channel].rx_buffer = NULL;
            Mcal_Spi_EbRuntime[channel].length = 0u;
            Mcal_Spi_EbRuntime[channel].is_configured = FALSE;
        }

        /* Initialize sequence results */
        for (uint32 seq = 0u; seq < MCAL_SPI_SEQ_COUNT; seq++)
        {
            Mcal_Spi_SeqResult[seq] = SPI_SEQ_OK;
        }

        /* initialize job results */
        for (uint32 job = 0u; job < MCAL_SPI_JOB_COUNT; job++)
        {
            Mcal_Spi_JobResult[job] = SPI_JOB_OK;
        }

        Mcal_Spi_Status = SPI_IDLE;
    }
}

Std_ReturnType Mcal_Spi_SetupEB(
    Mcal_SpiChannelIdType Channel,
    const Spi_DataBufferType *SrcDataBufferPtr,
    Spi_DataBufferType *DesDataBufferPtr,
    Spi_NumberOfDataType Length)
{
    Std_ReturnType result = E_NOT_OK;
    Mcal_SpiEbRuntimeType *eb;

    if ((Mcal_Spi_Status != SPI_UNINIT) &&
        ((uint32)Channel < MCAL_SPI_CH_COUNT) &&
        (Length > 0u) &&
        (Length <= MCAL_SPI_CH_MAX_LENGTH))
    {
        eb = &Mcal_Spi_EbRuntime[Channel];

        eb->tx_buffer = SrcDataBufferPtr;
        eb->rx_buffer = DesDataBufferPtr;
        eb->length = Length;
        eb->is_configured = TRUE;

        result = E_OK;
    }

    return result;
}

Std_ReturnType Mcal_Spi_SyncTransmit(Mcal_SpiSeqIdType Sequence)
{
    Std_ReturnType result = E_NOT_OK;

    const Mcal_SpiSeqCfgType *seq_cfg;
    const Mcal_SpiJobCfgType *job_cfg;
    const Mcal_SpiChannelCfgType *ch_cfg;
    const Mcal_SpiEbRuntimeType *eb;

    Mcal_SpiJobIdType job_id;
    Mcal_SpiChannelIdType channel_id;

    uint32 job_index;
    uint32 data_index;
    uint16 tx_frame;
    uint16 rx_frame;

    /* Validate driver status and sequence ID */
    if ((Mcal_Spi_Status == SPI_IDLE) &&
        ((uint32)Sequence < MCAL_SPI_SEQ_COUNT))
    {
        seq_cfg = &Mcal_SpiSeqCfg[Sequence];

        if ((seq_cfg->job_list != NULL) &&
            (seq_cfg->job_count > 0u))
        {
            result = E_OK;

            /* Check EB configuration for each job */
            for (job_index = 0u;
                 (job_index < seq_cfg->job_count) &&
                 (result == E_OK);
                 job_index++)
            {
                job_id = seq_cfg->job_list[job_index];
                job_cfg = &Mcal_SpiJobCfg[job_id];
                channel_id = Mcal_SpiJobCfg[job_id].channel_id;
                ch_cfg = &Mcal_SpiChannelCfg[channel_id];
                eb = &Mcal_Spi_EbRuntime[channel_id];
                if (eb->is_configured == FALSE)
                {
                    result = E_NOT_OK;
                }
                else
                {
                    Mcal_Spi_Status = SPI_BUSY;
                    Mcal_Spi_SeqResult[Sequence] = SPI_SEQ_PENDING;
                    /* Current job starts */
                    Mcal_Spi_JobResult[job_id] = SPI_JOB_PENDING;
                    /* Assert chip select */
                    gpio_put(ch_cfg->cs_pin, 0u);
                                        /* Transfer EB frames */
                    for (data_index = 0u;
                         data_index < eb->length;
                         data_index++)
                    {
                        tx_frame = (eb->tx_buffer != NULL) ? (uint16)eb->tx_buffer[data_index] : (uint16)MCAL_SPI_DEFAULT_TX_DATA;
                        /* Wait for TX FIFO space */
                        while (spi_is_writable(Mcal_Spi_GetHw(ch_cfg->HWchannel_id)) == false)
                        {
                            /* Poll */
                        }

                        /* Write TX FIFO */
                        spi_get_hw(Mcal_Spi_GetHw(ch_cfg->HWchannel_id))->dr = (uint32)tx_frame;

                        /* Wait for RX FIFO data */
                        while (spi_is_readable(Mcal_Spi_GetHw(ch_cfg->HWchannel_id)) == false)
                        {
                            /* Poll */
                        }

                        /* Read RX FIFO */
                        rx_frame = (uint16)(spi_get_hw(Mcal_Spi_GetHw(ch_cfg->HWchannel_id))->dr & 0xFFFFu);

                        if (eb->rx_buffer != NULL)
                        {
                            eb->rx_buffer[data_index] = (Spi_DataBufferType)rx_frame;
                        }
                    }

                    /* Wait for SPI hardware completion */
                    while (spi_is_busy(Mcal_Spi_GetHw(ch_cfg->HWchannel_id)) == true)
                    {
                        /* Poll */
                    }

                    /* Deassert chip select */
                    gpio_put(ch_cfg->cs_pin, 1u);

                    /* Job completed */
                    Mcal_Spi_JobResult[job_id] = SPI_JOB_OK;
                }
            }
            /* Sequence completed */
            Mcal_Spi_SeqResult[Sequence] = SPI_SEQ_OK;
            Mcal_Spi_Status = SPI_IDLE;
        }
    }
    return result;
}
