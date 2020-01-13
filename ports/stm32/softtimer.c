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

#include <stdint.h>
#include "py/runtime.h"
#include "irq.h"
#include "softtimer.h"

#define TICKS_PERIOD 0x80000000
#define TICKS_DIFF(t1, t0) ((int32_t)(((t1 - t0 + TICKS_PERIOD / 2) & (TICKS_PERIOD - 1)) - TICKS_PERIOD / 2))

extern __IO uint32_t uwTick;

volatile uint32_t soft_timer_next;

void soft_timer_deinit(void) {
    MP_STATE_PORT(soft_timer_heap) = NULL;
}

STATIC int soft_timer_lt(mp_pairheap_t *n1, mp_pairheap_t *n2) {
    soft_timer_entry_t *e1 = (soft_timer_entry_t*)n1;
    soft_timer_entry_t *e2 = (soft_timer_entry_t*)n2;
    return TICKS_DIFF(e1->expiry_ms, e2->expiry_ms) < 0;
}

STATIC void soft_timer_schedule_systick(uint32_t ticks_ms) {
    uint32_t irq_state = disable_irq();
    uint32_t uw_tick = uwTick;
    if (TICKS_DIFF(ticks_ms, uw_tick) <= 0) {
        soft_timer_next = uw_tick + 1;
    } else {
        soft_timer_next = ticks_ms;
    }
    enable_irq(irq_state);
}

// Must be executed at IRQ_PRI_PENDSV
void soft_timer_handler(void) {
    uint32_t ticks_ms = uwTick;
    soft_timer_entry_t *heap = MP_STATE_PORT(soft_timer_heap);
    while (heap != NULL && TICKS_DIFF(heap->expiry_ms, ticks_ms) <= 0) {
        soft_timer_entry_t *entry = heap;
        heap = (soft_timer_entry_t*)mp_pairheap_pop(soft_timer_lt, &heap->pairheap);
        mp_sched_schedule(entry->callback, MP_OBJ_FROM_PTR(entry));
        if (entry->mode == SOFT_TIMER_MODE_PERIODIC) {
            entry->expiry_ms += entry->delta_ms;
            heap = (soft_timer_entry_t*)mp_pairheap_push(soft_timer_lt, &heap->pairheap, &entry->pairheap);
        }
    }
    MP_STATE_PORT(soft_timer_heap) = heap;
    if (heap == NULL) {
        // No more timers left, set largest delay possible
        soft_timer_next = uwTick;
    } else {
        // Set soft_timer_next so SysTick calls us back at the correct time
        soft_timer_schedule_systick(heap->expiry_ms);
    }
}

void soft_timer_insert(soft_timer_entry_t *entry) {
    uint32_t irq_state = raise_irq_pri(IRQ_PRI_PENDSV);
    MP_STATE_PORT(soft_timer_heap) = (soft_timer_entry_t*)mp_pairheap_push(soft_timer_lt, &MP_STATE_PORT(soft_timer_heap)->pairheap, &entry->pairheap);
    if (entry == MP_STATE_PORT(soft_timer_heap)) {
        // This new timer became the earliest one so set soft_timer_next
        soft_timer_schedule_systick(entry->expiry_ms);
    }
    restore_irq_pri(irq_state);
}

void soft_timer_remove(soft_timer_entry_t *entry) {
    uint32_t irq_state = raise_irq_pri(IRQ_PRI_PENDSV);
    MP_STATE_PORT(soft_timer_heap) = (soft_timer_entry_t*)mp_pairheap_delete(soft_timer_lt, &MP_STATE_PORT(soft_timer_heap)->pairheap, &entry->pairheap);
    restore_irq_pri(irq_state);
}
