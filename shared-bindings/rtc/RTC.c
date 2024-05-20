// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Noralf Trønnes
// SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared/timeutils/timeutils.h"
#include "shared-bindings/rtc/__init__.h"
#include "shared-bindings/rtc/RTC.h"
#include "shared-bindings/time/__init__.h"

const rtc_rtc_obj_t rtc_rtc_obj = {{&rtc_rtc_type}};

//| class RTC:
//|     """Real Time Clock"""
//|
//|     def __init__(self) -> None:
//|         """This class represents the onboard Real Time Clock. It is a singleton and will always return the same instance."""
//|         ...
static mp_obj_t rtc_rtc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // No arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // return constant object
    return (mp_obj_t)&rtc_rtc_obj;
}

//|     datetime: time.struct_time
//|     """The current date and time of the RTC as a `time.struct_time`.
//|
//|     This must be set to the current date and time whenever the board loses power::
//|
//|       import rtc
//|       import time
//|
//|       r = rtc.RTC()
//|       r.datetime = time.struct_time((2019, 5, 29, 15, 14, 15, 0, -1, -1))
//|
//|
//|     Once set, the RTC will automatically update this value as time passes. You can read this
//|     property to get a snapshot of the current time::
//|
//|       current_time = r.datetime
//|       print(current_time)
//|       # struct_time(tm_year=2019, tm_month=5, ...)"""
static mp_obj_t rtc_rtc_obj_get_datetime(mp_obj_t self_in) {
    timeutils_struct_time_t tm;
    common_hal_rtc_get_time(&tm);
    return struct_time_from_tm(&tm);
}
MP_DEFINE_CONST_FUN_OBJ_1(rtc_rtc_get_datetime_obj, rtc_rtc_obj_get_datetime);

static mp_obj_t rtc_rtc_obj_set_datetime(mp_obj_t self_in, mp_obj_t datetime) {
    timeutils_struct_time_t tm;
    struct_time_to_tm(datetime, &tm);
    common_hal_rtc_set_time(&tm);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(rtc_rtc_set_datetime_obj, rtc_rtc_obj_set_datetime);

MP_PROPERTY_GETSET(rtc_rtc_datetime_obj,
    (mp_obj_t)&rtc_rtc_get_datetime_obj,
    (mp_obj_t)&rtc_rtc_set_datetime_obj);

//|     calibration: int
//|     """The RTC calibration value as an `int`.
//|
//|     A positive value speeds up the clock and a negative value slows it down.
//|
//|     **Limitations:** Calibration not supported on SAMD, Nordic, RP240, Spresense, and STM.
//|
//|     Range and value is hardware specific, but one step is often approximately 1 ppm::
//|
//|       import rtc
//|       import time
//|
//|       r = rtc.RTC()
//|       r.calibration = 1"""
//|
static mp_obj_t rtc_rtc_obj_get_calibration(mp_obj_t self_in) {
    int calibration = common_hal_rtc_get_calibration();
    return mp_obj_new_int(calibration);
}
MP_DEFINE_CONST_FUN_OBJ_1(rtc_rtc_get_calibration_obj, rtc_rtc_obj_get_calibration);

static mp_obj_t rtc_rtc_obj_set_calibration(mp_obj_t self_in, mp_obj_t calibration) {
    common_hal_rtc_set_calibration(mp_obj_get_int(calibration));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(rtc_rtc_set_calibration_obj, rtc_rtc_obj_set_calibration);

MP_PROPERTY_GETSET(rtc_rtc_calibration_obj,
    (mp_obj_t)&rtc_rtc_get_calibration_obj,
    (mp_obj_t)&rtc_rtc_set_calibration_obj);

static const mp_rom_map_elem_t rtc_rtc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_datetime), MP_ROM_PTR(&rtc_rtc_datetime_obj) },
    { MP_ROM_QSTR(MP_QSTR_calibration), MP_ROM_PTR(&rtc_rtc_calibration_obj) },
};
static MP_DEFINE_CONST_DICT(rtc_rtc_locals_dict, rtc_rtc_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    rtc_rtc_type,
    MP_QSTR_RTC,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, rtc_rtc_make_new,
    locals_dict, &rtc_rtc_locals_dict
    );
