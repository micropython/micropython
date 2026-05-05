/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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

#include <stdio.h>
#include <stdint.h>

#include "mpstate.h"
#include "gc.h"
#include "mpthread.h"
#include "gccollect.h"

unsigned int ReadSP(void) {
    uint32_t res;
    __asm volatile (
        "mov %0, r13\n"
        : "=r" (res)
        :
        :
        );

    return res;
}

void gc_stacktop_set(void *ptr) {
    MP_STATE_PORT(global_stacktop_ptr) = ptr;
}

void gc_collect(void) {
    // get current time, in case we want to time the GC
    #if 0
    uint32_t start = mp_hal_ticks_us();
    #endif
    // start the GC
    gc_collect_start();
    // get the registers and the sp
    uintptr_t sp = (uintptr_t)ReadSP();

    if (mp_is_python_thread()) {
        // trace the stack, including the registers (since they live on the stack in this function)
        gc_collect_root((void **)sp, ((uint32_t)MP_STATE_THREAD(stack_top) - sp) / sizeof(uint32_t));
    } else if (NULL != MP_STATE_PORT(global_stacktop_ptr)) {
        gc_collect_root((void **)sp, ((uint32_t)MP_STATE_PORT(global_stacktop_ptr) - sp) / sizeof(uint32_t));
    } else {
        // do nothing
    }

    // trace root pointers from any threads
    #if MICROPY_PY_THREAD
    mp_thread_gc_others();
    #endif

    // end the GC
    gc_collect_end();
}

MP_REGISTER_ROOT_POINTER(volatile void *global_stacktop_ptr);
