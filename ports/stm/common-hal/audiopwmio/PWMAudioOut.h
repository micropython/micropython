/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Artyom Skrobov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef MICROPY_INCLUDED_STM_COMMON_HAL_AUDIOPWM_AUDIOOUT_H
#define MICROPY_INCLUDED_STM_COMMON_HAL_AUDIOPWM_AUDIOOUT_H

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

void audiopwmout_reset(void);

#endif
