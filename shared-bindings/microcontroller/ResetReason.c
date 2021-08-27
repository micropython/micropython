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

#include "py/obj.h"
#include "py/enum.h"

#include "shared-bindings/microcontroller/ResetReason.h"

MAKE_ENUM_VALUE(mcu_reset_reason_type, reset_reason, POWER_ON, RESET_REASON_POWER_ON);
MAKE_ENUM_VALUE(mcu_reset_reason_type, reset_reason, BROWNOUT, RESET_REASON_BROWNOUT);
MAKE_ENUM_VALUE(mcu_reset_reason_type, reset_reason, SOFTWARE, RESET_REASON_SOFTWARE);
MAKE_ENUM_VALUE(mcu_reset_reason_type, reset_reason, DEEP_SLEEP_ALARM, RESET_REASON_DEEP_SLEEP_ALARM);
MAKE_ENUM_VALUE(mcu_reset_reason_type, reset_reason, RESET_PIN, RESET_REASON_RESET_PIN);
MAKE_ENUM_VALUE(mcu_reset_reason_type, reset_reason, WATCHDOG, RESET_REASON_WATCHDOG);
MAKE_ENUM_VALUE(mcu_reset_reason_type, reset_reason, UNKNOWN, RESET_REASON_UNKNOWN);

//| class ResetReason:
//|     """The reason the microntroller was last reset"""
//|
//|     POWER_ON: object
//|     """The microntroller was started from power off."""
//|
//|     BROWNOUT: object
//|     """The microntroller was reset due to too low a voltage."""
//|
//|     SOFTWARE: object
//|     """The microntroller was reset from software."""
//|
//|     DEEP_SLEEP_ALARM: object
//|     """The microntroller was reset for deep sleep and restarted by an alarm."""
//|
//|     RESET_PIN: object
//|     """The microntroller was reset by a signal on its reset pin. The pin might be connected to a reset button."""
//|
//|     WATCHDOG: object
//|     """The microcontroller was reset by its watchdog timer."""
//|
//|     UNKNOWN: object
//|     """The microntroller restarted for an unknown reason."""
//|
MAKE_ENUM_MAP(mcu_reset_reason) {
    MAKE_ENUM_MAP_ENTRY(reset_reason, POWER_ON),
    MAKE_ENUM_MAP_ENTRY(reset_reason, BROWNOUT),
    MAKE_ENUM_MAP_ENTRY(reset_reason, SOFTWARE),
    MAKE_ENUM_MAP_ENTRY(reset_reason, DEEP_SLEEP_ALARM),
    MAKE_ENUM_MAP_ENTRY(reset_reason, RESET_PIN),
    MAKE_ENUM_MAP_ENTRY(reset_reason, WATCHDOG),
    MAKE_ENUM_MAP_ENTRY(reset_reason, UNKNOWN),
};
STATIC MP_DEFINE_CONST_DICT(mcu_reset_reason_locals_dict, mcu_reset_reason_locals_table);

MAKE_PRINTER(microcontroller, mcu_reset_reason);

MAKE_ENUM_TYPE(microcontroller, ResetReason, mcu_reset_reason);
