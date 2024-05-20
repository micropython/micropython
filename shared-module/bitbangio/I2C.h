// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/digitalio/DigitalInOut.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    digitalio_digitalinout_obj_t scl;
    digitalio_digitalinout_obj_t sda;
    uint32_t us_delay;
    uint32_t us_timeout;
    volatile bool locked;
} bitbangio_i2c_obj_t;
