// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "driver/pulse_cnt.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t pin_a, pin_b;
    mp_int_t position;
    pcnt_unit_handle_t unit;
    pcnt_channel_handle_t channel_a;
    pcnt_channel_handle_t channel_b;
    int8_t divisor; // Number of quadrature edges required per count
} rotaryio_incrementalencoder_obj_t;
