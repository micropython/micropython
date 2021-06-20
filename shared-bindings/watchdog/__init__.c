/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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

#include <string.h>

#include "py/runtime.h"
#include "shared-bindings/watchdog/__init__.h"
#include "shared-bindings/watchdog/WatchDogMode.h"

//| """Watchdog Timer
//|
//| The `watchdog` module provides support for a Watchdog Timer. This timer will reset the device
//| if it hasn't been fed after a specified amount of time. This is useful to ensure the board
//| has not crashed or locked up. Note that on some platforms the watchdog timer cannot be disabled
//| once it has been enabled.
//|
//| The `WatchDogTimer` is used to restart the system when the application crashes and ends
//| up into a non recoverable state. Once started it cannot be stopped or
//| reconfigured in any way. After enabling, the application must "feed" the
//| watchdog periodically to prevent it from expiring and resetting the system.
//|
//| Example usage::
//|
//|     from microcontroller import watchdog as w
//|     from watchdog import WatchDogMode
//|     w.timeout=2.5 # Set a timeout of 2.5 seconds
//|     w.mode = WatchDogMode.RAISE
//|     w.feed()"""
//|

const mp_obj_type_t mp_type_WatchDogTimeout = {
    { &mp_type_type },
    .name = MP_QSTR_WatchDogTimeout,
    .make_new = mp_obj_exception_make_new,
    .attr = mp_obj_exception_attr,
    .parent = &mp_type_Exception,
};

mp_obj_exception_t mp_watchdog_timeout_exception = {
    .base.type = &mp_type_WatchDogTimeout,
    .traceback_alloc = 0,
    .traceback_len = 0,
    .traceback_data = NULL,
    .args = (mp_obj_tuple_t *)&mp_const_empty_tuple_obj,
};

STATIC const mp_rom_map_elem_t watchdog_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_watchdog) },
    { MP_ROM_QSTR(MP_QSTR_WatchDogMode),   MP_ROM_PTR(&watchdog_watchdogmode_type) },
    { MP_ROM_QSTR(MP_QSTR_WatchDogTimeout),   MP_ROM_PTR(&mp_type_WatchDogTimeout) },
};

STATIC MP_DEFINE_CONST_DICT(watchdog_module_globals, watchdog_module_globals_table);

const mp_obj_module_t watchdog_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&watchdog_module_globals,
};
