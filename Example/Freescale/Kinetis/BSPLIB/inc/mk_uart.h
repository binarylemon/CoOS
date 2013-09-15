/**
 * @file mk_uart.h
 * @brief UART driver for Freescale Kinetis family
 * @date 18. Aug 2013
 *
 * @author Jozef Maslik (maslo@binarylemon.com)
 */

#include <stdint.h>
#include <AppConfig.h>

#ifndef ASSERT
#define ASSERT(param)
#endif

/** UART init structure
 */
typedef struct {
    uint32_t clk;
    uint32_t baud;
} uart_cfg_t;

int uart_init(UART_TypeDef *uart, uart_cfg_t *cfg);
char uart_getc(UART_TypeDef *uart);
void uart_putc(UART_TypeDef *uart, char ch);
int uart_send(UART_TypeDef *uart, const char *data, const int len);
int uart_receive(UART_TypeDef *uart, char *data, const int len);
int uart_baud(UART_TypeDef *uart, uint32_t clk, uint32_t baud);
