/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
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

#include "autoreload.h"

#include "py/mphal.h"
#include "py/reload.h"
#include "supervisor/shared/tick.h"

supervisor_allocation *next_code_allocation;
#include "shared-bindings/supervisor/Runtime.h"

static volatile uint32_t autoreload_delay_ms = 0;
static bool autoreload_enabled = false;
static bool autoreload_suspended = false;

volatile bool reload_requested = false;

inline void autoreload_tick() {
    if (autoreload_delay_ms == 0) {
        return;
    }
    if (autoreload_delay_ms == 1 && autoreload_enabled &&
        !autoreload_suspended && !reload_requested) {
        mp_raise_reload_exception();
        reload_requested = true;
        supervisor_set_run_reason(RUN_REASON_AUTO_RELOAD);
        supervisor_disable_tick();
    }
    autoreload_delay_ms--;
}

void autoreload_enable() {
    autoreload_enabled = true;
    reload_requested = false;
}

void autoreload_disable() {
    autoreload_enabled = false;
}

void autoreload_suspend() {
    autoreload_suspended = true;
}

void autoreload_resume() {
    autoreload_suspended = false;
}

inline bool autoreload_is_enabled() {
    return autoreload_enabled;
}

void autoreload_start() {
    // Enable ticks if we haven't been tracking an autoreload delay. We check
    // our current state so that we only turn ticks on once. Multiple starts
    // can occur before we reload and then turn ticks off.
    if (autoreload_delay_ms == 0) {
        supervisor_enable_tick();
    }
    autoreload_delay_ms = CIRCUITPY_AUTORELOAD_DELAY_MS;
}

void autoreload_stop() {
    autoreload_delay_ms = 0;
    reload_requested = false;
}

void autoreload_now() {
    if (!autoreload_enabled || autoreload_suspended || reload_requested) {
        return;
    }
    mp_raise_reload_exception();
    reload_requested = true;
    supervisor_set_run_reason(RUN_REASON_AUTO_RELOAD);
}
