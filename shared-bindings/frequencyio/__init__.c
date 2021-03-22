/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Michael Schroeder
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
#include "shared-bindings/frequencyio/__init__.h"
#include "shared-bindings/frequencyio/FrequencyIn.h"

//| """Support for frequency based protocols
//|
//| .. warning:: This module is not available in SAMD21 builds. See the
//|   :ref:`module-support-matrix` for more info.
//|

//| All classes change hardware state and should be deinitialized when they
//| are no longer needed if the program continues after use. To do so, either
//| call :py:meth:`!deinit` or use a context manager. See
//| :ref:`lifetime-and-contextmanagers` for more info.
//|
//| For example::
//|
//|   import frequencyio
//|   import time
//|   from board import *
//|
//|   frequency = frequencyio.FrequencyIn(D13)
//|   frequency.capture_period = 15
//|   time.sleep(0.1)
//|
//| This example will initialize the the device, set
//| :py:data:`~frequencyio.FrequencyIn.capture_period`, and then sleep 0.1 seconds.
//| CircuitPython will automatically turn off FrequencyIn capture when it resets all
//| hardware after program completion. Use ``deinit()`` or a ``with`` statement
//| to do it yourself."""
//|

STATIC const mp_rom_map_elem_t frequencyio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_frequencyio) },
    { MP_ROM_QSTR(MP_QSTR_FrequencyIn), MP_ROM_PTR(&frequencyio_frequencyin_type) },
};

STATIC MP_DEFINE_CONST_DICT(frequencyio_module_globals, frequencyio_module_globals_table);

const mp_obj_module_t frequencyio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&frequencyio_module_globals,
};
