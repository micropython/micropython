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

#include "shared-bindings/supervisor/RunReason.h"

MAKE_ENUM_VALUE(canio_bus_state_type, run_reason, ERROR_ACTIVE, RUN_REASON_STARTUP);
MAKE_ENUM_VALUE(canio_bus_state_type, bus_state, ERROR_PASSIVE, RUN_REASON_AUTORELOAD);
MAKE_ENUM_VALUE(canio_bus_state_type, bus_state, ERROR_WARNING, RUN_REASON_SUPERVISOR_RELOAD);
MAKE_ENUM_VALUE(canio_bus_state_type, bus_state, BUS_OFF, RUN_REASON_RELOAD_HOTKEY);

//| class RunReason:
//|     """The state of the CAN bus"""
//|
//|     STARTUP: object
//|     """The first VM was run after the microcontroller started up. See `microcontroller.start_reason`
//|        for more detail why the microcontroller was started."""
//|
//|     AUTORELOAD: object
//|     """The VM was run due to a USB write to the filesystem."""
//|
//|     SUPERVISOR_RELOAD: object
//|     """The VM was run due to a call to `supervisor.reload()`."""
//|
//|     RELOAD_HOTKEY: object
//|     """The VM was run due CTRL-D."""
//|
MAKE_ENUM_MAP(canio_bus_state) {
    MAKE_ENUM_MAP_ENTRY(bus_state, ERROR_ACTIVE),
    MAKE_ENUM_MAP_ENTRY(bus_state, ERROR_PASSIVE),
    MAKE_ENUM_MAP_ENTRY(bus_state, ERROR_WARNING),
    MAKE_ENUM_MAP_ENTRY(bus_state, BUS_OFF),
};
STATIC MP_DEFINE_CONST_DICT(canio_bus_state_locals_dict, canio_bus_state_locals_table);

MAKE_PRINTER(canio, canio_bus_state);

MAKE_ENUM_TYPE(canio, BusState, canio_bus_state);
