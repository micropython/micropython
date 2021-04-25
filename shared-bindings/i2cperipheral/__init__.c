/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Noralf Trønnes
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
// #include "shared-bindings/i2cperipheral/__init__.h"
#include "shared-bindings/i2cperipheral/I2CPeripheral.h"

#include "py/runtime.h"

//| """Two wire serial protocol peripheral
//|
//| The `i2cperipheral` module contains classes to support an I2C peripheral.
//|
//| Example emulating a peripheral with 2 addresses (read and write)::
//|
//|   import board
//|   from i2cperipheral import I2CPeripheral
//|
//|   regs = [0] * 16
//|   index = 0
//|
//|   with I2CPeripheral(board.SCL, board.SDA, (0x40, 0x41)) as device:
//|       while True:
//|           r = device.request()
//|           if not r:
//|               # Maybe do some housekeeping
//|               continue
//|           with r:  # Closes the transfer if necessary by sending a NACK or feeding dummy bytes
//|               if r.address == 0x40:
//|                   if not r.is_read:  # Main write which is Selected read
//|                       b = r.read(1)
//|                       if not b or b[0] > 15:
//|                           break
//|                       index = b[0]
//|                       b = r.read(1)
//|                       if b:
//|                           regs[index] = b[0]
//|                   elif r.is_restart:  # Combined transfer: This is the Main read message
//|                       n = r.write(bytes([regs[index]]))
//|                   #else:
//|                       # A read transfer is not supported in this example
//|                       # If the microcontroller tries, it will get 0xff byte(s) by the ctx manager (r.close())
//|               elif r.address == 0x41:
//|                   if not r.is_read:
//|                       b = r.read(1)
//|                       if b and b[0] == 0xde:
//|                           # do something
//|                           pass
//|
//| This example sets up an I2C device that can be accessed from Linux like this::
//|
//|   $ i2cget -y 1 0x40 0x01
//|   0x00
//|   $ i2cset -y 1 0x40 0x01 0xaa
//|   $ i2cget -y 1 0x40 0x01
//|   0xaa
//|
//| .. warning::
//|    I2CPeripheral makes use of clock stretching in order to slow down
//|    the host.
//|    Make sure the I2C host supports this.
//|
//|    Raspberry Pi in particular does not support this with its I2C hw block.
//|    This can be worked around by using the ``i2c-gpio`` bit banging driver.
//|    Since the RPi firmware uses the hw i2c, it's not possible to emulate a HAT eeprom."""
//|

STATIC const mp_rom_map_elem_t i2cperipheral_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_i2cperipheral) },
    { MP_ROM_QSTR(MP_QSTR_I2CPeripheral), MP_ROM_PTR(&i2cperipheral_i2c_peripheral_type) },
};

STATIC MP_DEFINE_CONST_DICT(i2cperipheral_module_globals, i2cperipheral_module_globals_table);

const mp_obj_module_t i2cperipheral_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&i2cperipheral_module_globals,
};
