/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2022 Lee Atkinson, MeanStride Technology, Inc.
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
#include "shared-bindings/analogbufio/__init__.h"
#include "shared-bindings/analogbufio/BufferedIn.h"

//| """Analog Buffered IO Hardware Support
//|
//| The `analogbufio` module contains classes to provide access to analog-to-digital
//| conversion and digital-to-analog (DAC) for multiple value transfer.
//|
//| All classes change hardware state and should be deinitialized when they
//| are no longer needed if the program continues after use. To do so, either
//| call :py:meth:`!deinit` or use a context manager. See
//| :ref:`lifetime-and-contextmanagers` for more info.
//|
//| For example::
//|
//|   import analogbufio
//|   import array
//|   from board import *
//|
//|   length = 5000000
//|   mybuffer = array.array("H", 0x0000 for i in range(length))
//|   adc_in = analogbufio.BufferedIn(GP26, mybuffer, length)
//|   analogbufio.read()
//|   print(*mybuffer)
//|   adc_in.deinit()
//|
//| This example will initialize the the device, read and fill
//| :py:data:`~analogbufio.BufferedIn` to mybuffer
//|
//| TODO: For the essentials of `analogbufio`, see the `CircuitPython Essentials
//| Learn guide <https://learn.adafruit.com/circuitpython-essentials/circuitpython-analogbufio>`_
//|
//| TODO: For more information on using `analogbufio`, see `this additional Learn guide
//| <https://learn.adafruit.com/circuitpython-advanced-analog-inputs-and-outputs>`_
//| """

STATIC const mp_rom_map_elem_t analogbufio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_analogbufio) },
    { MP_ROM_QSTR(MP_QSTR_BufferedIn),   MP_ROM_PTR(&analogbufio_bufferedin_type) },
};

STATIC MP_DEFINE_CONST_DICT(analogbufio_module_globals, analogbufio_module_globals_table);

const mp_obj_module_t analogbufio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&analogbufio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_analogbufio, analogbufio_module, CIRCUITPY_ANALOGBUFIO);
