// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

typedef struct {
    mp_obj_base_t base;
    mp_obj_t *sample;
    NRF_PWM_Type *pwm;
    uint16_t *buffers[2];

    uint16_t quiescent_value;
    uint16_t scale;

    uint8_t left_channel_number;
    uint8_t right_channel_number;
    uint8_t sample_channel_count;
    uint8_t bytes_per_sample;

    IRQn_Type pwm_irq;

    bool playing;
    bool stopping;
    bool paused;
    bool loop;
    bool signed_to_unsigned;
    bool single_buffer;
} audiopwmio_pwmaudioout_obj_t;

void audiopwmout_background(void);
