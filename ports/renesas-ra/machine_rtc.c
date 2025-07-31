/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2021 Renesas Electronics Corporation
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

#include "py/runtime.h"
#include "extmod/modmachine.h"
#include "shared/timeutils/timeutils.h"
#include "extint.h"
#include "rtc.h"
#include "irq.h"
#if defined(RA4M1) | defined(RA4M3) | defined(RA4W1) | defined(RA6M1) | defined(RA6M2) | defined(RA6M3)
#include "ra_rtc.h"
#endif

#define RTC_INIT_YEAR   2019
#define RTC_INIT_MONTH  1
#define RTC_INIT_DATE   1
#define RTC_INIT_WEEKDAY    2   /* Tuesday */
#define RTC_INIT_HOUR   0
#define RTC_INIT_MINUTE 0
#define RTC_INIT_SECOND 0

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

#define HAL_StatusTypeDef       void
// rtc_info indicates various things about RTC startup
// it's a bit of a hack at the moment
static mp_uint_t rtc_info;

static uint32_t rtc_startup_tick;
static bool rtc_need_init_finalise = false;
static uint32_t rtc_wakeup_param;

static void rtc_calendar_config(void) {
    ra_rtc_t tm;
    tm.year = RTC_INIT_YEAR - 2000;
    tm.month = RTC_INIT_MONTH;
    tm.date = RTC_INIT_DATE;
    tm.weekday = RTC_INIT_WEEKDAY;
    tm.hour = RTC_INIT_HOUR;
    tm.minute = RTC_INIT_MINUTE;
    tm.second = RTC_INIT_SECOND;
    ra_rtc_set_time(&tm);
}

void rtc_get_time(RTC_TimeTypeDef *time) {
    ra_rtc_t dt;
    ra_rtc_get_time(&dt);
    time->DayLightSaving = 0;
    time->Hours = dt.hour;
    time->Minutes = dt.minute;
    time->SecondFraction = 0;
    time->Seconds = dt.second;
    // time->StoreOperation;
    time->SubSeconds = 0;
    time->TimeFormat = 0;
}

void rtc_get_date(RTC_DateTypeDef *date) {
    ra_rtc_t dt;
    ra_rtc_get_time(&dt);
    date->Date = dt.date;
    date->Month = dt.month;
    date->WeekDay = dt.weekday;
    date->Year = (uint8_t)(dt.year - 2000);
}

void rtc_init_start(bool force_init) {
    /* Configure RTC prescaler and RTC data registers */
    #if (MICROPY_HW_RTC_SOURCE == 1)
    // clock source is LOCO
    ra_rtc_init(1);
    #else
    // clock source is subclock
    ra_rtc_init(0);
    #endif
    rtc_need_init_finalise = false;

    if (!force_init) {
        // So far, this case (force_init == false) is not called.
        rtc_info |= 0x40000;
        // or rtc_info |= 0x80000;
        return;
    }

    rtc_startup_tick = HAL_GetTick();
    rtc_info = 0x3f000000 | (rtc_startup_tick & 0xffffff);
    rtc_need_init_finalise = true;
}

void rtc_init_finalise(void) {
    if (!rtc_need_init_finalise) {
        return;
    }
    rtc_info = 0x20000000;
    rtc_info |= (HAL_GetTick() - rtc_startup_tick) & 0xffff;
    rtc_calendar_config();
    rtc_need_init_finalise = false;
}

uint64_t mp_hal_time_ns(void) {
    uint64_t ns = 0;
    #if MICROPY_HW_ENABLE_RTC
    // Get current according to the RTC.
    rtc_init_finalise();
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;
    rtc_get_time(&time);
    rtc_get_date(&date);
    ns = timeutils_seconds_since_epoch(2000 + date.Year, date.Month, date.Date, time.Hours, time.Minutes, time.Seconds);
    ns *= 1000000000ULL;
    #endif
    return ns;
}

/******************************************************************************/
// MicroPython bindings

typedef struct _machine_rtc_obj_t {
    mp_obj_base_t base;
} machine_rtc_obj_t;

static const machine_rtc_obj_t machine_rtc_obj = {{&machine_rtc_type}};

/// \classmethod \constructor()
/// Create an RTC object.
static mp_obj_t machine_rtc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // return constant object
    return MP_OBJ_FROM_PTR(&machine_rtc_obj);
}

// force rtc to re-initialise
mp_obj_t machine_rtc_init(mp_obj_t self_in) {
    rtc_init_start(true);
    rtc_init_finalise();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(machine_rtc_init_obj, machine_rtc_init);

/// \method info()
/// Get information about the startup time and reset source.
///
///  - The lower 0xffff are the number of milliseconds the RTC took to
///    start up.
///  - Bit 0x10000 is set if a power-on reset occurred.
///  - Bit 0x20000 is set if an external reset occurred
mp_obj_t machine_rtc_info(mp_obj_t self_in) {
    return mp_obj_new_int(rtc_info);
}
MP_DEFINE_CONST_FUN_OBJ_1(machine_rtc_info_obj, machine_rtc_info);

/// \method datetime([datetimetuple])
/// Get or set the date and time of the RTC.
///
/// With no arguments, this method returns an 8-tuple with the current
/// date and time.  With 1 argument (being an 8-tuple) it sets the date
/// and time.
///
/// The 8-tuple has the following format:
///
///     (year, month, day, weekday, hours, minutes, seconds, subseconds)
///
/// `weekday` is 1-7 for Monday through Sunday.
///
/// `subseconds` counts down from 255 to 0

#define MEG_DIV_64 (1000000 / 64)
#define MEG_DIV_SCALE ((RTC_SYNCH_PREDIV + 1) / 64)

#if defined(MICROPY_HW_RTC_USE_US) && MICROPY_HW_RTC_USE_US
uint32_t rtc_subsec_to_us(uint32_t ss) {
    return ((RTC_SYNCH_PREDIV - ss) * MEG_DIV_64) / MEG_DIV_SCALE;
}

uint32_t rtc_us_to_subsec(uint32_t us) {
    return RTC_SYNCH_PREDIV - (us * MEG_DIV_SCALE / MEG_DIV_64);
}
#else
#define rtc_us_to_subsec
#define rtc_subsec_to_us
#endif

mp_obj_t machine_rtc_datetime(size_t n_args, const mp_obj_t *args) {
    rtc_init_finalise();
    if (n_args == 1) {
        ra_rtc_t time;
        ra_rtc_get_time(&time);
        mp_obj_t tuple[8] = {
            mp_obj_new_int(time.year),
            mp_obj_new_int(time.month),
            mp_obj_new_int(time.date),
            mp_obj_new_int(time.weekday),
            mp_obj_new_int(time.hour),
            mp_obj_new_int(time.minute),
            mp_obj_new_int(time.second),
            mp_obj_new_int(0),
        };
        return mp_obj_new_tuple(8, tuple);
    } else {
        // set date and time
        mp_obj_t *items;
        mp_obj_get_array_fixed_n(args[1], 8, &items);
        ra_rtc_t tm;
        tm.year = mp_obj_get_int(items[0]);
        tm.month = mp_obj_get_int(items[1]);
        tm.date = mp_obj_get_int(items[2]);
        tm.weekday = mp_obj_get_int(items[3]);
        tm.hour = mp_obj_get_int(items[4]);
        tm.minute = mp_obj_get_int(items[5]);
        tm.second = mp_obj_get_int(items[6]);
        ra_rtc_set_time(&tm);
        return mp_const_none;
    }
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_rtc_datetime_obj, 1, 2, machine_rtc_datetime);

// wakeup(None)
// wakeup(ms, callback=None) - ms should be between 4ms - 2000ms
// wakeup(wucksel, wut, callback) - not implemented
mp_obj_t machine_rtc_wakeup(size_t n_args, const mp_obj_t *args) {
    bool enable = false;
    mp_int_t ms;
    mp_obj_t callback = mp_const_none;
    uint32_t period = 0;
    if (args[1] == mp_const_none) {
        // disable wakeup
    } else {
        // time given in ms
        ms = mp_obj_get_int(args[1]);
        if (ms <= 4) {
            period = 6;
        } else if (ms <= 8) {
            period = 7;
        } else if (ms <= 16) {
            period = 8;
        } else if (ms <= 32) {
            period = 9;
        } else if (ms <= 63) {
            period = 10;
        } else if (ms <= 125) {
            period = 11;
        } else if (ms <= 250) {
            period = 12;
        } else if (ms <= 500) {
            period = 13;
        } else if (ms <= 1000) {
            period = 14;
        } else if (ms <= 2000) {
            period = 15;
        }
        enable = true;
    }
    if (n_args >= 2) {
        callback = args[2];
    }
    // set the callback
    MP_STATE_PORT(pyb_extint_callback)[EXTI_RTC_WAKEUP] = callback;
    pyb_extint_callback_arg[EXTI_RTC_WAKEUP] = MP_OBJ_NEW_SMALL_INT(EXTI_RTC_WAKEUP);
    rtc_wakeup_param = EXTI_RTC_WAKEUP;
    if (enable) {
        if (callback != mp_const_none) {
            ra_rtc_set_period_func((void *)extint_callback, (void *)&rtc_wakeup_param);
        } else {
            ra_rtc_set_period_func((void *)NULL, (void *)NULL);
        }
        ra_rtc_set_period_time(period);
        ra_rtc_period_on();
    } else {
        ra_rtc_period_off();
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_rtc_wakeup_obj, 2, 3, machine_rtc_wakeup);

// calibration(None)
// calibration(cal)
// When an integer argument is provided, check that it falls in the range [-63(s) to 63(s)]
// and set the calibration value; otherwise return calibration value
mp_obj_t machine_rtc_calibration(size_t n_args, const mp_obj_t *args) {
    rtc_init_finalise();
    mp_int_t cal;
    if (n_args == 2) {
        cal = mp_obj_get_int(args[1]);
        if (cal < -63 || cal > 63) {
            mp_raise_ValueError(MP_ERROR_TEXT("calibration value out of range"));
        } else {
            ra_rtc_set_adjustment(cal, 0); // calibration for second
        }
        return mp_const_none;
    } else {
        // get calibration register
        cal = (mp_int_t)ra_rtc_get_adjustment();
        return mp_obj_new_int(cal);
    }
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_rtc_calibration_obj, 1, 2, machine_rtc_calibration);

static const mp_rom_map_elem_t machine_rtc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_rtc_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_info), MP_ROM_PTR(&machine_rtc_info_obj) },
    { MP_ROM_QSTR(MP_QSTR_datetime), MP_ROM_PTR(&machine_rtc_datetime_obj) },
    { MP_ROM_QSTR(MP_QSTR_wakeup), MP_ROM_PTR(&machine_rtc_wakeup_obj) },
    { MP_ROM_QSTR(MP_QSTR_calibration), MP_ROM_PTR(&machine_rtc_calibration_obj) },
};
static MP_DEFINE_CONST_DICT(machine_rtc_locals_dict, machine_rtc_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_rtc_type,
    MP_QSTR_RTC,
    MP_TYPE_FLAG_NONE,
    make_new, machine_rtc_make_new,
    locals_dict, &machine_rtc_locals_dict
    );
