/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft
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

// bitbangio implements some standard protocols in the processor. Its only
// dependency is digitalio.

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/bitbangio/__init__.h"
#include "shared-bindings/bitbangio/I2C.h"
#include "shared-bindings/bitbangio/OneWire.h"
#include "shared-bindings/bitbangio/SPI.h"

#include "py/runtime.h"

//| """Digital protocols implemented by the CPU
//|
//| The `bitbangio` module contains classes to provide digital bus protocol
//| support regardless of whether the underlying hardware exists to use the
//| protocol.
//|
//| First try to use `busio` module instead which may utilize peripheral
//| hardware to implement the protocols. Native implementations will be faster
//| than bitbanged versions and have more capabilities.
//|
//| All classes change hardware state and should be deinitialized when they
//| are no longer needed if the program continues after use. To do so, either
//| call :py:meth:`!deinit` or use a context manager. See
//| :ref:`lifetime-and-contextmanagers` for more info.
//|
//| For example::
//|
//|   import bitbangio
//|   from board import *
//|
//|   i2c = bitbangio.I2C(SCL, SDA)
//|   print(i2c.scan())
//|   i2c.deinit()
//|
//| This example will initialize the the device, run
//| :py:meth:`~bitbangio.I2C.scan` and then :py:meth:`~bitbangio.I2C.deinit` the
//| hardware. The last step is optional because CircuitPython automatically
//| resets hardware after a program finishes."""
//|

STATIC const mp_rom_map_elem_t bitbangio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_bitbangio) },
    { MP_ROM_QSTR(MP_QSTR_I2C),   MP_ROM_PTR(&bitbangio_i2c_type) },
    { MP_ROM_QSTR(MP_QSTR_OneWire),   MP_ROM_PTR(&bitbangio_onewire_type) },
    { MP_ROM_QSTR(MP_QSTR_SPI),   MP_ROM_PTR(&bitbangio_spi_type) },
};

STATIC MP_DEFINE_CONST_DICT(bitbangio_module_globals, bitbangio_module_globals_table);

const mp_obj_module_t bitbangio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&bitbangio_module_globals,
};
