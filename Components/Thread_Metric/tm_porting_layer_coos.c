/**************************************************************************/
/*                                                                        */
/*            Copyright (c) 1996-2007 by Express Logic Inc.               */
/*                                                                        */
/*  This Original Work may be modified, distributed, or otherwise used in */
/*  any manner with no obligations other than the following:              */
/*                                                                        */
/*    1. This legend must be retained in its entirety in any source code  */
/*       copies of this Work.                                             */
/*                                                                        */
/*    2. This software may not be used in the development of an operating */
/*	 system product.                                                  */
/*                                                                        */
/*  This Original Work is hereby provided on an "AS IS" BASIS and WITHOUT */
/*  WARRANTY, either express or implied, including, without limitation,   */
/*  the warranties of NON-INFRINGEMENT, MERCHANTABILITY or FITNESS FOR A  */
/*  PARTICULAR PURPOSE. THE ENTIRE RISK AS TO THE QUALITY OF this         */
/*  ORIGINAL WORK IS WITH the user.                                       */
/*                                                                        */
/*  Express Logic, Inc. reserves the right to modify this software        */
/*  without notice.                                                       */
/*                                                                        */
/*  Express Logic, Inc.                     info@expresslogic.com         */
/*  11423 West Bernardo Court               http://www.expresslogic.com   */
/*  San Diego, CA  92127                                                  */
/*                                                                        */
/**************************************************************************/

/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** Thread-Metric Component                                              */
/**                                                                       */
/**   Porting Layer (Must be completed with RTOS specifics)               */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/* Include necessary files.  */

#include "tm_api.h"
#include <CoOS.h>
#include <AppConfig.h>

#include <mk_uart.h>

#include <stddef.h>
#include <string.h>

/* Define ThreadX mapping constants.  */

#define TM_THREADX_MAX_THREADS          10
#define TM_THREADX_MAX_QUEUES           1
#define TM_THREADX_MAX_SEMAPHORES       1
#define TM_THREADX_MAX_MEMORY_POOLS     1


/* Define the default ThreadX stack size.  */

#define TM_THREADX_THREAD_STACK_SIZE    (2096 / sizeof(OS_STK))


/* Define the default ThreadX queue size.  */

#define TM_THREADX_QUEUE_SIZE           200
#define QUE_MSG_CPY                     1

/* Define the default ThreadX memory pool size.  */

#define TM_THREADX_MEMORY_POOL_SIZE     2048


/* Define the number of timer interrupt ticks per second.  */

#define TM_THREADX_TICKS_PER_SECOND     CFG_SYSTICK_FREQ


/* Define ThreadX data structures.  */

OS_TID tm_thread_array[TM_THREADX_MAX_THREADS];
OS_EventID tm_queue_array[TM_THREADX_MAX_QUEUES];
OS_EventID tm_semaphore_array[TM_THREADX_MAX_SEMAPHORES];
OS_MMID tm_block_pool_array[TM_THREADX_MAX_MEMORY_POOLS];
OS_MMID tm_queue_mem_pool;

/* Define ThreadX object data areas.  */

OS_STK tm_thread_stack_area[TM_THREADX_MAX_THREADS*TM_THREADX_THREAD_STACK_SIZE];
void *tm_queue_memory_area[TM_THREADX_MAX_QUEUES * TM_THREADX_QUEUE_SIZE / 16];
unsigned char tm_pool_memory_area[TM_THREADX_MAX_MEMORY_POOLS * TM_THREADX_MEMORY_POOL_SIZE];

unsigned char tm_queue_mem_pool_area[TM_THREADX_MAX_QUEUES * TM_THREADX_QUEUE_SIZE];

/* Define array to remember the test entry function.  */
void *tm_thread_entry_functions[TM_THREADX_MAX_THREADS];

/* Remember the test initialization function.  */
void (*tm_initialization_function)(void);


/* This is the ThreadX thread entry.  It is going to call the Thread-Metric
   entry function saved earlier.  */
void tm_thread_entry(void *pdata)
{
    void (*entry_function)(void);

    /* Pickup the entry function from the saved array.  */
    entry_function =  (void (*)(void)) tm_thread_entry_functions[(int)pdata];

    /* Call the entry function.   */
    (entry_function)();
}


static void init(void) {
    uart_cfg_t cfg = {BusClock, 115200};

    // uart2
    PORTE_PCR16 = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;    // PTE16 = tx function, drive strength
    PORTE_PCR17 = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;    // PTE17 = rx function, drive strength
    SIM_SCGC4 |= SIM_SCGC4_UART2_MASK;  // enable clock

    uart_init(UART2_BASE_PTR, &cfg);
}


/* This function called from main performs basic RTOS initialization,
   calls the test initialization function, and then starts the RTOS function.  */
void tm_initialize(void (*test_initialization_function)(void))
{
    /* Save the test initialization function.  */
    tm_initialization_function =  test_initialization_function;

    init();

    CoInitOS();
#if QUE_MSG_CPY
    tm_queue_mem_pool = CoCreateMemPartition(&tm_queue_mem_pool_area[0], 16, TM_THREADX_MAX_QUEUES * (TM_THREADX_QUEUE_SIZE / 16 + 1));
#endif

    (tm_initialization_function)();
    CoStartOS();
}


/* This function takes a thread ID and priority and attempts to create the
   file in the underlying RTOS.  Valid priorities range from 1 through 31,
   where 1 is the highest priority and 31 is the lowest. If successful,
   the function should return TM_SUCCESS. Otherwise, TM_ERROR should be returned.   */
int  tm_thread_create(int thread_id, int priority, void (*entry_function)(void))
{
    /* Remember the actual thread entry.  */
    tm_thread_entry_functions[thread_id] =  (void*)entry_function;

//    tm_thread_array[thread_id] = CoCreateTask(tm_thread_entry, (void*)thread_id, priority, &tm_thread_stack_area[(thread_id + 1)* TM_THREADX_THREAD_STACK_SIZE - 1], TM_THREADX_THREAD_STACK_SIZE);
    tm_thread_array[thread_id] = CoCreateTaskEx(tm_thread_entry, (void*)thread_id, priority, &tm_thread_stack_area[(thread_id + 1)* TM_THREADX_THREAD_STACK_SIZE - 1], TM_THREADX_THREAD_STACK_SIZE, 0, 1);
    if (E_CREATE_FAIL != tm_thread_array[thread_id])
        return(TM_SUCCESS);
    else
        return(TM_ERROR);
}


/* This function resumes the specified thread.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_thread_resume(int thread_id)
{
    StatusType status;

    /* Attempt to resume the thread.  */
    status =  CoAwakeTask(tm_thread_array[thread_id]);

    /* Determine if the thread resume was successful.  */
    if (E_OK == status)
        return(TM_SUCCESS);
    else
        return(TM_ERROR);
}


/* This function suspends the specified thread.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_thread_suspend(int thread_id)
{
    StatusType status;

    /* Attempt to resume the thread.  */
    status =  CoSuspendTask(tm_thread_array[thread_id]);

    /* Determine if the thread resume was successful.  */
    if (E_OK == status)
        return(TM_SUCCESS);
    else
        return(TM_ERROR);
}


/* This function relinquishes to other ready threads at the same
   priority.  */
void tm_thread_relinquish(void)
{
    CoYield();
}


/* This function suspends the specified thread for the specified number
   of seconds.  If successful, the function should return TM_SUCCESS.
   Otherwise, TM_ERROR should be returned.  */
void tm_thread_sleep(int seconds) {
    CoTickDelay(seconds * CFG_SYSTICK_FREQ);
}


/* This function creates the specified queue.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int tm_queue_create(int queue_id) {
    /* Create the specified queue with 16-byte messages.  */
    tm_queue_array[queue_id] = CoCreateQueue(&tm_queue_memory_area[queue_id * TM_THREADX_QUEUE_SIZE / 16], TM_THREADX_QUEUE_SIZE / 16, EVENT_SORT_TYPE_FIFO);

    /* Determine if the queue create was successful.  */
    if (E_CREATE_FAIL != tm_queue_array[queue_id])
        return(TM_SUCCESS);
    else
        return(TM_ERROR);
}


/* This function sends a 16-byte message to the specified queue.  If successful,
   the function should return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_queue_send(int queue_id, unsigned long *message_ptr) {
    StatusType status;
#if QUE_MSG_CPY
    void *msg;

    /* Send the 16-byte message to the specified queue.  */
    msg = CoGetMemoryBuffer(tm_queue_mem_pool);
    memcpy(msg, message_ptr, 16);
    status = CoPostQueueMail(tm_queue_array[queue_id], msg);
#else
    status = CoPostQueueMail(tm_queue_array[queue_id], message_ptr);
#endif

    /* Determine if the queue send was successful.  */
    if (E_OK == status)
        return(TM_SUCCESS);
    else
        return(TM_ERROR);
}


/* This function receives a 16-byte message from the specified queue.  If successful,
   the function should return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_queue_receive(int queue_id, unsigned long *message_ptr) {
    StatusType status;
#if QUE_MSG_CPY
    void *msg;
#endif

    /* Receive a 16-byte message from the specified queue.  */
#if QUE_MSG_CPY
    msg = CoAcceptQueueMail(tm_queue_array[queue_id], &status);
#else
    message_ptr = CoAcceptQueueMail(tm_queue_array[queue_id], &status);
#endif

    /* Determine if the queue receive was successful.  */
    if (E_OK == status) {
#if QUE_MSG_CPY
        memcpy(message_ptr, msg, 16);
        CoFreeMemoryBuffer(tm_queue_mem_pool, msg);
#endif
        return(TM_SUCCESS);
    }
    else
        return(TM_ERROR);
}


/* This function creates the specified semaphore.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_semaphore_create(int semaphore_id) {
    /*  Create semaphore.  */
    tm_semaphore_array[semaphore_id] = CoCreateSem(1, 10, EVENT_SORT_TYPE_FIFO);

    /* Determine if the semaphore create was successful.  */
    if (E_CREATE_FAIL != tm_semaphore_array[semaphore_id])
        return(TM_SUCCESS);
    else
        return(TM_ERROR);
}


/* This function gets the specified semaphore.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int tm_semaphore_get(int semaphore_id) {
    StatusType status;

    /*  Get the semaphore.  */
    status = CoAcceptSem(tm_semaphore_array[semaphore_id]);

    /* Determine if the semaphore get was successful.  */
    if (E_OK == status)
        return(TM_SUCCESS);
    else
        return(TM_ERROR);
}


/* This function puts the specified semaphore.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int tm_semaphore_put(int semaphore_id) {
    StatusType status;

    /*  Put the semaphore.  */
    status = CoPostSem(tm_semaphore_array[semaphore_id]);

    /* Determine if the semaphore put was successful.  */
    if (E_OK == status)
        return(TM_SUCCESS);
    else
        return(TM_ERROR);
}


/* This function creates the specified memory pool that can support one or more
   allocations of 128 bytes.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_memory_pool_create(int pool_id) {
    /*  Create the memory pool.  */
    tm_block_pool_array[pool_id] = CoCreateMemPartition(&tm_pool_memory_area[pool_id*TM_THREADX_MEMORY_POOL_SIZE], 128, TM_THREADX_MEMORY_POOL_SIZE / 128);

    /* Determine if the block pool memory was successful.  */
    if (E_CREATE_FAIL != tm_block_pool_array[pool_id])
        return(TM_SUCCESS);
    else
        return(TM_ERROR);
}


/* This function allocates a 128 byte block from the specified memory pool.
   If successful, the function should return TM_SUCCESS. Otherwise, TM_ERROR
   should be returned.  */
int  tm_memory_pool_allocate(int pool_id, unsigned char **memory_ptr) {
    /*  Allocate a 128-byte block from the specified memory pool.  */
    *memory_ptr = CoGetMemoryBuffer(tm_block_pool_array[pool_id]);

    /* Determine if the block pool allocate was successful.  */
    if (NULL != *memory_ptr)
        return(TM_SUCCESS);
    else
        return(TM_ERROR);
}


/* This function releases a previously allocated 128 byte block from the specified
   memory pool. If successful, the function should return TM_SUCCESS. Otherwise, TM_ERROR
   should be returned.  */
int  tm_memory_pool_deallocate(int pool_id, unsigned char *memory_ptr) {
    StatusType status;

    /*  Release the 128-byte block back to the specified memory pool.  */
    status = CoFreeMemoryBuffer(tm_block_pool_array[pool_id], memory_ptr);

    /* Determine if the block pool release was successful.  */
    if (E_OK == status)
        return(TM_SUCCESS);
    else
        return(TM_ERROR);
}


