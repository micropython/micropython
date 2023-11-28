/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#include "py/mpconfig.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "shared/timeutils/timeutils.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "rom_map.h"
#include "prcm.h"
#include "pybrtc.h"
#include "mpirq.h"
#include "pybsleep.h"
#include "simplelink.h"
#include "modnetwork.h"
#include "modwlan.h"

/// \moduleref pyb
/// \class RTC - real time clock

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
STATIC const mp_irq_methods_t pyb_rtc_irq_methods;
STATIC pyb_rtc_obj_t pyb_rtc_obj;

/******************************************************************************
 FUNCTION-LIKE MACROS
 ******************************************************************************/
#define RTC_U16MS_CYCLES(msec)          ((msec   * 1024) / 1000)
#define RTC_CYCLES_U16MS(cycles)        ((cycles * 1000) / 1024)

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/
STATIC void pyb_rtc_set_time (uint32_t secs, uint16_t msecs);
STATIC uint32_t pyb_rtc_reset (void);
STATIC void pyb_rtc_disable_interupt (void);
STATIC void pyb_rtc_irq_enable (mp_obj_t self_in);
STATIC void pyb_rtc_irq_disable (mp_obj_t self_in);
STATIC int pyb_rtc_irq_flags (mp_obj_t self_in);
STATIC uint pyb_rtc_datetime_s_us(const mp_obj_t datetime, uint32_t *seconds);
STATIC mp_obj_t pyb_rtc_datetime(mp_obj_t self, const mp_obj_t datetime);
STATIC void pyb_rtc_set_alarm (pyb_rtc_obj_t *self, uint32_t seconds, uint16_t mseconds);
STATIC void rtc_msec_add(uint16_t msecs_1, uint32_t *secs, uint16_t *msecs_2);

/******************************************************************************
 DECLARE PUBLIC FUNCTIONS
 ******************************************************************************/
__attribute__ ((section (".boot")))
void pyb_rtc_pre_init(void) {
    // only if coming out of a power-on reset
    if (MAP_PRCMSysResetCauseGet() == PRCM_POWER_ON) {
        // Mark the RTC in use first
        MAP_PRCMRTCInUseSet();
        // reset the time and date
        pyb_rtc_reset();
    }
}

void pyb_rtc_get_time (uint32_t *secs, uint16_t *msecs) {
    uint16_t cycles;
    MAP_PRCMRTCGet (secs, &cycles);
    *msecs = RTC_CYCLES_U16MS(cycles);
}

uint32_t pyb_rtc_get_seconds (void) {
    uint32_t seconds;
    uint16_t mseconds;
    pyb_rtc_get_time(&seconds, &mseconds);
    return seconds;
}

void pyb_rtc_calc_future_time (uint32_t a_mseconds, uint32_t *f_seconds, uint16_t *f_mseconds) {
    uint32_t c_seconds;
    uint16_t c_mseconds;
    // get the current time
    pyb_rtc_get_time(&c_seconds, &c_mseconds);
    // calculate the future seconds
    *f_seconds = c_seconds + (a_mseconds / 1000);
    // calculate the "remaining" future mseconds
    *f_mseconds = a_mseconds % 1000;
    // add the current milliseconds
    rtc_msec_add (c_mseconds, f_seconds, f_mseconds);
}

void pyb_rtc_repeat_alarm (pyb_rtc_obj_t *self) {
    if (self->repeat) {
        uint32_t f_seconds, c_seconds;
        uint16_t f_mseconds, c_mseconds;

        pyb_rtc_get_time(&c_seconds, &c_mseconds);

        // subtract the time elapsed between waking up and setting up the alarm again
        int32_t wake_ms = ((c_seconds * 1000) + c_mseconds) - ((self->alarm_time_s * 1000) + self->alarm_time_ms);
        int32_t next_alarm = self->alarm_ms - wake_ms;
        next_alarm = next_alarm > 0 ? next_alarm : PYB_RTC_MIN_ALARM_TIME_MS;
        pyb_rtc_calc_future_time (next_alarm, &f_seconds, &f_mseconds);

        // now configure the alarm
        pyb_rtc_set_alarm (self, f_seconds, f_mseconds);
    }
}

void pyb_rtc_disable_alarm (void) {
    pyb_rtc_obj.alarmset = false;
    pyb_rtc_disable_interupt();
}

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/
STATIC void pyb_rtc_set_time (uint32_t secs, uint16_t msecs) {
    // add the RTC access time
    rtc_msec_add(RTC_ACCESS_TIME_MSEC, &secs, &msecs);
    // convert from mseconds to cycles
    msecs = RTC_U16MS_CYCLES(msecs);
    // now set the time
    MAP_PRCMRTCSet(secs, msecs);
}

STATIC uint32_t pyb_rtc_reset (void) {
    // fresh reset; configure the RTC Calendar
    // set the date to 1st Jan 2015
    // set the time to 00:00:00
    uint32_t seconds = timeutils_seconds_since_2000(2015, 1, 1, 0, 0, 0);
    // disable any running alarm
    pyb_rtc_disable_alarm();
    // Now set the RTC calendar time
    pyb_rtc_set_time(seconds, 0);
    return seconds;
}

STATIC void pyb_rtc_disable_interupt (void) {
    uint primsk = disable_irq();
    MAP_PRCMIntDisable(PRCM_INT_SLOW_CLK_CTR);
    (void)MAP_PRCMIntStatus();
    enable_irq(primsk);
}

STATIC void pyb_rtc_irq_enable (mp_obj_t self_in) {
    pyb_rtc_obj_t *self = self_in;
    // we always need interrupts if repeat is enabled
    if ((self->pwrmode & PYB_PWR_MODE_ACTIVE) || self->repeat) {
        MAP_PRCMIntEnable(PRCM_INT_SLOW_CLK_CTR);
    } else { // just in case it was already enabled before
        MAP_PRCMIntDisable(PRCM_INT_SLOW_CLK_CTR);
    }
    self->irq_enabled = true;
}

STATIC void pyb_rtc_irq_disable (mp_obj_t self_in) {
    pyb_rtc_obj_t *self = self_in;
    self->irq_enabled = false;
    if (!self->repeat) { // we always need interrupts if repeat is enabled
        pyb_rtc_disable_interupt();
    }
}

STATIC int pyb_rtc_irq_flags (mp_obj_t self_in) {
    pyb_rtc_obj_t *self = self_in;
    return self->irq_flags;
}

STATIC uint pyb_rtc_datetime_s_us(const mp_obj_t datetime, uint32_t *seconds) {
    timeutils_struct_time_t tm;
    uint32_t useconds;

    // set date and time
    mp_obj_t *items;
    size_t len;
    mp_obj_get_array(datetime, &len, &items);

    // verify the tuple
    if (len < 3 || len > 8) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid argument(s) value"));
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
STATIC mp_obj_t pyb_rtc_datetime(mp_obj_t self_in, const mp_obj_t datetime) {
    uint32_t seconds;
    uint32_t useconds;

    if (datetime != MP_OBJ_NULL) {
        useconds = pyb_rtc_datetime_s_us(datetime, &seconds);
        pyb_rtc_set_time (seconds, useconds / 1000);
    } else {
        seconds = pyb_rtc_reset();
    }

    // set WLAN time and date, this is needed to verify certificates
    wlan_set_current_time(seconds);
    return mp_const_none;
}

STATIC void pyb_rtc_set_alarm (pyb_rtc_obj_t *self, uint32_t seconds, uint16_t mseconds) {
    // disable the interrupt before updating anything
    if (self->irq_enabled) {
        MAP_PRCMIntDisable(PRCM_INT_SLOW_CLK_CTR);
    }
    // set the match value
    MAP_PRCMRTCMatchSet(seconds, RTC_U16MS_CYCLES(mseconds));
    self->alarmset = true;
    self->alarm_time_s = seconds;
    self->alarm_time_ms = mseconds;
    // enabled the interrupts again if applicable
    if (self->irq_enabled || self->repeat) {
        MAP_PRCMIntEnable(PRCM_INT_SLOW_CLK_CTR);
    }
}

STATIC void rtc_msec_add (uint16_t msecs_1, uint32_t *secs, uint16_t *msecs_2) {
    if (msecs_1 + *msecs_2 >= 1000) { // larger than one second
        *msecs_2 = (msecs_1 + *msecs_2) - 1000;
        *secs += 1; // carry flag
    } else {
        // simply add the mseconds
        *msecs_2 = msecs_1 + *msecs_2;
    }
}

/******************************************************************************/
// MicroPython bindings

STATIC const mp_arg_t pyb_rtc_init_args[] = {
    { MP_QSTR_id,                             MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_datetime,                       MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
};
STATIC mp_obj_t pyb_rtc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // parse args
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
    mp_arg_val_t args[MP_ARRAY_SIZE(pyb_rtc_init_args)];
    mp_arg_parse_all(n_args, all_args, &kw_args, MP_ARRAY_SIZE(args), pyb_rtc_init_args, args);

    // check the peripheral id
    if (args[0].u_int != 0) {
        mp_raise_OSError(MP_ENODEV);
    }

    // setup the object
    pyb_rtc_obj_t *self = &pyb_rtc_obj;
    self->base.type = &pyb_rtc_type;

    // set the time and date
    pyb_rtc_datetime((mp_obj_t)&pyb_rtc_obj, args[1].u_obj);

    // pass it to the sleep module
    pyb_sleep_set_rtc_obj (self);

    // return constant object
    return (mp_obj_t)&pyb_rtc_obj;
}

STATIC mp_obj_t pyb_rtc_init(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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

    // get the time from the RTC
    pyb_rtc_get_time(&seconds, &mseconds);
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
    pyb_rtc_reset();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_rtc_deinit_obj, pyb_rtc_deinit);

STATIC mp_obj_t pyb_rtc_alarm(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    STATIC const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,                            MP_ARG_INT,  {.u_int = 0} },
        { MP_QSTR_time,                          MP_ARG_OBJ,  {.u_obj = mp_const_none} },
        { MP_QSTR_repeat,     MP_ARG_KW_ONLY   | MP_ARG_BOOL, {.u_bool = false} },
    };

    // parse args
    pyb_rtc_obj_t *self = pos_args[0];
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(args), allowed_args, args);

    // check the alarm id
    if (args[0].u_int != 0) {
        mp_raise_OSError(MP_ENODEV);
    }

    uint32_t f_seconds;
    uint16_t f_mseconds;
    bool repeat = args[2].u_bool;
    if (mp_obj_is_type(args[1].u_obj, &mp_type_tuple)) { // datetime tuple given
        // repeat cannot be used with a datetime tuple
        if (repeat) {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid argument(s) value"));
        }
        f_mseconds = pyb_rtc_datetime_s_us (args[1].u_obj, &f_seconds) / 1000;
    } else { // then it must be an integer
        self->alarm_ms = mp_obj_get_int(args[1].u_obj);
        pyb_rtc_calc_future_time (self->alarm_ms, &f_seconds, &f_mseconds);
    }

    // store the repepat flag
    self->repeat = repeat;

    // now configure the alarm
    pyb_rtc_set_alarm (self, f_seconds, f_mseconds);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_rtc_alarm_obj, 1, pyb_rtc_alarm);

STATIC mp_obj_t pyb_rtc_alarm_left(size_t n_args, const mp_obj_t *args) {
    pyb_rtc_obj_t *self = args[0];
    int32_t ms_left;
    uint32_t c_seconds;
    uint16_t c_mseconds;

    // only alarm id 0 is available
    if (n_args > 1 && mp_obj_get_int(args[1]) != 0) {
        mp_raise_OSError(MP_ENODEV);
    }

    // get the current time
    pyb_rtc_get_time(&c_seconds, &c_mseconds);

    // calculate the ms left
    ms_left = ((self->alarm_time_s * 1000) + self->alarm_time_ms) - ((c_seconds * 1000) + c_mseconds);
    if (!self->alarmset || ms_left < 0) {
        ms_left = 0;
    }
    return mp_obj_new_int(ms_left);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_rtc_alarm_left_obj, 1, 2, pyb_rtc_alarm_left);

STATIC mp_obj_t pyb_rtc_alarm_cancel(size_t n_args, const mp_obj_t *args) {
    // only alarm id 0 is available
    if (n_args > 1 && mp_obj_get_int(args[1]) != 0) {
        mp_raise_OSError(MP_ENODEV);
    }
    // disable the alarm
    pyb_rtc_disable_alarm();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_rtc_alarm_cancel_obj, 1, 2, pyb_rtc_alarm_cancel);

/// \method irq(trigger, priority, handler, wake)
STATIC mp_obj_t pyb_rtc_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_arg_val_t args[mp_irq_INIT_NUM_ARGS];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, mp_irq_INIT_NUM_ARGS, mp_irq_init_args, args);
    pyb_rtc_obj_t *self = pos_args[0];

    // save the power mode data for later
    uint8_t pwrmode = (args[3].u_obj == mp_const_none) ? PYB_PWR_MODE_ACTIVE : mp_obj_get_int(args[3].u_obj);
    if (pwrmode > (PYB_PWR_MODE_ACTIVE | PYB_PWR_MODE_LPDS | PYB_PWR_MODE_HIBERNATE)) {
        goto invalid_args;
    }

    // check the trigger
    if (mp_obj_get_int(args[0].u_obj) == PYB_RTC_ALARM0) {
        self->pwrmode = pwrmode;
        pyb_rtc_irq_enable((mp_obj_t)self);
    } else {
        goto invalid_args;
    }

    // the interrupt priority is ignored since it's already set to to highest level by the sleep module
    // to make sure that the wakeup irqs are always called first when resuming from sleep

    // create the callback
    mp_obj_t _irq = mp_irq_new ((mp_obj_t)self, args[2].u_obj, &pyb_rtc_irq_methods);
    self->irq_obj = _irq;

    return _irq;

invalid_args:
    mp_raise_ValueError(MP_ERROR_TEXT("invalid argument(s) value"));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_rtc_irq_obj, 1, pyb_rtc_irq);

STATIC const mp_rom_map_elem_t pyb_rtc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init),            MP_ROM_PTR(&pyb_rtc_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),          MP_ROM_PTR(&pyb_rtc_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_now),             MP_ROM_PTR(&pyb_rtc_now_obj) },
    { MP_ROM_QSTR(MP_QSTR_alarm),           MP_ROM_PTR(&pyb_rtc_alarm_obj) },
    { MP_ROM_QSTR(MP_QSTR_alarm_left),      MP_ROM_PTR(&pyb_rtc_alarm_left_obj) },
    { MP_ROM_QSTR(MP_QSTR_alarm_cancel),    MP_ROM_PTR(&pyb_rtc_alarm_cancel_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq),             MP_ROM_PTR(&pyb_rtc_irq_obj) },

    // class constants
    { MP_ROM_QSTR(MP_QSTR_ALARM0),          MP_ROM_INT(PYB_RTC_ALARM0) },
};
STATIC MP_DEFINE_CONST_DICT(pyb_rtc_locals_dict, pyb_rtc_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    pyb_rtc_type,
    MP_QSTR_RTC,
    MP_TYPE_FLAG_NONE,
    make_new, pyb_rtc_make_new,
    locals_dict, &pyb_rtc_locals_dict
    );

STATIC const mp_irq_methods_t pyb_rtc_irq_methods = {
    .init = pyb_rtc_irq,
    .enable = pyb_rtc_irq_enable,
    .disable = pyb_rtc_irq_disable,
    .flags = pyb_rtc_irq_flags
};
