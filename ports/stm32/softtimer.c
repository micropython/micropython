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
#include "py/gc.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "irq.h"
#include "softtimer.h"

#define TICKS_PERIOD 0x80000000
#define TICKS_DIFF(t1, t0) ((int32_t)(((t1 - t0 + TICKS_PERIOD / 2) & (TICKS_PERIOD - 1)) - TICKS_PERIOD / 2))

extern __IO uint32_t uwTick;

volatile uint32_t soft_timer_next;

// Pointer to the pairheap of soft timer objects.
// This may contain bss/data pointers as well as GC-heap pointers,
// and is explicitly GC traced by soft_timer_gc_mark_all().
STATIC soft_timer_entry_t *soft_timer_heap;

STATIC int soft_timer_lt(mp_pairheap_t *n1, mp_pairheap_t *n2) {
    soft_timer_entry_t *e1 = (soft_timer_entry_t *)n1;
    soft_timer_entry_t *e2 = (soft_timer_entry_t *)n2;
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

void soft_timer_deinit(void) {
    // Pop off all the nodes which are allocated on the GC-heap.
    uint32_t irq_state = raise_irq_pri(IRQ_PRI_PENDSV);
    soft_timer_entry_t *heap_from = soft_timer_heap;
    soft_timer_entry_t *heap_to = (soft_timer_entry_t *)mp_pairheap_new(soft_timer_lt);
    while (heap_from != NULL) {
        soft_timer_entry_t *entry = (soft_timer_entry_t *)mp_pairheap_peek(soft_timer_lt, &heap_from->pairheap);
        heap_from = (soft_timer_entry_t *)mp_pairheap_pop(soft_timer_lt, &heap_from->pairheap);
        if (!(entry->flags & SOFT_TIMER_FLAG_GC_ALLOCATED)) {
            heap_to = (soft_timer_entry_t *)mp_pairheap_push(soft_timer_lt, &heap_to->pairheap, &entry->pairheap);
        }
    }
    soft_timer_heap = heap_to;
    restore_irq_pri(irq_state);
}

// Must be executed at IRQ_PRI_PENDSV
void soft_timer_handler(void) {
    uint32_t ticks_ms = uwTick;
    soft_timer_entry_t *heap = soft_timer_heap;
    while (heap != NULL && TICKS_DIFF(heap->expiry_ms, ticks_ms) <= 0) {
        soft_timer_entry_t *entry = heap;
        heap = (soft_timer_entry_t *)mp_pairheap_pop(soft_timer_lt, &heap->pairheap);
        if (entry->flags & SOFT_TIMER_FLAG_PY_CALLBACK) {
            mp_sched_schedule(entry->py_callback, MP_OBJ_FROM_PTR(entry));
        } else {
            entry->c_callback(entry);
        }
        if (entry->mode == SOFT_TIMER_MODE_PERIODIC) {
            entry->expiry_ms += entry->delta_ms;
            heap = (soft_timer_entry_t *)mp_pairheap_push(soft_timer_lt, &heap->pairheap, &entry->pairheap);
        }
    }
    soft_timer_heap = heap;
    if (heap == NULL) {
        // No more timers left, set largest delay possible
        soft_timer_next = uwTick;
    } else {
        // Set soft_timer_next so SysTick calls us back at the correct time
        soft_timer_schedule_systick(heap->expiry_ms);
    }
}

void soft_timer_gc_mark_all(void) {
    // Mark all soft timer nodes that are allocated on the GC-heap.
    // To avoid deep C recursion, pop and recreate the pairheap as nodes are marked.
    uint32_t irq_state = raise_irq_pri(IRQ_PRI_PENDSV);
    soft_timer_entry_t *heap_from = soft_timer_heap;
    soft_timer_entry_t *heap_to = (soft_timer_entry_t *)mp_pairheap_new(soft_timer_lt);
    while (heap_from != NULL) {
        soft_timer_entry_t *entry = (soft_timer_entry_t *)mp_pairheap_peek(soft_timer_lt, &heap_from->pairheap);
        heap_from = (soft_timer_entry_t *)mp_pairheap_pop(soft_timer_lt, &heap_from->pairheap);
        if (entry->flags & SOFT_TIMER_FLAG_GC_ALLOCATED) {
            gc_collect_root((void **)&entry, 1);
        }
        heap_to = (soft_timer_entry_t *)mp_pairheap_push(soft_timer_lt, &heap_to->pairheap, &entry->pairheap);
    }
    soft_timer_heap = heap_to;
    restore_irq_pri(irq_state);
}

void soft_timer_static_init(soft_timer_entry_t *entry, uint16_t mode, uint32_t delta_ms, void (*cb)(soft_timer_entry_t *)) {
    mp_pairheap_init_node(soft_timer_lt, &entry->pairheap);
    entry->flags = 0;
    entry->mode = mode;
    entry->delta_ms = delta_ms;
    entry->c_callback = cb;
}

void soft_timer_insert(soft_timer_entry_t *entry, uint32_t initial_delta_ms) {
    mp_pairheap_init_node(soft_timer_lt, &entry->pairheap);
    entry->expiry_ms = mp_hal_ticks_ms() + initial_delta_ms;
    uint32_t irq_state = raise_irq_pri(IRQ_PRI_PENDSV);
    soft_timer_heap = (soft_timer_entry_t *)mp_pairheap_push(soft_timer_lt, &soft_timer_heap->pairheap, &entry->pairheap);
    if (entry == soft_timer_heap) {
        // This new timer became the earliest one so set soft_timer_next
        soft_timer_schedule_systick(entry->expiry_ms);
    }
    restore_irq_pri(irq_state);
}

void soft_timer_remove(soft_timer_entry_t *entry) {
    uint32_t irq_state = raise_irq_pri(IRQ_PRI_PENDSV);
    soft_timer_heap = (soft_timer_entry_t *)mp_pairheap_delete(soft_timer_lt, &soft_timer_heap->pairheap, &entry->pairheap);
    restore_irq_pri(irq_state);
}
