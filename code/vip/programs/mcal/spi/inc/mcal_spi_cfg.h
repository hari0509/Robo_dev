#ifndef MCAL_SPI_CFG_H
#define MCAL_SPI_CFG_H

/*==============================================================================
 * Module : MCAL - SPI (Configuration)
 * Scope  : RP2040, SPI0/SPI1 hardware blocks
 *============================================================================*/
#include "Std_Types.h"

/* Physical SPI hardware blocks */
typedef enum {
    MCAL_SPI_HW_SPI0 = 0,
    MCAL_SPI_HW_SPI1,
    MCAL_SPI_HW_COUNT
} Mcal_SpiHwIdType;

/* Channel configuration */
typedef struct {
    Mcal_SpiHwIdType channel_id;  /* SPI0 / SPI1 */
    uint8          sck_pin;
    uint8          tx_pin;      /* MOSI */
    uint8          rx_pin;      /* MISO */
    uint8          cs_pin;      /* software-GPIO CS, not HW SS */
    uint32         baud_hz;
    uint8          cpol;        /* 0 = idle low, 1 = idle high */
    uint8          cpha;        /* 0 = sample leading edge, 1 = sample trailing edge */
    uint8          data_bits;   /* 4..16, per RP2040 SPI block */
} Mcal_SpiChannelCfgType;

/* Logical channel indices into Mcal_SpiChannelCfg[].
 * Add one entry here (and in mcal_spi_cfg.c) per physical device wired
 * to the board -- multiple logical channels may share a hardware block
 * as long as they use different cs_pin values. */
typedef enum {
    MCAL_SPI_CH_0 = 0,   /* Default: SPI0, GP2/GP3/GP4/GP5, 1 MHz, mode 0, 8-bit */
    MCAL_SPI_CH_COUNT
} Mcal_SpiChannelIdType;

extern const Mcal_SpiChannelCfgType Mcal_SpiChannelCfg[MCAL_SPI_CH_COUNT];

#endif /* MCAL_SPI_CFG_H */