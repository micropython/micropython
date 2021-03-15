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

// Machine is the HAL for low-level, hardware accelerated functions. It is not
// meant to simplify APIs, its only meant to unify them so that other modules
// do not require port specific logic.
//
// This file includes externs for all functions a port should implement to
// support the machine module.

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_BUSDEVICE_I2CDEVICE_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_BUSDEVICE_I2CDEVICE_H

#include "py/obj.h"

#include "shared-module/adafruit_bus_device/I2CDevice.h"
// #include "shared-bindings/busio/I2C.h"

// Type object used in Python. Should be shared between ports.
extern const mp_obj_type_t adafruit_bus_device_i2cdevice_type;

// Initializes the hardware peripheral.
extern void common_hal_adafruit_bus_device_i2cdevice_construct(adafruit_bus_device_i2cdevice_obj_t *self, mp_obj_t *i2c, uint8_t device_address);
extern void common_hal_adafruit_bus_device_i2cdevice_lock(adafruit_bus_device_i2cdevice_obj_t *self);
extern void common_hal_adafruit_bus_device_i2cdevice_unlock(adafruit_bus_device_i2cdevice_obj_t *self);
extern void common_hal_adafruit_bus_device_i2cdevice_probe_for_device(adafruit_bus_device_i2cdevice_obj_t *self);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_BUSDEVICE_I2CDEVICE_H
