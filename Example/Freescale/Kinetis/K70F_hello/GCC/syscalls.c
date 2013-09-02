/**
 * @file syscalls.c
 * @date 2. Sep 2013
 * @brief Minimalistic system calls implementation
 *
 * @author Jozef Maslik (maslo@binarylemon.com)
 *
 */

#include <stdlib.h>
#include <reent.h>
#include <sys/stat.h>
#include <mk_uart.h>

/** A reentrant version of read.
 *
 */
_ssize_t _read_r(struct _reent *reent, int fd, void *buf, size_t cnt) {
    _ssize_t rd = 0;
    char *p = (char*) buf;

    while (rd < cnt) {
        *p = uart_getchar(UART2_BASE_PTR);

        if ('\r' == *p) {
            *p = 0;
            break;
        }

        p++;
        rd++;
    }

    return rd;
}

/** A reentrant version of write
 *
 */
_ssize_t _write_r(struct _reent *reent, int fd, const void *buf, size_t cnt) {
    _ssize_t i;
    const char *p = buf;

    for (i = 0; i < cnt; i++) {
        uart_putch(UART2_BASE_PTR, *p++);
    }

    return i;
}

/** A reentrant version of close.
 *
 */
int _close_r(struct _reent *reent, int fd) {
    return 0;
}

/** A reentrant version of lseek.
 *
 */
off_t _lseek_r(struct _reent *reent, int fd, off_t pos, int whence) {
    return 0;
}

/** A reentrant version of fstat.
 * Always set character device.
 */
int _fstat_r(struct _reent *reent, int fd, struct stat *pstat) {
    pstat->st_mode = S_IFCHR;
    return 0;
}

/** Query whether output is terminal.
 *
 */
int _isatty_r(struct _reent *reent, int fd) {
    return 1;
}


extern char _end[];     // linker symbol - end of used memory
static char *heap_end;  // current end of the heap

/** Increase heap size - provide more space for libc memory allocator.
 *
 */
void* _sbrk_r(struct _reent *reent, ptrdiff_t incr) {
    char *base;

    if (!heap_end) {
        heap_end = _end;
    }
    base = heap_end;
    heap_end += incr;

    return base;    // pointer to start of new heap area
}

/** Get process id.
 *
 */
#define __MYPID 1
int _getpid(struct _reent *reent) {
//    return CoGetCurTaskID();
    return __MYPID;
}

/** Exit program.
 *
 */
void _exit(int sig) {
    while (1);
}

/** Send signal.
 *
 */
int _kill(int pid, int sig) {
    if (pid == __MYPID) {
        _exit(sig);
    }

    return 0;
}
