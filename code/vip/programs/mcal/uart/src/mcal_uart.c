#include "mcal_uart.h"
#include "mcal_uart_cfg.h"
#include <stdio.h>
#include <stdarg.h>

#define UART_TRACE_BUFFER_SIZE 256

/**
 * @brief Initialize all configured UART peripherals.
 */
void uart_init_all(void) {
    for (mcal_uart_id_t id = 0; id < MCAL_UART_ID_MAX; id++) {
        const mcal_uart_cfg_t *cfg = &g_uart_cfgs[id];

        // Set up our UART with the required speed.
        uart_init(cfg->uart_inst, cfg->baud_rate);

        // Set the TX and RX pins by using the function select on the GPIO
        gpio_set_function(cfg->tx_pin, GPIO_FUNC_UART);
        gpio_set_function(cfg->rx_pin, GPIO_FUNC_UART);
    }
}

/**
 * @brief Print a string to a specified UART.
 * 
 * @param id The ID of the UART peripheral to use.
 * @param str The string to print.
 */
void uart_trace(mcal_uart_id_t id, const char *str) {
    if (id >= MCAL_UART_ID_MAX) {
        return; // Invalid ID
    }
    uart_puts(g_uart_cfgs[id].uart_inst, str);
}

/**
 * @brief Print a string to UART0.
 * @param str The string to print.
 */
void uart0_trace(const char *str) {
    uart_trace(MCAL_UART_ID_0, str);
}

/**
 * @brief Print a string to UART1.
 * @param str The string to print.
 */
void uart1_trace(const char *str) {
    uart_trace(MCAL_UART_ID_1, str);
}

/**
 * @brief Print a formatted string to UART0.
 * @param format The format string.
 * @param ... The arguments for the format string.
 */
void uart0_tracef(const char *format, ...) {
    char buffer[UART_TRACE_BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    uart_trace(MCAL_UART_ID_0, buffer);
}

/**
 * @brief Print a formatted string to UART1.
 * @param format The format string.
 * @param ... The arguments for the format string.
 */
void uart1_tracef(const char *format, ...) {
    char buffer[UART_TRACE_BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    uart_trace(MCAL_UART_ID_1, buffer);
}

