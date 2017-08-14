/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Josef Gajdusek
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

#include "py/nlr.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "timeutils.h"
#include "user_interface.h"
#include "modpyb.h"

typedef struct _pyb_rtc_obj_t {
    mp_obj_base_t base;
} pyb_rtc_obj_t;

#define MEM_MAGIC           0x75507921
#define MEM_DELTA_ADDR      64
#define MEM_CAL_ADDR        (MEM_DELTA_ADDR + 2)
#define MEM_USER_MAGIC_ADDR (MEM_CAL_ADDR + 1)
#define MEM_USER_LEN_ADDR   (MEM_USER_MAGIC_ADDR + 1)
#define MEM_USER_DATA_ADDR  (MEM_USER_LEN_ADDR + 1)
#define MEM_USER_MAXLEN     (512 - (MEM_USER_DATA_ADDR - MEM_DELTA_ADDR) * 4)

// singleton RTC object
STATIC const pyb_rtc_obj_t pyb_rtc_obj = {{&pyb_rtc_type}};

// ALARM0 state
uint32_t pyb_rtc_alarm0_wake; // see MACHINE_WAKE_xxx constants
uint64_t pyb_rtc_alarm0_expiry; // in microseconds

// RTC overflow checking
STATIC uint32_t rtc_last_ticks;

void mp_hal_rtc_init(void) {
    uint32_t magic;

    system_rtc_mem_read(MEM_USER_MAGIC_ADDR, &magic, sizeof(magic));
    if (magic != MEM_MAGIC) {
        magic = MEM_MAGIC;
        system_rtc_mem_write(MEM_USER_MAGIC_ADDR, &magic, sizeof(magic));
        uint32_t cal = system_rtc_clock_cali_proc();
        int64_t delta = 0;
        system_rtc_mem_write(MEM_CAL_ADDR, &cal, sizeof(cal));
        system_rtc_mem_write(MEM_DELTA_ADDR, &delta, sizeof(delta));
        uint32_t len = 0;
        system_rtc_mem_write(MEM_USER_LEN_ADDR, &len, sizeof(len));
    }
    // system_get_rtc_time() is always 0 after reset/deepsleep
    rtc_last_ticks = system_get_rtc_time();

    // reset ALARM0 state
    pyb_rtc_alarm0_wake = 0;
    pyb_rtc_alarm0_expiry = 0;
}

STATIC mp_obj_t pyb_rtc_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // return constant object
    return (mp_obj_t)&pyb_rtc_obj;
}

void pyb_rtc_set_us_since_2000(uint64_t nowus) {
    uint32_t cal = system_rtc_clock_cali_proc();
    // Save RTC ticks for overflow detection.
    rtc_last_ticks = system_get_rtc_time();
    int64_t delta = nowus - (((uint64_t)rtc_last_ticks * cal) >> 12);

    // As the calibration value jitters quite a bit, to make the
    // clock at least somewhat practially usable, we need to store it
    system_rtc_mem_write(MEM_CAL_ADDR, &cal, sizeof(cal));
    system_rtc_mem_write(MEM_DELTA_ADDR, &delta, sizeof(delta));
};

uint64_t pyb_rtc_get_us_since_2000() {
    uint32_t cal;
    int64_t delta;
    uint32_t rtc_ticks;

    system_rtc_mem_read(MEM_CAL_ADDR, &cal, sizeof(cal));
    system_rtc_mem_read(MEM_DELTA_ADDR, &delta, sizeof(delta));

    // ESP-SDK system_get_rtc_time() only returns uint32 and therefore
    // overflow about every 7:45h.  Thus, we have to check for
    // overflow and handle it.
    rtc_ticks = system_get_rtc_time();
    if (rtc_ticks < rtc_last_ticks) {
        // Adjust delta because of RTC overflow.
        delta += (uint64_t)cal << 20;
        system_rtc_mem_write(MEM_DELTA_ADDR, &delta, sizeof(delta));
    }
    rtc_last_ticks = rtc_ticks;

    return (((uint64_t)rtc_ticks * cal) >> 12) + delta;
};

void rtc_prepare_deepsleep(uint64_t sleep_us) {
    // RTC time will reset at wake up. Let's be preared for this.
    int64_t delta = pyb_rtc_get_us_since_2000() + sleep_us;
    system_rtc_mem_write(MEM_DELTA_ADDR, &delta, sizeof(delta));
}

STATIC mp_obj_t pyb_rtc_datetime(mp_uint_t n_args, const mp_obj_t *args) {
    if (n_args == 1) {
        // Get time
        uint64_t msecs = pyb_rtc_get_us_since_2000() / 1000;

        timeutils_struct_time_t tm;
        timeutils_seconds_since_2000_to_struct_time(msecs / 1000, &tm);

        mp_obj_t tuple[8] = {
            mp_obj_new_int(tm.tm_year),
            mp_obj_new_int(tm.tm_mon),
            mp_obj_new_int(tm.tm_mday),
            mp_obj_new_int(tm.tm_wday),
            mp_obj_new_int(tm.tm_hour),
            mp_obj_new_int(tm.tm_min),
            mp_obj_new_int(tm.tm_sec),
            mp_obj_new_int(msecs % 1000)
        };

        return mp_obj_new_tuple(8, tuple);
    } else {
        // Set time
        mp_obj_t *items;
        mp_obj_get_array_fixed_n(args[1], 8, &items);

        pyb_rtc_set_us_since_2000(
            ((uint64_t)timeutils_seconds_since_2000(
                mp_obj_get_int(items[0]),
                mp_obj_get_int(items[1]),
                mp_obj_get_int(items[2]),
                mp_obj_get_int(items[4]),
                mp_obj_get_int(items[5]),
                mp_obj_get_int(items[6])) * 1000 + mp_obj_get_int(items[7])) * 1000);

        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_rtc_datetime_obj, 1, 2, pyb_rtc_datetime);

STATIC mp_obj_t pyb_rtc_memory(mp_uint_t n_args, const mp_obj_t *args) {
    uint8_t rtcram[MEM_USER_MAXLEN];
    uint32_t len;

    if (n_args == 1) {
        // read RTC memory

        system_rtc_mem_read(MEM_USER_LEN_ADDR, &len, sizeof(len));
        system_rtc_mem_read(MEM_USER_DATA_ADDR, rtcram, len + (4 - len % 4));

        return mp_obj_new_bytes(rtcram, len);
    } else {
        // write RTC memory

        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_READ);

        if (bufinfo.len > MEM_USER_MAXLEN) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError,
                "buffer too long"));
        }

        len = bufinfo.len;
        system_rtc_mem_write(MEM_USER_LEN_ADDR, &len, sizeof(len));

        int i = 0;
        for (; i < bufinfo.len; i++) {
            rtcram[i] = ((uint8_t *)bufinfo.buf)[i];
        }

        system_rtc_mem_write(MEM_USER_DATA_ADDR, rtcram, len + (4 - len % 4));

        return mp_const_none;
    }

}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_rtc_memory_obj, 1, 2, pyb_rtc_memory);

STATIC mp_obj_t pyb_rtc_alarm(mp_obj_t self_in, mp_obj_t alarm_id, mp_obj_t time_in) {
    (void)self_in; // unused

    // check we want alarm0
    if (mp_obj_get_int(alarm_id) != 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "invalid alarm"));
    }

    // set expiry time (in microseconds)
    pyb_rtc_alarm0_expiry = pyb_rtc_get_us_since_2000() + (uint64_t)mp_obj_get_int(time_in) * 1000;

    return mp_const_none;

}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_rtc_alarm_obj, pyb_rtc_alarm);

STATIC mp_obj_t pyb_rtc_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_trigger, ARG_wake };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_trigger, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_wake, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // check we want alarm0
    if (args[ARG_trigger].u_int != 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "invalid alarm"));
    }

    // set the wake value
    pyb_rtc_alarm0_wake = args[ARG_wake].u_int;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_rtc_irq_obj, 1, pyb_rtc_irq);

STATIC const mp_map_elem_t pyb_rtc_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_datetime), (mp_obj_t)&pyb_rtc_datetime_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_memory), (mp_obj_t)&pyb_rtc_memory_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_alarm), (mp_obj_t)&pyb_rtc_alarm_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_irq), (mp_obj_t)&pyb_rtc_irq_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ALARM0), MP_OBJ_NEW_SMALL_INT(0) },
};
STATIC MP_DEFINE_CONST_DICT(pyb_rtc_locals_dict, pyb_rtc_locals_dict_table);

const mp_obj_type_t pyb_rtc_type = {
    { &mp_type_type },
    .name = MP_QSTR_RTC,
    .make_new = pyb_rtc_make_new,
    .locals_dict = (mp_obj_t)&pyb_rtc_locals_dict,
};
