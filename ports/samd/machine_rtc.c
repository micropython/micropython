/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Damien P. George
 * Copyright (c) 2022 "Robert Hammelrath" <robert@hammelrath.com>
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

#include "py/runtime.h"
#include "shared/timeutils/timeutils.h"
#include "py/mphal.h"
#include "extmod/modmachine.h"
#include "sam.h"

typedef struct _machine_rtc_obj_t {
    mp_obj_base_t base;
    mp_obj_t callback;
} machine_rtc_obj_t;

// Singleton RTC object.
static const machine_rtc_obj_t machine_rtc_obj = {{&machine_rtc_type}};

// Start the RTC Timer.
void machine_rtc_start(bool force) {
    #if defined(MCU_SAMD21)

    if (RTC->MODE2.CTRL.bit.ENABLE == 0 || force) {
        // Enable the 1k Clock
        GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK8 | GCLK_CLKCTRL_ID_RTC;

        RTC->MODE2.CTRL.reg = RTC_MODE2_CTRL_SWRST;
        while (RTC->MODE2.STATUS.bit.SYNCBUSY) {
        }
        RTC->MODE2.CTRL.reg =
            RTC_MODE2_CTRL_MODE_CLOCK |
            RTC_MODE2_CTRL_PRESCALER_DIV1024 |
            RTC_MODE2_CTRL_ENABLE;
        while (RTC->MODE2.STATUS.bit.SYNCBUSY) {
        }
    }

    #elif defined(MCU_SAMD51)

    if (RTC->MODE2.CTRLA.bit.ENABLE == 0 || force) {
        RTC->MODE2.CTRLA.reg = RTC_MODE2_CTRLA_SWRST;
        while (RTC->MODE2.SYNCBUSY.bit.SWRST) {
        }
        RTC->MODE2.CTRLA.reg =
            RTC_MODE2_CTRLA_MODE_CLOCK |
            RTC_MODE2_CTRLA_CLOCKSYNC |
            RTC_MODE2_CTRLA_PRESCALER_DIV1024 |
            RTC_MODE2_CTRLA_ENABLE;
        while (RTC->MODE2.SYNCBUSY.bit.ENABLE) {
        }
    }
    #endif
}

// Get the time from the RTC and put it into a tm struct.
void rtc_gettime(timeutils_struct_time_t *tm) {
    tm->tm_year = RTC->MODE2.CLOCK.bit.YEAR + 2000;
    tm->tm_mon = RTC->MODE2.CLOCK.bit.MONTH;
    tm->tm_mday = RTC->MODE2.CLOCK.bit.DAY;
    tm->tm_hour = RTC->MODE2.CLOCK.bit.HOUR;
    tm->tm_min = RTC->MODE2.CLOCK.bit.MINUTE;
    tm->tm_sec = RTC->MODE2.CLOCK.bit.SECOND;
}

static mp_obj_t machine_rtc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // Check arguments.
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    // RTC was already started at boot time. So nothing to do here.
    // Return constant object.
    return (mp_obj_t)&machine_rtc_obj;
}

static mp_obj_t machine_rtc_datetime_helper(size_t n_args, const mp_obj_t *args, int hour_index) {
    // Rtc *rtc = RTC;
    if (n_args == 1) {
        // Get date and time.
        timeutils_struct_time_t tm;
        rtc_gettime(&tm);

        mp_obj_t tuple[8] = {
            mp_obj_new_int(tm.tm_year),
            mp_obj_new_int(tm.tm_mon),
            mp_obj_new_int(tm.tm_mday),
            mp_obj_new_int(timeutils_calc_weekday(tm.tm_year, tm.tm_mon, tm.tm_mday)),
            mp_obj_new_int(tm.tm_hour),
            mp_obj_new_int(tm.tm_min),
            mp_obj_new_int(tm.tm_sec),
            mp_obj_new_int(0),
        };
        return mp_obj_new_tuple(8, tuple);
    } else {
        // Set date and time.
        mp_obj_t *items;
        mp_obj_get_array_fixed_n(args[1], 8, &items);

        uint32_t date =
            RTC_MODE2_CLOCK_YEAR(mp_obj_get_int(items[0]) % 100) |
            RTC_MODE2_CLOCK_MONTH(mp_obj_get_int(items[1])) |
            RTC_MODE2_CLOCK_DAY(mp_obj_get_int(items[2])) |
            RTC_MODE2_CLOCK_HOUR(mp_obj_get_int(items[hour_index])) |
            RTC_MODE2_CLOCK_MINUTE(mp_obj_get_int(items[hour_index + 1])) |
            RTC_MODE2_CLOCK_SECOND(mp_obj_get_int(items[hour_index + 2]));

        RTC->MODE2.CLOCK.reg = date;
        #if defined(MCU_SAMD21)
        while (RTC->MODE2.STATUS.bit.SYNCBUSY) {
        }
        #elif defined(MCU_SAMD51)
        while (RTC->MODE2.SYNCBUSY.bit.CLOCKSYNC) {
        }
        #endif

        mp_hal_time_ns_set_from_rtc();
        return mp_const_none;
    }
}

static mp_obj_t machine_rtc_datetime(mp_uint_t n_args, const mp_obj_t *args) {
    return machine_rtc_datetime_helper(n_args, args, 4);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_rtc_datetime_obj, 1, 2, machine_rtc_datetime);

static mp_obj_t machine_rtc_init(mp_obj_t self_in, mp_obj_t date) {
    mp_obj_t args[2] = {self_in, date};
    machine_rtc_datetime_helper(2, args, 3);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_rtc_init_obj, machine_rtc_init);

// calibration(cal)
// When the argument is a number in the range [-16 to 15], set the calibration value.
static mp_obj_t machine_rtc_calibration(mp_obj_t self_in, mp_obj_t cal_in) {
    int8_t cal = 0;
    // Make it negative for a "natural" behavior:
    // value > 0: faster, value < 0: slower
    cal = -mp_obj_get_int(cal_in);
    RTC->MODE2.FREQCORR.reg = (uint8_t)cal;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_rtc_calibration_obj, machine_rtc_calibration);

static const mp_rom_map_elem_t machine_rtc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_rtc_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_datetime), MP_ROM_PTR(&machine_rtc_datetime_obj) },
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
