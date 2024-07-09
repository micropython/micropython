// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include "hal/include/hal_i2c_m_sync.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    struct i2c_m_sync_desc i2c_desc;
    bool has_lock;
    uint8_t scl_pin;
    uint8_t sda_pin;
} busio_i2c_obj_t;

extern Sercom *samd_i2c_get_sercom(const mcu_pin_obj_t *scl, const mcu_pin_obj_t *sda,
    uint8_t *sercom_index, uint32_t *sda_pinmux, uint32_t *scl_pinmux);
