// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/nlr.h"
#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"

#include "shared-bindings/alarm/time/TimeAlarm.h"
#include "shared-bindings/rtc/__init__.h"
#include "shared-bindings/time/__init__.h"

#if MICROPY_LONGINT_IMPL != MICROPY_LONGINT_IMPL_NONE
mp_obj_t MP_WEAK rtc_get_time_source_time(void) {
    mp_raise_RuntimeError(MP_ERROR_TEXT("RTC is not supported on this board"));
}
#endif

//| class TimeAlarm:
//|     """Trigger an alarm when the specified time is reached."""
//|
//|     def __init__(
//|         self, *, monotonic_time: Optional[float] = None, epoch_time: Optional[int] = None
//|     ) -> None:
//|         """Create an alarm that will be triggered when `time.monotonic()` would equal
//|         ``monotonic_time``, or when `time.time()` would equal ``epoch_time``.
//|         Only one of the two arguments can be given.
//|         The alarm is not active until it is passed to an
//|         `alarm`-enabling sleep function, such as `alarm.light_sleep_until_alarms()` or
//|         `alarm.exit_and_deep_sleep_until_alarms()`.
//|
//|         If the given time is already in the past, then an exception is raised.
//|         If the sleep happens after the given time, then it will wake immediately
//|         due to this time alarm.
//|         """
//|         ...
static mp_obj_t alarm_time_timealarm_make_new(const mp_obj_type_t *type,
    size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    alarm_time_timealarm_obj_t *self = mp_obj_malloc(alarm_time_timealarm_obj_t, &alarm_time_timealarm_type);

    enum { ARG_monotonic_time, ARG_epoch_time };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_monotonic_time, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_epoch_time, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    bool have_monotonic = args[ARG_monotonic_time].u_obj != mp_const_none;
    bool have_epoch = args[ARG_epoch_time].u_obj != mp_const_none;

    if (!(have_monotonic ^ have_epoch)) {
        mp_raise_ValueError(MP_ERROR_TEXT("Supply one of monotonic_time or epoch_time"));
    }

    mp_float_t monotonic_time = 0;   // To avoid compiler warning.
    if (have_monotonic) {
        monotonic_time = mp_obj_get_float(args[ARG_monotonic_time].u_obj);
    }

    mp_float_t monotonic_time_now = common_hal_time_monotonic_ms() / 1000.0;

    if (have_epoch) {
        #if MICROPY_LONGINT_IMPL == MICROPY_LONGINT_IMPL_NONE
        mp_raise_ValueError(MP_ERROR_TEXT("epoch_time not supported on this board"));
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
        mp_raise_ValueError(MP_ERROR_TEXT("Time is in the past."));
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
static mp_obj_t alarm_time_timealarm_obj_get_monotonic_time(mp_obj_t self_in) {
    alarm_time_timealarm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_float(common_hal_alarm_time_timealarm_get_monotonic_time(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(alarm_time_timealarm_get_monotonic_time_obj, alarm_time_timealarm_obj_get_monotonic_time);

MP_PROPERTY_GETTER(alarm_time_timealarm_monotonic_time_obj,
    (mp_obj_t)&alarm_time_timealarm_get_monotonic_time_obj);

static const mp_rom_map_elem_t alarm_time_timealarm_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_monotonic_time), MP_ROM_PTR(&alarm_time_timealarm_monotonic_time_obj) },
};

static MP_DEFINE_CONST_DICT(alarm_time_timealarm_locals_dict, alarm_time_timealarm_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    alarm_time_timealarm_type,
    MP_QSTR_TimeAlarm,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, alarm_time_timealarm_make_new,
    locals_dict, &alarm_time_timealarm_locals_dict
    );
