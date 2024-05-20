// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "shared-module/bitbangio/I2C.h"

#include "py/obj.h"

#include "src/rp2_common/hardware_i2c/include/hardware/i2c.h"

typedef struct {
    mp_obj_base_t base;
    i2c_inst_t *peripheral;
    bitbangio_i2c_obj_t bitbangio_i2c;
    bool has_lock;
    uint baudrate;
    uint8_t scl_pin;
    uint8_t sda_pin;
} busio_i2c_obj_t;

void reset_i2c(void);
