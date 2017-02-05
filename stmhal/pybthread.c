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

#include <string.h>
#include <stdio.h>

#include "py/obj.h"
#include "gccollect.h"
#include "irq.h"
#include "pybthread.h"

#if MICROPY_PY_THREAD

int pyb_thread_enabled;
pyb_thread_t *pyb_thread_cur;

void pyb_thread_init(pyb_thread_t *thread) {
    pyb_thread_cur = thread;
    pyb_thread_cur->sp = NULL; // will be set when this thread switches out
    pyb_thread_cur->local_state = 0; // will be set by mp_thread_init
    pyb_thread_cur->arg = NULL;
    pyb_thread_cur->stack = &_heap_end;
    pyb_thread_cur->stack_len = ((uint32_t)&_estack - (uint32_t)&_heap_end) / sizeof(uint32_t);
    pyb_thread_cur->prev = thread;
    pyb_thread_cur->next = thread;
}

STATIC void pyb_thread_terminate(void) {
    uint32_t irq_state = raise_irq_pri(IRQ_PRI_PENDSV);
    pyb_thread_cur->prev->next = pyb_thread_cur->next;
    pyb_thread_cur->next->prev = pyb_thread_cur->prev;
    if (pyb_thread_cur->next == pyb_thread_cur->prev) {
        pyb_thread_enabled = 0;
    }
    restore_irq_pri(irq_state);
    pyb_thread_yield(); // should not return
}

uint32_t pyb_thread_new(pyb_thread_t *thread, void *stack, size_t stack_len, void *entry, void *arg) {
    uint32_t *stack_top = (uint32_t*)stack + stack_len; // stack is full descending
    *--stack_top = 0x01000000; // xPSR (thumb bit set)
    *--stack_top = (uint32_t)entry & 0xfffffffe; // pc (must have bit 0 cleared, even for thumb code)
    *--stack_top = (uint32_t)pyb_thread_terminate; // lr
    *--stack_top = 0; // r12
    *--stack_top = 0; // r3
    *--stack_top = 0; // r2
    *--stack_top = 0; // r1
    *--stack_top = (uint32_t)arg; // r0
    *--stack_top = 0xfffffff9; // lr (return to thread mode, non-FP, use MSP)
    stack_top -= 8; // r4-r11
    stack_top -= 16; // s16-s31 (we assume all threads use FP registers)
    thread->sp = stack_top;
    thread->local_state = 0;
    thread->arg = arg;
    thread->stack = stack;
    thread->stack_len = stack_len;
    uint32_t irq_state = raise_irq_pri(IRQ_PRI_PENDSV);
    pyb_thread_enabled = 1;
    thread->next = pyb_thread_cur->next;
    thread->prev = pyb_thread_cur;
    pyb_thread_cur->next->prev = thread;
    pyb_thread_cur->next = thread;
    restore_irq_pri(irq_state);
    return (uint32_t)thread; // success
}

// should only be called from pendsv_isr_handler
void *pyb_thread_next(void *sp) {
    pyb_thread_cur->sp = sp;
    pyb_thread_cur = pyb_thread_cur->next;
    return pyb_thread_cur->sp;
}

#endif // MICROPY_PY_THREAD
