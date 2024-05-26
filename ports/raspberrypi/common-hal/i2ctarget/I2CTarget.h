// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Mark Komus, Ken Stillson, im-redactd
//
// SPDX-License-Identifier: MIT

#pragma once

#ifndef MICROPY_INCLUDED_RPI_COMMON_HAL_I2C_TARGET_H
#define MICROPY_INCLUDED_RPI_COMMON_HAL_I2C_TARGET_H

#include "py/obj.h"
#include "common-hal/microcontroller/Pin.h"
#include "src/rp2_common/hardware_i2c/include/hardware/i2c.h"

typedef struct {
    mp_obj_base_t base;

    uint8_t *addresses;
    unsigned int num_addresses;

    i2c_inst_t *peripheral;

    uint8_t scl_pin;
    uint8_t sda_pin;
} i2ctarget_i2c_target_obj_t;

#endif MICROPY_INCLUDED_RPI_COMMON_HAL_BUSIO_I2C_TARGET_H
