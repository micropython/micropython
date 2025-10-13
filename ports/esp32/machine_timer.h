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

typedef struct _machine_timer_obj_t {
    mp_obj_base_t base;

    // Timer type; virtual or hardware
    mp_uint_t type; // 0 = virtual, 1 = hardware


    // NOTE: For soft timers (period in ticks) is converted to OS ticks, this is a compile time
    // constant defined in FreeRTOSConfig.h as CONFIG_TICK_RATE_HZ (typically 100Hz or 1 tick every 10ms)
    // however its recommended to set this to 1000Hz (1ms tick) for better resolution by adding to
    // the board sdkconfig.defaults file.

    // Virtual timer specific fields
    TimerHandle_t vtimer;
    TickType_t v_start_tick; // Tick count when virtual timer was started

    // ESP32 specific fields
    timer_hal_context_t hal_context;
    mp_uint_t group;
    mp_uint_t index;

    mp_uint_t repeat;
    // ESP32 timers are 64-bit
    uint64_t period;

    // User callback to be invoked when timer expires
    mp_obj_t callback;

    // Interrupt related callbacks and state
    intr_handle_t handle;
    void (*handler)(struct _machine_timer_obj_t *timer);

    // Pointer to next timer in linked list of active timers
    struct _machine_timer_obj_t *next;
} machine_timer_obj_t;

machine_timer_obj_t *machine_timer_create(mp_uint_t timer);
void machine_timer_enable(machine_timer_obj_t *self);
void machine_timer_disable(machine_timer_obj_t *self);

uint32_t machine_timer_freq_hz(void);

#endif // MICROPY_INCLUDED_ESP32_MACHINE_TIMER_H
