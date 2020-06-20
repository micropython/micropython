/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Jeff Epler for Adafruit Industries
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

#include "supervisor/shared/tick.h"

#include "py/mpstate.h"
#include "supervisor/linker.h"
#include "supervisor/filesystem.h"
#include "supervisor/port.h"
#include "supervisor/shared/autoreload.h"

static volatile uint64_t PLACE_IN_DTCM_BSS(background_ticks);

#if CIRCUITPY_GAMEPAD
#include "shared-module/gamepad/__init__.h"
#endif

#if CIRCUITPY_GAMEPADSHIFT
#include "shared-module/gamepadshift/__init__.h"
#endif

#include "shared-bindings/microcontroller/__init__.h"

#if CIRCUITPY_WATCHDOG
#include "shared-bindings/watchdog/__init__.h"
#define WATCHDOG_EXCEPTION_CHECK() (MP_STATE_VM(mp_pending_exception) == &mp_watchdog_timeout_exception)
#else
#define WATCHDOG_EXCEPTION_CHECK() 0
#endif

void supervisor_tick(void) {
#if CIRCUITPY_FILESYSTEM_FLUSH_INTERVAL_MS > 0
    filesystem_tick();
#endif
#ifdef CIRCUITPY_AUTORELOAD_DELAY_MS
    autoreload_tick();
#endif
#ifdef CIRCUITPY_GAMEPAD_TICKS
    if (!(port_get_raw_ticks(NULL) & CIRCUITPY_GAMEPAD_TICKS)) {
        #if CIRCUITPY_GAMEPAD
        gamepad_tick();
        #endif
        #if CIRCUITPY_GAMEPADSHIFT
        gamepadshift_tick();
        #endif
    }
#endif
}

uint64_t supervisor_ticks_ms64() {
    uint64_t result;
    common_hal_mcu_disable_interrupts();
    result = port_get_raw_ticks(NULL);
    common_hal_mcu_enable_interrupts();
    result = result * 1000 / 1024;
    return result;
}

uint32_t supervisor_ticks_ms32() {
    return supervisor_ticks_ms64();
}

extern void run_background_tasks(void);

void PLACE_IN_ITCM(supervisor_run_background_tasks_if_tick)() {
    // TODO: Add a global that can be set by anyone to indicate we should run background tasks. That
    // way we can short circuit the background tasks early. We used to do it based on time but it
    // breaks cases where we wake up for a short period and then sleep. If we skipped the last
    // background task or more before sleeping we may end up starving a task like USB.
    run_background_tasks();
}

void mp_hal_delay_ms(mp_uint_t delay) {
    uint64_t start_tick = port_get_raw_ticks(NULL);
    // Adjust the delay to ticks vs ms.
    delay = delay * 1024 / 1000;
    uint64_t end_tick = start_tick + delay;
    int64_t remaining = delay;
    while (remaining > 0) {
        RUN_BACKGROUND_TASKS;
        // Check to see if we've been CTRL-Ced by autoreload or the user.
        if(MP_STATE_VM(mp_pending_exception) == MP_OBJ_FROM_PTR(&MP_STATE_VM(mp_kbd_exception)) ||
           MP_STATE_VM(mp_pending_exception) == MP_OBJ_FROM_PTR(&MP_STATE_VM(mp_reload_exception)) ||
           WATCHDOG_EXCEPTION_CHECK()) {
            break;
        }
        remaining = end_tick - port_get_raw_ticks(NULL);
        // We break a bit early so we don't risk setting the alarm before the time when we call
        // sleep.
        if (remaining < 1) {
            break;
        }
        port_interrupt_after_ticks(remaining);
        // Sleep until an interrupt happens.
        port_sleep_until_interrupt();
        remaining = end_tick - port_get_raw_ticks(NULL);
    }
}

volatile size_t tick_enable_count = 0;
extern void supervisor_enable_tick(void) {
    common_hal_mcu_disable_interrupts();
    if (tick_enable_count == 0) {
        port_enable_tick();
    }
    tick_enable_count++;
    common_hal_mcu_enable_interrupts();
}

extern void supervisor_disable_tick(void) {
    common_hal_mcu_disable_interrupts();
    if (tick_enable_count > 0) {
        tick_enable_count--;
    }
    if (tick_enable_count == 0) {
        port_disable_tick();
    }
    common_hal_mcu_enable_interrupts();
}
