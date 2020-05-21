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

STATIC watchdog_watchdogtimer_obj_t *wdt_singleton;
STATIC mp_obj_t watchdog_watchdogtimer_feed(mp_obj_t self_in);
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

STATIC void watchdogtimer_event_handler(nrf_timer_event_t event_type, void *p_context) {
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

//| class WDT:
//|     """Watchdog Timer"""
//|
//|     def __init__(self, ):
//|         """This class represents the system's Watchdog Timer. It is a
//|         singleton and will always return the same instance.
//|
//| """
//|         ...
//|
STATIC mp_obj_t watchdog_watchdogtimer_make_new(const mp_obj_type_t *type, size_t n_args,
                                 const mp_obj_t *pos_args,
                                 mp_map_t *kw_args) {
    enum { ARG_timeout, ARG_sleep, ARG_hardware };
    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_timeout, MP_ARG_OBJ | MP_ARG_REQUIRED},
        {MP_QSTR_sleep, MP_ARG_BOOL, {.u_bool = false}},
        {MP_QSTR_hardware, MP_ARG_BOOL, {.u_bool = false}},
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_obj_exception_clear_traceback(MP_OBJ_FROM_PTR(&MP_STATE_VM(mp_watchdog_exception)));

    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args),
                     allowed_args, args);
    mp_float_t timeout = mp_obj_get_float(args[ARG_timeout].u_obj);
    bool hardware = args[ARG_hardware].u_bool;
    bool sleep = args[ARG_sleep].u_bool;

    // If the hardware timer is already running, return that timer.
    // If the parameters have changed, then ignore them, but print
    // an error.
    if (wdt_singleton && hardware) {
        if ((sleep != wdt_singleton->sleep)
         || (hardware != wdt_singleton->hardware)
         || fabsf(timeout - wdt_singleton->timeout) > 0.01f) {
            // Print a warning indicating things aren't quite right
            // mp_printf(&mp_stderr_print, translate("warning: hardware timer was already running"));
        }
        watchdogtimer_hardware_feed();
        return wdt_singleton;
    }

    if (timeout <= 0) {
        mp_raise_ValueError(translate("watchdog timeout must be greater than 0"));
    }

    watchdog_watchdogtimer_obj_t *self = m_new_obj(watchdog_watchdogtimer_obj_t);
    self->base.type = &watchdog_watchdogtimer_type;
    self->timeout = timeout;
    self->sleep = sleep;
    self->hardware = hardware;

    if (hardware) {
        watchdogtimer_hardware_init(self->timeout, self->sleep);
        wdt_singleton = self;
    } else {
        uint64_t ticks = timeout * 31250ULL;
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

        nrfx_timer_init(timer, &timer_config, &watchdogtimer_event_handler);

        // true enables interrupt.
        nrfx_timer_clear(timer);
        nrfx_timer_compare(timer, NRF_TIMER_CC_CHANNEL0, ticks, true);
        nrfx_timer_resume(timer);
    }

    // Feed the watchdog, in case there's a timer that's already running
    // and it's only partially finished.
    mp_obj_t *self_obj = MP_OBJ_FROM_PTR(self);
    watchdog_watchdogtimer_feed(self_obj);
    return self_obj;
}

//|     def feed(self):
//|         """Feed the watchdog timer. This must be called regularly, otherwise
//|         the system will reset."""
//|         ...
//|
STATIC mp_obj_t watchdog_watchdogtimer_feed(mp_obj_t self_in) {
    watchdog_watchdogtimer_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->hardware) {
        watchdogtimer_hardware_feed();
    } else {
        nrfx_timer_clear(timer);
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

    if (!self->hardware) {
        timer_refcount--;
        if (timer_refcount == 0) {
            nrf_peripherals_free_timer(timer);
            timer = NULL;
        }
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(watchdog_watchdogtimer_deinit_obj, watchdog_watchdogtimer_deinit);

//|     timeout: int = ...
//|     """The maximum number of milliseconds that can elapse between calls
//|     to feed()"""
//|
STATIC mp_obj_t watchdog_watchdogtimer_get_timeout(mp_obj_t self_in) {
    watchdog_watchdogtimer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_float(self->timeout);
}
MP_DEFINE_CONST_FUN_OBJ_1(watchdog_watchdogtimer_get_timeout_obj, watchdog_watchdogtimer_get_timeout);

const mp_obj_property_t watchdog_watchdogtimer_timeout_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&watchdog_watchdogtimer_get_timeout_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

mp_obj_t watchdog_watchdogtimer___enter__(mp_obj_t self_in) {
    watchdog_watchdogtimer_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (!self->hardware) {
        nrfx_timer_resume(timer);
    }
    watchdog_watchdogtimer_feed(self_in);
    return self_in;
}
MP_DEFINE_CONST_FUN_OBJ_1(watchdog_watchdogtimer___enter___obj, watchdog_watchdogtimer___enter__);

STATIC mp_obj_t watchdog_watchdogtimer___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    watchdog_watchdogtimer_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (!self->hardware) {
        if (timer) {
            nrfx_timer_pause(timer);
        }
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(watchdog_watchdogtimer___exit___obj, 4, 4, watchdog_watchdogtimer___exit__);

STATIC const mp_rom_map_elem_t watchdog_watchdogtimer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_feed), MP_ROM_PTR(&watchdog_watchdogtimer_feed_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&watchdog_watchdogtimer_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_timeout), MP_ROM_PTR(&watchdog_watchdogtimer_timeout_obj) },
    // { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mp_stream_close_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&watchdog_watchdogtimer___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&watchdog_watchdogtimer___exit___obj) },
};
STATIC MP_DEFINE_CONST_DICT(watchdog_watchdogtimer_locals_dict, watchdog_watchdogtimer_locals_dict_table);

const mp_obj_type_t watchdog_watchdogtimer_type = {
    { &mp_type_type },
    .name = MP_QSTR_WatchDogTimer,
    .make_new = watchdog_watchdogtimer_make_new,
    .locals_dict = (mp_obj_dict_t*)&watchdog_watchdogtimer_locals_dict,
};
