/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2021 Damien P. George
 * Copyright (c) 2023 Infineon Technologies
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
#include "extmod/modutime.h"
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

// Convert a time expressed in seconds since the Epoch into an 8-tuple which
// contains: (year, month, mday, hour, minute, second, weekday, yearday)
STATIC mp_obj_t mp_utime_localtime_get(void) {
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
}

// time()
// Return the number of seconds since the Epoch.
STATIC mp_obj_t mp_utime_time_get(void) {
    struct tm current_date_time = {0};
    cy_rslt_t result = cyhal_rtc_read(&psoc6_rtc, &current_date_time);

    if (CY_RSLT_SUCCESS != result) {
        mp_raise_ValueError(MP_ERROR_TEXT("cyhal_rtc_read failed !"));
    }

    return mp_obj_new_int_from_ull(timeutils_seconds_since_epoch(current_date_time.tm_year, current_date_time.tm_mon, current_date_time.tm_mday,
        current_date_time.tm_hour, current_date_time.tm_min, current_date_time.tm_sec));
}
