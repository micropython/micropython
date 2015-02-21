/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Daniel Campora
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

#include <stdio.h>

#include "py/mpconfig.h"
#include MICROPY_HAL_H
#include "py/obj.h"
#include "py/runtime.h"
#include "modutime.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "rom_map.h"
#include "prcm.h"
#include "pybrtc.h"

/// \moduleref pyb
/// \class RTC - real time clock
///
/// The RTC is and independent clock that keeps track of the date
/// and time.
///
/// Example usage:
///
///     rtc = pyb.RTC()
///     rtc.datetime((2014, 5, 1, 4, 13, 0, 0, 0))
///     print(rtc.datetime())

void pybrtc_init(void) {
    // if RTC was previously set leave it alone
    if (MAP_PRCMSysResetCauseGet() == PRCM_POWER_ON) {
        // fresh reset; configure RTC Calendar
        // set the date to 1st Jan 2015
        // set the time to 00:00:00
        uint32_t seconds = mod_time_seconds_since_2000(2015, 1, 1, 0, 0, 0);

        MAP_PRCMRTCSet(seconds, 0);

        // Mark that the RTC is in use
        MAP_PRCMRTCInUseSet();
    }
}

/******************************************************************************/
// Micro Python bindings

typedef struct _pyb_rtc_obj_t {
    mp_obj_base_t base;
} pyb_rtc_obj_t;

STATIC const pyb_rtc_obj_t pyb_rtc_obj = {{&pyb_rtc_type}};

/// \classmethod \constructor()
/// Create an RTC object.
STATIC mp_obj_t pyb_rtc_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // return constant object
    return (mp_obj_t)&pyb_rtc_obj;
}

/// \method datetime([datetimetuple])
/// Get or set the date and time of the RTC.
///
/// With no arguments, this method returns an 8-tuple with the current
/// date and time.  With 1 argument (being an 8-tuple) it sets the date
/// and time.
///
/// The 8-tuple has the following format:
///
///     (year, month, day, weekday, hours, minutes, seconds, milliseconds)
///
/// `weekday` is 1-7 for Monday through Sunday.
///
mp_obj_t pyb_rtc_datetime(mp_uint_t n_args, const mp_obj_t *args) {
    mod_struct_time tm;
    uint32_t seconds;
    uint16_t mseconds;

    if (n_args == 1) {
        // get the seconds and the milliseconds from the RTC
        MAP_PRCMRTCGet(&seconds, &mseconds);
        mseconds = RTC_CYCLES_U16MS(mseconds);
        mod_time_seconds_since_2000_to_struct_time(seconds, &tm);

        mp_obj_t tuple[8] = {
            mp_obj_new_int(tm.tm_year),
            mp_obj_new_int(tm.tm_mon),
            mp_obj_new_int(tm.tm_mday),
            mp_obj_new_int(tm.tm_wday),
            mp_obj_new_int(tm.tm_hour),
            mp_obj_new_int(tm.tm_min),
            mp_obj_new_int(tm.tm_sec),
            mp_obj_new_int(mseconds)
        };
        return mp_obj_new_tuple(8, tuple);
    } else {
        // set date and time
        mp_obj_t *items;
        mp_obj_get_array_fixed_n(args[1], 8, &items);

        tm.tm_year = mp_obj_get_int(items[0]);
        tm.tm_mon = mp_obj_get_int(items[1]);
        tm.tm_mday = mp_obj_get_int(items[2]);
        // Skip the weekday
        tm.tm_hour = mp_obj_get_int(items[4]);
        tm.tm_min = mp_obj_get_int(items[5]);
        tm.tm_sec = mp_obj_get_int(items[6]);
        mseconds = mp_obj_get_int(items[7]);

        seconds = mod_time_seconds_since_2000(tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        mseconds = RTC_U16MS_CYCLES(mseconds);
        MAP_PRCMRTCSet(seconds, mseconds);

        return mp_const_none;
    }
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_rtc_datetime_obj, 1, 2, pyb_rtc_datetime);

STATIC const mp_map_elem_t pyb_rtc_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_datetime), (mp_obj_t)&pyb_rtc_datetime_obj },
};
STATIC MP_DEFINE_CONST_DICT(pyb_rtc_locals_dict, pyb_rtc_locals_dict_table);

const mp_obj_type_t pyb_rtc_type = {
    { &mp_type_type },
    .name = MP_QSTR_RTC,
    .make_new = pyb_rtc_make_new,
    .locals_dict = (mp_obj_t)&pyb_rtc_locals_dict,
};
