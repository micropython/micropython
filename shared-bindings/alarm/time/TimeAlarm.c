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

#include "py/nlr.h"
#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"

#include "shared-bindings/time/__init__.h"
#include "shared-bindings/alarm/time/TimeAlarm.h"

#include "supervisor/shared/translate.h"

#if MICROPY_LONGINT_IMPL != MICROPY_LONGINT_IMPL_NONE
mp_obj_t MP_WEAK rtc_get_time_source_time(void) {
    mp_raise_RuntimeError(translate("RTC is not supported on this board"));
}
#endif

//| class TimeAlarm:
//|     """Trigger an alarm when the specified time is reached."""
//|
//|     def __init__(self, monotonic_time: Optional[float] = None, epoch_time: Optional[int] = None) -> None:
//|         """Create an alarm that will be triggered when `time.monotonic()` would equal
//|         ``monotonic_time``, or when `time.time()` would equal ``epoch_time``.
//|         Only one of the two arguments can be given.
//|         The alarm is not active until it is passed to an
//|         `alarm`-enabling function, such as `alarm.light_sleep_until_alarms()` or
//|         `alarm.exit_and_deep_sleep_until_alarms()`.
//|
//|         If the given time is in the past when sleep occurs, the alarm will be triggered
//|         immediately.
//|         """
//|         ...
//|
STATIC mp_obj_t alarm_time_timealarm_make_new(const mp_obj_type_t *type,
    mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    alarm_time_timealarm_obj_t *self = m_new_obj(alarm_time_timealarm_obj_t);
    self->base.type = &alarm_time_timealarm_type;

    enum { ARG_monotonic_time, ARG_epoch_time };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_monotonic_time, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_epoch_time, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    bool have_monotonic = args[ARG_monotonic_time].u_obj != mp_const_none;
    bool have_epoch = args[ARG_epoch_time].u_obj != mp_const_none;

    if (!(have_monotonic ^ have_epoch)) {
        mp_raise_ValueError(translate("Supply one of monotonic_time or epoch_time"));
    }

    mp_float_t monotonic_time = 0;   // To avoid compiler warning.
    if (have_monotonic) {
        monotonic_time = mp_obj_get_float(args[ARG_monotonic_time].u_obj);
    }

    mp_float_t monotonic_time_now = common_hal_time_monotonic_ms() / 1000.0;

    if (have_epoch) {
        #if MICROPY_LONGINT_IMPL == MICROPY_LONGINT_IMPL_NONE
        mp_raise_ValueError(translate("epoch_time not supported on this board"));
        #else
        mp_uint_t epoch_time_secs = mp_obj_int_get_checked(args[ARG_epoch_time].u_obj);

        timeutils_struct_time_t tm;
        struct_time_to_tm(rtc_get_time_source_time(), &tm);
        mp_uint_t epoch_secs_now = timeutils_seconds_since_epoch(tm.tm_year, tm.tm_mon, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec);
        // How far in the future (in secs) is the requested time?
        mp_int_t epoch_diff = epoch_time_secs - epoch_secs_now;
        // Convert it to a future monotonic time.
        monotonic_time = monotonic_time_now + epoch_diff;
        #endif
    }

    if (monotonic_time < monotonic_time_now) {
        mp_raise_ValueError(translate("Time is in the past."));
    }

    common_hal_alarm_time_timealarm_construct(self, monotonic_time);

    return MP_OBJ_FROM_PTR(self);
}

//|     monotonic_time: float
//|     """When this time is reached, the alarm will trigger, based on the `time.monotonic()` clock.
//|        The time may be given as ``epoch_time`` in the constructor, but it is returned
//|        by this property only as a `time.monotonic()` time.
//|        """
//|
STATIC mp_obj_t alarm_time_timealarm_obj_get_monotonic_time(mp_obj_t self_in) {
    alarm_time_timealarm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_float(common_hal_alarm_time_timealarm_get_monotonic_time(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(alarm_time_timealarm_get_monotonic_time_obj, alarm_time_timealarm_obj_get_monotonic_time);

const mp_obj_property_t alarm_time_timealarm_monotonic_time_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&alarm_time_timealarm_get_monotonic_time_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

STATIC const mp_rom_map_elem_t alarm_time_timealarm_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_monotonic_time), MP_ROM_PTR(&alarm_time_timealarm_monotonic_time_obj) },
};

STATIC MP_DEFINE_CONST_DICT(alarm_time_timealarm_locals_dict, alarm_time_timealarm_locals_dict_table);

const mp_obj_type_t alarm_time_timealarm_type = {
    { &mp_type_type },
    .name = MP_QSTR_TimeAlarm,
    .make_new = alarm_time_timealarm_make_new,
    .locals_dict = (mp_obj_t)&alarm_time_timealarm_locals_dict,
};
