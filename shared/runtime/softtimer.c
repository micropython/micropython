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
#include "shared/runtime/mpirq.h"
#include "softtimer.h"

#ifdef MICROPY_SOFT_TIMER_TICKS_MS

extern __IO uint32_t MICROPY_SOFT_TIMER_TICKS_MS;

volatile uint32_t soft_timer_next;

static inline uint32_t soft_timer_get_ms(void) {
    return MICROPY_SOFT_TIMER_TICKS_MS;
}

static void soft_timer_schedule_at_ms(uint32_t ticks_ms) {
    uint32_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    uint32_t uw_tick = MICROPY_SOFT_TIMER_TICKS_MS;
    if (soft_timer_ticks_diff(ticks_ms, uw_tick) <= 0) {
        soft_timer_next = uw_tick + 1;
    } else {
        soft_timer_next = ticks_ms;
    }
    MICROPY_END_ATOMIC_SECTION(atomic_state);
}

#endif

// Pointer to the pairheap of soft timer objects.
// This may contain bss/data pointers as well as GC-heap pointers,
// and is explicitly GC traced by soft_timer_gc_mark_all().
static soft_timer_entry_t *soft_timer_heap;

static int soft_timer_lt(mp_pairheap_t *n1, mp_pairheap_t *n2) {
    soft_timer_entry_t *e1 = (soft_timer_entry_t *)n1;
    soft_timer_entry_t *e2 = (soft_timer_entry_t *)n2;
    return soft_timer_ticks_diff(e1->expiry_ms, e2->expiry_ms) < 0;
}

void soft_timer_deinit(void) {
    // Pop off all the nodes which are allocated on the GC-heap.
    MICROPY_PY_PENDSV_ENTER;
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
    MICROPY_PY_PENDSV_EXIT;
}

// Must be executed at IRQ_PRI_PENDSV
void soft_timer_handler(void) {
    uint32_t ticks_ms = soft_timer_get_ms();
    soft_timer_entry_t *heap = soft_timer_heap;
    while (heap != NULL && soft_timer_ticks_diff(heap->expiry_ms, ticks_ms) <= 0) {
        soft_timer_entry_t *entry = heap;
        heap = (soft_timer_entry_t *)mp_pairheap_pop(soft_timer_lt, &heap->pairheap);
        if (entry->flags & SOFT_TIMER_FLAG_PY_CALLBACK) {
            if (mp_irq_dispatch(entry->py_callback, MP_OBJ_FROM_PTR(entry),
                entry->flags & SOFT_TIMER_FLAG_HARD_CALLBACK)) {
                // Uncaught exception; disable the callback so it doesn't run again.
                entry->mode = SOFT_TIMER_MODE_ONE_SHOT;
            }
        } else if (entry->c_callback) {
            entry->c_callback(entry);
        }
        if (entry->mode == SOFT_TIMER_MODE_PERIODIC) {
            entry->expiry_ms += entry->delta_ms;
            heap = (soft_timer_entry_t *)mp_pairheap_push(soft_timer_lt, &heap->pairheap, &entry->pairheap);
        }
    }
    soft_timer_heap = heap;

    // Schedule the port's timer to call us back at the correct time.
    if (heap != NULL) {
        soft_timer_schedule_at_ms(heap->expiry_ms);
    }
}

void soft_timer_gc_mark_all(void) {
    // Mark all soft timer nodes that are allocated on the GC-heap.
    // To avoid deep C recursion, pop and recreate the pairheap as nodes are marked.
    MICROPY_PY_PENDSV_ENTER;
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
    MICROPY_PY_PENDSV_EXIT;
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
    entry->expiry_ms = soft_timer_get_ms() + initial_delta_ms;
    MICROPY_PY_PENDSV_ENTER;
    soft_timer_heap = (soft_timer_entry_t *)mp_pairheap_push(soft_timer_lt, &soft_timer_heap->pairheap, &entry->pairheap);
    if (entry == soft_timer_heap) {
        // This new timer became the earliest one so schedule a callback.
        soft_timer_schedule_at_ms(entry->expiry_ms);
    }
    MICROPY_PY_PENDSV_EXIT;
}

void soft_timer_remove(soft_timer_entry_t *entry) {
    MICROPY_PY_PENDSV_ENTER;
    soft_timer_heap = (soft_timer_entry_t *)mp_pairheap_delete(soft_timer_lt, &soft_timer_heap->pairheap, &entry->pairheap);
    MICROPY_PY_PENDSV_EXIT;
}
