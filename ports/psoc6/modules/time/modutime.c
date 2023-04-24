/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2021 Damien P. George
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
#include "stdio.h"

// micropython includes
#include "extmod/utime_mphal.h"
#include "py/runtime.h"
#include "shared/timeutils/timeutils.h"

// MTB includes
#include "cyhal.h"

// object defined in rtc.c
extern cyhal_rtc_t psoc6_rtc;

cyhal_timer_t psoc6_timer;

void time_init(void) {
    const cyhal_timer_cfg_t timer_cfg =
    {
        .compare_value = 0,                 /* Timer compare value, not used */
        .period = 15000000,                 /* Timer period set to a large enough value */
                                            /* compared to event being measured */
        .direction = CYHAL_TIMER_DIR_UP,    /* Timer counts up */
        .is_compare = false,                /* Don't use compare mode */
        .is_continuous = true,              /* Run timer indefinitely */
        .value = 0                          /* Initial value of counter */
    };
    /* Initialize the timer object. Does not use pin output ('pin' is NC) and
     * does not use a pre-configured clock source ('clk' is NULL). */
    cyhal_timer_init(&psoc6_timer, NC, NULL);
    /* Apply timer configuration such as period, count direction, run mode, etc. */
    cyhal_timer_configure(&psoc6_timer, &timer_cfg);
    /* Set the frequency of timer to 1 MHz */
    cyhal_timer_set_frequency(&psoc6_timer, 1000000);
    /* Start the timer with the configured settings */
    cyhal_timer_start(&psoc6_timer);
}

void time_deinit(void) {
    cyhal_timer_stop(&psoc6_timer);
}

// localtime([secs])
// Convert a time expressed in seconds since the Epoch into an 8-tuple which
// contains: (year, month, mday, hour, minute, second, weekday, yearday)
// If secs is not provided or None, then the current time from is used.
STATIC mp_obj_t time_localtime(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0 || args[0] == mp_const_none) {
        struct tm current_date_time = {0};
        cy_rslt_t result = cyhal_rtc_read(&psoc6_rtc, &current_date_time);

        if (CY_RSLT_SUCCESS != result) {
            mp_raise_ValueError(MP_ERROR_TEXT("cyhal_rtc_read failed !"));
        }

        mp_obj_t tuple[8] = {
            mp_obj_new_int(current_date_time.tm_year),
            mp_obj_new_int(current_date_time.tm_mon),
            mp_obj_new_int(current_date_time.tm_mday),
            mp_obj_new_int(current_date_time.tm_hour),
            mp_obj_new_int(current_date_time.tm_min),
            mp_obj_new_int(current_date_time.tm_sec),
            mp_obj_new_int(current_date_time.tm_wday),
            mp_obj_new_int(timeutils_year_day(current_date_time.tm_year, current_date_time.tm_mon, current_date_time.tm_mday)),
        };

        return mp_obj_new_tuple(8, tuple);
    } else {
        // Convert given seconds to tuple.
        mp_int_t seconds = mp_obj_get_int(args[0]);
        timeutils_struct_time_t tm;
        timeutils_seconds_since_epoch_to_struct_time(seconds, &tm);

        mp_obj_t tuple[8] = {
            tuple[0] = mp_obj_new_int(tm.tm_year),
            tuple[1] = mp_obj_new_int(tm.tm_mon),
            tuple[2] = mp_obj_new_int(tm.tm_mday),
            tuple[3] = mp_obj_new_int(tm.tm_hour),
            tuple[4] = mp_obj_new_int(tm.tm_min),
            tuple[5] = mp_obj_new_int(tm.tm_sec),
            tuple[6] = mp_obj_new_int(tm.tm_wday),
            tuple[7] = mp_obj_new_int(tm.tm_yday),
        };

        return mp_obj_new_tuple(8, tuple);
    }
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(time_localtime_obj, 0, 1, time_localtime);


// mktime()
// This is inverse function of localtime. It's argument is a full 8-tuple
// which expresses a time as per localtime. It returns an integer which is
// the number of seconds since the Epoch.
STATIC mp_obj_t time_mktime(mp_obj_t tuple) {
    size_t len;
    mp_obj_t *elem;
    mp_obj_get_array(tuple, &len, &elem);

    // localtime generates a tuple of len 8. CPython uses 9, so we accept both.
    if (len < 8 || len > 9) {
        mp_raise_TypeError(MP_ERROR_TEXT("mktime needs a tuple of length 8 or 9"));
    }

    return mp_obj_new_int_from_uint(timeutils_mktime(mp_obj_get_int(elem[0]),
        mp_obj_get_int(elem[1]), mp_obj_get_int(elem[2]), mp_obj_get_int(elem[3]),
        mp_obj_get_int(elem[4]), mp_obj_get_int(elem[5])));
}
MP_DEFINE_CONST_FUN_OBJ_1(time_mktime_obj, time_mktime);


// time()
// Return the number of seconds since the Epoch.
STATIC mp_obj_t time_time(void) {
    // datetime_t t;
    // rtc_get_datetime(&t);
    struct tm current_date_time = {0};
    cy_rslt_t result = cyhal_rtc_read(&psoc6_rtc, &current_date_time);

    if (CY_RSLT_SUCCESS != result) {
        mp_raise_ValueError(MP_ERROR_TEXT("cyhal_rtc_read failed !"));
    }

    return mp_obj_new_int_from_ull(timeutils_seconds_since_epoch(current_date_time.tm_year, current_date_time.tm_mon, current_date_time.tm_mday,
        current_date_time.tm_hour, current_date_time.tm_min, current_date_time.tm_sec));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(time_time_obj, time_time);


STATIC const mp_rom_map_elem_t mp_module_time_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_utime) },

    { MP_ROM_QSTR(MP_QSTR_gmtime), MP_ROM_PTR(&time_localtime_obj) },
    { MP_ROM_QSTR(MP_QSTR_localtime), MP_ROM_PTR(&time_localtime_obj) },
    { MP_ROM_QSTR(MP_QSTR_mktime), MP_ROM_PTR(&time_mktime_obj) },

    { MP_ROM_QSTR(MP_QSTR_time), MP_ROM_PTR(&time_time_obj) },
    { MP_ROM_QSTR(MP_QSTR_time_ns), MP_ROM_PTR(&mp_utime_time_ns_obj) },

    { MP_ROM_QSTR(MP_QSTR_sleep), MP_ROM_PTR(&mp_utime_sleep_obj) },
    { MP_ROM_QSTR(MP_QSTR_sleep_ms), MP_ROM_PTR(&mp_utime_sleep_ms_obj) },
    { MP_ROM_QSTR(MP_QSTR_sleep_us), MP_ROM_PTR(&mp_utime_sleep_us_obj) },

    { MP_ROM_QSTR(MP_QSTR_ticks_ms), MP_ROM_PTR(&mp_utime_ticks_ms_obj) },
    { MP_ROM_QSTR(MP_QSTR_ticks_us), MP_ROM_PTR(&mp_utime_ticks_us_obj) },
    { MP_ROM_QSTR(MP_QSTR_ticks_cpu), MP_ROM_PTR(&mp_utime_ticks_cpu_obj) },
    { MP_ROM_QSTR(MP_QSTR_ticks_add), MP_ROM_PTR(&mp_utime_ticks_add_obj) },
    { MP_ROM_QSTR(MP_QSTR_ticks_diff), MP_ROM_PTR(&mp_utime_ticks_diff_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_time_globals, mp_module_time_globals_table);


const mp_obj_module_t mp_module_utime = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_time_globals,
};

MP_REGISTER_MODULE(MP_QSTR_utime, mp_module_utime);
