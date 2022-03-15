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

#include "shared/runtime/interrupt_char.h"
#include "py/mphal.h"
#include "py/mpstate.h"
#include "py/runtime.h"
#include "supervisor/linker.h"
#include "supervisor/filesystem.h"
#include "supervisor/background_callback.h"
#include "supervisor/port.h"
#include "supervisor/shared/stack.h"

#if CIRCUITPY_BLEIO_HCI
#include "common-hal/_bleio/__init__.h"
#endif

#if CIRCUITPY_DISPLAYIO
#include "shared-module/displayio/__init__.h"
#endif

#if CIRCUITPY_GAMEPADSHIFT
#include "shared-module/gamepadshift/__init__.h"
#endif

#if CIRCUITPY_KEYPAD
#include "shared-module/keypad/__init__.h"
#endif

#include "shared-bindings/microcontroller/__init__.h"

#if CIRCUITPY_WATCHDOG
#include "shared-bindings/watchdog/__init__.h"
#define WATCHDOG_EXCEPTION_CHECK() (MP_STATE_VM(mp_pending_exception) == &mp_watchdog_timeout_exception)
#else
#define WATCHDOG_EXCEPTION_CHECK() 0
#endif

static volatile uint64_t PLACE_IN_DTCM_BSS(background_ticks);

static background_callback_t tick_callback;

static volatile uint64_t last_finished_tick = 0;

static volatile size_t tick_enable_count = 0;

static void supervisor_background_tasks(void *unused) {
    port_start_background_task();

    assert_heap_ok();

    #if CIRCUITPY_BLEIO_HCI
    bleio_hci_background();
    #endif

    #if CIRCUITPY_DISPLAYIO
    displayio_background();
    #endif

    filesystem_background();

    port_background_task();

    assert_heap_ok();

    last_finished_tick = port_get_raw_ticks(NULL);

    port_finish_background_task();
}

bool supervisor_background_tasks_ok(void) {
    return port_get_raw_ticks(NULL) - last_finished_tick < 1024;
}

void supervisor_tick(void) {
    #if CIRCUITPY_FILESYSTEM_FLUSH_INTERVAL_MS > 0
    filesystem_tick();
    #endif

    #ifdef CIRCUITPY_GAMEPAD_TICKS
    if (!(port_get_raw_ticks(NULL) & CIRCUITPY_GAMEPAD_TICKS)) {
        #if CIRCUITPY_GAMEPADSHIFT
        gamepadshift_tick();
        #endif
    }
    #endif

    #if CIRCUITPY_KEYPAD
    keypad_tick();
    #endif

    background_callback_add(&tick_callback, supervisor_background_tasks, NULL);
}

uint64_t supervisor_ticks_ms64() {
    uint64_t result;
    result = port_get_raw_ticks(NULL);
    result = result * 1000 / 1024;
    return result;
}

uint32_t supervisor_ticks_ms32() {
    return supervisor_ticks_ms64();
}


void PLACE_IN_ITCM(supervisor_run_background_tasks_if_tick)() {
    background_callback_run_all();
}

void mp_hal_delay_ms(mp_uint_t delay_ms) {
    uint64_t start_tick = port_get_raw_ticks(NULL);
    // Adjust the delay to ticks vs ms.
    uint64_t delay_ticks = (delay_ms * (uint64_t)1024) / 1000;
    uint64_t end_tick = start_tick + delay_ticks;
    int64_t remaining = delay_ticks;

    // Loop until we've waited long enough or we've been CTRL-Ced by autoreload
    // or the user.
    while (remaining > 0 && !mp_hal_is_interrupted()) {
        RUN_BACKGROUND_TASKS;
        remaining = end_tick - port_get_raw_ticks(NULL);
        // We break a bit early so we don't risk setting the alarm before the time when we call
        // sleep.
        if (remaining < 1) {
            break;
        }
        port_interrupt_after_ticks(remaining);
        // Idle until an interrupt happens.
        port_idle_until_interrupt();
        remaining = end_tick - port_get_raw_ticks(NULL);
    }
}

void supervisor_enable_tick(void) {
    common_hal_mcu_disable_interrupts();
    if (tick_enable_count == 0) {
        port_enable_tick();
    }
    tick_enable_count++;
    common_hal_mcu_enable_interrupts();
}

void supervisor_disable_tick(void) {
    common_hal_mcu_disable_interrupts();
    if (tick_enable_count > 0) {
        tick_enable_count--;
    }
    if (tick_enable_count == 0) {
        port_disable_tick();
    }
    common_hal_mcu_enable_interrupts();
}
