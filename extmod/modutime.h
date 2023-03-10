/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2016 Damien P. George
 * Copyright (c) 2016 Paul Sokolovsky
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
#ifndef MICROPY_INCLUDED_EXTMOD_MODUTIME_H
#define MICROPY_INCLUDED_EXTMOD_MODUTIME_H

#include "py/obj.h"

MP_DECLARE_CONST_FUN_OBJ_1(mp_utime_mktime_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_utime_sleep_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_utime_sleep_ms_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_utime_sleep_us_obj);
MP_DECLARE_CONST_FUN_OBJ_0(mp_utime_ticks_ms_obj);
MP_DECLARE_CONST_FUN_OBJ_0(mp_utime_ticks_us_obj);
MP_DECLARE_CONST_FUN_OBJ_0(mp_utime_ticks_cpu_obj);
MP_DECLARE_CONST_FUN_OBJ_2(mp_utime_ticks_diff_obj);
MP_DECLARE_CONST_FUN_OBJ_2(mp_utime_ticks_add_obj);
MP_DECLARE_CONST_FUN_OBJ_0(mp_utime_time_ns_obj);

#endif // MICROPY_INCLUDED_EXTMOD_MODUTIME_H
