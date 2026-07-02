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

#include "driver/gptimer.h"
#include "esp_timer.h"

typedef struct _machine_timer_obj_t {
    mp_obj_base_t base;

    // Positive means hardware, -1 means virtual
    mp_int_t id;
    union {
        gptimer_handle_t hardware;
        esp_timer_handle_t virtual;
    } handle;

    // Period is in units of the timers' frequency,
    // as returned by machine_timer_freq_hz()
    uint64_t period;
    bool repeat;

    // Virtual timers don't have counters, thus we
    // emulate it by calculating based on the time
    int64_t virtual_started;
    int64_t virtual_stopped;

    // Default handler simply schedules execution of the context
    // (which usually is a callable Python object). Other C code
    // may set different handlers; see for example UART's RXIDLE.
    bool (*handler)(struct _machine_timer_obj_t *timer);
    mp_obj_t handler_ctx;

    // Pointer to next timer in linked list of active timers
    struct _machine_timer_obj_t *next;
} machine_timer_obj_t;

// Externalize machine timer for use elsewhere in board support
machine_timer_obj_t *machine_timer_create(mp_int_t id);
uint32_t machine_timer_freq_hz(machine_timer_obj_t *self);
void machine_timer_configure(machine_timer_obj_t *self);
void machine_timer_start(machine_timer_obj_t *self);
void machine_timer_stop(machine_timer_obj_t *self);
mp_obj_t machine_timer_deinit(mp_obj_t self_in);

#endif // MICROPY_INCLUDED_ESP32_MACHINE_TIMER_H
