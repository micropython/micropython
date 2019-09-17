/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 "Eric Poulsen" <eric@zyxod.com>
 * Copyright (c) 2017 "Tom Manning" <tom@manningetal.com>
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
#include "wm_sockets2.0.3.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "wm_include.h"
#include "wm_rtc.h"

#include "py/nlr.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "timeutils.h"

#include "modmachine.h"

typedef struct _machine_rtc_obj_t {
    mp_obj_base_t base;
    mp_uint_t alarm_seconds;
    mp_obj_t handler;
} machine_rtc_obj_t;

typedef struct machine_rtc_irq_obj_t {
    mp_obj_base_t base;
} machine_rtc_irq_obj_t;

STATIC const machine_rtc_irq_obj_t machine_rtc_irq_object;

// singleton RTC object
STATIC const machine_rtc_obj_t machine_rtc_obj = {{&machine_rtc_type}};

STATIC mp_obj_t machine_rtc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // return constant object
    return (mp_obj_t)&machine_rtc_obj;
}

STATIC mp_obj_t machine_rtc_datetime_helper(mp_uint_t n_args, const mp_obj_t *args) {
    struct tm tblock;
    if (n_args == 1) {
        // Get time
        tls_get_rtc(&tblock);

        mp_uint_t seconds = timeutils_mktime(tblock.tm_year + W600_YEAR_BASE, tblock.tm_mon, tblock.tm_mday,
                                             tblock.tm_hour, tblock.tm_min, tblock.tm_sec);
        timeutils_struct_time_t tm;
        timeutils_seconds_since_2000_to_struct_time(seconds, &tm);

        //Zeller
        //tm.tm_wday = ((((tblock.tm_year + W600_YEAR_BASE) / 100) / 4) - 2 * ((tblock.tm_year + W600_YEAR_BASE) / 100) + ((tblock.tm_year + W600_YEAR_BASE) % 100) + (((tblock.tm_year + W600_YEAR_BASE) % 100) / 4) + (13 * ((tblock.tm_mon < 3 ? (12 + tblock.tm_mon) : tblock.tm_mon) + 1) / 5) + tblock.tm_mday - 1) % 7;

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

        mp_obj_t *items;
        mp_obj_get_array_fixed_n(args[1], 8, &items);
        tblock.tm_year = mp_obj_get_int(items[0]) - W600_YEAR_BASE;
        tblock.tm_mon = mp_obj_get_int(items[1]);
        tblock.tm_mday = mp_obj_get_int(items[2]);
        tblock.tm_hour = mp_obj_get_int(items[4]);
        tblock.tm_min = mp_obj_get_int(items[5]);
        tblock.tm_sec = mp_obj_get_int(items[6]);
        tls_set_rtc(&tblock);
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
    struct tm tblock;
    tblock.tm_year = 2015 - W600_YEAR_BASE;
    tblock.tm_mon = 1;
    tblock.tm_mday = 1;
    tblock.tm_hour = 0;
    tblock.tm_min = 0;
    tblock.tm_sec = 0;
    tls_set_rtc(&tblock);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_rtc_deinit_obj, 0, 1, machine_rtc_deinit);

STATIC mp_obj_t machine_rtc_datetime(mp_uint_t n_args, const mp_obj_t *args) {
    return machine_rtc_datetime_helper(n_args, args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_rtc_datetime_obj, 1, 2, machine_rtc_datetime);

STATIC mp_obj_t machine_rtc_alarm(mp_uint_t n_args, const mp_obj_t *args) {
    machine_rtc_obj_t *self = args[0];
    struct tm tblock;
    mp_obj_t *items;
    mp_obj_get_array_fixed_n(args[1], 8, &items);
    tblock.tm_year = mp_obj_get_int(items[0]) - W600_YEAR_BASE;
    tblock.tm_mon = mp_obj_get_int(items[1]);
    tblock.tm_mday = mp_obj_get_int(items[2]);
    tblock.tm_hour = mp_obj_get_int(items[4]);
    tblock.tm_min = mp_obj_get_int(items[5]);
    tblock.tm_sec = mp_obj_get_int(items[6]);
    tls_rtc_timer_start(&tblock);

    self->alarm_seconds = timeutils_mktime(tblock.tm_year + W600_YEAR_BASE,
                                           tblock.tm_mon, tblock.tm_mday,
                                           tblock.tm_hour, tblock.tm_min,
                                           tblock.tm_sec);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_rtc_alarm_obj, 1, 2, machine_rtc_alarm);

STATIC mp_obj_t machine_rtc_alarm_left(size_t n_args, const mp_obj_t *args) {
    machine_rtc_obj_t *self = args[0];
    int64_t ms_left;
    mp_uint_t seconds;
    struct tm tblock;

    tls_get_rtc(&tblock);

    seconds = timeutils_mktime(tblock.tm_year + W600_YEAR_BASE,
                               tblock.tm_mon, tblock.tm_mday,
                               tblock.tm_hour, tblock.tm_min,
                               tblock.tm_sec);

    ms_left = (self->alarm_seconds - seconds) * 1000;

    if (ms_left < 0) {
        ms_left = 0;
    }
    return mp_obj_new_int(ms_left);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_rtc_alarm_left_obj, 1, 2, machine_rtc_alarm_left);

STATIC mp_obj_t machine_rtc_cancel(mp_uint_t n_args, const mp_obj_t *args) {
    tls_rtc_timer_stop();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_rtc_cancel_obj, 0, 1, machine_rtc_cancel);

STATIC void machine_rtc_alarm_irq(u8 *arg) {
    machine_rtc_obj_t *self = (machine_rtc_obj_t *)arg;

    if (self->handler) {
        mp_sched_schedule(self->handler, self);//MP_OBJ_FROM_PTR(self));
    }
}

STATIC mp_obj_t machine_rtc_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_handler };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_handler, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    machine_rtc_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (n_args > 1 || kw_args->used != 0) {
        // configure irq
        mp_obj_t handler = args[ARG_handler].u_obj;
        if (handler == mp_const_none) {
            handler = MP_OBJ_NULL;
        }
        self->handler = handler;
        tls_rtc_isr_register(machine_rtc_alarm_irq, self);
    }

    // return the irq object
    return mp_const_none;//MP_OBJ_FROM_PTR(&machine_rtc_irq_object);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_rtc_irq_obj, 1, machine_rtc_irq);

STATIC const mp_rom_map_elem_t pyb_rtc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_rtc_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_rtc_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_datetime), MP_ROM_PTR(&machine_rtc_datetime_obj) },
    { MP_ROM_QSTR(MP_QSTR_now), MP_ROM_PTR(&machine_rtc_now_obj) },
    { MP_ROM_QSTR(MP_QSTR_alarm), MP_ROM_PTR(&machine_rtc_alarm_obj) },
    { MP_ROM_QSTR(MP_QSTR_alarm_left), MP_ROM_PTR(&machine_rtc_alarm_left_obj) },
    { MP_ROM_QSTR(MP_QSTR_cancel), MP_ROM_PTR(&machine_rtc_cancel_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq), MP_ROM_PTR(&machine_rtc_irq_obj) },
};
STATIC MP_DEFINE_CONST_DICT(pyb_rtc_locals_dict, pyb_rtc_locals_dict_table);

const mp_obj_type_t machine_rtc_type = {
    { &mp_type_type },
    .name = MP_QSTR_RTC,
    .make_new = machine_rtc_make_new,
    .locals_dict = (mp_obj_t) &pyb_rtc_locals_dict,
};

