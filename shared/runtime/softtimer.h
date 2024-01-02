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

static inline int32_t soft_timer_ticks_diff(uint32_t t1, uint32_t t0) {
    // t1 is after t0 (i.e. positive result) if there exists a uint32_t X <= INT_MAX
    // such that t0 + X = t1. Otherwise t1 is interpreted to be earlier than
    // t0 (negative result).
    return t1 - t0;
}

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

#if !defined(MICROPY_SOFT_TIMER_TICKS_MS)
// IF MICROPY_SOFT_TIMER_TICKS_MS is not defined then the port must provide the
// following timer functions:
// - soft_timer_get_ms() must return a 32-bit millisecond counter that wraps around.
// - soft_timer_schedule_at_ms(ticks_ms) must schedule a callback of soft_timer_handler()
//   when the above millisecond counter reaches the given ticks_ms value.  If ticks_ms
//   is behind the current counter (using int32_t arithmetic) then the callback should
//   be scheduled immediately.  The callback of soft_timer_handler() should be made at
//   pend-SV IRQ level, or equivalent.
uint32_t soft_timer_get_ms(void);
void soft_timer_schedule_at_ms(uint32_t ticks_ms);

// Optional port-specific initialisation function (provided and called by the port if needed).
void soft_timer_init(void);
#endif

#endif // MICROPY_INCLUDED_SHARED_RUNTIME_SOFTTIMER_H
