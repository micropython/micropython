/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Dan Halbert for Adafruit Industries
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

#include "shared-bindings/board/__init__.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/alarm/time/TimeAlarm.h"

#include "py/nlr.h"
#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "supervisor/shared/translate.h"

//| class TimeAlarm:
//|     """Trigger an alarm when `time.monotonic()` reaches the given value."""
//|
//|     def __init__(self, monotonic_time: float) -> None:
//|         """Create an alarm that will be triggered when `time.monotonic()` would equal
//|         ``monotonic_time``.
//|         The alarm is not active until it is passed to an
//|         `alarm`-enabling function, such as `alarm.sleep_until_alarms()` or
//|         `alarm.set_deep_sleep_alarms()`.
//|
//|         If the given time is in the past when sleep occurs, the alarm will be triggered
//|         immediately.
//|         """
//|         ...
//|
STATIC mp_obj_t alarm_time_time_alarm_make_new(const mp_obj_type_t *type,
        mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    mp_arg_check_num(n_args, kw_args, 1, 1, false);

    alarm_time_time_alarm_obj_t *self = m_new_obj(alarm_time_time_alarm_obj_t);
    self->base.type = &alarm_time_time_alarm_type;

    mp_float_t secs = mp_obj_get_float(args[0]);

    common_hal_alarm_time_time_alarm_construct(self, secs);

    return MP_OBJ_FROM_PTR(self);
}

//|     monotonic_time: float
//|     """The time at which to trigger, based on the `time.monotonic()` clock."""
//|
STATIC mp_obj_t alarm_time_time_alarm_obj_get_monotonic_time(mp_obj_t self_in) {
    alarm_time_time_alarm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_float(common_hal_alarm_time_time_alarm_get_monotonic_time(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(alarm_time_time_alarm_get_monotonic_time_obj, alarm_time_time_alarm_obj_get_monotonic_time);

const mp_obj_property_t alarm_time_time_alarm_monotonic_time_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&alarm_time_time_alarm_get_monotonic_time_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC const mp_rom_map_elem_t alarm_time_time_alarm_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_monotonic_time), MP_ROM_PTR(&alarm_time_time_alarm_monotonic_time_obj) },
};

STATIC MP_DEFINE_CONST_DICT(alarm_time_time_alarm_locals_dict, alarm_time_time_alarm_locals_dict_table);

const mp_obj_type_t alarm_time_time_alarm_type = {
    { &mp_type_type },
    .name = MP_QSTR_TimeAlarm,
    .make_new = alarm_time_time_alarm_make_new,
    .locals_dict = (mp_obj_t)&alarm_time_time_alarm_locals_dict,
};
