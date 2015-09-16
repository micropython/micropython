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

#include <std.h>

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
#include "simplelink.h"
#include "modnetwork.h"
#include "modwlan.h"
#include "mpexception.h"

/// \moduleref pyb
/// \class RTC - real time clock

/******************************************************************************
 DEFINE TYPES
 ******************************************************************************/
typedef struct _pyb_rtc_obj_t {
    mp_obj_base_t base;
    byte prwmode;
    bool alarmset;
    bool repeat;
} pyb_rtc_obj_t;

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
STATIC const mp_cb_methods_t pybrtc_cb_methods;
STATIC pyb_rtc_obj_t pyb_rtc_obj = {.prwmode = 0, .alarmset = false, .repeat = false};

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/
STATIC uint32_t pyb_rtc_reset (mp_obj_t self_in);
STATIC void pyb_rtc_callback_enable (mp_obj_t self_in);
STATIC void pyb_rtc_callback_disable (mp_obj_t self_in);
STATIC mp_obj_t pyb_rtc_datetime(mp_obj_t self, const mp_obj_t datetime);

/******************************************************************************
 DECLARE PUBLIC FUNCTIONS
 ******************************************************************************/
__attribute__ ((section (".boot")))
void pyb_rtc_pre_init(void) {
    // if the RTC was previously set, leave it alone
    if (MAP_PRCMSysResetCauseGet() == PRCM_POWER_ON) {
        // Mark the RTC in use first
        MAP_PRCMRTCInUseSet();
        // reset the time and date
        pyb_rtc_reset((mp_obj_t)&pyb_rtc_obj);
    }
}

uint32_t pyb_rtc_get_seconds (void) {
    uint32_t seconds;
    uint16_t mseconds;
    MAP_PRCMRTCGet(&seconds, &mseconds);
    return seconds;
}

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/
STATIC uint32_t pyb_rtc_reset (mp_obj_t self_in) {
    // fresh reset; configure the RTC Calendar
    // set the date to 1st Jan 2015
    // set the time to 00:00:00
    uint32_t seconds = timeutils_seconds_since_2000(2015, 1, 1, 0, 0, 0);
    // Now set the RTC calendar seconds
    MAP_PRCMRTCSet(seconds, 0);
    return seconds;
}

STATIC void pyb_rtc_callback_enable (mp_obj_t self_in) {
    pyb_rtc_obj_t *self = self_in;
    // check the wake from param
    if (self->prwmode & PYB_PWR_MODE_ACTIVE) {
        // enable the slow clock interrupt
        MAP_PRCMIntEnable(PRCM_INT_SLOW_CLK_CTR);
    } else {
        // just in case it was already enabled before
        MAP_PRCMIntDisable(PRCM_INT_SLOW_CLK_CTR);
    }
    pybsleep_configure_timer_wakeup (self->prwmode);
}

STATIC void pyb_rtc_callback_disable (mp_obj_t self_in) {
    pyb_rtc_obj_t *self = self_in;
    // check the wake from param
    if (self->prwmode & PYB_PWR_MODE_ACTIVE) {
        // disable the slow clock interrupt
        MAP_PRCMIntDisable(PRCM_INT_SLOW_CLK_CTR);
    }
    // disable wake from ldps and hibernate
    pybsleep_configure_timer_wakeup (PYB_PWR_MODE_ACTIVE);
    // read the interrupt status to clear any pending interrupt
    (void)MAP_PRCMIntStatus();
}

STATIC uint pyb_rtc_datetime_s_us(const mp_obj_t datetime, uint32_t *seconds) {
    timeutils_struct_time_t tm;
    uint32_t useconds;

    // set date and time
    mp_obj_t *items;
    uint len;
    mp_obj_get_array(datetime, &len, &items);

    // verify the tuple
    if (len < 3 || len > 8) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
    }

    tm.tm_year = mp_obj_get_int(items[0]);
    tm.tm_mon = mp_obj_get_int(items[1]);
    tm.tm_mday = mp_obj_get_int(items[2]);
    if (len < 7) {
        useconds = 0;
    } else {
        useconds = mp_obj_get_int(items[6]);
    }
    if (len < 6) {
        tm.tm_sec = 0;
    } else {
        tm.tm_sec = mp_obj_get_int(items[5]);
    }
    if (len < 5) {
        tm.tm_min = 0;
    } else {
        tm.tm_min = mp_obj_get_int(items[4]);
    }
    if (len < 4) {
        tm.tm_hour = 0;
    } else {
        tm.tm_hour = mp_obj_get_int(items[3]);
    }
    *seconds = timeutils_seconds_since_2000(tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    return useconds;
}

/// The 8-tuple has the same format as CPython's datetime object:
///
///     (year, month, day, hours, minutes, seconds, milliseconds, tzinfo=None)
///
STATIC mp_obj_t pyb_rtc_datetime(mp_obj_t self, const mp_obj_t datetime) {
    uint32_t seconds;
    uint32_t useconds;

    if (datetime != MP_OBJ_NULL) {
        useconds = pyb_rtc_datetime_s_us(datetime, &seconds);
        MAP_PRCMRTCSet(seconds, RTC_U16MS_CYCLES(useconds / 1000));
    } else {
        seconds = pyb_rtc_reset(self);
    }

    // set WLAN time and date, this is needed to verify certificates
    wlan_set_current_time(seconds);
    return mp_const_none;
}

/******************************************************************************/
// Micro Python bindings

STATIC const mp_arg_t pyb_rtc_init_args[] = {
    { MP_QSTR_id,                             MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_datetime,                       MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
};
STATIC mp_obj_t pyb_rtc_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *all_args) {
    // parse args
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
    mp_arg_val_t args[MP_ARRAY_SIZE(pyb_rtc_init_args)];
    mp_arg_parse_all(n_args, all_args, &kw_args, MP_ARRAY_SIZE(args), pyb_rtc_init_args, args);

    // check the peripheral id
    if (args[0].u_int != 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_resource_not_avaliable));
    }

    // setup the object
    pyb_rtc_obj_t *self = &pyb_rtc_obj;
    self->base.type = &pyb_rtc_type;

    // set the time and date
    pyb_rtc_datetime((mp_obj_t)&pyb_rtc_obj, args[1].u_obj);

    // return constant object
    return (mp_obj_t)&pyb_rtc_obj;
}

STATIC mp_obj_t pyb_rtc_init (mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(pyb_rtc_init_args) - 1];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(args), &pyb_rtc_init_args[1], args);
    return pyb_rtc_datetime(pos_args[0], args[0].u_obj);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_rtc_init_obj, 1, pyb_rtc_init);

STATIC mp_obj_t pyb_rtc_now (mp_obj_t self_in) {
    timeutils_struct_time_t tm;
    uint32_t seconds;
    uint16_t mseconds;

    // get the seconds and the milliseconds from the RTC
    MAP_PRCMRTCGet(&seconds, &mseconds);
    mseconds = RTC_CYCLES_U16MS(mseconds);
    timeutils_seconds_since_2000_to_struct_time(seconds, &tm);

    mp_obj_t tuple[8] = {
        mp_obj_new_int(tm.tm_year),
        mp_obj_new_int(tm.tm_mon),
        mp_obj_new_int(tm.tm_mday),
        mp_obj_new_int(tm.tm_hour),
        mp_obj_new_int(tm.tm_min),
        mp_obj_new_int(tm.tm_sec),
        mp_obj_new_int(mseconds * 1000),
        mp_const_none
    };
    return mp_obj_new_tuple(8, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_rtc_now_obj, pyb_rtc_now);

STATIC mp_obj_t pyb_rtc_deinit (mp_obj_t self_in) {
    pyb_rtc_reset (self_in);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_rtc_deinit_obj, pyb_rtc_deinit);

STATIC mp_obj_t pyb_rtc_alarm (mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    STATIC const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,                            MP_ARG_INT,  {.u_int = 0} },
        { MP_QSTR_time,                          MP_ARG_OBJ,  {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_repeat,     MP_ARG_KW_ONLY   | MP_ARG_BOOL, {.u_bool = false} },
    };

    // parse args
    pyb_rtc_obj_t *self = pos_args[0];
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(args), allowed_args, args);

    // check the alarm id
    if (args[0].u_int != 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_resource_not_avaliable));
    }

    uint32_t a_seconds;
    uint16_t a_mseconds;
    if (MP_OBJ_IS_TYPE(args[1].u_obj, &mp_type_tuple)) { // datetime tuple given
        a_mseconds = pyb_rtc_datetime_s_us (args[1].u_obj, &a_seconds) / 1000;
    } else { // then it must be an integer or MP_OBJ_NULL
        uint32_t c_seconds;
        uint16_t c_mseconds;
        if (MP_OBJ_IS_INT(args[1].u_obj)) {
            a_seconds = 0, a_mseconds = mp_obj_get_int(args[1].u_obj);
        } else {
            a_seconds = 1, a_mseconds = 0;
        }
        // get the seconds and the milliseconds from the RTC
        MAP_PRCMRTCGet(&c_seconds, &c_mseconds);
        a_mseconds += RTC_CYCLES_U16MS(c_mseconds);
        // calculate the future time
        a_seconds += c_seconds + (a_mseconds / 1000);
        a_mseconds -= ((a_mseconds / 1000) * 1000);
    }

    // disable the interrupt before updating anything
    pyb_rtc_callback_disable((mp_obj_t)self);

    // set the match value
    MAP_PRCMRTCMatchSet(a_seconds, a_mseconds);

    // enabled it again (according to the power mode)
    pyb_rtc_callback_enable((mp_obj_t)self);

    // set the alarmset flag and store the repeat one
    self->alarmset = true;
    self->repeat = args[2].u_bool;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_rtc_alarm_obj, 1, pyb_rtc_alarm);

STATIC mp_obj_t pyb_rtc_alarm_left (mp_obj_t self_in) {
    pyb_rtc_obj_t *self = self_in;
    uint32_t a_seconds, c_seconds;
    uint16_t a_mseconds, c_mseconds;
    int32_t ms_left;

    // get the alarm time
    MAP_PRCMRTCMatchGet(&a_seconds, &a_mseconds);
    a_mseconds = RTC_CYCLES_U16MS(a_mseconds);
    // get the current time
    MAP_PRCMRTCGet(&c_seconds, &c_mseconds);
    c_mseconds = RTC_CYCLES_U16MS(c_mseconds);
    // calculate the ms left
    ms_left = ((a_seconds * 1000) + a_mseconds) - ((c_seconds * 1000) + c_mseconds);
    if (!self->alarmset || ms_left < 0) {
        ms_left = 0;
    }
    return mp_obj_new_int(ms_left);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_rtc_alarm_left_obj, pyb_rtc_alarm_left);

/// \method callback(handler, value, pwrmode)
/// Creates a callback object associated with the real time clock
/// min num of arguments is 1 (value). The value is the alarm time
/// in the future, in msec
/// FIXME
STATIC mp_obj_t pyb_rtc_callback (mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_arg_val_t args[mpcallback_INIT_NUM_ARGS];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, mpcallback_INIT_NUM_ARGS, mpcallback_init_args, args);
    pyb_rtc_obj_t *self = pos_args[0];

    // check if any parameters were passed
    mp_obj_t _callback = mpcallback_find((mp_obj_t)&pyb_rtc_obj);
    if (kw_args->used > 0) {
        uint32_t f_mseconds = MAX(1, mp_obj_get_int(args[3].u_obj));
        uint32_t seconds;
        uint16_t mseconds;
        // get the seconds and the milliseconds from the RTC
        MAP_PRCMRTCGet(&seconds, &mseconds);
        mseconds = RTC_CYCLES_U16MS(mseconds);

        // configure the rtc alarm accordingly
        seconds += f_mseconds / 1000;
        mseconds += f_mseconds - ((f_mseconds / 1000) * 1000);

        // disable the interrupt before updating anything
        pyb_rtc_callback_disable((mp_obj_t)&pyb_rtc_obj);

        // set the match value
        MAP_PRCMRTCMatchSet(seconds, mseconds);

        // save the power mode data for later
        self->prwmode = args[4].u_int;

        // create the callback
        _callback = mpcallback_new ((mp_obj_t)&pyb_rtc_obj, args[1].u_obj, &pybrtc_cb_methods, true);

        // set the lpds callback
        pybsleep_set_timer_lpds_callback(_callback);

        // the interrupt priority is ignored since it's already set to to highest level by the sleep module
        // to make sure that the wakeup callbacks are always called first when resuming from sleep

        // enable the interrupt
        pyb_rtc_callback_enable((mp_obj_t)&pyb_rtc_obj);
    } else if (!_callback) {
        _callback = mpcallback_new ((mp_obj_t)&pyb_rtc_obj, mp_const_none, &pybrtc_cb_methods, false);
    }
    return _callback;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_rtc_callback_obj, 1, pyb_rtc_callback);

STATIC const mp_map_elem_t pyb_rtc_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_init),            (mp_obj_t)&pyb_rtc_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_deinit),          (mp_obj_t)&pyb_rtc_deinit_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_now),             (mp_obj_t)&pyb_rtc_now_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_alarm),           (mp_obj_t)&pyb_rtc_alarm_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_alarm_left),      (mp_obj_t)&pyb_rtc_alarm_left_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_callback),        (mp_obj_t)&pyb_rtc_callback_obj },
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
