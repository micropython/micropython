/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Damien P. George
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
#ifndef MICROPY_INCLUDED_STM32_PYBTHREAD_H
#define MICROPY_INCLUDED_STM32_PYBTHREAD_H

#include "py/mpprint.h"

typedef struct _pyb_thread_t {
    void *sp;
    uint32_t local_state;
    void *arg;                  // thread Python args, a GC root pointer
    void *stack;                // pointer to the stack
    size_t stack_len;           // number of words in the stack
    uint32_t timeslice;
    struct _pyb_thread_t *all_next;
    struct _pyb_thread_t *run_prev;
    struct _pyb_thread_t *run_next;
    struct _pyb_thread_t *queue_next;
} pyb_thread_t;

typedef pyb_thread_t *pyb_mutex_t;

extern volatile int pyb_thread_enabled;
extern pyb_thread_t *volatile pyb_thread_all;
extern pyb_thread_t *volatile pyb_thread_cur;

void pyb_thread_init(pyb_thread_t *th);
void pyb_thread_deinit();
uint32_t pyb_thread_new(pyb_thread_t *th, void *stack, size_t stack_len, void *entry, void *arg);
void pyb_thread_dump(const mp_print_t *print);

static inline uint32_t pyb_thread_get_id(void) {
    return (uint32_t)pyb_thread_cur;
}

static inline void pyb_thread_set_local(void *value) {
    pyb_thread_cur->local_state = (uint32_t)value;
}

static inline void *pyb_thread_get_local(void) {
    return (void *)pyb_thread_cur->local_state;
}

static inline void pyb_thread_yield(void) {
    if (pyb_thread_cur->run_next == pyb_thread_cur) {
        __WFI();
    } else {
        SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
    }
}

void pyb_mutex_init(pyb_mutex_t *m);
int pyb_mutex_lock(pyb_mutex_t *m, int wait);
void pyb_mutex_unlock(pyb_mutex_t *m);

#endif // MICROPY_INCLUDED_STM32_PYBTHREAD_H
