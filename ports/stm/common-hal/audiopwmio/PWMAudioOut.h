// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Artyom Skrobov
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *pin[2];
    uint16_t quiescent_value;

    uint16_t *buffer[2];
    uint16_t buffer_length[2];
    uint16_t buffer_ptr[2];

    mp_obj_t *sample;
    int16_t sample_offset;
    uint8_t sample_channel_count;
    uint8_t bytes_per_sample;

    // PCM-to-PWM conversion state
    int8_t pos[2];   // -1 for off, +1 for on
    uint16_t len[2];

    uint16_t period;
    bool stopping;
    bool paused;
    bool loop;
} audiopwmio_pwmaudioout_obj_t;
