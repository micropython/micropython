/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Noralf Trønnes
 * SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
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

#include "py/obj.h"
#include "py/runtime.h"
#include "shared-bindings/rtc/__init__.h"
#include "shared-bindings/rtc/RTC.h"
#include "shared-bindings/time/__init__.h"

//| """Real Time Clock
//|
//| The `rtc` module provides support for a Real Time Clock. You can access and manage the
//| RTC using :class:`rtc.RTC`. It also backs the :func:`time.time` and :func:`time.localtime`
//| functions using the onboard RTC if present."""
//|

void rtc_reset(void) {
    MP_STATE_VM(rtc_time_source) = (mp_obj_t)&rtc_rtc_obj;
}

mp_obj_t rtc_get_time_source_time(void) {
    mp_obj_t datetime = mp_load_attr(MP_STATE_VM(rtc_time_source), MP_QSTR_datetime);
    timeutils_struct_time_t tm;
    struct_time_to_tm(datetime, &tm);
    // This sets tm_wday and tm_yday
    return struct_time_from_tm(&tm);
}

//| def set_time_source(rtc: RTC) -> None:
//|     """Sets the RTC time source used by :func:`time.localtime`.
//|     The default is :class:`rtc.RTC`, but it's useful to use this to override the
//|     time source for testing purposes. For example::
//|
//|       import rtc
//|       import time
//|
//|       class RTC(object):
//|           @property
//|           def datetime(self):
//|               return time.struct_time((2018, 3, 17, 21, 1, 47, 0, 0, 0))
//|
//|       r = RTC()
//|       rtc.set_time_source(r)"""
//|     ...
//|
STATIC mp_obj_t rtc_set_time_source(mp_obj_t time_source) {
    MP_STATE_VM(rtc_time_source) = time_source;

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(rtc_set_time_source_obj, rtc_set_time_source);

STATIC const mp_rom_map_elem_t rtc_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_rtc) },
    { MP_ROM_QSTR(MP_QSTR_set_time_source), MP_ROM_PTR(&rtc_set_time_source_obj) },
    { MP_ROM_QSTR(MP_QSTR_RTC), MP_ROM_PTR(&rtc_rtc_type) },
};

STATIC MP_DEFINE_CONST_DICT(rtc_module_globals, rtc_module_globals_table);

const mp_obj_module_t rtc_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&rtc_module_globals,
};
