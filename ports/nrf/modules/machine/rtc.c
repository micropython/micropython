/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Nick Moore for Adafruit Industries
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

#include "py/runtime.h"
#include "modmachine.h"

#if MICROPY_PY_MACHINE_RTC

#include "py/mphal.h"
#include "shared/timeutils/timeutils.h"
#include "rtc.h"


// These values are placed before and after the current RTC count.  They are
// used to determine if the RTC count is valid.  These randomly-generated values
// will be set when the RTC value is set in order to mark the RTC as valid. If
// the system crashes or reboots, these values will remain undisturbed and the
// RTC offset will remain valid.
//
// If MicroPython is updated or these symbols shift around, the prefix and
// suffix will no longer match, and the time will no longer be valid.
#define RTC_OFFSET_CHECK_PREFIX 0x25ea7e2a
#define RTC_OFFSET_CHECK_SUFFIX 0x2b80b69e

void rtc_offset_check(void) {
    // If the prefix and suffix are not valid, zero-initialize the RTC offset.
    if ((rtc_offset[0] != RTC_OFFSET_CHECK_PREFIX) || (rtc_offset[2] != RTC_OFFSET_CHECK_SUFFIX)) {
        rtc_offset[1] = 0;
    }
}

void rtc_get_time(timeutils_struct_time_t *tm) {
    uint64_t ticks_s = ticks_ms_64() / 1000;
    timeutils_seconds_since_2000_to_struct_time(rtc_offset[1] + ticks_s, tm);
}

void rtc_set_time(timeutils_struct_time_t *tm) {
    uint64_t ticks_s = ticks_ms_64() / 1000;
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

typedef struct _machine_rtc_obj_t {
    mp_obj_base_t base;
} machine_rtc_obj_t;

// singleton RTC object
static const machine_rtc_obj_t machine_rtc_obj = {{&machine_rtc_type}};

static mp_obj_t machine_rtc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // return constant object
    return (mp_obj_t)&machine_rtc_obj;
}

static mp_obj_t machine_rtc_datetime(mp_uint_t n_args, const mp_obj_t *args) {
    if (n_args == 1) {
        timeutils_struct_time_t t;

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

        rtc_set_time(&t);

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

#endif // MICROPY_PY_MACHINE_RTC
