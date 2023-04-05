/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Artyom Skrobov
 * Copyright (c) 2023 Jeff Epler for Adafruit Industries
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

#pragma once

#include "shared-module/synthio/Synthesizer.h"

extern const mp_obj_type_t synthio_synthesizer_type;

void common_hal_synthio_synthesizer_construct(synthio_synthesizer_obj_t *self,
    uint32_t sample_rate, const int16_t *waveform, uint16_t waveform_len);

void common_hal_synthio_synthesizer_deinit(synthio_synthesizer_obj_t *self);
bool common_hal_synthio_synthesizer_deinited(synthio_synthesizer_obj_t *self);
uint32_t common_hal_synthio_synthesizer_get_sample_rate(synthio_synthesizer_obj_t *self);
uint8_t common_hal_synthio_synthesizer_get_bits_per_sample(synthio_synthesizer_obj_t *self);
uint8_t common_hal_synthio_synthesizer_get_channel_count(synthio_synthesizer_obj_t *self);
void common_hal_synthio_synthesizer_release(synthio_synthesizer_obj_t *self, mp_obj_t to_release);
void common_hal_synthio_synthesizer_press(synthio_synthesizer_obj_t *self, mp_obj_t to_press);
void common_hal_synthio_synthesizer_release_all(synthio_synthesizer_obj_t *self);
mp_obj_t common_hal_synthio_synthesizer_get_pressed_notes(synthio_synthesizer_obj_t *self);
