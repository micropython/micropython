// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Noralf Trønnes
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;

    uint8_t *addresses;
    unsigned int num_addresses;

    Sercom *sercom;
    uint8_t scl_pin;
    uint8_t sda_pin;
    bool writing;
} i2ctarget_i2c_target_obj_t;
