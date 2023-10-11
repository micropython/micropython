/*
 * This file is part of Adafruit for EFR32 project
 *
 * The MIT License (MIT)
 *
 * Copyright 2023 Silicon Laboratories Inc. www.silabs.com
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

#include "py/runtime.h"
#include "common-hal/watchdog/WatchDogTimer.h"

#include "shared-bindings/watchdog/__init__.h"
#include "shared-bindings/microcontroller/__init__.h"

#include "em_wdog.h"
#include "em_cmu.h"

void common_hal_watchdog_feed(watchdog_watchdogtimer_obj_t *self) {
    WDOGn_Feed(DEFAULT_WDOG);
}

void common_hal_watchdog_deinit(watchdog_watchdogtimer_obj_t *self) {
    WDOG_Enable(false);
}

mp_float_t common_hal_watchdog_get_timeout(watchdog_watchdogtimer_obj_t *self) {
    return self->timeout;
}

void common_hal_watchdog_set_timeout(watchdog_watchdogtimer_obj_t *self,
    mp_float_t new_timeout) {

    // Max timeout is 256000 ms
    uint64_t timeout = new_timeout * 1000;
    mp_arg_validate_int_max(timeout, 256000, MP_QSTR_WatchDogTimeout);

    if ((uint32_t)self->timeout != (uint32_t)new_timeout) {

        // Watchdog Initialize settings
        WDOG_Init_TypeDef wdogInit = WDOG_INIT_DEFAULT;

        switch ((uint32_t)new_timeout)
        {
            case 1:
                wdogInit.perSel = wdogPeriod_1k;
                break;
            case 2:
                wdogInit.perSel = wdogPeriod_2k;
                break;
            case 4:
                wdogInit.perSel = wdogPeriod_4k;
                break;
            case 8:
                wdogInit.perSel = wdogPeriod_8k;
                break;
            case 16:
                wdogInit.perSel = wdogPeriod_16k;
                break;
            case 32:
                wdogInit.perSel = wdogPeriod_32k;
                break;
            case 64:
                wdogInit.perSel = wdogPeriod_64k;
                break;
            case 128:
                wdogInit.perSel = wdogPeriod_128k;
                break;
            case 256:
                wdogInit.perSel = wdogPeriod_256k;
                break;
            default:
                mp_raise_ValueError(
                    translate("Timeout value supported: 1,2,4,8,16,32,64,128,256"));

        }

        self->timeout = new_timeout;
        // Enable clock for the WDOG module; has no effect on xG21
        CMU_ClockEnable(cmuClock_WDOG0, true);

        // ULFRCO as clock source
        CMU_ClockSelectSet(cmuClock_WDOG0, cmuSelect_ULFRCO);

        wdogInit.em1Run = true;
        wdogInit.em2Run = true;
        wdogInit.em3Run = true;

        // Initializing watchdog with chosen settings
        WDOGn_Init(DEFAULT_WDOG, &wdogInit);
    }
}

watchdog_watchdogmode_t common_hal_watchdog_get_mode
    (watchdog_watchdogtimer_obj_t *self) {
    return self->mode;
}

void common_hal_watchdog_set_mode(watchdog_watchdogtimer_obj_t *self,
    watchdog_watchdogmode_t new_mode) {
    if (self->mode != new_mode) {
        if (new_mode == WATCHDOGMODE_RAISE) {
            mp_raise_NotImplementedError(
                translate("RAISE mode is not implemented"));
        } else if (new_mode == WATCHDOGMODE_NONE) {
            self->mode = WATCHDOGMODE_NONE;
            common_hal_watchdog_deinit(self);
        }
        self->mode = new_mode;
        common_hal_watchdog_set_timeout(self, self->timeout);
    }
}
