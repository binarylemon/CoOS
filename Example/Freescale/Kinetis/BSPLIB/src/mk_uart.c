/**
 * @file mk_uart.c
 * @brief UART driver for Freescale Kinetis family
 * @date 18. Aug 2013
 *
 * @author Jozef Maslik (maslo@binarylemon.com)
 */

#include "mk_uart.h"
#include <assert.h>


static int uart_baud(UART_TypeDef *uart, uint32_t clk, uint32_t baud) {
    uint32_t sbr = 0, brfa = 0;
    int res = -1;

    if ((clk > 0) && (baud > 0)) {
        sbr = clk / (baud << 4);
        if (sbr > 0) {
            brfa = ((((clk - baud * (sbr << 4)) << 1) + (baud >> 1)) / baud);
            if (brfa >= 32) {
                sbr += 1;
                brfa = 0;
            }
        }

        if (!(sbr > ((UART_BDH_SBR_MASK << 8) | UART_BDL_SBR_MASK))) {
            uart->BDH = UART_BDH_SBR(sbr);
            uart->BDL = UART_BDL_SBR(sbr);
            uart->C4 &= ~UART_C4_BRFA_MASK;
            uart->C4 |= UART_C4_BRFA(brfa);

            res = 0;
        }
    }

    return res;
}

int uart_init(UART_TypeDef *uart, uart_cfg_t *cfg) {
    assert(uart);

    // interrupt vectors

    // clock, pins

    // baudrate
    uart_baud(uart, cfg->clk, cfg->baud);

    // 8b mode
    uart->C1 = 0;

    // disable wakeups
    /*devc->uart->C2 &= (~ (UART_C2_RWU_MASK));
    devc->uart->C4 &= (~ (UART_C4_MAEN1_MASK | UART_C4_MAEN2_MASK));
    devc->uart->MA1 = 0;
    devc->uart->MA2 = 0;*/

    // disable all error interrupts
    uart->C3 = 0;

    uart->C2 &= ~(UART_C2_RIE_MASK | UART_C2_TIE_MASK);   // disable rx, tx interrupt
    uart->C2 |= UART_C2_RE_MASK | UART_C2_TE_MASK;        // enable transmiter and receiver

    return 0;
}

char uart_getchar(UART_TypeDef *uart) {
    assert(uart);

    // wait until receive char
    while (!(uart->S1 & UART_S1_RDRF_MASK));

    return uart->D;
}

void uart_putch(UART_TypeDef *uart, char ch) {
    assert(uart);

    // wait until free register
    while(!(uart->S1 & UART_S1_TDRE_MASK));

    uart->D = ch;
}

int uart_write(UART_TypeDef *uart, char *data, int len) {
    // TODO implement me
    return 0;
}

int uart_read(UART_TypeDef *uart, char *data, int len) {
    // TODO implement me
    return 0;
}
