#ifndef MCAL_UART_CFG_H
#define MCAL_UART_CFG_H

#include "hardware/uart.h"

/*
 * =================================================================================
 *   UART Configuration Table
 * =================================================================================
 *
 * This configuration uses an array of structures to define settings for each
 * UART peripheral used in the system.
 *
 * +-------------+----------------------------------+------------------------------+
 * | Parameter   | Description                      | Values                       |
 * +-------------+----------------------------------+------------------------------+
 * | uart_inst   | The hardware UART instance.      | uart0, uart1                 |
 * +-------------+----------------------------------+------------------------------+
 * | baud_rate   | The communication speed.         | 9600, 115200, etc.           |
 * +-------------+----------------------------------+------------------------------+
 * | tx_pin      | The GPIO for transmitting data.  | Any valid GPIO pin number    |
 * +-------------+----------------------------------+------------------------------+
 * | rx_pin      | The GPIO for receiving data.     | Any valid GPIO pin number    |
 * +-------------+----------------------------------+------------------------------+
 *
 */

// Enum to identify the configured UART peripherals
typedef enum {
    MCAL_UART_ID_0,
    MCAL_UART_ID_1,
    MCAL_UART_ID_MAX
} mcal_uart_id_t;

// Structure to hold configuration for a single UART peripheral
typedef struct {
    uart_inst_t *uart_inst;
    uint32_t baud_rate;
    uint8_t tx_pin;
    uint8_t rx_pin;
} mcal_uart_cfg_t;

// Declare the configuration array for all UARTs
extern const mcal_uart_cfg_t g_uart_cfgs[MCAL_UART_ID_MAX];

#endif // MCAL_UART_CFG_H
