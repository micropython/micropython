// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Paul Sokolovsky
//
// SPDX-License-Identifier: MIT

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

//| class WatchDogTimeout(Exception):
//|     """Exception raised when the watchdog timer is set to
//|     ``WatchDogMode.RAISE`` and expires.
//|
//|     Example::
//|
//|         import microcontroller
//|         import watchdog
//|         import time
//|
//|         wdt = microcontroller.watchdog
//|         wdt.timeout = 5
//|
//|         while True:
//|             wdt.mode = watchdog.WatchDogMode.RAISE
//|             print("Starting loop -- should exit after five seconds")
//|             try:
//|                 while True:
//|                     time.sleep(10)  # Also works with pass
//|             except watchdog.WatchDogTimeout as e:
//|                 print("Watchdog expired")
//|             except Exception as e:
//|                 print("Other exception")
//|
//|         print("Exited loop")
//|     """
//|

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_WatchDogTimeout,
    MP_QSTR_WatchDogTimeout,
    MP_TYPE_FLAG_NONE,
    make_new, mp_obj_exception_make_new,
    attr, mp_obj_exception_attr,
    parent, &mp_type_Exception
    );

mp_obj_exception_t mp_watchdog_timeout_exception = {
    .base.type = &mp_type_WatchDogTimeout,
    .args = (mp_obj_tuple_t *)&mp_const_empty_tuple_obj,
    .traceback = (mp_obj_traceback_t *)&mp_const_empty_traceback_obj,
};

static const mp_rom_map_elem_t watchdog_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_watchdog) },
    { MP_ROM_QSTR(MP_QSTR_WatchDogMode),   MP_ROM_PTR(&watchdog_watchdogmode_type) },
    { MP_ROM_QSTR(MP_QSTR_WatchDogTimeout),   MP_ROM_PTR(&mp_type_WatchDogTimeout) },
};

static MP_DEFINE_CONST_DICT(watchdog_module_globals, watchdog_module_globals_table);

const mp_obj_module_t watchdog_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&watchdog_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_watchdog, watchdog_module);
