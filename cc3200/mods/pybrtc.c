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
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "rom_map.h"
#include "prcm.h"
#include "pybrtc.h"
#include "pybsleep.h"
#include "mpcallback.h"
#include "timeutils.h"

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

/******************************************************************************
 DECLARE CONSTANTS
 ******************************************************************************/
#define PYBRTC_CLOCK_FREQUENCY_HZ                   32768
#define PYBRTC_MIN_INTERVAL_VALUE                   25

/******************************************************************************
 DEFINE TYPES
 ******************************************************************************/
typedef struct {
    byte prwmode;
} pybrtc_data_t;

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
STATIC pybrtc_data_t pybrtc_data;
STATIC const mp_cb_methods_t pybrtc_cb_methods;
STATIC const mp_obj_base_t pyb_rtc_obj = {&pyb_rtc_type};

/******************************************************************************
 DECLARE PUBLIC FUNCTIONS
 ******************************************************************************/
__attribute__ ((section (".boot")))
void pybrtc_init(void) {
    // if the RTC was previously set, leave it alone
    if (MAP_PRCMSysResetCauseGet() == PRCM_POWER_ON) {
        // fresh reset; configure the RTC Calendar
        // set the date to 1st Jan 2015
        // set the time to 00:00:00
        uint32_t seconds = timeutils_seconds_since_2000(2015, 1, 1, 0, 0, 0);

        // Mark the RTC in use first
        MAP_PRCMRTCInUseSet();

        // Now set the RTC calendar seconds
        MAP_PRCMRTCSet(seconds, 0);
    }
}

void pyb_rtc_callback_disable (mp_obj_t self_in) {
    // check the wake from param
    if (pybrtc_data.prwmode & PYB_PWR_MODE_ACTIVE) {
        // disable the slow clock interrupt
        MAP_PRCMIntDisable(PRCM_INT_SLOW_CLK_CTR);
    }
    // disable wake from ldps and hibernate
    pybsleep_configure_timer_wakeup (PYB_PWR_MODE_ACTIVE);
    // read the interrupt status to clear any pending interrupt
    (void)MAP_PRCMIntStatus();
}

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/
STATIC void pyb_rtc_callback_enable (mp_obj_t self_in) {
    // check the wake from param
    if (pybrtc_data.prwmode & PYB_PWR_MODE_ACTIVE) {
        // enable the slow clock interrupt
        MAP_PRCMIntEnable(PRCM_INT_SLOW_CLK_CTR);
    }
    else {
        // just in case it was already enabled before
        MAP_PRCMIntDisable(PRCM_INT_SLOW_CLK_CTR);
    }
    pybsleep_configure_timer_wakeup (pybrtc_data.prwmode);
}

/******************************************************************************/
// Micro Python bindings

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
/// date and time. With 1 argument (being an 8-tuple) it sets the date
/// and time.
///
/// The 8-tuple has the following format:
///
///     (year, month, day, weekday, hours, minutes, seconds, milliseconds)
///
/// `weekday` is 0-6 for Monday through Sunday.
///
mp_obj_t pyb_rtc_datetime(mp_uint_t n_args, const mp_obj_t *args) {
    timeutils_struct_time_t tm;
    uint32_t seconds;
    uint16_t mseconds;

    if (n_args == 1) {
        // get the seconds and the milliseconds from the RTC
        MAP_PRCMRTCGet(&seconds, &mseconds);
        mseconds = RTC_CYCLES_U16MS(mseconds);
        timeutils_seconds_since_2000_to_struct_time(seconds, &tm);

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

        seconds = timeutils_seconds_since_2000(tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        mseconds = RTC_U16MS_CYCLES(mseconds);
        MAP_PRCMRTCSet(seconds, mseconds);

        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_rtc_datetime_obj, 1, 2, pyb_rtc_datetime);

/// \method callback(handler, value, pwrmode)
/// Creates a callback object associated with the real time clock
/// min num of arguments is 1 (value). The value is the alarm time
/// in the future, in msec
STATIC mp_obj_t pyb_rtc_callback (mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_arg_val_t args[mpcallback_INIT_NUM_ARGS];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, mpcallback_INIT_NUM_ARGS, mpcallback_init_args, args);

    // check if any parameters were passed
    mp_obj_t _callback = mpcallback_find((mp_obj_t)&pyb_rtc_obj);
    if (kw_args->used > 0 || !_callback) {
        uint32_t f_mseconds = MAX(1, args[3].u_int);
        uint32_t seconds;
        uint16_t mseconds;
        // get the seconds and the milliseconds from the RTC
        MAP_PRCMRTCGet(&seconds, &mseconds);
        mseconds = RTC_CYCLES_U16MS(mseconds);

        // configure the rtc alarm accordingly
        seconds += f_mseconds / 1000;
        mseconds += f_mseconds - ((f_mseconds / 1000) * 1000);

        // disable the interrupt before updating anything
        // (the object is not relevant here, the function already knows it)
        pyb_rtc_callback_disable(NULL);

        // set the match value
        MAP_PRCMRTCMatchSet(seconds, mseconds);

        // save the power mode data for later
        pybrtc_data.prwmode = args[4].u_int;

        // create the callback
        _callback = mpcallback_new ((mp_obj_t)&pyb_rtc_obj, args[1].u_obj, &pybrtc_cb_methods);

        // set the lpds callback
        pybsleep_set_timer_lpds_callback(_callback);

        // the interrupt priority is ignored since it's already set to to highest level by the sleep module
        // to make sure that the wakeup callbacks are always called first when resuming from sleep

        // enable the interrupt (the object is not relevant here, the function already knows it)
        pyb_rtc_callback_enable(NULL);
    }
    return _callback;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_rtc_callback_obj, 1, pyb_rtc_callback);

STATIC const mp_map_elem_t pyb_rtc_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_datetime), (mp_obj_t)&pyb_rtc_datetime_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_callback), (mp_obj_t)&pyb_rtc_callback_obj },
};
STATIC MP_DEFINE_CONST_DICT(pyb_rtc_locals_dict, pyb_rtc_locals_dict_table);

const mp_obj_type_t pyb_rtc_type = {
    { &mp_type_type },
    .name = MP_QSTR_RTC,
    .make_new = pyb_rtc_make_new,
    .locals_dict = (mp_obj_t)&pyb_rtc_locals_dict,
};

STATIC const mp_cb_methods_t pybrtc_cb_methods = {
    .init = pyb_rtc_callback,
    .enable = pyb_rtc_callback_enable,
    .disable = pyb_rtc_callback_disable,
};
