/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

static inline int msec_sleep_tv(struct timeval *tv) {
    usleep(tv->tv_sec * 1000000 + tv->tv_usec);
    return 0;
}

STATIC mp_obj_t mod_time_time(void) {
    return mp_obj_new_int((mp_int_t)time(NULL));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_time_time_obj, mod_time_time);

STATIC mp_obj_t mod_time_sleep(mp_obj_t arg) {
    MP_THREAD_GIL_EXIT();
    sleep(mp_obj_get_int(arg));
    MP_THREAD_GIL_ENTER();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_time_sleep_obj, mod_time_sleep);

STATIC mp_obj_t mod_time_strftime(size_t n_args, const mp_obj_t *args) {
    time_t t;
    if (n_args == 1) {
        t = time(NULL);
    } else {
        // CPython requires passing struct tm, but we allow to pass time_t
        // (and don't support struct tm so far).
        t = mp_obj_get_int(args[1]);
    }
    struct tm *tm = localtime(&t);
    char buf[32];
    size_t sz = strftime(buf, sizeof(buf), mp_obj_str_get_str(args[0]), tm);
    return mp_obj_new_str(buf, sz, false);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_time_strftime_obj, 1, 2, mod_time_strftime);

STATIC const mp_rom_map_elem_t mp_module_time_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_utime) },
    { MP_ROM_QSTR(MP_QSTR_sleep), MP_ROM_PTR(&mod_time_sleep_obj) },
    { MP_ROM_QSTR(MP_QSTR_time), MP_ROM_PTR(&mod_time_time_obj) },
    { MP_ROM_QSTR(MP_QSTR_strftime), MP_ROM_PTR(&mod_time_strftime_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_time_globals, mp_module_time_globals_table);

const mp_obj_module_t mp_module_time = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_time_globals,
};

