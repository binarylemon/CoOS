/**
 * @file mk_uart.c
 * @brief UART driver for Freescale Kinetis family
 * @date 18. Aug 2013
 *
 * @author Jozef Maslik (maslo@binarylemon.com)
 */

#include "mk_uart.h"
#include <coos.h>

#define UART_QUE_SIZE   0

typedef struct {
    UART_TypeDef *base;
#if 0 == UART_QUE_SIZE
    char *rx;
    const char *tx;
    int cnt_rx;
    int cnt_tx;
    OS_EventID sem_rx;
    OS_EventID sem_tx;
#else
    OS_EventID rx_que;
    OS_EventID tx_que;
    void *rx[UART_QUE_SIZE];
    void *tx[UART_QUE_SIZE];
#endif
} uart_t;

uart_t uart_data[5];

#if 0 == UART_QUE_SIZE
static void uart_rxtx_irq(uart_t *uart) {
    uint32_t status, cnt;

    CoEnterISR();
    status = uart->base->S1;

    // rx
    if (uart->rx && status & UART_S1_RDRF_MASK) {
        cnt = uart->base->RCFIFO;
        while (uart->cnt_rx && cnt--) {
            *uart->rx = uart->base->D;
            uart->rx++;
            uart->cnt_rx--;
        }

        if (0 == uart->cnt_rx) {
            uart->rx = 0;

            // data received, disable receiver interrupt
            uart->base->C2 &= ~UART_C2_RIE_MASK;
            isr_PostSem(uart->sem_rx);
        }
    }

    // tx
    if (uart->tx && status & UART_S1_TDRE_MASK) {
        cnt = (1 << (uart->base->PFIFO & UART_PFIFO_TXFIFOSIZE_MASK)) - uart->base->TCFIFO;
        while (uart->cnt_tx && cnt--) {
            uart->base->D = *uart->tx;
            uart->tx++;
            uart->cnt_tx--;
        }

        if (0 == uart->cnt_tx) {
            uart->tx = 0;
            // all data sent, disable transmit interrupt
            uart->base->C2 &= ~UART_C2_TIE_MASK;
            isr_PostSem(uart->sem_tx);
        }
    }

    CoExitISR();
}
#else
static void uart_rxtx_irq(uart_t *uart) {
    uint32_t status, cnt;
    char ch;
    StatusType res;

    CoEnterISR();
    status = uart->base->S1;

    // rx
    if (status & UART_S1_RDRF_MASK) {
        cnt = uart->base->RCFIFO;
        while (cnt--) {
            isr_PostQueueMail(uart->rx_que, (void*)(uart->base->D));
        }
    }

    // tx
    if (status & UART_S1_TDRE_MASK && uart->base->C2 & UART_C2_TIE_MASK) {
        cnt = (1 << (uart->base->PFIFO & UART_PFIFO_TXFIFOSIZE_MASK)) - uart->base->TCFIFO;
        while (cnt--) {
            ch = (uint32_t)CoAcceptQueueMailEx(uart->tx_que, &res);
            if (E_OK == res) {
                uart->base->D = ch;
            }
            else {
                // disable TDRE interrupt
                uart->base->C2 &= ~UART_C2_TIE_MASK;
                break;
            }
        }
    }

    CoExitISR();
}
#endif

#define UART_IRQ(n)     void UART##n##_RX_TX_IRQHandler(void) { uart_rxtx_irq(&uart_data[n]); }

UART_IRQ(0);
UART_IRQ(1);
UART_IRQ(2);
UART_IRQ(3);
UART_IRQ(4);
UART_IRQ(5);


static int uart_get_num(UART_TypeDef *uart) {
    if (UART0_BASE_PTR == uart) {
        return 0;
    }
    else if (UART1_BASE_PTR == uart) {
        return 1;
    }
    else if (UART2_BASE_PTR == uart) {
        return 2;
    }
    else if (UART3_BASE_PTR == uart) {
        return 3;
    }

    return 0;
}

static int uart_get_irq(UART_TypeDef *uart) {
    if (UART0_BASE_PTR == uart) {
        return UART0_RX_TX_IRQn;
    }
    else if (UART1_BASE_PTR == uart) {
        return UART1_RX_TX_IRQn;
    }
    else if (UART2_BASE_PTR == uart) {
        return UART2_RX_TX_IRQn;
    }
    else if (UART3_BASE_PTR == uart) {
        return UART3_RX_TX_IRQn;
    }

    return 0;
}

int uart_baud(UART_TypeDef *uart, uint32_t clk, uint32_t baud) {
    uint32_t sbr = 0, brfa = 0;
    int res = -1;
    ASSERT(uart);

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
    int num;
    ASSERT(uart);
    ASSERT(cfg);

    // init data struct
    num = uart_get_num(uart);

    uart_data[num].base = uart;

#if 0 == UART_QUE_SIZE
    uart_data[num].rx = 0;
    uart_data[num].tx = 0;
    uart_data[num].cnt_rx = 0;
    uart_data[num].cnt_tx = 0;
    uart_data[num].sem_rx = CoCreateSem(0, 1, EVENT_SORT_TYPE_FIFO);
    uart_data[num].sem_tx = CoCreateSem(0, 1, EVENT_SORT_TYPE_FIFO);
#else
    uart_data[num].rx_que = CoCreateQueueEx(uart_data[num].rx, UART_QUE_SIZE, EVENT_SORT_TYPE_FIFO);
    uart_data[num].tx_que = CoCreateQueueEx(uart_data[num].tx, UART_QUE_SIZE, EVENT_SORT_TYPE_FIFO);
#endif

    // interrupt vectors
    NVIC_SetPriority(uart_get_irq(uart), 6);
    NVIC_EnableIRQ(uart_get_irq(uart));

    // clock, pins

    // baudrate
    uart_baud(uart, cfg->clk, cfg->baud);

    // 8b mode
    uart->C1 = 0;

    // FIFO - set watermark to ~ 3/4 fifo size
    if (1 < (1 << (uart->PFIFO & UART_PFIFO_TXFIFOSIZE_MASK >> UART_PFIFO_TXFIFOSIZE_SHIFT)) &&
        1 < (1 << (uart->PFIFO & UART_PFIFO_RXFIFOSIZE_MASK >> UART_PFIFO_RXFIFOSIZE_SHIFT))) {

        uart->TWFIFO = UART_TWFIFO_TXWATER(3 * (1 << (uart->PFIFO & UART_PFIFO_TXFIFOSIZE_MASK >> UART_PFIFO_TXFIFOSIZE_SHIFT)) / 4);
        uart->RWFIFO = UART_RWFIFO_RXWATER(3 * (1 << (uart->PFIFO & UART_PFIFO_RXFIFOSIZE_MASK >> UART_PFIFO_RXFIFOSIZE_SHIFT)) / 4);

        uart->PFIFO = UART_PFIFO_RXFE_MASK | UART_PFIFO_TXFE_MASK;  // enable fifo

        uart->CFIFO = UART_CFIFO_RXFLUSH_MASK | UART_CFIFO_TXFLUSH_MASK;    // flush fifo
    }

    // disable all error interrupts
    uart->C3 = 0;

    uart->C2 &= ~(UART_C2_RIE_MASK | UART_C2_TIE_MASK);   // disable rx, tx interrupt
    uart->C2 |= UART_C2_RE_MASK | UART_C2_TE_MASK;        // enable transmiter and receiver

    return 0;
}

#if 0 == UART_QUE_SIZE

char uart_getc(UART_TypeDef *uart) {
    ASSERT(uart);

    // wait until receive char
    while (!(uart->S1 & UART_S1_RDRF_MASK));

    return uart->D;
}

void uart_putc(UART_TypeDef *uart, char ch) {
    ASSERT(uart);

    // wait until free register
    while (!(uart->S1 & UART_S1_TDRE_MASK));

    uart->D = ch;
}

#else
char uart_getc(UART_TypeDef *uart) {
    StatusType res;
    char ch;

    ASSERT(uart);

    ch = (uint32_t)CoPendQueueMailEx(uart_data[uart_get_num(uart)].rx_que, 0, &res);

    return (E_OK == res) ? ch : 0;
}

void uart_putc(UART_TypeDef *uart, char ch) {
    StatusType res;
    ASSERT(uart);

    res = CoPostQueueMailEx(uart_data[uart_get_num(uart)].tx_que, (void*)ch, 0);
    if (E_OK == res) {
        uart->C2 |= UART_C2_TIE_MASK;
    }
}

#endif

#if 0 == UART_QUE_SIZE
int uart_send(UART_TypeDef *uart, const char *data, const int len) {
    int num;
    ASSERT(uart);
    ASSERT(data);

    num = uart_get_num(uart);
    uart_data[num].tx = data;
    uart_data[num].cnt_tx = len;

    // enable tx empty interrupt
    uart->C2 |= UART_C2_TIE_MASK;

    CoPendSem(uart_data[num].sem_tx, 0);

    uart_data[num].tx = 0;
    return 0;
}

int uart_receive(UART_TypeDef *uart, char *data, const int len) {
    int num;
    ASSERT(uart);
    ASSERT(data);
    // TODO implement me

    num = uart_get_num(uart);
    uart_data[num].rx = data;
    uart_data[num].cnt_rx = len;

    // enable rx full interrupt
    uart->C2 |= UART_C2_RIE_MASK;

    // wait until buffer will be received or other break event
    CoPendSem(uart_data[num].sem_rx, 0);

    uart_data->rx = 0;

    return len - uart_data->cnt_rx;
}
#else
int uart_receive(UART_TypeDef *uart, char *data, const int len) {
    int i = len;
    char *p = data;
    ASSERT(uart);
    ASSERT(data);

    while (i--) {
        *p = uart_getc(uart);
        p++;
    }

    return len - i;
}

int uart_send(UART_TypeDef *uart, const char *data, const int len) {
    int i = len;
    const char *p = data;
    ASSERT(uart);
    ASSERT(data);

    while (i--) {
        uart_putc(uart, *p);
        p++;
    }

    return 0;
}

#endif
