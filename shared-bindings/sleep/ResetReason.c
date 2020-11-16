/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
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

#include "shared-bindings/sleep/ResetReason.h"

MAKE_ENUM_VALUE(sleep_reset_reason_type, reset_reason, POWER_VALID, RESET_REASON_POWER_VALID);
MAKE_ENUM_VALUE(sleep_reset_reason_type, reset_reason, SOFTWARE, RESET_REASON_SOFTWARE);
MAKE_ENUM_VALUE(sleep_reset_reason_type, reset_reason, DEEP_SLEEP_ALARM, RESET_REASON_DEEP_SLEEP_ALARM);
MAKE_ENUM_VALUE(sleep_reset_reason_type, reset_reason, EXTERNAL, RESET_REASON_EXTERNAL);

//| class ResetReason:
//|     """The reason the chip was last reset"""
//|
//|     POWER_VALID: object
//|     """The chip was reset and started once power levels were valid."""
//|
//|     SOFTWARE: object
//|     """The chip was reset from software."""
//|
//|     DEEP_SLEEP_ALARM: object
//|     """The chip was reset for deep sleep and started by an alarm."""
//|
//|     EXTERNAL: object
//|     """The chip was reset by an external input such as a button."""
//|
MAKE_ENUM_MAP(sleep_reset_reason) {
    MAKE_ENUM_MAP_ENTRY(reset_reason, POWER_VALID),
    MAKE_ENUM_MAP_ENTRY(reset_reason, SOFTWARE),
    MAKE_ENUM_MAP_ENTRY(reset_reason, DEEP_SLEEP_ALARM),
    MAKE_ENUM_MAP_ENTRY(reset_reason, EXTERNAL),
};
STATIC MP_DEFINE_CONST_DICT(sleep_reset_reason_locals_dict, sleep_reset_reason_locals_table);

MAKE_PRINTER(sleep, sleep_reset_reason);

MAKE_ENUM_TYPE(sleep, ResetReason, sleep_reset_reason);
