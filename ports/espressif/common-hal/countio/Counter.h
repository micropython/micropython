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
    pcnt_unit_handle_t unit;
    pcnt_channel_handle_t channel;
    mp_int_t count;
    uint8_t pin;
} countio_counter_obj_t;
