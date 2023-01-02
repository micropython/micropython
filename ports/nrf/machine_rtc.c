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
// #include "hardware/rtc.h"
// #include "pico/util/datetime.h"
#include "modmachine.h"
#include "nrf_nvic.h"
#include "nrfx_rtc.h"

// Circuitpython

//common-hal/microcontroller/__init__.c

nrf_nvic_state_t nrf_nvic_state = {0};

static volatile uint32_t nesting_count = 0;
static uint8_t is_nested_critical_region;

void common_hal_mcu_disable_interrupts() {
    if (nesting_count == 0) {
        // Unlike __disable_irq(), this should only be called the first time
        // "is_nested_critical_region" is sd's equivalent of our nesting count
        // so a nested call would store 0 in the global and make the later
        // exit call not actually reenable interrupts
        //
        // This only disables interrupts of priority 2 through 7; levels 0, 1,
        // and 4, are exclusive to softdevice and should never be used, so
        // this limitation is not important.
        sd_nvic_critical_region_enter(&is_nested_critical_region);
    }
    __DMB();
    nesting_count++;
}

void common_hal_mcu_enable_interrupts() {
    if (nesting_count == 0) {
        // This is very very bad because it means there was mismatched disable/enables so we
        // crash.
        //reset_into_safe_mode(HARD_CRASH);
    }
    nesting_count--;
    if (nesting_count > 0) {
        return;
    }
    __DMB();
    sd_nvic_critical_region_exit(is_nested_critical_region);
}

//port.c
static volatile struct {
    uint32_t prefix;
    uint64_t overflowed_ticks;
    uint32_t suffix;
} overflow_tracker __attribute__((section(".uninitialized")));
const nrfx_rtc_t rtc_instance = NRFX_RTC_INSTANCE(2);

uint64_t port_get_raw_ticks(uint8_t *subticks) {
    common_hal_mcu_disable_interrupts();
    uint32_t rtc = nrfx_rtc_counter_get(&rtc_instance);
    uint64_t overflow_count = overflow_tracker.overflowed_ticks;
    common_hal_mcu_enable_interrupts();

    if (subticks != NULL) {
        *subticks = (rtc % 32);
    }
    return overflow_count + rtc / 32;
}

//RTC.C

#define RTC_OFFSET_CHECK_PREFIX 0x25ea7e2a
#define RTC_OFFSET_CHECK_SUFFIX 0x2b80b69e

// This is the time in seconds since 2000 that the RTC was started.
__attribute__((section(".uninitialized"))) static uint32_t rtc_offset[3];

void rtc_get_time(timeutils_struct_time_t *tm) {
    uint64_t ticks_s = port_get_raw_ticks(NULL) / 1024;
    timeutils_seconds_since_2000_to_struct_time(rtc_offset[1] + ticks_s, tm);
}

void rtc_set_time(timeutils_struct_time_t *tm) {
    uint64_t ticks_s = port_get_raw_ticks(NULL) / 1024;
    uint32_t epoch_s = timeutils_seconds_since_2000(
        tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec
        );
    rtc_offset[1] = epoch_s - ticks_s;

    // Set the prefix and suffix in order to indicate the time is valid.  This
    // must be done after the offset is updated, in case there is a crash or
    // power failure.
    rtc_offset[0] = RTC_OFFSET_CHECK_PREFIX;
    rtc_offset[2] = RTC_OFFSET_CHECK_SUFFIX;
}

// end of Circuitpython



typedef struct _machine_rtc_obj_t {
    mp_obj_base_t base;
} machine_rtc_obj_t;

// singleton RTC object
STATIC const machine_rtc_obj_t machine_rtc_obj = {{&machine_rtc_type}};

STATIC mp_obj_t machine_rtc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    // bool r = rtc_running();

    // if (!r) {
        // This shouldn't happen as rtc_init() is already called in main so
        // it's here just in case
    //    rtc_init();
    //    datetime_t t = { .month = 1, .day = 1 };
    //    rtc_set_datetime(&t);
    //}
    // return constant object
    return (mp_obj_t)&machine_rtc_obj;
}

STATIC mp_obj_t machine_rtc_datetime(mp_uint_t n_args, const mp_obj_t *args) {
    if (n_args == 1) {
        //bool ret;
        //datetime_t t;
        timeutils_struct_time_t t;

        //ret = rtc_get_datetime(&t);
        //if (!ret) {
        //    mp_raise_OSError(MP_EIO);
        //}
        rtc_get_time(&t);

        mp_obj_t tuple[8] = {
            mp_obj_new_int(t.tm_year),
            mp_obj_new_int(t.tm_mon),
            mp_obj_new_int(t.tm_mday),
            mp_obj_new_int(t.tm_wday),
            mp_obj_new_int(t.tm_hour),
            mp_obj_new_int(t.tm_min),
            mp_obj_new_int(t.tm_sec),
            mp_obj_new_int(0)
        };

        return mp_obj_new_tuple(8, tuple);
    } else {
        mp_obj_t *items;

        mp_obj_get_array_fixed_n(args[1], 8, &items);

        timeutils_struct_time_t t = {
            .tm_year = mp_obj_get_int(items[0]),
            .tm_mon = mp_obj_get_int(items[1]),
            .tm_mday = mp_obj_get_int(items[2]),
            .tm_hour = mp_obj_get_int(items[4]),
            .tm_min = mp_obj_get_int(items[5]),
            .tm_sec = mp_obj_get_int(items[6]),
        };
        // Deliberately ignore the weekday argument and compute the proper value
        t.tm_wday = timeutils_calc_weekday(t.tm_year, t.tm_mon, t.tm_mday);

        //if (!rtc_set_time(&t)) {
        //    mp_raise_OSError(MP_EINVAL);
        //}
        rtc_set_time(&t);

    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_rtc_datetime_obj, 1, 2, machine_rtc_datetime);

STATIC const mp_rom_map_elem_t machine_rtc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_datetime), MP_ROM_PTR(&machine_rtc_datetime_obj) },
};
STATIC MP_DEFINE_CONST_DICT(machine_rtc_locals_dict, machine_rtc_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_rtc_type,
    MP_QSTR_RTC,
    MP_TYPE_FLAG_NONE,
    make_new, machine_rtc_make_new,
    locals_dict, &machine_rtc_locals_dict
    );
