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
#ifndef MICROPY_INCLUDED_STM32_SOFTTIMER_H
#define MICROPY_INCLUDED_STM32_SOFTTIMER_H

#include "py/obj.h"

#define SOFT_TIMER_MODE_ONE_SHOT (1)
#define SOFT_TIMER_MODE_PERIODIC (2)

typedef struct _soft_timer_entry_t {
    mp_obj_base_t base; // so struct can be used as an object and still be traced by GC
    struct _soft_timer_entry_t *next;
    uint32_t mode;
    uint32_t expiry_ms;
    uint32_t delta_ms; // for periodic mode
    mp_obj_t callback;
} soft_timer_entry_t;

extern volatile uint32_t soft_timer_next;

void soft_timer_deinit(void);
void soft_timer_handler(void);
void soft_timer_insert(soft_timer_entry_t *entry);
void soft_timer_remove(soft_timer_entry_t *entry);

#endif // MICROPY_INCLUDED_STM32_SOFTTIMER_H
