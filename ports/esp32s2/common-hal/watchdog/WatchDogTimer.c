/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 microDev
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

#include "esp_task_wdt.h"

void esp_task_wdt_isr_user_handler(void) {
    mp_obj_exception_clear_traceback(MP_OBJ_FROM_PTR(&mp_watchdog_timeout_exception));
    MP_STATE_VM(mp_pending_exception) = &mp_watchdog_timeout_exception;
    #if MICROPY_ENABLE_SCHEDULER
    if (MP_STATE_VM(sched_state) == MP_SCHED_IDLE) {
        MP_STATE_VM(sched_state) = MP_SCHED_PENDING;
    }
    #endif
}

void common_hal_watchdog_feed(watchdog_watchdogtimer_obj_t *self) {
    if (esp_task_wdt_reset() != ESP_OK) {
        mp_raise_RuntimeError(translate("watchdog not initialized"));
    }
}

void common_hal_watchdog_deinit(watchdog_watchdogtimer_obj_t *self) {
    if (esp_task_wdt_delete(NULL) == ESP_OK && esp_task_wdt_deinit() == ESP_OK) {
        self->mode = WATCHDOGMODE_NONE;
    }
}

void watchdog_reset(void) {
    common_hal_watchdog_deinit(&common_hal_mcu_watchdogtimer_obj);
}

static void wdt_config(watchdog_watchdogtimer_obj_t *self) {
    // enable panic hanler in WATCHDOGMODE_RESET mode
    // initialize Task Watchdog Timer (TWDT)
    if (esp_task_wdt_init((uint32_t)self->timeout, (self->mode == WATCHDOGMODE_RESET)) != ESP_OK) {
        mp_raise_RuntimeError(translate("Initialization failed due to lack of memory"));
    }
    esp_task_wdt_add(NULL);
}

mp_float_t common_hal_watchdog_get_timeout(watchdog_watchdogtimer_obj_t *self) {
    return self->timeout;
}

void common_hal_watchdog_set_timeout(watchdog_watchdogtimer_obj_t *self, mp_float_t new_timeout) {
    if ((uint64_t)new_timeout > UINT32_MAX) {
        mp_raise_ValueError(translate("timeout duration exceeded the maximum supported value"));
    }
    if ((uint32_t)self->timeout != (uint32_t)new_timeout) {
        self->timeout = new_timeout;
        wdt_config(self);
    }
}

watchdog_watchdogmode_t common_hal_watchdog_get_mode(watchdog_watchdogtimer_obj_t *self) {
    return self->mode;
}

void common_hal_watchdog_set_mode(watchdog_watchdogtimer_obj_t *self, watchdog_watchdogmode_t new_mode) {
    if (self->mode != new_mode) {
        self->mode = new_mode;
        wdt_config(self);
    }
}
