// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_ATMEL_SAMD_SHARED_MODULE_BUSDEVICE_I2CDEVICE_H
#define MICROPY_INCLUDED_ATMEL_SAMD_SHARED_MODULE_BUSDEVICE_I2CDEVICE_H

#include "py/obj.h"
#include "common-hal/busio/I2C.h"

typedef struct {
    mp_obj_base_t base;
    mp_obj_t *i2c;
    uint8_t device_address;
} adafruit_bus_device_i2cdevice_obj_t;

#endif // MICROPY_INCLUDED_ATMEL_SAMD_SHARED_MODULE_BUSDEVICE_I2CDEVICE_H
