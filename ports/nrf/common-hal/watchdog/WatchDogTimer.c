/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Nick Moore for Adafruit Industries
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
#include <string.h>

#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"

#include "common-hal/watchdog/WatchDogTimer.h"

#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/watchdog/__init__.h"
#include "shared-bindings/watchdog/WatchDogTimer.h"

#include "supervisor/port.h"

#include "nrf/timers.h"
#include "nrf_wdt.h"
#include "nrfx_wdt.h"
#include "nrfx_timer.h"

STATIC uint8_t timer_refcount = 0;
STATIC nrfx_timer_t *timer = NULL;
STATIC nrfx_wdt_t wdt = NRFX_WDT_INSTANCE(0);
STATIC nrfx_wdt_channel_id wdt_channel_id;

STATIC void watchdogtimer_timer_event_handler(nrf_timer_event_t event_type, void *p_context) {
    watchdog_watchdogtimer_obj_t *self = MP_OBJ_TO_PTR(p_context);
    if (event_type != NRF_TIMER_EVENT_COMPARE0) {
        // Spurious event.
        return;
    }

    // If the timer hits without being cleared, pause the timer and raise an exception.
    nrfx_timer_pause(timer);
    self->mode = WATCHDOGMODE_NONE;
    mp_obj_exception_clear_traceback(MP_OBJ_FROM_PTR(&mp_watchdog_timeout_exception));
    MP_STATE_VM(mp_pending_exception) = &mp_watchdog_timeout_exception;
    #if MICROPY_ENABLE_SCHEDULER
    if (MP_STATE_VM(sched_state) == MP_SCHED_IDLE) {
        MP_STATE_VM(sched_state) = MP_SCHED_PENDING;
    }
    #endif
}

static void timer_free(void) {
    timer_refcount--;
    if (timer_refcount == 0) {
        nrf_peripherals_free_timer(timer);
        timer = NULL;
    }
}

// This function is called if the timer expires. The system will reboot
// in 1/16384 of a second. Issue a reboot ourselves so we can do any
// cleanup necessary.
STATIC void watchdogtimer_watchdog_event_handler(void) {
    reset_cpu();
}

void common_hal_watchdog_feed(watchdog_watchdogtimer_obj_t *self) {
    if (self->mode == WATCHDOGMODE_RESET) {
        nrfx_wdt_feed(&wdt);
    } else if (self->mode == WATCHDOGMODE_RAISE) {
        nrfx_timer_clear(timer);
    }
}

void common_hal_watchdog_deinit(watchdog_watchdogtimer_obj_t *self) {
    if (self->mode == WATCHDOGMODE_RESET) {
        mp_raise_NotImplementedError(translate("WatchDogTimer cannot be deinitialized once mode is set to RESET"));
    }
    if (timer) {
        timer_free();
    }
    self->mode = WATCHDOGMODE_NONE;
}

void watchdog_reset(void) {
    common_hal_watchdog_deinit(&common_hal_mcu_watchdogtimer_obj);
}

mp_float_t common_hal_watchdog_get_timeout(watchdog_watchdogtimer_obj_t *self) {
    return self->timeout;
}

void common_hal_watchdog_set_timeout(watchdog_watchdogtimer_obj_t *self, mp_float_t timeout) {
    if (self->mode == WATCHDOGMODE_RAISE) {
        // If the WatchDogTimer is already running in "RAISE" mode, reset the timer
        // with the new value.
        uint64_t ticks = timeout * 31250ULL;
        if (ticks > UINT32_MAX) {
            mp_raise_ValueError(translate("timeout duration exceeded the maximum supported value"));
        }
        nrfx_timer_clear(timer);
        nrfx_timer_compare(timer, NRF_TIMER_CC_CHANNEL0, ticks, true);
    }

    self->timeout = timeout;
}

watchdog_watchdogmode_t common_hal_watchdog_get_mode(watchdog_watchdogtimer_obj_t *self) {
    return self->mode;
}

void common_hal_watchdog_set_mode(watchdog_watchdogtimer_obj_t *self, watchdog_watchdogmode_t new_mode) {
    watchdog_watchdogmode_t current_mode = self->mode;

    if (new_mode == WATCHDOGMODE_RAISE) {
        if (timer_refcount == 0) {
            timer = nrf_peripherals_allocate_timer_or_throw();
        }
        timer_refcount++;

        nrfx_timer_config_t timer_config = {
            .frequency = NRF_TIMER_FREQ_31250Hz,
            .mode = NRF_TIMER_MODE_TIMER,
            .bit_width = NRF_TIMER_BIT_WIDTH_32,
            .interrupt_priority = NRFX_TIMER_DEFAULT_CONFIG_IRQ_PRIORITY,
            .p_context = self,
        };

        nrfx_timer_init(timer, &timer_config, &watchdogtimer_timer_event_handler);

        uint64_t ticks = nrfx_timer_ms_to_ticks(timer, self->timeout * 1000);
        if (ticks > UINT32_MAX) {
            mp_raise_ValueError(translate("timeout duration exceeded the maximum supported value"));
        }

        // true enables interrupt.
        nrfx_timer_clear(timer);
        nrfx_timer_compare(timer, NRF_TIMER_CC_CHANNEL0, ticks, true);
        nrfx_timer_resume(timer);

    } else if (new_mode == WATCHDOGMODE_RESET) {
        uint64_t ticks = self->timeout * 1000.0f;
        if (ticks > UINT32_MAX) {
            mp_raise_ValueError(translate("timeout duration exceeded the maximum supported value"));
        }

        nrfx_wdt_config_t config = {
            .reload_value = ticks, // in units of ms
            .behaviour = NRF_WDT_BEHAVIOUR_RUN_SLEEP,
            NRFX_WDT_IRQ_CONFIG
        };

        nrfx_err_t err_code;
        err_code = nrfx_wdt_init(&wdt, &config, watchdogtimer_watchdog_event_handler);
        if (err_code != NRFX_SUCCESS) {
            mp_raise_OSError(1);
        }
        err_code = nrfx_wdt_channel_alloc(&wdt, &wdt_channel_id);
        if (err_code != NRFX_SUCCESS) {
            mp_raise_OSError(1);
        }
        nrfx_wdt_enable(&wdt);
        nrfx_wdt_feed(&wdt);
    }

    // If we just switched away from RAISE, disable the timmer.
    if (current_mode == WATCHDOGMODE_RAISE && new_mode != WATCHDOGMODE_RAISE) {
        timer_free();
    }

    self->mode = new_mode;
}
