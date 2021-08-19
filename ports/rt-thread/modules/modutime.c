/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Armink (armink.ztl@gmail.com)
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

#include <rtthread.h>
#include <sys/time.h>

#include "py/runtime.h"
#include "py/smallint.h"
#include "py/mphal.h"
#include "extmod/utime_mphal.h"
#include "shared/timeutils/timeutils.h"
#include <math.h>

STATIC mp_obj_t mod_time_time(void)
{
#if MICROPY_PY_BUILTINS_FLOAT
    struct timeval tv;
    gettimeofday(&tv, NULL);
    mp_float_t val = tv.tv_sec + (mp_float_t)tv.tv_usec / 1000000;
    return mp_obj_new_float(val);
#else
    return mp_obj_new_int((mp_int_t)time(NULL));
#endif
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_time_time_obj, mod_time_time);

STATIC mp_obj_t mod_time_localtime(size_t n_args, const mp_obj_t *args)
{
    time_t t;
    if (n_args == 0)
    {
        t = time(NULL);
    }
    else
    {
#if MICROPY_PY_BUILTINS_FLOAT
        mp_float_t val = mp_obj_get_float(args[0]);
        t = (time_t)MICROPY_FLOAT_C_FUN(trunc)(val);
#else
        t = mp_obj_get_int(args[0]);
#endif
    }
    struct tm *tm = localtime(&t);

    mp_obj_t ret = mp_obj_new_tuple(9, NULL);

    mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR(ret);
    tuple->items[0] = MP_OBJ_NEW_SMALL_INT(tm->tm_year + 1900);
    tuple->items[1] = MP_OBJ_NEW_SMALL_INT(tm->tm_mon + 1);
    tuple->items[2] = MP_OBJ_NEW_SMALL_INT(tm->tm_mday);
    tuple->items[3] = MP_OBJ_NEW_SMALL_INT(tm->tm_hour);
    tuple->items[4] = MP_OBJ_NEW_SMALL_INT(tm->tm_min);
    tuple->items[5] = MP_OBJ_NEW_SMALL_INT(tm->tm_sec);
    int wday = tm->tm_wday - 1;
    if (wday < 0)
    {
        wday = 6;
    }
    tuple->items[6] = MP_OBJ_NEW_SMALL_INT(wday);
    tuple->items[7] = MP_OBJ_NEW_SMALL_INT(tm->tm_yday + 1);
    tuple->items[8] = MP_OBJ_NEW_SMALL_INT(tm->tm_isdst);

    return ret;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_time_localtime_obj, 0, 1, mod_time_localtime);

/// \function mktime()
/// This is inverse function of localtime. It's argument is a full 8-tuple
/// which expresses a time as per localtime. It returns an integer which is
/// the number of seconds since Jan 1, 2000.
STATIC mp_obj_t time_mktime(mp_obj_t tuple)
{

    size_t len;
    mp_obj_t *elem;

    mp_obj_get_array(tuple, &len, &elem);

    // localtime generates a tuple of len 8. CPython uses 9, so we accept both.
    if (len < 8 || len > 9)
    {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "mktime needs a tuple of length 8 or 9 (%d given)", len));
    }

    return mp_obj_new_int_from_uint(timeutils_mktime(mp_obj_get_int(elem[0]),
                                    mp_obj_get_int(elem[1]), mp_obj_get_int(elem[2]), mp_obj_get_int(elem[3]),
                                    mp_obj_get_int(elem[4]), mp_obj_get_int(elem[5])));
}
MP_DEFINE_CONST_FUN_OBJ_1(time_mktime_obj, time_mktime);

STATIC const mp_rom_map_elem_t mp_module_time_globals_table[] =
{
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_utime) },
    { MP_ROM_QSTR(MP_QSTR_sleep), MP_ROM_PTR(&mp_utime_sleep_obj) },
    { MP_ROM_QSTR(MP_QSTR_sleep_ms), MP_ROM_PTR(&mp_utime_sleep_ms_obj) },
    { MP_ROM_QSTR(MP_QSTR_sleep_us), MP_ROM_PTR(&mp_utime_sleep_us_obj) },
    { MP_ROM_QSTR(MP_QSTR_time), MP_ROM_PTR(&mod_time_time_obj) },
    { MP_ROM_QSTR(MP_QSTR_ticks_ms), MP_ROM_PTR(&mp_utime_ticks_ms_obj) },
    { MP_ROM_QSTR(MP_QSTR_ticks_us), MP_ROM_PTR(&mp_utime_ticks_us_obj) },
    { MP_ROM_QSTR(MP_QSTR_ticks_cpu), MP_ROM_PTR(&mp_utime_ticks_cpu_obj) },
    { MP_ROM_QSTR(MP_QSTR_ticks_add), MP_ROM_PTR(&mp_utime_ticks_add_obj) },
    { MP_ROM_QSTR(MP_QSTR_ticks_diff), MP_ROM_PTR(&mp_utime_ticks_diff_obj) },
    { MP_ROM_QSTR(MP_QSTR_localtime), MP_ROM_PTR(&mod_time_localtime_obj) },
    { MP_ROM_QSTR(MP_QSTR_mktime), MP_ROM_PTR(&time_mktime_obj) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_time_globals, mp_module_time_globals_table);

const mp_obj_module_t mp_module_time =
{
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *) &mp_module_time_globals,
};

#endif // MICROPY_PY_UTIME
