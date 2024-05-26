// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include "py/obj.h"

#include "peripherals/broadcom/defines.h"
#include "peripherals/broadcom/gpio.h"

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *scl_pin;
    const mcu_pin_obj_t *sda_pin;
    BSC0_Type *peripheral;
    uint8_t index;
    bool has_lock;
    bool finish_write;
    uint8_t last_write_data;
} busio_i2c_obj_t;

void reset_i2c(void);
