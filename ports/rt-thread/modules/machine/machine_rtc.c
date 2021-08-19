/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 ChenYong (chenyong@rt-thread.com)
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
#include <stdint.h>
#include <string.h>

#include "py/nlr.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "shared/timeutils/timeutils.h"
#include "modmachine.h"

#ifdef MICROPYTHON_USING_MACHINE_RTC

#include <rtthread.h>
#include <drivers/rtc.h>
#include <time.h>

#define MP_YEAR_BASE   1900

const mp_obj_type_t machine_rtc_type;

// singleton RTC object
STATIC const mp_obj_base_t machine_rtc_obj = {&machine_rtc_type};

STATIC void error_check(bool status, const char *msg) {
    if (!status) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, msg));
    }
}

STATIC mp_obj_t machine_rtc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
#define MP_RTC_DEV_NAME "rtc"
    rt_device_t rtc_deivce = RT_NULL;

    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // check RTC device
    rtc_deivce = rt_device_find(MP_RTC_DEV_NAME);
    if (rtc_deivce == RT_NULL || rtc_deivce->type != RT_Device_Class_RTC) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "RTC(%s) don't exist", MP_RTC_DEV_NAME)); 
    }

    // return constant object
    return (mp_obj_t)&machine_rtc_obj;
}

STATIC mp_obj_t machine_rtc_datetime_helper(mp_uint_t n_args, const mp_obj_t *args) {
    if (n_args == 1) {
        struct tm *tblock;
        time_t t;
        // Get time
        t = time(RT_NULL);
        tblock = localtime(&t);

        mp_uint_t seconds = timeutils_mktime(tblock->tm_year + MP_YEAR_BASE, tblock->tm_mon + 1, tblock->tm_mday,
                                             tblock->tm_hour, tblock->tm_min, tblock->tm_sec);
        timeutils_struct_time_t tm;
        timeutils_seconds_since_2000_to_struct_time(seconds, &tm);

        mp_obj_t tuple[8] = {
            mp_obj_new_int(tm.tm_year),
            mp_obj_new_int(tm.tm_mon),
            mp_obj_new_int(tm.tm_mday),
            mp_obj_new_int(tm.tm_wday),
            mp_obj_new_int(tm.tm_hour),
            mp_obj_new_int(tm.tm_min),
            mp_obj_new_int(tm.tm_sec),
            mp_obj_new_int(0)
        };

        return mp_obj_new_tuple(8, tuple);
    } else {
        // Set time
        rt_err_t result;
        mp_obj_t *items;

        mp_obj_get_array_fixed_n(args[1], 8, &items);
        result = set_date(mp_obj_get_int(items[0]), mp_obj_get_int(items[1]), mp_obj_get_int(items[2]));
        error_check(result == RT_EOK, "Set date error");
        result = set_time(mp_obj_get_int(items[4]), mp_obj_get_int(items[5]), mp_obj_get_int(items[6]));
        error_check(result == RT_EOK, "Set time error");
        return mp_const_none;
    }
}

STATIC mp_obj_t machine_rtc_now(mp_uint_t n_args, const mp_obj_t *args) {
    return machine_rtc_datetime_helper(1, args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_rtc_now_obj, 0, 1, machine_rtc_now);

STATIC mp_obj_t machine_rtc_init(mp_uint_t n_args, const mp_obj_t *args) {
    return machine_rtc_datetime_helper(n_args, args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_rtc_init_obj, 1, 2, machine_rtc_init);

STATIC mp_obj_t machine_rtc_deinit(mp_uint_t n_args, const mp_obj_t *args) {
    rt_err_t result;
    struct tm tblock;

    tblock.tm_year = 2015 - MP_YEAR_BASE;
    tblock.tm_mon = 0;
    tblock.tm_mday = 1;
    tblock.tm_hour = 0;
    tblock.tm_min = 0;
    tblock.tm_sec = 0;
    result = set_date(tblock.tm_year + MP_YEAR_BASE, tblock.tm_mon + 1, tblock.tm_mday);
    error_check(result == RT_EOK, "Set date error");
    result = set_time(tblock.tm_hour, tblock.tm_min, tblock.tm_sec);
    error_check(result == RT_EOK, "Set time error");
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_rtc_deinit_obj, 0, 1, machine_rtc_deinit);

STATIC const mp_rom_map_elem_t machine_rtc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_rtc_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_rtc_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_now), MP_ROM_PTR(&machine_rtc_now_obj) },
};
STATIC MP_DEFINE_CONST_DICT(machine_rtc_locals_dict, machine_rtc_locals_dict_table);

const mp_obj_type_t machine_rtc_type = {
    { &mp_type_type },
    .name = MP_QSTR_RTC,
    .make_new = machine_rtc_make_new,
    .locals_dict = (mp_obj_t) &machine_rtc_locals_dict,
};

#endif // MICROPYTHON_USING_MACHINE_RTC
