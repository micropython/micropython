/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Jeff Epler for Adafruit Industries
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

#include <math.h>

#include "py/runtime.h"

#include "shared-bindings/watchdog/__init__.h"
#include "shared-bindings/watchdog/WatchDogTimer.h"
#include "common-hal/watchdog/WatchDogTimer.h"

#include "component/wdt.h"

void common_hal_watchdog_feed(watchdog_watchdogtimer_obj_t *self) {
    WDT->CLEAR.reg = WDT_CLEAR_CLEAR_KEY;
}

void common_hal_watchdog_deinit(watchdog_watchdogtimer_obj_t *self) {
    if (self->mode == WATCHDOGMODE_RESET) {
        mp_raise_RuntimeError(translate("WatchDogTimer cannot be deinitialized once mode is set to RESET"));
    } else {
        self->mode = WATCHDOGMODE_NONE;
    }
}

mp_float_t common_hal_watchdog_get_timeout(watchdog_watchdogtimer_obj_t *self) {
    return self->timeout;
}

STATIC void setup_wdt(watchdog_watchdogtimer_obj_t *self, int setting) {
    OSC32KCTRL->OSCULP32K.bit.EN1K = 1;  // Enable out 1K (for WDT)

    // disable watchdog for config
    WDT->CTRLA.reg = 0;
    while (WDT->SYNCBUSY.reg) { // Sync CTRL write
    }

    WDT->INTENCLR.reg = WDT_INTENCLR_EW;   // Disable early warning interrupt
    WDT->CONFIG.bit.PER = setting; // Set period for chip reset
    WDT->CTRLA.bit.WEN = 0;     // Disable window mode
    while (WDT->SYNCBUSY.reg) { // Sync CTRL write
    }
    common_hal_watchdog_feed(self); // Clear watchdog interval
    WDT->CTRLA.bit.ENABLE = 1; // Start watchdog now!
    while (WDT->SYNCBUSY.reg) {
    }
}

void common_hal_watchdog_set_timeout(watchdog_watchdogtimer_obj_t *self, mp_float_t new_timeout) {
    int wdt_cycles = (int)(new_timeout * 1024);
    if (wdt_cycles < 8) {
        wdt_cycles = 8;
    }
    if (wdt_cycles > 16384) {
        mp_raise_ValueError(translate("timeout duration exceeded the maximum supported value"));
    }
    // ceil(log2(n)) = 32 - __builtin_clz(n - 1) when n > 1 (if int is 32 bits)
    int log2_wdt_cycles = (sizeof(int) * CHAR_BIT) - __builtin_clz(wdt_cycles - 1);
    int setting = log2_wdt_cycles - 3; // CYC8_Val is 0
    float timeout = (8 << setting) / 1024.f;

    if (self->mode == WATCHDOGMODE_RESET) {
        setup_wdt(self, setting);
    }
    self->timeout = timeout;
}

watchdog_watchdogmode_t common_hal_watchdog_get_mode(watchdog_watchdogtimer_obj_t *self) {
    return self->mode;
}

void common_hal_watchdog_set_mode(watchdog_watchdogtimer_obj_t *self, watchdog_watchdogmode_t new_mode) {
    if (self->mode != new_mode) {
        if (new_mode == WATCHDOGMODE_RAISE) {
            mp_raise_NotImplementedError(translate("RAISE mode is not implemented"));
        } else if (new_mode == WATCHDOGMODE_NONE) {
            common_hal_watchdog_deinit(self);
        }
        self->mode = new_mode;
        common_hal_watchdog_set_timeout(self, self->timeout);
    }
}
