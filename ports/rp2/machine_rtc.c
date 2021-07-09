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

#include "py/nlr.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "shared/timeutils/timeutils.h"
#include "hardware/rtc.h"
#include "pico/util/datetime.h"
#include "modmachine.h"

typedef struct _machine_rtc_obj_t {
    mp_obj_base_t base;
} machine_rtc_obj_t;

// singleton RTC object
STATIC const machine_rtc_obj_t machine_rtc_obj = {{&machine_rtc_type}};

STATIC mp_obj_t machine_rtc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    bool r = rtc_running();

    if (!r) {
        // This shouldn't happen as rtc_init() is already called in main so
        // it's here just in case
        rtc_init();
        datetime_t t = { .month = 1, .day = 1 };
        rtc_set_datetime(&t);
    }
    // return constant object
    return (mp_obj_t)&machine_rtc_obj;
}

STATIC mp_obj_t machine_rtc_datetime(mp_uint_t n_args, const mp_obj_t *args) {
    if (n_args == 1) {
        bool ret;
        datetime_t t;

        ret = rtc_get_datetime(&t);
        if (!ret) {
            mp_raise_OSError(MP_EIO);
        }

        mp_obj_t tuple[8] = {
            mp_obj_new_int(t.year),
            mp_obj_new_int(t.month),
            mp_obj_new_int(t.day),
            mp_obj_new_int(t.dotw),
            mp_obj_new_int(t.hour),
            mp_obj_new_int(t.min),
            mp_obj_new_int(t.sec),
            mp_obj_new_int(0)
        };

        return mp_obj_new_tuple(8, tuple);
    } else {
        mp_obj_t *items;

        mp_obj_get_array_fixed_n(args[1], 8, &items);

        datetime_t t = {
            .year = mp_obj_get_int(items[0]),
            .month = mp_obj_get_int(items[1]),
            .day = mp_obj_get_int(items[2]),
            .hour = mp_obj_get_int(items[4]),
            .min = mp_obj_get_int(items[5]),
            .sec = mp_obj_get_int(items[6]),
        };
        // Deliberately ignore the weekday argument and compute the proper value
        t.dotw = timeutils_calc_weekday(t.year, t.month, t.day);

        if (!rtc_set_datetime(&t)) {
            mp_raise_OSError(MP_EINVAL);
        }

    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_rtc_datetime_obj, 1, 2, machine_rtc_datetime);

STATIC const mp_rom_map_elem_t machine_rtc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_datetime), MP_ROM_PTR(&machine_rtc_datetime_obj) },
};
STATIC MP_DEFINE_CONST_DICT(machine_rtc_locals_dict, machine_rtc_locals_dict_table);

const mp_obj_type_t machine_rtc_type = {
    { &mp_type_type },
    .name = MP_QSTR_RTC,
    .make_new = machine_rtc_make_new,
    .locals_dict = (mp_obj_t)&machine_rtc_locals_dict,
};
