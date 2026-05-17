/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Angus Gratton
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
#include "lwip/memp.h"

// This is a link-time patch to enforce the limit of max active TCP PCBs. A
// workaround for upstream issue https://github.com/espressif/esp-idf/issues/9670
//
// Without this limit the number of TCP PCBs in TIME-WAIT is unbounded, which can
// have two problems on systems with a lot of short-lived TCP connections:
//
// - Higher memory usage.
// - Increased chance of stalled TCP connections due to port reuse.

static unsigned active_tcp_pcbs;

void *__real_memp_malloc(memp_t type);
void __real_memp_free(memp_t type, void *mem);

void *__wrap_memp_malloc(memp_t type) {
    if (type != MEMP_TCP_PCB) {
        return __real_memp_malloc(type);
    }

    if (active_tcp_pcbs >= MEMP_NUM_TCP_PCB) {
        return NULL;
    }

    void *res = __real_memp_malloc(MEMP_TCP_PCB);
    if (res != NULL) {
        ++active_tcp_pcbs;
    }
    return res;
}

void __wrap_memp_free(memp_t type, void *mem) {
    __real_memp_free(type, mem);
    if (type == MEMP_TCP_PCB && mem != NULL) {
        assert(active_tcp_pcbs);
        --active_tcp_pcbs;
    }
}
