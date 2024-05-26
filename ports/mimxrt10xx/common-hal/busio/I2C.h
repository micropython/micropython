// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
// SPDX-FileCopyrightText: Copyright (c) 2019 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "fsl_common.h"
#include "periph.h"
#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    LPI2C_Type *i2c;
    bool has_lock;
    const mcu_periph_obj_t *scl;
    const mcu_periph_obj_t *sda;
} busio_i2c_obj_t;

void i2c_reset(void);
