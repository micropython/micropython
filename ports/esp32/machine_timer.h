/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2015 Damien P. George
 * Copyright (c) 2016 Paul Sokolovsky
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

#ifndef MICROPY_INCLUDED_ESP32_MACHINE_TIMER_H
#define MICROPY_INCLUDED_ESP32_MACHINE_TIMER_H

#include "hal/timer_hal.h"
#include "hal/timer_ll.h"
#include "soc/timer_periph.h"

#define TIMER_DIVIDER  8

// TIMER_BASE_CLK is normally 80MHz. TIMER_DIVIDER ought to divide this exactly
#define TIMER_SCALE    (APB_CLK_FREQ / TIMER_DIVIDER)

#define TIMER_FLAGS    0

typedef struct _machine_timer_obj_t {
    mp_obj_base_t base;

    timer_hal_context_t hal_context;
    mp_uint_t group;
    mp_uint_t index;

    mp_uint_t repeat;
    // ESP32 timers are 64-bit
    uint64_t period;

    mp_obj_t callback;

    intr_handle_t handle;
    void (*handler)(struct _machine_timer_obj_t *timer);

    struct _machine_timer_obj_t *next;
} machine_timer_obj_t;

machine_timer_obj_t *machine_timer_create(mp_uint_t timer);
void machine_timer_enable(machine_timer_obj_t *self);
void machine_timer_disable(machine_timer_obj_t *self);

#endif // MICROPY_INCLUDED_ESP32_MACHINE_TIMER_H
