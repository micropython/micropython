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
#include "shared-bindings/analogio/__init__.h"
#include "shared-bindings/analogio/AnalogIn.h"
#include "shared-bindings/analogio/AnalogOut.h"

//| """Analog hardware support
//|
//| The `analogio` module contains classes to provide access to analog IO
//| typically implemented with digital-to-analog (DAC) and analog-to-digital
//| (ADC) converters.
//|
//| All classes change hardware state and should be deinitialized when they
//| are no longer needed if the program continues after use. To do so, either
//| call :py:meth:`!deinit` or use a context manager. See
//| :ref:`lifetime-and-contextmanagers` for more info.
//|
//| For example::
//|
//|   import analogio
//|   from board import *
//|
//|   pin = analogio.AnalogIn(A0)
//|   print(pin.value)
//|   pin.deinit()
//|
//| This example will initialize the the device, read
//| :py:data:`~analogio.AnalogIn.value` and then
//| :py:meth:`~analogio.AnalogIn.deinit` the hardware. The last step is optional
//| because CircuitPython will do it automatically after the program finishes."""
//|

STATIC const mp_rom_map_elem_t analogio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_analogio) },
    { MP_ROM_QSTR(MP_QSTR_AnalogIn),   MP_ROM_PTR(&analogio_analogin_type) },
    { MP_ROM_QSTR(MP_QSTR_AnalogOut),   MP_ROM_PTR(&analogio_analogout_type) },
};

STATIC MP_DEFINE_CONST_DICT(analogio_module_globals, analogio_module_globals_table);

const mp_obj_module_t analogio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&analogio_module_globals,
};
