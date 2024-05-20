// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "common-hal/busio/I2C.h"

typedef struct {
    mp_obj_base_t base;
    mp_obj_t *i2c;
    uint8_t device_address;
} adafruit_bus_device_i2cdevice_obj_t;
