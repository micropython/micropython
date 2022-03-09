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

static volatile uint32_t autoreload_countdown_ms = 0;

// True if user has disabled autoreload.
static bool autoreload_enabled = false;

// Non-zero if autoreload is temporarily off, due to an AUTORELOAD_SUSPEND_... reason.
static uint32_t autoreload_suspended = 0;

// True if autoreload has been triggered. Wait for CIRCUITPY_AUTORELOAD_DELAY_MS before doing the
// autoreload, in case further writes arrive.
static bool autoreload_countdown = false;

// True if something has requested a reload/restart.
volatile bool reload_requested = false;

void autoreload_reset() {
    if (autoreload_countdown) {
        supervisor_disable_tick();
        autoreload_countdown = false;
    }
    autoreload_countdown_ms = 0;
    reload_requested = false;
}

inline void autoreload_tick() {
    if (!autoreload_countdown) {
        return;
    }
    if (autoreload_countdown_ms > 0) {
        autoreload_countdown_ms--;
    }
    if (autoreload_countdown_ms == 0 && autoreload_enabled &&
        autoreload_suspended == 0 && !reload_requested) {
        reload_requested = true;
        autoreload_countdown = false;
        supervisor_disable_tick();
        supervisor_set_run_reason(RUN_REASON_AUTO_RELOAD);
        mp_raise_reload_exception();
    }
}

void autoreload_enable() {
    autoreload_enabled = true;
    reload_requested = false;
    autoreload_countdown = false;
}

void autoreload_disable() {
    autoreload_enabled = false;
    autoreload_countdown = false;
}

void autoreload_suspend(uint32_t suspend_reason_mask) {
    autoreload_suspended |= suspend_reason_mask;
}

void autoreload_resume(uint32_t suspend_reason_mask) {
    autoreload_suspended &= ~suspend_reason_mask;
}

inline bool autoreload_is_enabled() {
    return autoreload_enabled;
}

void autoreload_start_countdown() {
    // Avoid multiple tick enables.
    if (!autoreload_countdown) {
        supervisor_enable_tick();
        autoreload_countdown = true;
    }
    // Start or restart the countdown interval.
    autoreload_countdown_ms = CIRCUITPY_AUTORELOAD_DELAY_MS;
}
