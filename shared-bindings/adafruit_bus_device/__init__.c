// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Mark Komus
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/objproperty.h"

#include "shared-bindings/adafruit_bus_device/__init__.h"
#include "shared-bindings/adafruit_bus_device/i2c_device/I2CDevice.h"
#include "shared-bindings/adafruit_bus_device/spi_device/SPIDevice.h"

static const mp_rom_map_elem_t adafruit_bus_device_i2c_device_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_i2c_device) },
    { MP_ROM_QSTR(MP_QSTR_I2CDevice),   MP_ROM_PTR(&adafruit_bus_device_i2cdevice_type) },
};
static MP_DEFINE_CONST_DICT(adafruit_bus_device_i2c_device_globals, adafruit_bus_device_i2c_device_globals_table);

const mp_obj_module_t adafruit_bus_device_i2c_device_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&adafruit_bus_device_i2c_device_globals,
};

static const mp_rom_map_elem_t adafruit_bus_device_spi_device_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_spi_device) },
    { MP_ROM_QSTR(MP_QSTR_SPIDevice),   MP_ROM_PTR(&adafruit_bus_device_spidevice_type) },
};
static MP_DEFINE_CONST_DICT(adafruit_bus_device_spi_device_globals, adafruit_bus_device_spi_device_globals_table);

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
static const mp_rom_map_elem_t adafruit_bus_device_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_adafruit_bus_device) },
    { MP_ROM_QSTR(MP_QSTR_i2c_device),   MP_ROM_PTR(&adafruit_bus_device_i2c_device_module) },
    { MP_ROM_QSTR(MP_QSTR_spi_device),   MP_ROM_PTR(&adafruit_bus_device_spi_device_module) },
};

static MP_DEFINE_CONST_DICT(adafruit_bus_device_module_globals, adafruit_bus_device_module_globals_table);

const mp_obj_module_t adafruit_bus_device_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&adafruit_bus_device_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_adafruit_bus_device, adafruit_bus_device_module);
MP_REGISTER_MODULE(MP_QSTR_adafruit_bus_device_dot_i2c_device, adafruit_bus_device_i2c_device_module);
MP_REGISTER_MODULE(MP_QSTR_adafruit_bus_device_dot_spi_device, adafruit_bus_device_spi_device_module);
