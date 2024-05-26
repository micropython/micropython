// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright 2019 Sony Semiconductor Solutions Corporation
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "common-hal/microcontroller/Pin.h"

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *pin;
    int8_t number;
} analogio_analogin_obj_t;

void analogin_reset(void);
