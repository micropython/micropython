// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;

    mp_obj_t *sample;
    uint8_t *buffers[2];
    uint8_t *sample_data, *sample_end;

    uint16_t buffer_length;
    uint16_t sample_rate;
    uint32_t hold_value;

    uint8_t next_buffer;
    uint8_t bit_clock_pin_number;
    uint8_t word_select_pin_number;
    uint8_t data_pin_number;

    uint8_t channel_count;
    uint8_t bytes_per_sample;

    bool left_justified : 1;
    bool playing : 1;
    bool stopping : 1;
    bool paused : 1;
    bool loop : 1;
    bool samples_signed : 1;
    bool single_buffer : 1;
} audiobusio_i2sout_obj_t;

void i2s_reset(void);
void i2s_background(void);
