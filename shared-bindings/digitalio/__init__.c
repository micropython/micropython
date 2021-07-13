/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
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

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/digitalio/__init__.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/digitalio/Direction.h"
#include "shared-bindings/digitalio/DriveMode.h"
#include "shared-bindings/digitalio/Pull.h"

#include "py/runtime.h"

//| """Basic digital pin support
//|
//| The `digitalio` module contains classes to provide access to basic digital IO.
//|
//| All classes change hardware state and should be deinitialized when they
//| are no longer needed if the program continues after use. To do so, either
//| call :py:meth:`!deinit` or use a context manager. See
//| :ref:`lifetime-and-contextmanagers` for more info.
//|
//| For example::
//|
//|   import digitalio
//|   from board import *
//|
//|   pin = digitalio.DigitalInOut(D13)
//|   print(pin.value)
//|
//| This example will initialize the the device, read
//| :py:data:`~digitalio.DigitalInOut.value` and then
//| :py:meth:`~digitalio.DigitalInOut.deinit` the hardware.
//|
//| Here is blinky::
//|
//|   import digitalio
//|   from board import *
//|   import time
//|
//|   led = digitalio.DigitalInOut(D13)
//|   led.direction = digitalio.Direction.OUTPUT
//|   while True:
//|       led.value = True
//|       time.sleep(0.1)
//|       led.value = False
//|       time.sleep(0.1)"""
//|

STATIC const mp_rom_map_elem_t digitalio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_digitalio) },
    { MP_ROM_QSTR(MP_QSTR_DigitalInOut),  MP_ROM_PTR(&digitalio_digitalinout_type) },

    // Enum-like Classes.
    { MP_ROM_QSTR(MP_QSTR_Direction),          MP_ROM_PTR(&digitalio_direction_type) },
    { MP_ROM_QSTR(MP_QSTR_DriveMode),          MP_ROM_PTR(&digitalio_drive_mode_type) },
    { MP_ROM_QSTR(MP_QSTR_Pull),               MP_ROM_PTR(&digitalio_pull_type) },
};

STATIC MP_DEFINE_CONST_DICT(digitalio_module_globals, digitalio_module_globals_table);

const mp_obj_module_t digitalio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&digitalio_module_globals,
};
