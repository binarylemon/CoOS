/**
 * @file isr.h
 * @date 2. Sep 2013
 * @brief Define interrupt service routines referenced by the vector table.
 *
 * @note
 * Only "vectors.c" should include this header file.
 *
 */

#ifndef __ISR_H
#define __ISR_H

void PendSV_Handler(void);
void SysTick_Handler(void);
void tm_interrupt_preemption_handler(void);
void  tm_interrupt_handler(void);

#undef VECTOR_014
#define VECTOR_014  PendSV_Handler

#undef VECTOR_015
#define VECTOR_015  SysTick_Handler

#if 4 == TM_BENCH
#undef VECTOR_016
#define VECTOR_016  tm_interrupt_handler
#elif 5 == TM_BENCH
#undef VECTOR_016
#define VECTOR_016  tm_interrupt_preemption_handler
#endif

#endif  //__ISR_H
