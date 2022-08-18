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
#include "shared-bindings/adcbuffer/__init__.h"
#include "shared-bindings/adcbuffer/BufferedInput.h"

// #ifdef CIRCUITPY_BUFFEREDINPUT#endif

//| """Analog buffered hardware support
//|
//| The `adcbuffer` module contains classes to provide access to analog-to-digital
//| conversion and digital-to-analog (DAC) for multiple value transfer.
//|
//| All classes change hardware state and should be deinitialized when they
//| are no longer needed if the program continues after use. To do so, either
//| call :py:meth:`!deinit` or use a context manager. See
//| :ref:`lifetime-and-contextmanagers` for more info.
//|
//| For example::
//|
//|   import adcbuffer
///   import array
//|   from board import *
//|
///   length = 5000000
///   mybuffer = array.array("H", [0] * length)
//|   adcbuf_obj = adcbuffer.BufferdInPut(GP26, mybuffer, length)
//|   adcbuffer.readmultiple()
//|   print(*mybuffer)
//|   adcbuf_obj.deinit()
//|
//| This example will initialize the the device, read and fill
//| :py:data:`~adcbuffer.BufferdInPut` to mybuffer and then
//| :py:meth:`~adcbuffer.BufferedInPut.deinit` the hardware. The last step is optional
//| because CircuitPython will do it automatically after the program finishes.
//|
//| TODO: For the essentials of `adcbuffer`, see the `CircuitPython Essentials
//| Learn guide <https://learn.adafruit.com/circuitpython-essentials/circuitpython-adcbuffer>`_
//|
//| TODO: For more information on using `adcbuffer`, see `this additional Learn guide
//| <https://learn.adafruit.com/circuitpython-advanced-analog-inputs-and-outputs>`_
//| """
//|

STATIC const mp_rom_map_elem_t adcbuffer_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_adcbuffer) },
    { MP_ROM_QSTR(MP_QSTR_Bufferedinput),   MP_ROM_PTR(&adcbuffer_bufferedinput_type) },
    // #ifdef CIRCUITPY_BUFFEREDINPUT    #endif
};

STATIC MP_DEFINE_CONST_DICT(adcbuffer_module_globals, adcbuffer_module_globals_table);

const mp_obj_module_t adcbuffer_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&adcbuffer_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_adcbuffer, adcbuffer_module, CIRCUITPY_ADCBUFFER);
