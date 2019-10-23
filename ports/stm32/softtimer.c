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
    MP_STATE_PORT(soft_timer_head) = NULL;
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
    soft_timer_entry_t *head = MP_STATE_PORT(soft_timer_head);
    while (head != NULL && TICKS_DIFF(head->expiry_ms, ticks_ms) <= 0) {
        mp_sched_schedule(head->callback, MP_OBJ_FROM_PTR(head));
        if (head->mode == SOFT_TIMER_MODE_PERIODIC) {
            head->expiry_ms += head->delta_ms;
            // Shift this node along to its new position
            soft_timer_entry_t *cur = head;
            while (cur->next != NULL && TICKS_DIFF(head->expiry_ms, cur->next->expiry_ms) >= 0) {
                cur = cur->next;
            }
            if (cur != head) {
                soft_timer_entry_t *next = head->next;
                head->next = cur->next;
                cur->next = head;
                head = next;
            }
        } else {
            head = head->next;
        }
    }
    MP_STATE_PORT(soft_timer_head) = head;
    if (head == NULL) {
        // No more timers left, set largest delay possible
        soft_timer_next = uwTick;
    } else {
        // Set soft_timer_next so SysTick calls us back at the correct time
        soft_timer_schedule_systick(head->expiry_ms);
    }
}

void soft_timer_insert(soft_timer_entry_t *entry) {
    uint32_t irq_state = raise_irq_pri(IRQ_PRI_PENDSV);
    soft_timer_entry_t **head_ptr = &MP_STATE_PORT(soft_timer_head);
    while (*head_ptr != NULL && TICKS_DIFF(entry->expiry_ms, (*head_ptr)->expiry_ms) >= 0) {
        head_ptr = &(*head_ptr)->next;
    }
    entry->next = *head_ptr;
    *head_ptr = entry;
    if (head_ptr == &MP_STATE_PORT(soft_timer_head)) {
        // This new timer became the earliest one so set soft_timer_next
        soft_timer_schedule_systick((*head_ptr)->expiry_ms);
    }
    restore_irq_pri(irq_state);
}

void soft_timer_remove(soft_timer_entry_t *entry) {
    uint32_t irq_state = raise_irq_pri(IRQ_PRI_PENDSV);
    soft_timer_entry_t **cur = &MP_STATE_PORT(soft_timer_head);
    while (*cur != NULL) {
        if (*cur == entry) {
            *cur = entry->next;
            break;
        }
    }
    restore_irq_pri(irq_state);
}
