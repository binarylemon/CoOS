/**
 *******************************************************************************
 * @file       OsQueue.h
 * @version   V1.1.4    
 * @date      2011.04.20
 * @brief      Queue management header file	
 * @details    This file including some defines and declares about queue management.
 *
 * @author CooCox
 * @author Jozef Maslik (maslo@binarylemon.com)
 *
 *******************************************************************************
 * @copy
 *
 * INTERNAL FILE,DON'T PUBLIC.
 * 
 * <h2><center>&copy; COPYRIGHT 2009 CooCox </center></h2>
 *******************************************************************************
 */ 


#ifndef _QUEUE_H
#define _QUEUE_H

#include <coocox.h>

/**
 * @struct   Queue  queue.h	
 * @brief    Queue struct
 * @details  This struct use to manage queue.
 *	
 */
typedef struct Queue
{
    void    **qStart;                   /*!<                                  */
    U8      id;                         /*!<                                  */
    U16     head;                       /*!< The header of queue              */
    U16     tail;                       /*!< The end of queue                 */
    U16     qMaxSize;                   /*!< The max size of queue            */
    U16     qSize;                      /*!< Current size of queue            */
    P_ECB   fb_ecb;                     /*!< feedback event for extended queue fns */
}QCB,*P_QCB;


#endif
