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
#include <stdlib.h>
#include <inttypes.h>
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

#define rtc_assert_raise(msg, ret)   if (ret != CY_RSLT_SUCCESS) { \
        mp_raise_ValueError(MP_ERROR_TEXT(msg)); \
}


cyhal_rtc_t psoc6_rtc;
static bool rtc_memory = false;

typedef struct _machine_rtc_obj_t {
    mp_obj_base_t base;
    mp_obj_t callback;
    mp_obj_t alarm_period_s;
    mp_obj_t alarm_elapse_time_s;
    bool alarmset;
    bool repeat;
} machine_rtc_obj_t;

// singleton RTC object
static machine_rtc_obj_t machine_rtc_obj = {.base = {&machine_rtc_type}};

bool rtc_memory_write_enabled() {
    return rtc_memory;
}

/* This function is run from main.c to init the RTC at boot time. This will set the RTC to PSoC default time: 1st Jan 2000*/
void mod_rtc_init(void) {
    cy_rslt_t result = cyhal_rtc_init(&psoc6_rtc);
    rtc_assert_raise("cyhal_rtc_init failed !", result);
}

void mod_rtc_deinit() {
    if (rtc_memory_write_enabled() == false) {
        cyhal_rtc_free(&psoc6_rtc);
    }
}

// Helper function to set/get datetime
static mp_obj_t machine_rtc_datetime_helper(mp_uint_t n_args, const mp_obj_t *args) {
    struct tm current_date_time;
    if (n_args == 1) {
        cy_rslt_t result = cyhal_rtc_read(&psoc6_rtc, &current_date_time);
        rtc_assert_raise("cyhal_rtc_read failed !", result);

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

        current_date_time.tm_year = mp_obj_get_int(items[0]) - TM_YEAR_BASE;
        current_date_time.tm_mon = mp_obj_get_int(items[1]) - 1;
        current_date_time.tm_mday = mp_obj_get_int(items[2]);
        current_date_time.tm_wday = mp_obj_get_int(items[3]);
        current_date_time.tm_hour = mp_obj_get_int(items[4]);
        current_date_time.tm_min = mp_obj_get_int(items[5]);
        current_date_time.tm_sec = mp_obj_get_int(items[6]);

        cy_rslt_t result = cyhal_rtc_write(&psoc6_rtc, &current_date_time);
        rtc_assert_raise("cyhal_rtc_write failed ! Check if field values entered are within the specified range.", result);
    }
    return mp_const_none;
}

static inline uint64_t rtc_get_datetime_in_sec(mp_obj_t datetime) {
    size_t len;
    mp_obj_t *elem;
    mp_obj_get_array(datetime, &len, &elem);

    // localtime generates a tuple of len 8. CPython uses 9, so we accept both.
    if (len < 8 || len > 9) {
        mp_raise_TypeError(MP_ERROR_TEXT("mktime needs a tuple of length 8 or 9"));
    }

    return timeutils_mktime(mp_obj_get_int(elem[0]),
        mp_obj_get_int(elem[1]), mp_obj_get_int(elem[2]), mp_obj_get_int(elem[3]),
        mp_obj_get_int(elem[4]), mp_obj_get_int(elem[5]));

}

static inline uint64_t rtc_get_current_time_in_sec() {
    mp_obj_t datetime = machine_rtc_datetime_helper(1, NULL);

    size_t len;
    mp_obj_t *elem;
    mp_obj_get_array(datetime, &len, &elem);

    // localtime generates a tuple of len 8. CPython uses 9, so we accept both.
    if (len < 8 || len > 9) {
        mp_raise_TypeError(MP_ERROR_TEXT("mktime needs a tuple of length 8 or 9"));
    }

    return timeutils_mktime(mp_obj_get_int(elem[0]),
        mp_obj_get_int(elem[1]), mp_obj_get_int(elem[2]), mp_obj_get_int(elem[4]),
        mp_obj_get_int(elem[5]), mp_obj_get_int(elem[6]));

}

static void rtc_irq_handler(void *self, cyhal_rtc_event_t event);

static void rtc_alarm_setup(machine_rtc_obj_t *self) {
    cyhal_rtc_set_alarm_by_seconds(&psoc6_rtc, mp_obj_get_int(self->alarm_period_s));
    cyhal_rtc_register_callback(&psoc6_rtc, (cyhal_rtc_event_callback_t)rtc_irq_handler, self);
    cyhal_rtc_enable_event(&psoc6_rtc, CYHAL_RTC_ALARM, 3, true);
}

void rtc_irq_handler(void *self_in, cyhal_rtc_event_t event) {
    machine_rtc_obj_t *self = (machine_rtc_obj_t *)self_in;
    if (self->callback != NULL) {
        if (self->repeat) {
            rtc_alarm_setup(self);
        }
        mp_call_function_1((mp_obj_t)self->callback, mp_obj_new_int(event));
    }
}

static inline void rtc_get_dtime_struct(const mp_obj_t datetime, struct tm *dtime) {
    // set date and time
    mp_obj_t *items;
    size_t len;
    mp_obj_get_array(datetime, &len, &items);

    dtime->tm_sec = mp_obj_get_int(items[5]);
    dtime->tm_min = mp_obj_get_int(items[4]);
    dtime->tm_hour = mp_obj_get_int(items[3]);
    dtime->tm_mday = mp_obj_get_int(items[2]);
    dtime->tm_mon = mp_obj_get_int(items[1]) - 1;
    dtime->tm_year = mp_obj_get_int(items[0]) - TM_YEAR_BASE;
    dtime->tm_wday = 0;
    dtime->tm_yday = 0;
    dtime->tm_isdst = 0;

}

/******************************************************************************/
// MicroPython bindings

// RTC constructor
static mp_obj_t machine_rtc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    return (mp_obj_t)&machine_rtc_obj;
}

// RTC.init(datetime)
static mp_obj_t machine_rtc_init(mp_obj_t self_in, mp_obj_t datetime) {
    machine_rtc_obj_t *self = (machine_rtc_obj_t *)self_in;
    self->alarm_elapse_time_s = NULL;
    self->alarm_period_s = NULL;
    self->alarmset = false;
    self->callback = NULL;
    self->repeat = false;

    mp_obj_t args[2] = {self_in, datetime};
    machine_rtc_datetime_helper(2, args);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_rtc_init_obj, machine_rtc_init);

// RTC.datetime([datetime])
static mp_obj_t machine_rtc_datetime(mp_uint_t n_args, const mp_obj_t *datetime) {
    return machine_rtc_datetime_helper(n_args, datetime);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_rtc_datetime_obj, 1, 2, machine_rtc_datetime);

// RTC.deinit()
static mp_obj_t machine_rtc_deinit(mp_obj_t self_in) {
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
    rtc_assert_raise("cyhal_rtc_write failed during RTC deinitialization!", result);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_rtc_deinit_obj, machine_rtc_deinit);

// RTC.now()
static mp_obj_t machine_rtc_now(mp_obj_t self_in) {
    return machine_rtc_datetime_helper(1, NULL);
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_rtc_now_obj, machine_rtc_now);

// RTC.alarm()
static mp_obj_t machine_rtc_alarm(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_time, MP_ARG_OBJ,  {.u_obj = mp_const_none} },
        { MP_QSTR_repeat, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };

    // parse args
    uint64_t dtime_sec;
    machine_rtc_obj_t *self = pos_args[0];
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(args), allowed_args, args);

    uint64_t alarm_set_time_s = rtc_get_current_time_in_sec();

    self->repeat = args[1].u_bool;
    if (mp_obj_is_type(args[0].u_obj, &mp_type_tuple)) { // datetime tuple given
        // repeat cannot be used with a datetime tuple
        if (self->repeat) {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid argument(s) value"));
        }
        struct tm dtime;
        cyhal_alarm_active_t alarm_active =
        {
            .en_sec = 1,
            .en_min = 1,
            .en_hour = 1,
            .en_day = 0,
            .en_date = 1,
            .en_month = 1,
        };
        dtime_sec = rtc_get_datetime_in_sec(args[0].u_obj);
        self->alarm_period_s = mp_obj_new_int_from_uint(dtime_sec - alarm_set_time_s);
        rtc_get_dtime_struct(args[0].u_obj, &dtime);
        cyhal_rtc_set_alarm(&psoc6_rtc, &dtime, alarm_active);
    } else { // then it must be an integer
        self->alarm_period_s = mp_obj_new_int_from_uint(mp_obj_get_int(args[0].u_obj) / 1000);
        rtc_alarm_setup(self);
    }
    self->alarm_elapse_time_s = mp_obj_new_int_from_uint(alarm_set_time_s + mp_obj_get_int(self->alarm_period_s));
    self->alarmset = true;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_rtc_alarm_obj, 1, machine_rtc_alarm);

// RTC.alarm_left()
static mp_obj_t machine_rtc_alarm_left(size_t n_args, const mp_obj_t *args) {
    machine_rtc_obj_t *self = args[0];
    // only alarm id 0 is available
    if (n_args > 1 && mp_obj_get_int(args[1]) != CYHAL_RTC_ALARM) {
        mp_raise_OSError(MP_ENODEV);
    }
    if (self->alarmset) {
        uint64_t curr_time = rtc_get_current_time_in_sec();
        return mp_obj_new_int_from_uint((mp_obj_get_int(self->alarm_elapse_time_s) >= curr_time) ? ((mp_obj_get_int(self->alarm_elapse_time_s) - curr_time) * 1000) : 0);
    }
    mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Alarm not set! \n"));
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_rtc_alarm_left_obj, 1, 2, machine_rtc_alarm_left);

// RTC.cancel()
static mp_obj_t machine_rtc_cancel(size_t n_args, const mp_obj_t *args) {
    // only alarm id 0 is available
    if (n_args > 1 && mp_obj_get_int(args[1]) != CYHAL_RTC_ALARM) {
        mp_raise_OSError(MP_ENODEV);
    }
    // disable the alarm
    machine_rtc_obj_t *self = args[0];
    self->alarmset = false;
    cyhal_rtc_enable_event(&psoc6_rtc, CYHAL_RTC_ALARM, 3, false);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_rtc_alarm_cancel_obj, 1, 2, machine_rtc_cancel);

// RTC.memory()
static mp_obj_t machine_rtc_memory(size_t n_args, const mp_obj_t *args) {
    rtc_memory = true;
    return machine_rtc_datetime_helper(n_args, args);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_rtc_memory_obj, 1, 2, machine_rtc_memory);

// RTC.irq()
static mp_obj_t machine_rtc_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_trigger, ARG_handler, ARG_wake};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_trigger, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = CYHAL_RTC_ALARM} },
        { MP_QSTR_handler, MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_wake, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} }
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    machine_rtc_obj_t *self = pos_args[0];

    self->callback = args[ARG_handler].u_obj;
    if (args[ARG_handler].u_obj == mp_const_none) {
        self->callback = NULL;
    }

    if (args[ARG_trigger].u_int != CYHAL_RTC_ALARM) {
        mp_raise_OSError(MP_ENODEV);
    }

    if (args[ARG_wake].u_int != -1) {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("wake not implemented!\n"));
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_rtc_irq_obj, 1, machine_rtc_irq);

static const mp_rom_map_elem_t machine_rtc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__),     MP_ROM_PTR(&machine_rtc_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_init),        MP_ROM_PTR(&machine_rtc_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),      MP_ROM_PTR(&machine_rtc_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_datetime),    MP_ROM_PTR(&machine_rtc_datetime_obj) },
    { MP_ROM_QSTR(MP_QSTR_now),         MP_ROM_PTR(&machine_rtc_now_obj) },
    { MP_ROM_QSTR(MP_QSTR_alarm),       MP_ROM_PTR(&machine_rtc_alarm_obj) },
    { MP_ROM_QSTR(MP_QSTR_alarm_left),  MP_ROM_PTR(&machine_rtc_alarm_left_obj) },
    { MP_ROM_QSTR(MP_QSTR_cancel),      MP_ROM_PTR(&machine_rtc_alarm_cancel_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq),         MP_ROM_PTR(&machine_rtc_irq_obj) },
    { MP_ROM_QSTR(MP_QSTR_memory),      MP_ROM_PTR(&machine_rtc_memory_obj)},
    // class constants
    { MP_ROM_QSTR(MP_QSTR_ALARM0),      MP_ROM_INT(CYHAL_RTC_ALARM) },

};
static MP_DEFINE_CONST_DICT(machine_rtc_locals_dict, machine_rtc_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_rtc_type,
    MP_QSTR_RTC,
    MP_TYPE_FLAG_NONE,
    make_new, machine_rtc_make_new,
    locals_dict, &machine_rtc_locals_dict
    );
