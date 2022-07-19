/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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
#ifndef MICROPY_INCLUDED_SHARED_RUNTIME_SOFTTIMER_H
#define MICROPY_INCLUDED_SHARED_RUNTIME_SOFTTIMER_H

#include "py/pairheap.h"

#define SOFT_TIMER_FLAG_PY_CALLBACK (1)
#define SOFT_TIMER_FLAG_GC_ALLOCATED (2)

#define SOFT_TIMER_MODE_ONE_SHOT (1)
#define SOFT_TIMER_MODE_PERIODIC (2)

typedef struct _soft_timer_entry_t {
    mp_pairheap_t pairheap;
    uint16_t flags;
    uint16_t mode;
    uint32_t expiry_ms;
    uint32_t delta_ms; // for periodic mode
    union {
        void (*c_callback)(struct _soft_timer_entry_t *);
        mp_obj_t py_callback;
    };
} soft_timer_entry_t;

extern volatile uint32_t soft_timer_next;

void soft_timer_deinit(void);
void soft_timer_handler(void);
void soft_timer_gc_mark_all(void);

void soft_timer_static_init(soft_timer_entry_t *entry, uint16_t mode, uint32_t delta_ms, void (*cb)(soft_timer_entry_t *));
void soft_timer_insert(soft_timer_entry_t *entry, uint32_t initial_delta_ms);
void soft_timer_remove(soft_timer_entry_t *entry);

// The timer will be reinserted into the heap so that it is called after initial_delta_ms milliseconds.
// After that, if it's periodic, it will continue to be called every entry->delta_ms milliseconds.
static inline void soft_timer_reinsert(soft_timer_entry_t *entry, uint32_t initial_delta_ms) {
    soft_timer_remove(entry);
    soft_timer_insert(entry, initial_delta_ms);
}

#endif // MICROPY_INCLUDED_SHARED_RUNTIME_SOFTTIMER_H
