/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Sean Cross for Adafruit Industries
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

#include "shared-bindings/watchdog/WatchDogMode.h"

//| class WatchDogMode:
//|     """run state of the watchdog timer"""
//|
//|     def __init__(self, ):
//|         """Enum-like class to define the run mode of the watchdog timer."""
//|
//|     NONE: Any = ...
//|     """Take no action if the watchdog timer expires.
//|
//|     :type watchdog.WatchDogMode:"""
//|
//|     RAISE: Any = ...
//|     """Raise an exception when the WatchDogTimer expires.
//|
//|     :type watchdog.WatchDogMode:"""
//|
//|     RESET: Any = ...
//|     """Reset the system if the WatchDogTimer expires.
//|
//|     :type watchdog.WatchDogMode:"""
//|
const mp_obj_type_t watchdog_watchdogmode_type;

const watchdog_watchdogmode_obj_t watchdog_watchdogmode_none_obj = {
    { &watchdog_watchdogmode_type },
};

const watchdog_watchdogmode_obj_t watchdog_watchdogmode_raise_obj = {
    { &watchdog_watchdogmode_type },
};

const watchdog_watchdogmode_obj_t watchdog_watchdogmode_reset_obj = {
    { &watchdog_watchdogmode_type },
};

STATIC const mp_rom_map_elem_t watchdog_watchdogmode_locals_dict_table[] = {
    {MP_ROM_QSTR(MP_QSTR_NONE),   MP_ROM_PTR(&watchdog_watchdogmode_none_obj)},
    {MP_ROM_QSTR(MP_QSTR_RAISE),  MP_ROM_PTR(&watchdog_watchdogmode_raise_obj)},
    {MP_ROM_QSTR(MP_QSTR_RESET),  MP_ROM_PTR(&watchdog_watchdogmode_reset_obj)},
};
STATIC MP_DEFINE_CONST_DICT(watchdog_watchdogmode_locals_dict, watchdog_watchdogmode_locals_dict_table);

STATIC void watchdog_watchdogmode_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    qstr runmode = MP_QSTR_NONE;
    if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&watchdog_watchdogmode_raise_obj)) {
        runmode = MP_QSTR_RAISE;
    }
    else if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&watchdog_watchdogmode_reset_obj)) {
        runmode = MP_QSTR_RESET;
    }
    mp_printf(print, "%q.%q.%q", MP_QSTR_watchdog, MP_QSTR_WatchDogMode,
              runmode);
}

const mp_obj_type_t watchdog_watchdogmode_type = {
    { &mp_type_type },
    .name = MP_QSTR_WatchDogMode,
    .print = watchdog_watchdogmode_print,
    .locals_dict = (mp_obj_t)&watchdog_watchdogmode_locals_dict,
};
