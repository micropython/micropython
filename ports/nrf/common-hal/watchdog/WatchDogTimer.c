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

#include "py/obj.h"
#include "py/runtime.h"

#include "common-hal/watchdog/WatchDogTimer.h"
#include "nrf_wdt.h"

#define WATCHDOG_RELOAD_COUNT 2

void common_hal_watchdog_init(uint32_t duration, bool pause_during_sleep) {
    unsigned int channel;
    nrf_wdt_behaviour_t behaviour = NRF_WDT_BEHAVIOUR_RUN_SLEEP_HALT;
    if (pause_during_sleep) {
        behaviour = NRF_WDT_BEHAVIOUR_PAUSE_SLEEP_HALT;
    }

    nrf_wdt_behaviour_set(NRF_WDT, behaviour);

    uint64_t ticks = (duration * 32768ULL) / 1000;
    if (ticks > UINT32_MAX) {
        mp_raise_ValueError(translate("timeout duration exceeded the maximum supported value"));
    }
    nrf_wdt_reload_value_set(NRF_WDT, (uint32_t) ticks);

    for (channel = 0; channel < WATCHDOG_RELOAD_COUNT; channel++) {
        nrf_wdt_reload_request_enable(NRF_WDT, channel);
    }

    nrf_wdt_task_trigger(NRF_WDT, NRF_WDT_TASK_START);
}

void common_hal_watchdog_feed(void) {
    unsigned int channel;
    for (channel = 0; channel < WATCHDOG_RELOAD_COUNT; channel++) {
        nrf_wdt_reload_request_set(NRF_WDT, (nrf_wdt_rr_register_t)(NRF_WDT_RR0 + channel));
    }
}

void common_hal_watchdog_disable(void) {
    // mp_raise_ValueError(translate("Watchdog timer cannot be disabled -- board will reset shortly"));
}

/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Paul Sokolovsky
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

#include <string.h>

#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/watchdog/__init__.h"
#include "shared-bindings/watchdog/WatchDogTimer.h"

static watchdog_watchdogtimer_obj_t *wdt_singleton;

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
    enum { ARG_timeout_ms, ARG_sleep };
    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_timeout_ms, MP_ARG_INT | MP_ARG_REQUIRED},
        {MP_QSTR_sleep, MP_ARG_BOOL, {.u_bool = false}},
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];

    if (wdt_singleton)
        return wdt_singleton;

    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args),
                     allowed_args, args);

    if (args[ARG_timeout_ms].u_int <= 0) {
        mp_raise_ValueError(translate("watchdog timeout must be greater than 0"));
    }

    watchdog_watchdogtimer_obj_t *self = m_new_obj(watchdog_watchdogtimer_obj_t);
    self->base.type = &watchdog_watchdogtimer_type;
    self->timeout = args[ARG_timeout_ms].u_int;
    self->sleep = args[ARG_sleep].u_bool;

    common_hal_watchdog_init(self->timeout, self->sleep);
    wdt_singleton = self;
    return MP_OBJ_FROM_PTR(self);
}

//|     def feed(self):
//|         """Feed the watchdog timer. This must be called regularly, otherwise
//|         the system will reset."""
//|         ...
//|
STATIC mp_obj_t watchdog_watchdogtimer_feed(mp_obj_t self_in) {
    (void)self_in;
    common_hal_watchdog_feed();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(watchdog_watchdogtimer_feed_obj, watchdog_watchdogtimer_feed);

//|     def deinit(self):
//|         """Stop the watchdog timer. This may raise an error if the watchdog
//|         timer cannot be disabled on this platform."""
//|         ...
//|
STATIC mp_obj_t watchdog_watchdogtimer_deinit(mp_obj_t self_in) {
    (void)self_in;
    common_hal_watchdog_disable();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(watchdog_watchdogtimer_deinit_obj, watchdog_watchdogtimer_deinit);

//|     timeout: int = ...
//|     """The maximum number of milliseconds that can elapse between calls
//|     to feed()"""
//|
STATIC mp_obj_t watchdog_watchdogtimer_obj_get_timeout(mp_obj_t self_in) {
    watchdog_watchdogtimer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(self->timeout);
}
MP_DEFINE_CONST_FUN_OBJ_1(watchdog_watchdogtimer_obj_get_timeout_obj, watchdog_watchdogtimer_obj_get_timeout);

const mp_obj_property_t watchdog_watchdogtimer_timeout_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&watchdog_watchdogtimer_obj_get_timeout_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC const mp_rom_map_elem_t watchdog_watchdogtimer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_feed), MP_ROM_PTR(&watchdog_watchdogtimer_feed_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&watchdog_watchdogtimer_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_timeout), MP_ROM_PTR(&watchdog_watchdogtimer_timeout_obj) },
};
STATIC MP_DEFINE_CONST_DICT(watchdog_watchdogtimer_locals_dict, watchdog_watchdogtimer_locals_dict_table);

const mp_obj_type_t watchdog_watchdogtimer_type = {
    { &mp_type_type },
    .name = MP_QSTR_WatchDogTimer,
    .make_new = watchdog_watchdogtimer_make_new,
    .locals_dict = (mp_obj_dict_t*)&watchdog_watchdogtimer_locals_dict,
};
