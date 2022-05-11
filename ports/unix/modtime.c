/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-2017 Paul Sokolovsky
 * Copyright (c) 2014-2017 Damien P. George
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

#include "py/mpconfig.h"
#if MICROPY_PY_UTIME

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>

#include "py/runtime.h"
#include "py/smallint.h"
#include "py/mphal.h"
#include "extmod/utime_mphal.h"

#ifdef _WIN32
static inline int msec_sleep_tv(struct timeval *tv) {
    msec_sleep(tv->tv_sec * 1000.0 + tv->tv_usec / 1000.0);
    return 0;
}
#define sleep_select(a, b, c, d, e) msec_sleep_tv((e))
#else
#define sleep_select select
#endif

// mingw32 defines CLOCKS_PER_SEC as ((clock_t)<somevalue>) but preprocessor does not handle casts
#if defined(__MINGW32__) && !defined(__MINGW64_VERSION_MAJOR)
#define MP_REMOVE_BRACKETSA(x)
#define MP_REMOVE_BRACKETSB(x) MP_REMOVE_BRACKETSA x
#define MP_REMOVE_BRACKETSC(x) MP_REMOVE_BRACKETSB x
#define MP_CLOCKS_PER_SEC MP_REMOVE_BRACKETSC(CLOCKS_PER_SEC)
#else
#define MP_CLOCKS_PER_SEC CLOCKS_PER_SEC
#endif

#if defined(MP_CLOCKS_PER_SEC)
#define CLOCK_DIV (MP_CLOCKS_PER_SEC / MICROPY_FLOAT_CONST(1000.0))
#else
#error Unsupported clock() implementation
#endif

STATIC mp_obj_t mod_time_time(void) {
    #if MICROPY_PY_BUILTINS_FLOAT && MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE
    struct timeval tv;
    gettimeofday(&tv, NULL);
    mp_float_t val = tv.tv_sec + (mp_float_t)tv.tv_usec / 1000000;
    return mp_obj_new_float(val);
    #else
    return mp_obj_new_int((mp_int_t)time(NULL));
    #endif
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_time_time_obj, mod_time_time);

// Note: this is deprecated since CPy3.3, but pystone still uses it.
STATIC mp_obj_t mod_time_clock(void) {
    #if MICROPY_PY_BUILTINS_FLOAT
    // float cannot represent full range of int32 precisely, so we pre-divide
    // int to reduce resolution, and then actually do float division hoping
    // to preserve integer part resolution.
    return mp_obj_new_float((clock() / 1000) / CLOCK_DIV);
    #else
    return mp_obj_new_int((mp_int_t)clock());
    #endif
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_time_clock_obj, mod_time_clock);

STATIC mp_obj_t mod_time_sleep(mp_obj_t arg) {
    #if MICROPY_PY_BUILTINS_FLOAT
    struct timeval tv;
    mp_float_t val = mp_obj_get_float(arg);
    mp_float_t ipart;
    tv.tv_usec = (time_t)MICROPY_FLOAT_C_FUN(round)(MICROPY_FLOAT_C_FUN(modf)(val, &ipart) * MICROPY_FLOAT_CONST(1000000.));
    tv.tv_sec = (suseconds_t)ipart;
    int res;
    while (1) {
        MP_THREAD_GIL_EXIT();
        res = sleep_select(0, NULL, NULL, NULL, &tv);
        MP_THREAD_GIL_ENTER();
        #if MICROPY_SELECT_REMAINING_TIME
        // TODO: This assumes Linux behavior of modifying tv to the remaining
        // time.
        if (res != -1 || errno != EINTR) {
            break;
        }
        mp_handle_pending(true);
        // printf("select: EINTR: %ld:%ld\n", tv.tv_sec, tv.tv_usec);
        #else
        break;
        #endif
    }
    RAISE_ERRNO(res, errno);
    #else
    int seconds = mp_obj_get_int(arg);
    for (;;) {
        MP_THREAD_GIL_EXIT();
        seconds = sleep(seconds);
        MP_THREAD_GIL_ENTER();
        if (seconds == 0) {
            break;
        }
        mp_handle_pending(true);
    }
    #endif
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_time_sleep_obj, mod_time_sleep);

STATIC mp_obj_t mod_time_gm_local_time(size_t n_args, const mp_obj_t *args, struct tm *(*time_func)(const time_t *timep)) {
    time_t t;
    if (n_args == 0) {
        t = time(NULL);
    } else {
        #if MICROPY_PY_BUILTINS_FLOAT && MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE
        mp_float_t val = mp_obj_get_float(args[0]);
        t = (time_t)MICROPY_FLOAT_C_FUN(trunc)(val);
        #else
        t = mp_obj_get_int(args[0]);
        #endif
    }
    struct tm *tm = time_func(&t);

    mp_obj_t ret = mp_obj_new_tuple(9, NULL);

    mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR(ret);
    tuple->items[0] = MP_OBJ_NEW_SMALL_INT(tm->tm_year + 1900);
    tuple->items[1] = MP_OBJ_NEW_SMALL_INT(tm->tm_mon + 1);
    tuple->items[2] = MP_OBJ_NEW_SMALL_INT(tm->tm_mday);
    tuple->items[3] = MP_OBJ_NEW_SMALL_INT(tm->tm_hour);
    tuple->items[4] = MP_OBJ_NEW_SMALL_INT(tm->tm_min);
    tuple->items[5] = MP_OBJ_NEW_SMALL_INT(tm->tm_sec);
    int wday = tm->tm_wday - 1;
    if (wday < 0) {
        wday = 6;
    }
    tuple->items[6] = MP_OBJ_NEW_SMALL_INT(wday);
    tuple->items[7] = MP_OBJ_NEW_SMALL_INT(tm->tm_yday + 1);
    tuple->items[8] = MP_OBJ_NEW_SMALL_INT(tm->tm_isdst);

    return ret;
}

STATIC mp_obj_t mod_time_gmtime(size_t n_args, const mp_obj_t *args) {
    return mod_time_gm_local_time(n_args, args, gmtime);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_time_gmtime_obj, 0, 1, mod_time_gmtime);

STATIC mp_obj_t mod_time_localtime(size_t n_args, const mp_obj_t *args) {
    return mod_time_gm_local_time(n_args, args, localtime);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_time_localtime_obj, 0, 1, mod_time_localtime);

STATIC mp_obj_t mod_time_mktime(mp_obj_t tuple) {
    size_t len;
    mp_obj_t *elem;
    mp_obj_get_array(tuple, &len, &elem);

    // localtime generates a tuple of len 8. CPython uses 9, so we accept both.
    if (len < 8 || len > 9) {
        mp_raise_TypeError(MP_ERROR_TEXT("mktime needs a tuple of length 8 or 9"));
    }

    struct tm time = {
        .tm_year = mp_obj_get_int(elem[0]) - 1900,
        .tm_mon = mp_obj_get_int(elem[1]) - 1,
        .tm_mday = mp_obj_get_int(elem[2]),
        .tm_hour = mp_obj_get_int(elem[3]),
        .tm_min = mp_obj_get_int(elem[4]),
        .tm_sec = mp_obj_get_int(elem[5]),
    };
    if (len == 9) {
        time.tm_isdst = mp_obj_get_int(elem[8]);
    } else {
        time.tm_isdst = -1; // auto-detect
    }
    time_t ret = mktime(&time);
    if (ret == -1) {
        mp_raise_msg(&mp_type_OverflowError, MP_ERROR_TEXT("invalid mktime usage"));
    }
    return mp_obj_new_int(ret);
}
MP_DEFINE_CONST_FUN_OBJ_1(mod_time_mktime_obj, mod_time_mktime);

STATIC const mp_rom_map_elem_t mp_module_time_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_utime) },
    { MP_ROM_QSTR(MP_QSTR_clock), MP_ROM_PTR(&mod_time_clock_obj) },
    { MP_ROM_QSTR(MP_QSTR_sleep), MP_ROM_PTR(&mod_time_sleep_obj) },
    { MP_ROM_QSTR(MP_QSTR_sleep_ms), MP_ROM_PTR(&mp_utime_sleep_ms_obj) },
    { MP_ROM_QSTR(MP_QSTR_sleep_us), MP_ROM_PTR(&mp_utime_sleep_us_obj) },
    { MP_ROM_QSTR(MP_QSTR_time), MP_ROM_PTR(&mod_time_time_obj) },
    { MP_ROM_QSTR(MP_QSTR_ticks_ms), MP_ROM_PTR(&mp_utime_ticks_ms_obj) },
    { MP_ROM_QSTR(MP_QSTR_ticks_us), MP_ROM_PTR(&mp_utime_ticks_us_obj) },
    { MP_ROM_QSTR(MP_QSTR_ticks_cpu), MP_ROM_PTR(&mp_utime_ticks_cpu_obj) },
    { MP_ROM_QSTR(MP_QSTR_ticks_add), MP_ROM_PTR(&mp_utime_ticks_add_obj) },
    { MP_ROM_QSTR(MP_QSTR_ticks_diff), MP_ROM_PTR(&mp_utime_ticks_diff_obj) },
    { MP_ROM_QSTR(MP_QSTR_time_ns), MP_ROM_PTR(&mp_utime_time_ns_obj) },
    { MP_ROM_QSTR(MP_QSTR_gmtime), MP_ROM_PTR(&mod_time_gmtime_obj) },
    { MP_ROM_QSTR(MP_QSTR_localtime), MP_ROM_PTR(&mod_time_localtime_obj) },
    { MP_ROM_QSTR(MP_QSTR_mktime), MP_ROM_PTR(&mod_time_mktime_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_time_globals, mp_module_time_globals_table);

const mp_obj_module_t mp_module_time = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_time_globals,
};

#endif // MICROPY_PY_UTIME
