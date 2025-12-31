#ifndef MCAL_UART_H
#define MCAL_UART_H

#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "mcal_uart_cfg.h"

/**
 * @brief Initialize all configured UART peripherals.
 */
void uart_init_all(void);

/**
 * @brief Print a string to a specified UART.
 * 
 * @param id The ID of the UART peripheral to use.
 * @param str The string to print.
 */
void uart_trace(mcal_uart_id_t id, const char *str);

/**
 * @brief Print a string to UART0.
 * @param str The string to print.
 */
void uart0_trace(const char *str);

/**
 * @brief Print a string to UART1.
 * @param str The string to print.
 */
void uart1_trace(const char *str);

/**
 * @brief Print a formatted string to UART0.
 * @param format The format string.
 * @param ... The arguments for the format string.
 */
void uart0_tracef(const char *format, ...);

/**
 * @brief Print a formatted string to UART1.
 * @param format The format string.
 * @param ... The arguments for the format string.
 */
void uart1_tracef(const char *format, ...);

#endif // MCAL_UART_H
