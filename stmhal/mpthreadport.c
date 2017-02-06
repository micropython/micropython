/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
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

#include "py/mpconfig.h"
#include "py/mpstate.h"
#include "py/gc.h"
#include "py/mpthread.h"
#include "gccollect.h"

#if MICROPY_PY_THREAD

// the mutex controls access to the linked list
STATIC mp_thread_mutex_t thread_mutex;

void mp_thread_init(void) {
    mp_thread_mutex_init(&thread_mutex);
    mp_thread_set_state(&mp_state_ctx.thread);
}

void mp_thread_gc_others(void) {
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (pyb_thread_t *th = pyb_thread_all; th != NULL; th = th->all_next) {
        gc_collect_root((void**)&th, 1);
        gc_collect_root(&th->arg, 1);
        gc_collect_root(&th->stack, 1);
        if (th != pyb_thread_cur) {
            gc_collect_root(th->stack, th->stack_len);
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
}

void mp_thread_create(void *(*entry)(void*), void *arg, size_t *stack_size) {
    if (*stack_size == 0) {
        *stack_size = 4096; // default stack size
    } else if (*stack_size < 2048) {
        *stack_size = 2048; // minimum stack size
    }

    // round stack size to a multiple of the word size
    size_t stack_len = *stack_size / sizeof(uint32_t);
    *stack_size = stack_len * sizeof(uint32_t);

    // allocate stack and linked-list node (must be done outside thread_mutex lock)
    uint32_t *stack = m_new(uint32_t, stack_len);
    pyb_thread_t *th = m_new_obj(pyb_thread_t);

    mp_thread_mutex_lock(&thread_mutex, 1);

    // create thread
    uint32_t id = pyb_thread_new(th, stack, stack_len, entry, arg);
    if (id == 0) {
        mp_thread_mutex_unlock(&thread_mutex);
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "can't create thread"));
    }

    mp_thread_mutex_unlock(&thread_mutex);

    // adjust stack_size to provide room to recover from hitting the limit
    *stack_size -= 1024;
}

void mp_thread_start(void) {
}

void mp_thread_finish(void) {
}

#endif // MICROPY_PY_THREAD
