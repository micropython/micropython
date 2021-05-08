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

//| class WatchDogTimer:
//|     """Timer that is used to detect code lock ups and automatically reset the microcontroller
//|        when one is detected.
//|
//|        A lock up is detected when the watchdog hasn't been fed after a given duration. So, make
//|        sure to call `feed` within the timeout.
//|     """
//|

//|     def __init__(self) -> None:
//|         """Not currently dynamically supported. Access the sole instance through `microcontroller.watchdog`."""
//|         ...
//|

//|     def feed(self) -> None:
//|         """Feed the watchdog timer. This must be called regularly, otherwise
//|         the timer will expire."""
//|         ...
//|
STATIC mp_obj_t watchdog_watchdogtimer_feed(mp_obj_t self_in) {
    watchdog_watchdogtimer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    watchdog_watchdogmode_t current_mode = common_hal_watchdog_get_mode(self);

    if (current_mode == WATCHDOGMODE_NONE) {
        mp_raise_ValueError(translate("WatchDogTimer is not currently running"));
    }

    common_hal_watchdog_feed(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(watchdog_watchdogtimer_feed_obj, watchdog_watchdogtimer_feed);

//|     def deinit(self) -> None:
//|         """Stop the watchdog timer. This may raise an error if the watchdog
//|         timer cannot be disabled on this platform."""
//|         ...
//|
STATIC mp_obj_t watchdog_watchdogtimer_deinit(mp_obj_t self_in) {
    watchdog_watchdogtimer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_watchdog_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(watchdog_watchdogtimer_deinit_obj, watchdog_watchdogtimer_deinit);

//|     timeout: float
//|     """The maximum number of seconds that can elapse between calls
//|     to feed()"""
//|
STATIC mp_obj_t watchdog_watchdogtimer_obj_get_timeout(mp_obj_t self_in) {
    watchdog_watchdogtimer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_float(common_hal_watchdog_get_timeout(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(watchdog_watchdogtimer_get_timeout_obj, watchdog_watchdogtimer_obj_get_timeout);

STATIC mp_obj_t watchdog_watchdogtimer_obj_set_timeout(mp_obj_t self_in, mp_obj_t timeout_obj) {
    watchdog_watchdogtimer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_float_t timeout = mp_obj_get_float(timeout_obj);

    if (timeout <= 0) {
        mp_raise_ValueError(translate("watchdog timeout must be greater than 0"));
    }

    common_hal_watchdog_set_timeout(self, timeout);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(watchdog_watchdogtimer_set_timeout_obj, watchdog_watchdogtimer_obj_set_timeout);

const mp_obj_property_t watchdog_watchdogtimer_timeout_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&watchdog_watchdogtimer_get_timeout_obj,
              (mp_obj_t)&watchdog_watchdogtimer_set_timeout_obj,
              MP_ROM_NONE},
};

//|     mode: WatchDogMode
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
//|     Once set, the WatchDogTimer will perform the specified action if the timer expires."""
//|
STATIC mp_obj_t watchdog_watchdogtimer_obj_get_mode(mp_obj_t self_in) {
    watchdog_watchdogtimer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return watchdog_watchdogmode_type_to_obj(common_hal_watchdog_get_mode(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(watchdog_watchdogtimer_get_mode_obj, watchdog_watchdogtimer_obj_get_mode);

STATIC mp_obj_t watchdog_watchdogtimer_obj_set_mode(mp_obj_t self_in, mp_obj_t mode_obj) {
    watchdog_watchdogtimer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    watchdog_watchdogmode_t current_mode = common_hal_watchdog_get_mode(self);
    watchdog_watchdogmode_t new_mode = watchdog_watchdogmode_obj_to_type(mode_obj);
    mp_float_t current_timeout = common_hal_watchdog_get_timeout(self);

    // When setting the mode, the timeout value must be greater than zero
    if (new_mode == WATCHDOGMODE_RESET || new_mode == WATCHDOGMODE_RAISE) {
        if (current_timeout <= 0) {
            mp_raise_ValueError(translate("WatchDogTimer.timeout must be greater than 0"));
        }
    }

    // Don't allow changing the mode once the watchdog timer has been started
    if (current_mode == WATCHDOGMODE_RESET && new_mode != WATCHDOGMODE_RESET) {
        mp_raise_TypeError(translate("WatchDogTimer.mode cannot be changed once set to WatchDogMode.RESET"));
    }

    common_hal_watchdog_set_mode(self, new_mode);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(watchdog_watchdogtimer_set_mode_obj, watchdog_watchdogtimer_obj_set_mode);

const mp_obj_property_t watchdog_watchdogtimer_mode_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&watchdog_watchdogtimer_get_mode_obj,
              (mp_obj_t)&watchdog_watchdogtimer_set_mode_obj,
              MP_ROM_NONE},
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
    .locals_dict = (mp_obj_dict_t *)&watchdog_watchdogtimer_locals_dict,
};
