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
#include <test.h>


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

    CoInitOS();

    mainTestSetup();

    CoStartOS();

    return 0;
}
