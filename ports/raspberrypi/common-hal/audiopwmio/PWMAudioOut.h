// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/pwmio/PWMOut.h"

#include "audio_dma.h"

typedef struct {
    mp_obj_base_t base;
    pwmio_pwmout_obj_t left_pwm;
    pwmio_pwmout_obj_t right_pwm;
    audio_dma_t dma;
    uint16_t quiescent_value;
    uint8_t pacing_timer;
    bool stereo;     // if false, only using left_pwm.
    bool swap_channel;
} audiopwmio_pwmaudioout_obj_t;

void audiopwmout_background(void);
