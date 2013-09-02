/**
 * @file mk_uart.h
 * @brief UART driver for Freescale Kinetis family
 * @date 18. Aug 2013
 *
 * @author Jozef Maslik (maslo@binarylemon.com)
 */

#include <stdint.h>
#include <AppConfig.h>

/** UART init structure
 */
typedef struct {
    uint32_t clk;
    uint32_t baud;
} uart_cfg_t;

int uart_init(UART_TypeDef *uart, uart_cfg_t *cfg);
char uart_getchar(UART_TypeDef *uart);
void uart_putch(UART_TypeDef *uart, char ch);