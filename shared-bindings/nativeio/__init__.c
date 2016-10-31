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

// nativeio is the HAL for low-level, hardware accelerated classes. It
// is not meant to simplify APIs, its only meant to unify them so that other
// libraries do not require port specific logic.

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/nativeio/__init__.h"
#include "shared-bindings/nativeio/AnalogIn.h"
#include "shared-bindings/nativeio/AnalogOut.h"
#include "shared-bindings/nativeio/DigitalInOut.h"
#include "shared-bindings/nativeio/I2C.h"
#include "shared-bindings/nativeio/PWMOut.h"
#include "shared-bindings/nativeio/SPI.h"
#include "shared-bindings/nativeio/TouchIn.h"
#include "shared-bindings/nativeio/UART.h"
#include "common-hal/nativeio/types.h"
#include "shared-bindings/nativeio/__init__.h"

#include "py/runtime.h"

//| :mod:`nativeio` --- Hardware accelerated behavior
//| =================================================
//|
//| .. module:: nativeio
//|   :synopsis: Hardware accelerated behavior
//|   :platform: SAMD21
//|
//| The `nativeio` module contains classes to provide access to IO accelerated
//| by hardware on the onboard microcontroller. The classes are meant to align
//| with commonly hardware accelerated IO and not necessarily match up with
//| microcontroller structure (because it varies).
//|
//| If the microcontroller doesn't not support the behavior in a hardware
//| accelerated fashion it throws a NotImplementedError on construction. Use
//| `bitbangio` module instead which only depends on
//| :py:class:`~nativeio.DigitalInOut` and is shared across hardware ports.
//|
//| Libraries
//|
//| .. toctree::
//|     :maxdepth: 3
//|
//|     AnalogIn
//|     AnalogOut
//|     DigitalInOut
//|     I2C
//|     PWMOut
//|     SPI
//|
//| All libraries change hardware state and should be deinitialized when they
//| are no longer needed. To do so, either call :py:meth:`!deinit` or use a
//| context manager.
//|
//| For example::
//|
//|   import nativeio
//|   from board import *
//|
//|   with nativeio.I2C(SCL, SDA) as i2c:
//|     i2c.scan()
//|
//| This example will initialize the the device, run
//| :py:meth:`~nativeio.I2C.scan` and then :py:meth:`~nativeio.I2C.deinit` the
//| hardware.
//|
//| Here is blinky::
//|
//|   import nativeio
//|   from board import *
//|   import time
//|
//|   with nativeio.DigitalInOut(D13) as led:
//|     led.switch_to_output()
//|     while True:
//|       led.value = True
//|       time.sleep(0.1)
//|       led.value = False
//|       time.sleep(0.1)
//|

STATIC const mp_rom_map_elem_t nativeio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_nativeio) },
    { MP_ROM_QSTR(MP_QSTR_AnalogIn),   MP_ROM_PTR(&nativeio_analogin_type) },
    { MP_ROM_QSTR(MP_QSTR_AnalogOut),   MP_ROM_PTR(&nativeio_analogout_type) },
    { MP_ROM_QSTR(MP_QSTR_DigitalInOut),  MP_ROM_PTR(&nativeio_digitalinout_type) },
    { MP_ROM_QSTR(MP_QSTR_I2C),   MP_ROM_PTR(&nativeio_i2c_type) },
    { MP_ROM_QSTR(MP_QSTR_PWMOut), MP_ROM_PTR(&nativeio_pwmout_type) },
    { MP_ROM_QSTR(MP_QSTR_SPI),   MP_ROM_PTR(&nativeio_spi_type) },
    { MP_ROM_QSTR(MP_QSTR_TouchIn),   MP_ROM_PTR(&nativeio_touchin_type) },
    { MP_ROM_QSTR(MP_QSTR_UART),   MP_ROM_PTR(&nativeio_uart_type) },
};

STATIC MP_DEFINE_CONST_DICT(nativeio_module_globals, nativeio_module_globals_table);

const mp_obj_module_t nativeio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&nativeio_module_globals,
};
