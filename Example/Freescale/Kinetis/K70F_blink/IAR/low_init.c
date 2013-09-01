/**
 * @file low_init.c
 * @date 18. Aug 2013
 * @brief This file provides low level initialization for K60N512
 *
 */ 

#include <MK70F.h>

/** Low level init function.
 */
int __low_level_init(void) {
    SystemInit();
    return 1;
}
