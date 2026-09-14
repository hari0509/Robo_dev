#include "mcal_spi_cfg.h"

/* GP2 = SCK, GP3 = TX/MOSI, GP4 = RX/MISO, GP5 = CS (software GPIO) */
const Mcal_SpiChannelCfgType Mcal_SpiChannelCfg[MCAL_SPI_CH_COUNT] = {
    [MCAL_SPI_CH_0] = {
        .channel_id = MCAL_SPI_HW_SPI0,
        .sck_pin    = 2u,
        .tx_pin     = 3u,
        .rx_pin     = 4u,
        .cs_pin     = 5u,
        .baud_hz    = 1000000u,
        .cpol       = 0u,
        .cpha       = 0u,
        .data_bits  = 8u,
    },
};