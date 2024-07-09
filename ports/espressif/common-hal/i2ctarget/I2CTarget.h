// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "peripherals/i2c.h"
#include "common-hal/microcontroller/Pin.h"

typedef struct {
    mp_obj_base_t base;
    i2c_port_t i2c_num;
    uint8_t *addresses;
    uint8_t num_addresses;
    const mcu_pin_obj_t *scl_pin;
    const mcu_pin_obj_t *sda_pin;
} i2ctarget_i2c_target_obj_t;
