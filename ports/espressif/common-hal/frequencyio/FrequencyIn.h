// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "driver/pulse_cnt.h"
#include "driver/gptimer.h"

// This data is used in the interrupt and needs to be in internal RAM.
typedef struct {
    pcnt_unit_handle_t unit;
    int pulse_count;
} _internal_data_t;

typedef struct {
    mp_obj_base_t base;
    pcnt_channel_handle_t channel;
    gptimer_handle_t timer;
    _internal_data_t *internal_data;
    uint8_t pin;
    uint16_t capture_period_ms;
} frequencyio_frequencyin_obj_t;
