/**
 * @file syscalls.c
 * @brief system calls mapping
 * @date 30. Aug 2013
 *
 * @author Jozef Maslik (maslo@binarylemon.com)
 */

#include <mk_uart.h>
#include <yfuns.h>
#include <MK70F.h>

_STD_BEGIN

#pragma module_name = "?__write"

size_t __write(int hndl, const unsigned char *buf, size_t size) {
    size_t tx = 0;

    if (!buf) {
        return 0;
    }

    if (hndl != _LLIO_STDOUT && hndl != _LLIO_STDERR) {
        return _LLIO_ERROR;
    }

    while (size--) {
        uart_putc(UART2_BASE_PTR, *buf++);
        tx++;
    }

    return tx;
}

_STD_END
