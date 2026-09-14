#ifndef MCAL_SPI_H
#define MCAL_SPI_H

/*==============================================================================
 * Module : MCAL - SPI (Driver)
 * Scope  : RP2040, SPI0/SPI1 hardware blocks, software-GPIO CS
 *============================================================================*/

#include "mcal_spi_cfg.h"
#include "Std_Types.h"  /* Std_ReturnType, E_OK/E_NOT_OK -- point this at the
                          * repo's actual common header if the path differs */

typedef enum {
    MCAL_SPI_UNINIT = 0,
    MCAL_SPI_IDLE,
    MCAL_SPI_BUSY
} Mcal_Spi_StatusType;

/* Bring up every configured channel: mux SCK/TX/RX to hardware, apply
 * baud/mode, and init CS as a software GPIO output, idle high. */
void Mcal_Spi_Init(void);

/* Tear down every channel: release pin muxing, mark UNINIT. */
void Mcal_Spi_DeInit(void);

Mcal_Spi_StatusType Mcal_Spi_GetStatus(Mcal_SpiChannelIdType channel);

/* Runtime baud change on an already-initialized channel. */
Std_ReturnType Mcal_Spi_SetBaudrate(Mcal_SpiChannelIdType channel, uint32 baud_hz);

/* Manual CS control, for callers that need to hold CS across several
 * back-to-back transfers (e.g. write register address, then read data,
 * without releasing the device in between). */
void Mcal_Spi_CsAssert(Mcal_SpiChannelIdType channel);
void Mcal_Spi_CsDeassert(Mcal_SpiChannelIdType channel);

/* Blocking full-duplex transfer; CS is asserted/deasserted internally
 * for the duration of this single call.
 * tx_buf == NULL clocks out 0xFF filler bytes; rx_buf == NULL discards
 * received bytes. */
Std_ReturnType Mcal_Spi_Transfer(Mcal_SpiChannelIdType channel,
                                  const uint8 *tx_buf,
                                  uint8 *rx_buf,
                                  size len);

/* Same as Mcal_Spi_Transfer(), but never touches CS -- caller must
 * bracket the sequence with Mcal_Spi_CsAssert() / Mcal_Spi_CsDeassert(). */
Std_ReturnType Mcal_Spi_TransferNoCs(Mcal_SpiChannelIdType channel,
                                      const uint8 *tx_buf,
                                      uint8 *rx_buf,
                                      size len);

#endif /* MCAL_SPI_H */