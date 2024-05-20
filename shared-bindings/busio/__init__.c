// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/busio/__init__.h"
#include "shared-bindings/busio/I2C.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/busio/UART.h"

#include "py/runtime.h"

//| """Hardware accelerated external bus access
//|
//| The `busio` module contains classes to support a variety of serial
//| protocols.
//|
//| When the microcontroller does not support the behavior in a hardware
//| accelerated fashion it may internally use a bitbang routine. However, if
//| hardware support is available on a subset of pins but not those provided,
//| then a RuntimeError will be raised. Use the `bitbangio` module to explicitly
//| bitbang a serial protocol on any general purpose pins.
//|
//| All classes change hardware state and should be deinitialized when they
//| are no longer needed if the program continues after use. To do so, either
//| call :py:meth:`!deinit` or use a context manager. See
//| :ref:`lifetime-and-contextmanagers` for more info.
//|
//| For example::
//|
//|   import busio
//|   from board import *
//|
//|   i2c = busio.I2C(SCL, SDA)
//|   i2c.try_lock()
//|   print(i2c.scan())
//|   i2c.unlock()
//|   i2c.deinit()
//|
//| This example will initialize the the device, lock the I2C bus, run
//| :py:meth:`~busio.I2C.scan`, unlock the bus,
//| and then :py:meth:`~busio.I2C.deinit` the hardware.
//| The last step is optional because CircuitPython automatically
//| resets hardware after a program finishes.
//|
//| Note that drivers will typically handle communication if provided the bus
//| instance (such as ``busio.I2C(board.SCL, board.SDA)``), and that many of
//| the methods listed here are lower level functionalities that are needed
//| for working with custom drivers.
//|
//| Tutorial for I2C and SPI:
//| https://learn.adafruit.com/circuitpython-basics-i2c-and-spi
//|
//| Tutorial for UART:
//| https://learn.adafruit.com/circuitpython-essentials/circuitpython-uart-serial
//|
//| .. jinja
//| """

static const mp_rom_map_elem_t busio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_busio) },
    { MP_ROM_QSTR(MP_QSTR_I2C),   MP_ROM_PTR(&busio_i2c_type) },
    { MP_ROM_QSTR(MP_QSTR_SPI),   MP_ROM_PTR(&busio_spi_type) },
    { MP_ROM_QSTR(MP_QSTR_UART),   MP_ROM_PTR(&busio_uart_type) },
};

static MP_DEFINE_CONST_DICT(busio_module_globals, busio_module_globals_table);

const mp_obj_module_t busio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&busio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_busio, busio_module);
