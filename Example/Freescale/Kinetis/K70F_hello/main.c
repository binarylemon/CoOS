/**
 * @file main.c
 * @date 25 Jul 2013
 * @brief Very simple demo which blink with LED on TWR-K60N512 board
 *
 * @author Jozef Maslik (maslo@binarylemon.com)
 *
 */

#include <CoOS.h>
#include <AppConfig.h>

#include <mk_uart.h>

#include <stdio.h>

#define TASK_LED_STACK_SIZE     128
#define TASK_LED_PRI            3

OS_STK task_led_stack[TASK_LED_STACK_SIZE];

static void task_led(void *pdata) {
    (void)pdata;

    printf("Hello World :)\r\n");

    for (;;) {
        printf(".");
        CoTickDelay(10);    // 100ms delay
    }
}

static void init(void) {
    uart_cfg_t cfg = {BusClock, 115200};

    // uart2
    PORTE_PCR16 = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;    // PTE16 = tx function, drive strength
    PORTE_PCR17 = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;    // PTE17 = rx function, drive strength
    SIM_SCGC4 |= SIM_SCGC4_UART2_MASK;  // enable clock

    uart_init(UART2_BASE_PTR, &cfg);
}

int main() {
    init();

#ifndef __ICCARM__
    /* disable stdout buffering - when buffering is enabled, each printf sends data after \n, but we want send it immediately */
    setbuf(stdout, NULL);   // disable stdout buffering
    setbuf(stdin, NULL);    // disable stdin buffering
#endif

    CoInitOS();

    CoCreateTask(task_led, (void *)0, TASK_LED_PRI, &task_led_stack[TASK_LED_STACK_SIZE - 1], TASK_LED_STACK_SIZE);

    CoStartOS();

    return 0;
}
