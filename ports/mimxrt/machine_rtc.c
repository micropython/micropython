/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Damien P. George
 * Copyright (c) 2021 "Robert Hammelrath" <robert@hammelrath.com>
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
#include "modmachine.h"
#include "ticks.h"
#include "fsl_snvs_lp.h"

typedef struct _machine_rtc_obj_t {
    mp_obj_base_t base;
    mp_obj_t callback;
} machine_rtc_obj_t;

// Singleton RTC object.
STATIC const machine_rtc_obj_t machine_rtc_obj = {{&machine_rtc_type}};
uint32_t us_offset = 0;

// Start the RTC Timer.
void machine_rtc_start(void) {

    SNVS_LP_SRTC_StartTimer(SNVS);
    // If the date is not set, set it to a more recent start date,
    // MicroPython's first commit.
    snvs_lp_srtc_datetime_t srtc_date;
    SNVS_LP_SRTC_GetDatetime(SNVS, &srtc_date);
    if (srtc_date.year <= 1970) {
        srtc_date = (snvs_lp_srtc_datetime_t) {
            .year = 2013,
            .month = 10,
            .day = 14,
            .hour = 19,
            .minute = 53,
            .second = 11,
        };
        SNVS_LP_SRTC_SetDatetime(SNVS, &srtc_date);
    }
}

STATIC mp_obj_t machine_rtc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // Check arguments.
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // Return constant object.
    return (mp_obj_t)&machine_rtc_obj;
}

STATIC mp_obj_t machine_rtc_datetime_helper(size_t n_args, const mp_obj_t *args) {
    if (n_args == 1) {
        // Get date and time.
        snvs_lp_srtc_datetime_t srtc_date;
        SNVS_LP_SRTC_GetDatetime(SNVS, &srtc_date);

        mp_obj_t tuple[8] = {
            mp_obj_new_int(srtc_date.year),
            mp_obj_new_int(srtc_date.month),
            mp_obj_new_int(srtc_date.day),
            mp_obj_new_int(timeutils_calc_weekday(srtc_date.year, srtc_date.month, srtc_date.day)),
            mp_obj_new_int(srtc_date.hour),
            mp_obj_new_int(srtc_date.minute),
            mp_obj_new_int(srtc_date.second),
            mp_obj_new_int((ticks_us64() + us_offset) % 1000000),
        };
        return mp_obj_new_tuple(8, tuple);
    } else {
        // Set date and time.
        mp_obj_t *items;
        mp_int_t year;
        mp_obj_get_array_fixed_n(args[1], 8, &items);

        snvs_lp_srtc_datetime_t srtc_date;
        year = mp_obj_get_int(items[0]);
        srtc_date.year = year >= 100 ? year : year + 2000; // allow 21 for 2021
        srtc_date.month = mp_obj_get_int(items[1]);
        srtc_date.day = mp_obj_get_int(items[2]);
        // Ignore weekday at items[3]
        srtc_date.hour = mp_obj_get_int(items[4]);
        srtc_date.minute = mp_obj_get_int(items[5]);
        srtc_date.second = mp_obj_get_int(items[6]);
        if (SNVS_LP_SRTC_SetDatetime(SNVS, &srtc_date) != kStatus_Success) {
            mp_raise_ValueError(NULL);
        }
        us_offset = (1000000 + mp_obj_get_int(items[7]) - ticks_us64() % 1000000) % 1000000;

        return mp_const_none;
    }
}

STATIC mp_obj_t machine_rtc_datetime(mp_uint_t n_args, const mp_obj_t *args) {
    return machine_rtc_datetime_helper(n_args, args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_rtc_datetime_obj, 1, 2, machine_rtc_datetime);

STATIC mp_obj_t machine_rtc_now(mp_obj_t self_in) {
    // Get date and time in CPython order.
    snvs_lp_srtc_datetime_t srtc_date;
    SNVS_LP_SRTC_GetDatetime(SNVS, &srtc_date);

    mp_obj_t tuple[8] = {
        mp_obj_new_int(srtc_date.year),
        mp_obj_new_int(srtc_date.month),
        mp_obj_new_int(srtc_date.day),
        mp_obj_new_int(srtc_date.hour),
        mp_obj_new_int(srtc_date.minute),
        mp_obj_new_int(srtc_date.second),
        mp_obj_new_int((ticks_us64() + us_offset) % 1000000),
        mp_const_none,
    };
    return mp_obj_new_tuple(8, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_rtc_now_obj, machine_rtc_now);

STATIC mp_obj_t machine_rtc_init(mp_obj_t self_in, mp_obj_t date) {
    mp_obj_t args[2] = {self_in, date};
    machine_rtc_datetime_helper(2, args);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_rtc_init_obj, machine_rtc_init);

// calibration(cal)
// When the argument is a number in the range [-16 to 15], set the calibration value.
STATIC mp_obj_t machine_rtc_calibration(mp_obj_t self_in, mp_obj_t cal_in) {
    mp_int_t cal = 0;
    snvs_lp_srtc_config_t snvsSrtcConfig;
    cal = mp_obj_get_int(cal_in);
    if (cal < -16 || cal > 15) {
        mp_raise_ValueError(MP_ERROR_TEXT("value out of range -16 to 15"));
    } else {
        snvsSrtcConfig.srtcCalEnable = true;
        snvsSrtcConfig.srtcCalValue = cal & 0x1f; // 5 bit 2's complement
        SNVS_LP_SRTC_Init(SNVS, &snvsSrtcConfig);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_rtc_calibration_obj, machine_rtc_calibration);

STATIC const mp_rom_map_elem_t machine_rtc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_rtc_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_datetime), MP_ROM_PTR(&machine_rtc_datetime_obj) },
    { MP_ROM_QSTR(MP_QSTR_now), MP_ROM_PTR(&machine_rtc_now_obj) },
    { MP_ROM_QSTR(MP_QSTR_calibration), MP_ROM_PTR(&machine_rtc_calibration_obj) },
};
STATIC MP_DEFINE_CONST_DICT(machine_rtc_locals_dict, machine_rtc_locals_dict_table);

const mp_obj_type_t machine_rtc_type = {
    { &mp_type_type },
    .name = MP_QSTR_RTC,
    .make_new = machine_rtc_make_new,
    .locals_dict = (mp_obj_t)&machine_rtc_locals_dict,
};
