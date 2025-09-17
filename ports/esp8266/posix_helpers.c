/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Paul Sokolovsky
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
 */

#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include "py/mphal.h"
#include "py/gc.h"

// Functions for external libs like axTLS, BerkeleyDB, etc.

void *malloc(size_t size) {
    void *p = gc_alloc(size, 0);
    if (p == NULL) {
        // POSIX requires ENOMEM to be set in case of error
        errno = ENOMEM;
    }
    return p;
}
void free(void *ptr) {
    gc_free(ptr);
}
void *calloc(size_t nmemb, size_t size) {
    return malloc(nmemb * size);
}
void *realloc(void *ptr, size_t size) {
    void *p = gc_realloc(ptr, size, true);
    if (p == NULL) {
        // POSIX requires ENOMEM to be set in case of error
        errno = ENOMEM;
    }
    return p;
}

#undef htonl
#undef ntohl
uint32_t ntohl(uint32_t netlong) {
    return MP_BE32TOH(netlong);
}
uint32_t htonl(uint32_t netlong) {
    return MP_HTOBE32(netlong);
}

time_t time(time_t *t) {
    return mp_hal_ticks_ms() / 1000;
}

time_t mktime(void *tm) {
    return 0;
}
