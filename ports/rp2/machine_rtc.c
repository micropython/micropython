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

#include <stdio.h>
#include <string.h>

#include <time.h>
#include <sys/time.h>

#include "pico/aon_timer.h"

#include "py/nlr.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "extmod/modmachine.h"
#include "shared/timeutils/timeutils.h"

typedef struct _machine_rtc_obj_t {
    mp_obj_base_t base;
} machine_rtc_obj_t;

// singleton RTC object
static const machine_rtc_obj_t machine_rtc_obj = {{&machine_rtc_type}};

static mp_obj_t machine_rtc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    bool r = aon_timer_is_running();

    if (!r) {
        // This shouldn't happen. it's here just in case
        struct timespec ts = { 0, 0 };
        aon_timer_start(&ts);
        mp_hal_time_ns_set_from_rtc();
    }
    // return constant object
    return (mp_obj_t)&machine_rtc_obj;
}

static mp_obj_t machine_rtc_datetime(mp_uint_t n_args, const mp_obj_t *args) {
    if (n_args == 1) {
        struct timespec ts;
        timeutils_struct_time_t tm;
        aon_timer_get_time(&ts);
        timeutils_seconds_since_epoch_to_struct_time(ts.tv_sec, &tm);
        mp_obj_t tuple[8] = {
            mp_obj_new_int(tm.tm_year),
            mp_obj_new_int(tm.tm_mon),
            mp_obj_new_int(tm.tm_mday),
            mp_obj_new_int(tm.tm_wday),
            mp_obj_new_int(tm.tm_hour),
            mp_obj_new_int(tm.tm_min),
            mp_obj_new_int(tm.tm_sec),
            mp_obj_new_int(0),
        };
        return mp_obj_new_tuple(8, tuple);
    } else {
        mp_obj_t *items;
        mp_obj_get_array_fixed_n(args[1], 8, &items);
        timeutils_struct_time_t tm = {
            .tm_year = mp_obj_get_int(items[0]),
            .tm_mon = mp_obj_get_int(items[1]),
            .tm_mday = mp_obj_get_int(items[2]),
            .tm_hour = mp_obj_get_int(items[4]),
            .tm_min = mp_obj_get_int(items[5]),
            .tm_sec = mp_obj_get_int(items[6]),
        };
        struct timespec ts = { 0, 0 };
        ts.tv_sec = timeutils_seconds_since_epoch(tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        aon_timer_set_time(&ts);
        mp_hal_time_ns_set_from_rtc();
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_rtc_datetime_obj, 1, 2, machine_rtc_datetime);

static const mp_rom_map_elem_t machine_rtc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_datetime), MP_ROM_PTR(&machine_rtc_datetime_obj) },
};
static MP_DEFINE_CONST_DICT(machine_rtc_locals_dict, machine_rtc_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_rtc_type,
    MP_QSTR_RTC,
    MP_TYPE_FLAG_NONE,
    make_new, machine_rtc_make_new,
    locals_dict, &machine_rtc_locals_dict
    );
