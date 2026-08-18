/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 OpenMV LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * newlib back-end stubs for the prebuilt morselib.
 *
 * morselib is compiled against newlib and pulls in a few C library functions
 * (sscanf, qsort, setjmp, ...).  Those transitively reference newlib's syscall
 * back-end, which the ports do not provide: MicroPython links -nostdlib and uses
 * its own heap, so nothing else needs it.  morselib never calls these at run
 * time, so the stubs only exist to satisfy the link, and _sbrk deliberately
 * fails rather than hand out any of MicroPython's memory.  They are weak so that
 * a port supplying real implementations takes precedence.
 */

#include <errno.h>
#include <stddef.h>
#include <sys/stat.h>

__attribute__((weak)) void *_sbrk(ptrdiff_t incr) {
    (void)incr;
    errno = ENOMEM;
    return (void *)-1;
}

__attribute__((weak)) int _write(int fd, const char *buf, int len) {
    (void)fd;
    (void)buf;
    (void)len;
    errno = ENOSYS;
    return -1;
}

__attribute__((weak)) int _read(int fd, char *buf, int len) {
    (void)fd;
    (void)buf;
    (void)len;
    errno = ENOSYS;
    return -1;
}

__attribute__((weak)) int _close(int fd) {
    (void)fd;
    errno = ENOSYS;
    return -1;
}

__attribute__((weak)) int _lseek(int fd, int ptr, int dir) {
    (void)fd;
    (void)ptr;
    (void)dir;
    errno = ENOSYS;
    return -1;
}

__attribute__((weak)) int _fstat(int fd, struct stat *st) {
    (void)fd;
    (void)st;
    errno = ENOSYS;
    return -1;
}

__attribute__((weak)) int _isatty(int fd) {
    (void)fd;
    errno = ENOSYS;
    return 0;
}

__attribute__((weak)) int _getpid(void) {
    return 1;
}

__attribute__((weak)) int _kill(int pid, int sig) {
    (void)pid;
    (void)sig;
    errno = ENOSYS;
    return -1;
}
