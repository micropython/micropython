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

//| """Watchdog Timer
//|
//| The `watchdog` module provides support for a Watchdog Timer. This timer will reset the device
//| if it hasn't been fed after a specified amount of time. This is useful to ensure the board
//| has not crashed or locked up. You can enable the watchdog timer using :class:`wdt.WDT`.
//| Note that on some platforms the watchdog timer cannot be disabled once it has been enabled.
//|
//| The WatchDogTimer is used to restart the system when the application crashes and ends
//| up into a non recoverable state. Once started it cannot be stopped or
//| reconfigured in any way. After enabling, the application must "feed" the
//| watchdog periodically to prevent it from expiring and resetting the system.
//|
//| Note that this module can't be imported and used directly. The sole
//| instance of :class:`WatchDogTimer` is available at
//| :attr:`microcontroller.watchdog`.
//|
//| Example usage::
//|
//|     from microcontroller.watchdog import WatchDogTimer
//|     wdt = WatchDogTimer(timeout=2.5)  # enable it with a timeout of 2.5 seconds
//|     wdt.feed()"""
//|
