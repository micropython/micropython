/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 "Krzysztof Adamski" <k@japko.eu>
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

// std includes
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>


// micropython includes
#include "py/nlr.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "shared/timeutils/timeutils.h"
#include "py/misc.h"


// MTB includes
#include "cyhal.h"


// port-specific includes
#include "modmachine.h"

/* The values are reflected to match MPY reset values*/
#define RTC_INIT_YEAR       2015
#define RTC_INIT_MONTH      1   /* January */
#define RTC_INIT_MDAY       1
#define RTC_INIT_WDAY       4   /* Thursday */
#define RTC_INIT_HOUR       0
#define RTC_INIT_MINUTE     0
#define RTC_INIT_SECOND     0
#define RTC_INIT_DST        0
#define TM_YEAR_BASE        (1900u)
cyhal_rtc_t psoc6_rtc;


typedef struct _machine_rtc_obj_t {
    mp_obj_base_t base;
} machine_rtc_obj_t;


// singleton RTC object
STATIC const machine_rtc_obj_t machine_rtc_obj = {{&machine_rtc_type}};

/* This function is run from main.c to init the RTC at boot time. This will set the RTC to PSoC default time: 1st Jan 2000*/
void rtc_init(void) {
    cy_rslt_t result = cyhal_rtc_init(&psoc6_rtc);

    if (CY_RSLT_SUCCESS != result) {
        mp_raise_ValueError(MP_ERROR_TEXT("cyhal_rtc_init failed !"));
    }
}

// Machine RTC methods - port-specific definitions
// RTC constructor
STATIC mp_obj_t machine_rtc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    // return constant object
    return (mp_obj_t)&machine_rtc_obj;
}

// Helper function to set/get datetime
STATIC mp_obj_t machine_rtc_datetime_helper(mp_uint_t n_args, const mp_obj_t *args) {
    if (n_args == 1) {
        struct tm current_date_time = {0};
        cy_rslt_t result = cyhal_rtc_read(&psoc6_rtc, &current_date_time);
        if (CY_RSLT_SUCCESS != result) {
            mp_raise_ValueError(MP_ERROR_TEXT("cyhal_rtc_read failed !"));
        }

        mp_obj_t tuple[8] = {
            mp_obj_new_int(current_date_time.tm_year + TM_YEAR_BASE),
            mp_obj_new_int(current_date_time.tm_mon + 1),
            mp_obj_new_int(current_date_time.tm_mday),
            mp_obj_new_int(current_date_time.tm_wday),
            mp_obj_new_int(current_date_time.tm_hour),
            mp_obj_new_int(current_date_time.tm_min),
            mp_obj_new_int(current_date_time.tm_sec),
            mp_obj_new_int(0)
        };
        return mp_obj_new_tuple(8, tuple);
    } else {
        mp_obj_t *items;

        mp_obj_get_array_fixed_n(args[1], 8, &items);

        struct tm current_date_time =
        {
            .tm_year = mp_obj_get_int(items[0]) - TM_YEAR_BASE,
            .tm_mon = mp_obj_get_int(items[1]) - 1,
            .tm_mday = mp_obj_get_int(items[2]),
            .tm_wday = mp_obj_get_int(items[3]),
            .tm_hour = mp_obj_get_int(items[4]),
            .tm_min = mp_obj_get_int(items[5]),
            .tm_sec = mp_obj_get_int(items[6]),
        };

        cy_rslt_t result = cyhal_rtc_write(&psoc6_rtc, &current_date_time);

        if (CY_RSLT_SUCCESS != result) {
            mp_raise_ValueError(MP_ERROR_TEXT("cyhal_rtc_write failed ! Check if field values entered are within the specified range."));
        }
    }
    return mp_const_none;
}

// RTC.datetime([datetime])
STATIC mp_obj_t machine_rtc_datetime(mp_uint_t n_args, const mp_obj_t *args) {
    return machine_rtc_datetime_helper(n_args, args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_rtc_datetime_obj, 1, 2, machine_rtc_datetime);

// RTC.init(datetime)
STATIC mp_obj_t machine_rtc_init(mp_obj_t self_in, mp_obj_t date) {
    mp_obj_t args[2] = {self_in, date};
    // Check if RTC is correctly initialized already through main
    bool r = cyhal_rtc_is_enabled(&psoc6_rtc);
    if (!r) {
        rtc_init();
    }
    machine_rtc_datetime_helper(2, args);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_rtc_init_obj, machine_rtc_init);

// RTC.deinit()
STATIC mp_obj_t machine_rtc_deinit(mp_obj_t self_in) {
    /* Resets RTC to 1st Jan' 2015 as mentioned in MPY guide*/
    struct tm reset_date_time = {
        .tm_year = RTC_INIT_YEAR - TM_YEAR_BASE,
        .tm_mon = RTC_INIT_MONTH - 1,
        .tm_mday = RTC_INIT_MDAY,
        .tm_wday = RTC_INIT_WDAY,
        .tm_hour = RTC_INIT_HOUR,
        .tm_min = RTC_INIT_MINUTE,
        .tm_sec = RTC_INIT_SECOND,
        .tm_isdst = RTC_INIT_DST
    };
    cy_rslt_t result = cyhal_rtc_write(&psoc6_rtc, &reset_date_time);
    if (CY_RSLT_SUCCESS != result) {
        mp_raise_ValueError(MP_ERROR_TEXT("cyhal_rtc_write failed during RTC deinitialization!"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_rtc_deinit_obj, machine_rtc_deinit);

// RTC.now()
STATIC mp_obj_t machine_rtc_now(mp_obj_t self_in) {
    return machine_rtc_datetime_helper(1, NULL);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_rtc_now_obj, machine_rtc_now);

STATIC const mp_rom_map_elem_t machine_rtc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_rtc_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_rtc_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_datetime), MP_ROM_PTR(&machine_rtc_datetime_obj) },
    { MP_ROM_QSTR(MP_QSTR_now), MP_ROM_PTR(&machine_rtc_now_obj) },
};
STATIC MP_DEFINE_CONST_DICT(machine_rtc_locals_dict, machine_rtc_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_rtc_type,
    MP_QSTR_RTC,
    MP_TYPE_FLAG_NONE,
    make_new, machine_rtc_make_new,
    locals_dict, &machine_rtc_locals_dict
    );
