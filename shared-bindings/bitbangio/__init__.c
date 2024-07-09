// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

// bitbangio implements some standard protocols in the processor. Its only
// dependency is digitalio.

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/bitbangio/__init__.h"
#include "shared-bindings/bitbangio/I2C.h"
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

static const mp_rom_map_elem_t bitbangio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_bitbangio) },
    { MP_ROM_QSTR(MP_QSTR_I2C),   MP_ROM_PTR(&bitbangio_i2c_type) },
    { MP_ROM_QSTR(MP_QSTR_SPI),   MP_ROM_PTR(&bitbangio_spi_type) },
};

static MP_DEFINE_CONST_DICT(bitbangio_module_globals, bitbangio_module_globals_table);

const mp_obj_module_t bitbangio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&bitbangio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_bitbangio, bitbangio_module);
