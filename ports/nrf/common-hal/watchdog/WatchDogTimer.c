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

#include "shared-bindings/watchdog/__init__.h"
#include "shared-bindings/watchdog/WatchDogTimer.h"

#include "nrf_wdt.h"
#include "nrfx_timer.h"
#include "nrf/timers.h"

STATIC uint8_t timer_refcount = 0;
#define WATCHDOG_RELOAD_COUNT 2
STATIC nrfx_timer_t *timer = NULL;

STATIC void watchdogtimer_hardware_init(mp_float_t duration, bool pause_during_sleep) {
    unsigned int channel;
    nrf_wdt_behaviour_t behaviour = NRF_WDT_BEHAVIOUR_RUN_SLEEP_HALT;
    if (pause_during_sleep) {
        behaviour = NRF_WDT_BEHAVIOUR_PAUSE_SLEEP_HALT;
    }

    nrf_wdt_behaviour_set(NRF_WDT, behaviour);

    uint64_t ticks = duration * 32768ULL;
    if (ticks > UINT32_MAX) {
        mp_raise_ValueError(translate("timeout duration exceeded the maximum supported value"));
    }
    nrf_wdt_reload_value_set(NRF_WDT, (uint32_t) ticks);

    for (channel = 0; channel < WATCHDOG_RELOAD_COUNT; channel++) {
        nrf_wdt_reload_request_enable(NRF_WDT, channel);
    }

    nrf_wdt_task_trigger(NRF_WDT, NRF_WDT_TASK_START);
}

STATIC void watchdogtimer_hardware_feed(void) {
    unsigned int channel;
    for (channel = 0; channel < WATCHDOG_RELOAD_COUNT; channel++) {
        nrf_wdt_reload_request_set(NRF_WDT, (nrf_wdt_rr_register_t)(NRF_WDT_RR0 + channel));
    }
}

NORETURN void mp_raise_WatchDogTimeout(void) {
    nlr_raise(MP_OBJ_FROM_PTR(&MP_STATE_VM(mp_watchdog_exception)));
}

STATIC void watchdogtimer_timer_event_handler(nrf_timer_event_t event_type, void *p_context) {
    (void)p_context;
    if (event_type != NRF_TIMER_EVENT_COMPARE0) {
        // Spurious event.
        return;
    }

    // If the timer hits without being cleared, pause the timer and raise an exception.
    nrfx_timer_pause(timer);
    MP_STATE_VM(mp_pending_exception) = MP_OBJ_FROM_PTR(&MP_STATE_VM(mp_watchdog_exception));
#if MICROPY_ENABLE_SCHEDULER
    if (MP_STATE_VM(sched_state) == MP_SCHED_IDLE) {
        MP_STATE_VM(sched_state) = MP_SCHED_PENDING;
    }
#endif
}

void watchdog_watchdogtimer_reset(void) {
    if (timer != NULL) {
        nrf_peripherals_free_timer(timer);
    }
    timer = NULL;
    timer_refcount = 0;
}

//|     def feed(self):
//|         """Feed the watchdog timer. This must be called regularly, otherwise
//|         the timer will expire."""
//|         ...
//|
STATIC mp_obj_t watchdog_watchdogtimer_feed(mp_obj_t self_in) {
    watchdog_watchdogtimer_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->mode == WATCHDOGMODE_RESET) {
        watchdogtimer_hardware_feed();
    } else if (self->mode == WATCHDOGMODE_RAISE) {
        nrfx_timer_clear(timer);
    } else if (self->mode == WATCHDOGMODE_NONE) {
        mp_raise_ValueError(translate("WatchDogTimer is not currently running"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(watchdog_watchdogtimer_feed_obj, watchdog_watchdogtimer_feed);

//|     def deinit(self):
//|         """Stop the watchdog timer. This may raise an error if the watchdog
//|         timer cannot be disabled on this platform."""
//|         ...
//|
STATIC mp_obj_t watchdog_watchdogtimer_deinit(mp_obj_t self_in) {
    watchdog_watchdogtimer_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->mode == WATCHDOGMODE_RAISE) {
        timer_refcount--;
        if (timer_refcount == 0) {
            nrf_peripherals_free_timer(timer);
            timer = NULL;
        }
    } else if (self->mode == WATCHDOGMODE_RESET) {
        mp_raise_NotImplementedError(translate("WatchDogTimer cannot be deinitialized once mode is set to RESET"));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(watchdog_watchdogtimer_deinit_obj, watchdog_watchdogtimer_deinit);

//|     timeout: float = ...
//|     """The maximum number of seconds that can elapse between calls
//|     to feed()"""
//|
STATIC mp_obj_t watchdog_watchdogtimer_obj_get_timeout(mp_obj_t self_in) {
    watchdog_watchdogtimer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_float(self->timeout);
}
MP_DEFINE_CONST_FUN_OBJ_1(watchdog_watchdogtimer_get_timeout_obj, watchdog_watchdogtimer_obj_get_timeout);

STATIC mp_obj_t watchdog_watchdogtimer_obj_set_timeout(mp_obj_t self_in, mp_obj_t timeout_obj) {
    watchdog_watchdogtimer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_float_t timeout = mp_obj_get_float(timeout_obj);

    if (timeout <= 0) {
        mp_raise_ValueError(translate("watchdog timeout must be greater than 0"));
    }

    if (self->mode == WATCHDOGMODE_RESET) {
        // If the WatchDogTimer is already running in "RESET" mode, raise an error
        // since the mode cannot be changed once started.
        mp_raise_TypeError(translate("Cannot change the timeout once mode is WatchDogMode.RESET"));
    } else if (self->mode == WATCHDOGMODE_RAISE) {
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
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(watchdog_watchdogtimer_set_timeout_obj, watchdog_watchdogtimer_obj_set_timeout);

const mp_obj_property_t watchdog_watchdogtimer_timeout_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&watchdog_watchdogtimer_get_timeout_obj,
              (mp_obj_t)&watchdog_watchdogtimer_set_timeout_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|     mode: watchdog.WatchDogMode = ...
//|     """The current operating mode of the WatchDogTimer `watchdog.WatchDogMode`.
//|
//|     Setting a WatchDogMode activates the WatchDog::
//|
//|       import microcontroller
//|       import watchdog
//|
//|       w = microcontroller.watchdog
//|       w.timeout = 5
//|       w.mode = watchdog.WatchDogMode.RAISE
//|
//|
//|     Once set, the WatchDogTimer will perform the specified action if the timer expires.
//|
STATIC mp_obj_t watchdog_watchdogtimer_obj_get_mode(mp_obj_t self_in) {
    watchdog_watchdogtimer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    switch (self->mode) {
        case WATCHDOGMODE_NONE: default: return (mp_obj_t)MP_ROM_PTR(&watchdog_watchdogmode_none_obj);
        case WATCHDOGMODE_RAISE: return (mp_obj_t)MP_ROM_PTR(&watchdog_watchdogmode_raise_obj);
        case WATCHDOGMODE_RESET: return (mp_obj_t)MP_ROM_PTR(&watchdog_watchdogmode_reset_obj);
    }
}
MP_DEFINE_CONST_FUN_OBJ_1(watchdog_watchdogtimer_get_mode_obj, watchdog_watchdogtimer_obj_get_mode);

STATIC mp_obj_t watchdog_watchdogtimer_obj_set_mode(mp_obj_t self_in, mp_obj_t mode_obj) {
    watchdog_watchdogtimer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    watchdog_watchdogmode_obj_t *mode = MP_OBJ_TO_PTR(mode_obj);
    if (mode == MP_ROM_PTR(&watchdog_watchdogmode_none_obj)) {
        if (self->mode == WATCHDOGMODE_RESET) {
            mp_raise_TypeError(translate("WatchDogTimer mode cannot be changed once set to WatchDogMode.RESET"));
        }
        else if (self->mode == WATCHDOGMODE_RAISE) {
            timer_refcount--;
            if (timer_refcount == 0) {
                nrf_peripherals_free_timer(timer);
                timer = NULL;
            }
        }
        self->mode = WATCHDOGMODE_NONE;

    } else if (mode == MP_ROM_PTR(&watchdog_watchdogmode_raise_obj)) {
        if (self->timeout <= 0) {
            mp_raise_ValueError(translate("watchdog timeout must be greater than 0"));
        }
        if (self->mode == WATCHDOGMODE_RESET) {
            mp_raise_ValueError(translate("WatchDogTimer mode cannot be changed once set to WatchDogMode.RESET"));
        }
        else if (self->mode == WATCHDOGMODE_NONE) {
            uint64_t ticks = self->timeout * 31250ULL;
            if (ticks > UINT32_MAX) {
                mp_raise_ValueError(translate("timeout duration exceeded the maximum supported value"));
            }

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

            // true enables interrupt.
            nrfx_timer_clear(timer);
            nrfx_timer_compare(timer, NRF_TIMER_CC_CHANNEL0, ticks, true);
            nrfx_timer_resume(timer);
        }
        self->mode = WATCHDOGMODE_RAISE;

    } else if (mode == MP_ROM_PTR(&watchdog_watchdogmode_reset_obj)) {
        if (self->timeout <= 0) {
            mp_raise_ValueError(translate("watchdog timeout must be greater than 0"));
        }
        if (self->mode == WATCHDOGMODE_RAISE) {
            timer_refcount--;
            if (timer_refcount == 0) {
                nrf_peripherals_free_timer(timer);
                timer = NULL;
            }
        }
        watchdogtimer_hardware_init(self->timeout, self->sleep);
        self->mode = WATCHDOGMODE_RESET;
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(watchdog_watchdogtimer_set_mode_obj, watchdog_watchdogtimer_obj_set_mode);

const mp_obj_property_t watchdog_watchdogtimer_mode_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&watchdog_watchdogtimer_get_mode_obj,
              (mp_obj_t)&watchdog_watchdogtimer_set_mode_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC const mp_rom_map_elem_t watchdog_watchdogtimer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_feed), MP_ROM_PTR(&watchdog_watchdogtimer_feed_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&watchdog_watchdogtimer_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_timeout), MP_ROM_PTR(&watchdog_watchdogtimer_timeout_obj) },
    { MP_ROM_QSTR(MP_QSTR_mode), MP_ROM_PTR(&watchdog_watchdogtimer_mode_obj) },
};
STATIC MP_DEFINE_CONST_DICT(watchdog_watchdogtimer_locals_dict, watchdog_watchdogtimer_locals_dict_table);

const mp_obj_type_t watchdog_watchdogtimer_type = {
    { &mp_type_type },
    .name = MP_QSTR_WatchDogTimer,
    // .make_new = watchdog_watchdogtimer_make_new,
    .locals_dict = (mp_obj_dict_t*)&watchdog_watchdogtimer_locals_dict,
};
