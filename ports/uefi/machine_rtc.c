/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Nicko van Someren
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

// machine.RTC for the UEFI port: the idiomatic MicroPython face over the firmware
// real-time clock (RuntimeServices GetTime/SetTime). Compiled directly; the machine
// module glue in modmachine.c references machine_rtc_type.
//
// datetime() reads the port's single anchored wall clock (uefi_time.c) rather than
// RuntimeServices->GetTime directly, so its seconds and sub-seconds come from one
// monotone source (no ~1 s skew if a separate sub-second clock has drifted near a tick
// roll-over). datetime(tuple) writes the firmware RTC and re-seeds that anchor, so
// time()/time_ns()/localtime() immediately track the new setting. Sub-seconds are
// microseconds.

#include "py/runtime.h"
#include "py/mperrno.h"
#include "shared/timeutils/timeutils.h"
#include "uefi_port.h"
#include "efi.h"

typedef struct _machine_rtc_obj_t {
    mp_obj_base_t base;
} machine_rtc_obj_t;

const mp_obj_type_t machine_rtc_type;

// The firmware clock is a singleton; a single const instance backs every RTC().
static const machine_rtc_obj_t machine_rtc_obj = { { &machine_rtc_type } };

static mp_obj_t machine_rtc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)type;
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    // Optional leading id; only the single system RTC (0) is supported.
    if (n_args > 0 && mp_obj_get_int(args[0]) != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("RTC doesn't exist"));
    }
    return MP_OBJ_FROM_PTR(&machine_rtc_obj);
}

// Write the firmware RTC from an 8-tuple, then re-anchor the wall clock. Raises OSError
// if the platform refuses SetTime (e.g. a read-only RTC).
static void machine_rtc_set_from_tuple(mp_obj_t datetime) {
    size_t len;
    mp_obj_t *items;
    mp_obj_get_array(datetime, &len, &items);
    if (len != 8) {
        mp_raise_ValueError(MP_ERROR_TEXT("RTC datetime needs an 8-tuple"));
    }
    EFI_TIME t;
    memset(&t, 0, sizeof(t));
    t.Year = (UINT16)mp_obj_get_int(items[0]);
    t.Month = (UINT8)mp_obj_get_int(items[1]);
    t.Day = (UINT8)mp_obj_get_int(items[2]);
    // items[3] is weekday: the firmware derives it, so it is ignored on set.
    t.Hour = (UINT8)mp_obj_get_int(items[4]);
    t.Minute = (UINT8)mp_obj_get_int(items[5]);
    t.Second = (UINT8)mp_obj_get_int(items[6]);
    t.Nanosecond = (UINT32)(mp_obj_get_int(items[7]) * 1000); // subseconds are microseconds
    t.TimeZone = 0x07FF;  // EFI_UNSPECIFIED_TIMEZONE: store as local wall time
    t.Daylight = 0;
    if (EFI_ERROR(mp_uefi_st->RuntimeServices->SetTime(&t))) {
        mp_raise_OSError(MP_EIO);
    }
    mp_uefi_time_anchor();
}

// datetime([tuple]) -> get returns the 8-tuple; set (one arg) writes the RTC.
static mp_obj_t machine_rtc_datetime(size_t n_args, const mp_obj_t *args) {
    if (n_args > 1) {
        machine_rtc_set_from_tuple(args[1]);
        return mp_const_none;
    }
    uint64_t wall_ns = mp_uefi_wall_ns();
    timeutils_struct_time_t tm;
    timeutils_seconds_since_epoch_to_struct_time((mp_timestamp_t)(wall_ns / 1000000000ULL), &tm);
    mp_obj_t tuple[8] = {
        mp_obj_new_int(tm.tm_year),
        mp_obj_new_int(tm.tm_mon),
        mp_obj_new_int(tm.tm_mday),
        mp_obj_new_int(tm.tm_wday),
        mp_obj_new_int(tm.tm_hour),
        mp_obj_new_int(tm.tm_min),
        mp_obj_new_int(tm.tm_sec),
        mp_obj_new_int((mp_int_t)((wall_ns % 1000000000ULL) / 1000ULL)), // subseconds (us)
    };
    return mp_obj_new_tuple(8, tuple);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_rtc_datetime_obj, 1, 2, machine_rtc_datetime);

// init(tuple): set the clock (convenience alias used by some code that does RTC().init()).
static mp_obj_t machine_rtc_init(mp_obj_t self_in, mp_obj_t datetime) {
    (void)self_in;
    machine_rtc_set_from_tuple(datetime);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_rtc_init_obj, machine_rtc_init);

static const mp_rom_map_elem_t machine_rtc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_datetime), MP_ROM_PTR(&machine_rtc_datetime_obj) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_rtc_init_obj) },
};
static MP_DEFINE_CONST_DICT(machine_rtc_locals_dict, machine_rtc_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_rtc_type,
    MP_QSTR_RTC,
    MP_TYPE_FLAG_NONE,
    make_new, machine_rtc_make_new,
    locals_dict, &machine_rtc_locals_dict
    );
