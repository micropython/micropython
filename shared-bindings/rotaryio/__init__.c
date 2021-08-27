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
#include "shared-bindings/rotaryio/__init__.h"
#include "shared-bindings/rotaryio/IncrementalEncoder.h"

//| """Support for reading rotation sensors
//|
//| The `rotaryio` module contains classes to read different rotation encoding schemes. See
//| `Wikipedia's Rotary Encoder page <https://en.wikipedia.org/wiki/Rotary_encoder>`_ for more
//| background.
//|
//| All classes change hardware state and should be deinitialized when they
//| are no longer needed if the program continues after use. To do so, either
//| call :py:meth:`!deinit` or use a context manager. See
//| :ref:`lifetime-and-contextmanagers` for more info."""
//|

STATIC const mp_rom_map_elem_t rotaryio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_rotaryio) },
    { MP_ROM_QSTR(MP_QSTR_IncrementalEncoder), MP_ROM_PTR(&rotaryio_incrementalencoder_type) },
};

STATIC MP_DEFINE_CONST_DICT(rotaryio_module_globals, rotaryio_module_globals_table);

const mp_obj_module_t rotaryio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&rotaryio_module_globals,
};
