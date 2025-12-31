#include "mcal_uart_cfg.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"

// Define the configuration array for all UARTs
const mcal_uart_cfg_t g_uart_cfgs[MCAL_UART_ID_MAX] = {
    // Configuration for UART0
    [MCAL_UART_ID_0] = {
        .uart_inst = uart0,
        .baud_rate = 115200,
        .tx_pin = 0,
        .rx_pin = 1
    },
    // Configuration for UART1
    [MCAL_UART_ID_1] = {
        .uart_inst = uart1,
        .baud_rate = 9600,
        .tx_pin = 4,
        .rx_pin = 5
    }
};
