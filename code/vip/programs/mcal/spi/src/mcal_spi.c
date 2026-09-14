#include "mcal_spi.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"

/* Zero-initialized at load time -> every entry starts MCAL_SPI_UNINIT (0)
 * even before Mcal_Spi_Init() runs. */
static Mcal_Spi_StatusType Mcal_Spi_State[MCAL_SPI_CH_COUNT];

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

void Mcal_Spi_Init(void)
{
    for (uint32_t ch = 0u; ch < MCAL_SPI_CH_COUNT; ch++) {
        const Mcal_SpiChannelCfgType *cfg = &Mcal_SpiChannelCfg[ch];
        spi_inst_t *hw = Mcal_Spi_GetHw(cfg->channel_id);

        spi_init(hw, cfg->baud_hz);
        spi_set_format(hw, cfg->data_bits,
                        Mcal_Spi_MapCpol(cfg->cpol),
                        Mcal_Spi_MapCpha(cfg->cpha),
                        SPI_MSB_FIRST);

        gpio_set_function(cfg->sck_pin, GPIO_FUNC_SPI);
        gpio_set_function(cfg->tx_pin, GPIO_FUNC_SPI);
        gpio_set_function(cfg->rx_pin, GPIO_FUNC_SPI);

        /* CS stays plain GPIO -- MCAL drives it directly, it is never
         * muxed to GPIO_FUNC_SPI (no hardware SS on this channel). */
        gpio_init(cfg->cs_pin);
        gpio_set_dir(cfg->cs_pin, GPIO_OUT);
        gpio_put(cfg->cs_pin, 1u); /* idle high */

        Mcal_Spi_State[ch] = MCAL_SPI_IDLE;
    }
}

void Mcal_Spi_DeInit(void)
{
    for (uint32_t ch = 0u; ch < MCAL_SPI_CH_COUNT; ch++) {
        const Mcal_SpiChannelCfgType *cfg = &Mcal_SpiChannelCfg[ch];
        spi_inst_t *hw = Mcal_Spi_GetHw(cfg->channel_id);

        spi_deinit(hw);
        gpio_set_function(cfg->sck_pin, GPIO_FUNC_SIO);
        gpio_set_function(cfg->tx_pin, GPIO_FUNC_SIO);
        gpio_set_function(cfg->rx_pin, GPIO_FUNC_SIO);

        Mcal_Spi_State[ch] = MCAL_SPI_UNINIT;
    }
}

Mcal_Spi_StatusType Mcal_Spi_GetStatus(Mcal_SpiChannelIdType channel)
{
    if (channel >= MCAL_SPI_CH_COUNT) {
        return MCAL_SPI_UNINIT;
    }
    return Mcal_Spi_State[channel];
}

Std_ReturnType Mcal_Spi_SetBaudrate(Mcal_SpiChannelIdType channel, uint32_t baud_hz)
{
    if ((channel >= MCAL_SPI_CH_COUNT) || (Mcal_Spi_State[channel] == MCAL_SPI_UNINIT)) {
        return E_NOT_OK;
    }
    spi_inst_t *hw = Mcal_Spi_GetHw(Mcal_SpiChannelCfg[channel].channel_id);
    (void)spi_set_baudrate(hw, baud_hz);
    return E_OK;
}

void Mcal_Spi_CsAssert(Mcal_SpiChannelIdType channel)
{
    if (channel < MCAL_SPI_CH_COUNT) {
        gpio_put(Mcal_SpiChannelCfg[channel].cs_pin, 0u);
    }
}

void Mcal_Spi_CsDeassert(Mcal_SpiChannelIdType channel)
{
    if (channel < MCAL_SPI_CH_COUNT) {
        gpio_put(Mcal_SpiChannelCfg[channel].cs_pin, 1u);
    }
}

Std_ReturnType Mcal_Spi_TransferNoCs(Mcal_SpiChannelIdType channel,
                                      const uint8_t *tx_buf,
                                      uint8_t *rx_buf,
                                      size_t len)
{
    if ((channel >= MCAL_SPI_CH_COUNT) || (Mcal_Spi_State[channel] != MCAL_SPI_IDLE)) {
        return E_NOT_OK;
    }
    if (len == 0u) {
        return E_OK;
    }

    spi_inst_t *hw = Mcal_Spi_GetHw(Mcal_SpiChannelCfg[channel].channel_id);
    int result;

    Mcal_Spi_State[channel] = MCAL_SPI_BUSY;

    if ((tx_buf != NULL) && (rx_buf != NULL)) {
        result = spi_write_read_blocking(hw, tx_buf, rx_buf, len);
    } else if (tx_buf != NULL) {
        result = spi_write_blocking(hw, tx_buf, len);
    } else if (rx_buf != NULL) {
        result = spi_read_blocking(hw, 0xFFu, rx_buf, len);
    } else {
        result = -1;
    }

    Mcal_Spi_State[channel] = MCAL_SPI_IDLE;

    return (result == (int)len) ? E_OK : E_NOT_OK;
}

Std_ReturnType Mcal_Spi_Transfer(Mcal_SpiChannelIdType channel,
                                  const uint8_t *tx_buf,
                                  uint8_t *rx_buf,
                                  size_t len)
{
    Std_ReturnType ret;

    Mcal_Spi_CsAssert(channel);
    ret = Mcal_Spi_TransferNoCs(channel, tx_buf, rx_buf, len);
    Mcal_Spi_CsDeassert(channel);

    return ret;
}