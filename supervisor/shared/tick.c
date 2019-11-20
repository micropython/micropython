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
#include "supervisor/filesystem.h"
#include "supervisor/shared/autoreload.h"

static volatile uint64_t ticks_ms;
static volatile uint32_t background_ticks_ms32;

#if CIRCUITPY_GAMEPAD
#include "shared-module/gamepad/__init__.h"
#endif

#if CIRCUITPY_GAMEPADSHIFT
#include "shared-module/gamepadshift/__init__.h"
#endif

#include "shared-bindings/microcontroller/__init__.h"

void supervisor_tick(void) {

    ticks_ms ++;

#if CIRCUITPY_FILESYSTEM_FLUSH_INTERVAL_MS > 0
    filesystem_tick();
#endif
#ifdef CIRCUITPY_AUTORELOAD_DELAY_MS
    autoreload_tick();
#endif
#ifdef CIRCUITPY_GAMEPAD_TICKS
    if (!(ticks_ms & CIRCUITPY_GAMEPAD_TICKS)) {
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
    result = ticks_ms;
    common_hal_mcu_enable_interrupts();
    return result;
}

uint32_t supervisor_ticks_ms32() {
    return ticks_ms;
}

extern void run_background_tasks(void);

void supervisor_run_background_tasks_if_tick() {
    uint32_t now32 = ticks_ms;

    if (now32 == background_ticks_ms32) {
        return;
    }
    background_ticks_ms32 = now32;

    run_background_tasks();
}

void supervisor_fake_tick() {
    uint32_t now32 = ticks_ms;
    background_ticks_ms32 = (now32 - 1);
}
