// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Mark Komus
//
// SPDX-License-Identifier: MIT

// Machine is the HAL for low-level, hardware accelerated functions. It is not
// meant to simplify APIs, its only meant to unify them so that other modules
// do not require port specific logic.
//
// This file includes externs for all functions a port should implement to
// support the machine module.

#pragma once

#include "py/obj.h"

#include "shared-module/adafruit_bus_device/i2c_device/I2CDevice.h"
// #include "shared-bindings/busio/I2C.h"

// Type object used in Python. Should be shared between ports.
extern const mp_obj_type_t adafruit_bus_device_i2cdevice_type;

// Initializes the hardware peripheral.
extern void common_hal_adafruit_bus_device_i2cdevice_construct(adafruit_bus_device_i2cdevice_obj_t *self, mp_obj_t *i2c, uint8_t device_address);
extern void common_hal_adafruit_bus_device_i2cdevice_lock(adafruit_bus_device_i2cdevice_obj_t *self);
extern void common_hal_adafruit_bus_device_i2cdevice_unlock(adafruit_bus_device_i2cdevice_obj_t *self);
extern void common_hal_adafruit_bus_device_i2cdevice_probe_for_device(adafruit_bus_device_i2cdevice_obj_t *self);
