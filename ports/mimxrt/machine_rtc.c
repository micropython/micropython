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

#include "py/obj.h"
#include "py/runtime.h"
#include "lib/timeutils/timeutils.h"
#include "machine_rtc.h"

typedef struct _machine_rtc_obj_t {
    mp_obj_base_t base;
    mp_obj_t callback;
} machine_rtc_obj_t;


// singleton RTC object
STATIC const machine_rtc_obj_t machine_rtc_obj = {{&machine_rtc_type}};

// Start up the RTC if needed

// calculate the weekday from the date
// The result is zero based with 0 = sunday.
// by Michael Keith and Tom Craver, 1990

static int weekday(int y, int m, int d) {
    return (d += m < 3 ? y-- : y - 2, 23 * m / 9 + d + 4 + y / 4 - y / 100 + y / 400) % 7;
}

STATIC mp_obj_t machine_rtc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    SNVS_LP_SRTC_StartTimer(SNVS);

    // return constant object
    return (mp_obj_t)&machine_rtc_obj;
}

STATIC mp_obj_t machine_get_time() {
    snvs_lp_srtc_datetime_t srtcDate;

    SNVS_LP_SRTC_GetDatetime(SNVS, &srtcDate);

    mp_obj_t tuple[8] = {
        mp_obj_new_int(srtcDate.year),
        mp_obj_new_int(srtcDate.month),
        mp_obj_new_int(srtcDate.day),
        mp_obj_new_int(weekday(srtcDate.year, srtcDate.month, srtcDate.day)),
        mp_obj_new_int(srtcDate.hour),
        mp_obj_new_int(srtcDate.minute),
        mp_obj_new_int(srtcDate.second),
        mp_obj_new_int(0)
    };
    return mp_obj_new_tuple(8, tuple);
}

STATIC mp_obj_t machine_rtc_datetime_helper(mp_uint_t n_args, const mp_obj_t *args) {
    snvs_lp_srtc_datetime_t srtcDate;
    if (n_args == 1) {
        // Get time
        return machine_get_time();
    } else {
        // Set time
        mp_obj_t *items;
        mp_int_t year;
        mp_obj_get_array_fixed_n(args[1], 8, &items);

        year = mp_obj_get_int(items[0]);
        srtcDate.year = year >= 100 ? year : year + 2000; // allow 21 for 2021
        srtcDate.month = mp_obj_get_int(items[1]);
        srtcDate.day = mp_obj_get_int(items[2]);
        srtcDate.hour = mp_obj_get_int(items[4]);
        srtcDate.minute = mp_obj_get_int(items[5]);
        srtcDate.second = mp_obj_get_int(items[6]);
        SNVS_LP_SRTC_SetDatetime(SNVS, &srtcDate);

        return mp_const_none;
    }
}

STATIC mp_obj_t machine_rtc_datetime(mp_uint_t n_args, const mp_obj_t *args) {
    return machine_rtc_datetime_helper(n_args, args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_rtc_datetime_obj, 1, 2, machine_rtc_datetime);

STATIC mp_obj_t machine_rtc_now(mp_obj_t self_in) {
    (void)self_in; // unused
    return machine_get_time();
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_rtc_now_obj, machine_rtc_now);

STATIC mp_obj_t machine_rtc_init(mp_obj_t self_in, mp_obj_t date) {
    mp_obj_t args[2] = {self_in, date};
    machine_rtc_datetime_helper(2, args);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_rtc_init_obj, machine_rtc_init);

// calibration(cal)
// When the argument falls in a number in the range [-16 to 15],
// set the calibration value.

STATIC mp_obj_t machine_rtc_calibration(mp_obj_t self_in, mp_obj_t cal_in) {
    mp_int_t cal = 0;
    snvs_lp_srtc_config_t snvsSrtcConfig;
    cal = mp_obj_get_int(cal_in);
    if (cal < -16 || cal > 15) {
        mp_raise_ValueError(MP_ERROR_TEXT("value out of range -16 to 15"));
    } else {
        snvsSrtcConfig.srtcCalEnable = true;
        snvsSrtcConfig.srtcCalValue = cal & 0x1f;  // 5 bit 2's complement
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
