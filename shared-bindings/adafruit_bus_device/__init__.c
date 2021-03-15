/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Mark Komus
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
#include "py/mphal.h"
#include "py/objproperty.h"

#include "shared-bindings/adafruit_bus_device/__init__.h"
#include "shared-bindings/adafruit_bus_device/I2CDevice.h"
#include "shared-bindings/adafruit_bus_device/SPIDevice.h"

STATIC const mp_rom_map_elem_t adafruit_bus_device_i2c_device_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_i2c_device) },
    { MP_ROM_QSTR(MP_QSTR_I2CDevice),   MP_ROM_PTR(&adafruit_bus_device_i2cdevice_type) },
};
STATIC MP_DEFINE_CONST_DICT(adafruit_bus_device_i2c_device_globals, adafruit_bus_device_i2c_device_globals_table);

const mp_obj_module_t adafruit_bus_device_i2c_device_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&adafruit_bus_device_i2c_device_globals,
};

STATIC const mp_rom_map_elem_t adafruit_bus_device_spi_device_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_spi_device) },
    { MP_ROM_QSTR(MP_QSTR_SPIDevice),   MP_ROM_PTR(&adafruit_bus_device_spidevice_type) },
};
STATIC MP_DEFINE_CONST_DICT(adafruit_bus_device_spi_device_globals, adafruit_bus_device_spi_device_globals_table);

const mp_obj_module_t adafruit_bus_device_spi_device_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&adafruit_bus_device_spi_device_globals,
};

//| """Hardware accelerated external bus access
//|
//| The I2CDevice and SPIDevice helper classes make managing transaction state on a bus easy.
//| For example, they manage locking the bus to prevent other concurrent access. For SPI
//| devices, it manages the chip select and protocol changes such as mode. For I2C, it
//| manages the device address."""
//|
STATIC const mp_rom_map_elem_t adafruit_bus_device_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_adafruit_bus_device) },
    { MP_ROM_QSTR(MP_QSTR_i2c_device),   MP_ROM_PTR(&adafruit_bus_device_i2c_device_module) },
    { MP_ROM_QSTR(MP_QSTR_spi_device),   MP_ROM_PTR(&adafruit_bus_device_spi_device_module) },
};

STATIC MP_DEFINE_CONST_DICT(adafruit_bus_device_module_globals, adafruit_bus_device_module_globals_table);

const mp_obj_module_t adafruit_bus_device_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&adafruit_bus_device_module_globals,
};
