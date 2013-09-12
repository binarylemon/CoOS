/**************************************************************************//**
 * @file     system_MK60F.c
 * @brief    CMSIS Cortex-M4F Device Peripheral Access Layer Source File for
 *           Device MK60Fxx
 * @version
 *
 * @note
 * Copyright (C) 2010-2012 ARM Limited. All rights reserved.
 *
 * @par
 * ARM Limited (ARM) is supplying this software for use with Cortex-M 
 * processor based microcontrollers.  This file can be freely distributed 
 * within development tools that are supporting such ARM based processors. 
 *
 * @par
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/


#include <stdint.h>
#include "MK60F.h"


/*----------------------------------------------------------------------------
  DEFINES
 *----------------------------------------------------------------------------*/
    
/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
/* ToDo: add here your necessary defines for device initialization     
         following is an example for different system frequencies             */
#define __HSI             ( 6000000UL)
#define __XTAL            (12000000UL)    /* Oscillator frequency             */
#define __SYS_OSC_CLK     (50000000UL)    /* Main oscillator frequency        */

/* 120MHz core clock and platform settings */
#define __SYSTEM_CLOCK          (120000000UL)

#define PLL_REF_CLOCK_DIV       5   // PRDIV
#define PLL_CLOCK_MUL           24  // VDIV

#define CORE_DIV                1   // 120MHz
#define BUS_DIV                 2   // 60MHz
#define FLEXBUS_DIV             3   // 40MHz
#define FLASH_DIV               6   // 20MHz


/* RANGE0 frequency mode set */
#if 40000 >= __SYS_OSC_CLK
#define RANGE0_VAL      0
#elif 8000000 >= __SYS_OSC_CLK
#define RANGE0_VAL      1
#else
//#define RANGE0_VAL      1       // for silicon rev. 1.0 (__SYS_OSC_CLK >= 8000000)
#define RANGE0_VAL      2
#endif

/* FRDIV value set */
#if 1250000 >= __SYS_OSC_CLK
#define FRDIV_VAL       0
#elif 2500000 >= __SYS_OSC_CLK
#define FRDIV_VAL       1
#elif 5000000 >= __SYS_OSC_CLK
#define FRDIV_VAL       2
#elif 10000000 >= __SYS_OSC_CLK
#define FRDIV_VAL       3
#elif 20000000 >= __SYS_OSC_CLK
#define FRDIV_VAL       4
#else
#define FRDIV_VAL       5
#endif

#if 1 > PLL_REF_CLOCK_DIV || 8 < PLL_REF_CLOCK_DIV
#error
#endif

#if 16 > PLL_CLOCK_MUL || 47 < PLL_CLOCK_MUL
#error
#endif


/** Disable watchdog.
 */
static void wdt_disable(void) {
    // unlock watchdog
    WDOG_UNLOCK = 0xc520;
    WDOG_UNLOCK = 0xd928;

    // disable watchdog
    WDOG_STCTRLH &= ~(WDOG_STCTRLH_WDOGEN_MASK);
}

/** Clock to peripherals enable.
 */
static void clock_enable(void) {
    // GPIO clock enable
    SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK | SIM_SCGC5_PORTF_MASK);

    // ENET clock enable
    SIM_SCGC2 |= SIM_SCGC2_ENET_MASK;

    SIM_SCGC1 = 0xFFFFFFFF;
    SIM_SCGC2 = 0xFFFFFFFF;
    SIM_SCGC3 = 0xFFFFFFFF;
    SIM_SCGC4 = 0xFFFFFFFF;
    SIM_SCGC5 = 0xFFFFFFFF;
    SIM_SCGC6 = 0xFFFFFFFF;
    SIM_SCGC7 = 0xFFFFFFFF;
}

static void trace_clk_init(void) {
    // trace clock to the core clock frequency
    SIM_SOPT2 |= SIM_SOPT2_TRACECLKSEL_MASK;

    // enable the TRACE_CLKOUT pin function on PTA6 (alt7 function)
    PORTA_PCR6 = PORT_PCR_MUX(0x7);
}


/*----------------------------------------------------------------------------
  Clock Variable definitions
 *----------------------------------------------------------------------------*/
/* ToDo: initialize SystemCoreClock with the system core clock frequency value
         achieved after system intitialization.
         This means system core clock frequency after call to SystemInit()    */
uint32_t SystemCoreClock = __SYSTEM_CLOCK;  /*!< System Clock Frequency (Core Clock)*/
uint32_t BusClock = __SYSTEM_CLOCK / BUS_DIV;


/*----------------------------------------------------------------------------
  Clock functions
 *----------------------------------------------------------------------------*/
void SystemCoreClockUpdate(void)            /* Get Core Clock Frequency      */
{
    uint32_t mcgclk, prdiv, vdiv;

    prdiv = ((MCG_C5 & MCG_C5_PRDIV0_MASK) >> MCG_C5_PRDIV0_SHIFT) + 1;
    vdiv = ((MCG_C6 & MCG_C6_VDIV0_MASK) >> MCG_C6_VDIV0_SHIFT) + 16;

    mcgclk = ((__SYS_OSC_CLK / prdiv) * vdiv) / 2;
    SystemCoreClock = mcgclk / CORE_DIV;
    BusClock = mcgclk / BUS_DIV;
}

/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System.
 */
void SystemInit(void) {
    wdt_disable();
    clock_enable();
    trace_clk_init();

    // system dividers
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(CORE_DIV - 1) | SIM_CLKDIV1_OUTDIV2(BUS_DIV - 1) | SIM_CLKDIV1_OUTDIV3(FLEXBUS_DIV - 1) | SIM_CLKDIV1_OUTDIV4(FLASH_DIV - 1);
    
    // after reset, we are in FEI mode
    
    // enable external clock source - OSC0
    MCG_C2 = MCG_C2_LOCRE0_MASK | MCG_C2_RANGE0(RANGE0_VAL) | (0 << MCG_C2_HGO0_SHIFT) | (0 << MCG_C2_EREFS0_SHIFT);
    
    // select clock mode, we want FBE mode
    // CLKS = 2, FRDIV = FRDIV_VAL, IREFS = 0, IRCLKEN = 0, IREFSTEN = 0
    MCG_C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(FRDIV_VAL);
    
    /* wait until the MCG has moved into the proper mode */
    // if the external oscillator is used need to wait for OSCINIT to set
//    while (!(MCG_S & MCG_S_OSCINIT_MASK));
    
    // wait for reference clock status bit is cleared and clock source is ext ref clk
    while ((MCG_S & MCG_S_IREFST_MASK) || MCG_S_CLKST(2) != (MCG_S & MCG_S_CLKST_MASK));

    // ... FBE mode
    
    // enable clock monitor for osc0
    MCG_C6 = MCG_C6_CME0_MASK;

    // PLL0
    MCG_C5 = MCG_C5_PRDIV0(PLL_REF_CLOCK_DIV - 1);       // set PLL0 ref divider, osc0 is reference

    MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV0(PLL_CLOCK_MUL - 16);     // set VDIV and enable PLL
        
    // wait to lock...
    while (!(MCG_S & MCG_S_PLLST_MASK));
    while (!(MCG_S & MCG_S_LOCK0_MASK));
    
    // ... PBE mode
    MCG_C1 &= ~MCG_C1_CLKS_MASK;        // CLKS = 0, select PLL as MCG_OUT
    
    while (MCG_S_CLKST(3) != (MCG_S & MCG_S_CLKST_MASK));

    // ... PEE mode
    
    SystemCoreClockUpdate();
}
