/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 MicroDev
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

#include "py/enum.h"

#include "shared-bindings/watchdog/WatchDogMode.h"

MAKE_ENUM_VALUE(watchdog_watchdogmode_type, watchdogmode, RAISE, WATCHDOGMODE_RAISE);
MAKE_ENUM_VALUE(watchdog_watchdogmode_type, watchdogmode, RESET, WATCHDOGMODE_RESET);

//| class WatchDogMode:
//|     """Run state of the watchdog timer."""
//|
//|     RAISE: WatchDogMode
//|     """Raise an exception when the `WatchDogTimer` expires."""
//|
//|     RESET: WatchDogMode
//|     """Reset the system when the `WatchDogTimer` expires."""
//|
MAKE_ENUM_MAP(watchdog_watchdogmode) {
    MAKE_ENUM_MAP_ENTRY(watchdogmode, RAISE),
    MAKE_ENUM_MAP_ENTRY(watchdogmode, RESET),
};
STATIC MP_DEFINE_CONST_DICT(watchdog_watchdogmode_locals_dict, watchdog_watchdogmode_locals_table);

MAKE_PRINTER(watchdog, watchdog_watchdogmode);

MAKE_ENUM_TYPE(watchdog, WatchDogMode, watchdog_watchdogmode);
