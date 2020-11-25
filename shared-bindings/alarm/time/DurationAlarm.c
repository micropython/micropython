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
#include "shared-bindings/alarm/time/DurationAlarm.h"

#include "py/nlr.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "supervisor/shared/translate.h"

//| class DurationAlarm:
//|     """Trigger an alarm at a specified interval from now."""
//|
//|     def __init__(self, secs: float) -> None:
//|         """Create an alarm that will be triggered in ``secs`` seconds from the time
//|         sleep starts. The alarm is not active until it is listed in an
//|         `alarm`-enabling function, such as `alarm.sleep_until_alarms()` or
//|         `alarm.set_deep_sleep_alarms()`.
//|         """
//|         ...
//|
STATIC mp_obj_t alarm_time_duration_alarm_make_new(const mp_obj_type_t *type,
        mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    mp_arg_check_num(n_args, kw_args, 1, 1, false);

    alarm_time_duration_alarm_obj_t *self = m_new_obj(alarm_time_duration_alarm_obj_t);
    self->base.type = &alarm_time_duration_alarm_type;

    mp_float_t secs = mp_obj_get_float(args[0]);

    common_hal_alarm_time_duration_alarm_construct(self, secs);

    return MP_OBJ_FROM_PTR(self);
}

//|     def __eq__(self, other: object) -> bool:
//|         """Two DurationAlarm objects are equal if their durations differ by less than a millisecond."""
//|         ...
//|
STATIC mp_obj_t alarm_time_duration_alarm_binary_op(mp_binary_op_t op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    switch (op) {
        case MP_BINARY_OP_EQUAL:
            if (MP_OBJ_IS_TYPE(rhs_in, &alarm_time_duration_alarm_type)) {
                return mp_obj_new_bool(
                    abs(common_hal_alarm_time_duration_alarm_get_duration(lhs_in) -
                        common_hal_alarm_time_duration_alarm_get_duration(rhs_in)) < 0.001f);
            }
            return mp_const_false;

        default:
            return MP_OBJ_NULL; // op not supported
    }
}

STATIC const mp_rom_map_elem_t alarm_time_duration_alarm_locals_dict_table[] = {
};

STATIC MP_DEFINE_CONST_DICT(alarm_time_duration_alarm_locals_dict, alarm_time_duration_alarm_locals_dict_table);

const mp_obj_type_t alarm_time_duration_alarm_type = {
    { &mp_type_type },
    .name = MP_QSTR_DurationAlarm,
    .make_new = alarm_time_duration_alarm_make_new,
    .binary_op = alarm_time_duration_alarm_binary_op,
    .locals_dict = (mp_obj_t)&alarm_time_duration_alarm_locals_dict,
};
