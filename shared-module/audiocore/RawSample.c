/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#include "shared-bindings/audiocore/RawSample.h"

#include <stdint.h>

#include "shared-module/audiocore/RawSample.h"

void common_hal_audioio_rawsample_construct(audioio_rawsample_obj_t *self,
    uint8_t *buffer,
    uint32_t len,
    uint8_t bytes_per_sample,
    bool samples_signed,
    uint8_t channel_count,
    uint32_t sample_rate) {
    self->buffer = buffer;
    self->bits_per_sample = bytes_per_sample * 8;
    self->samples_signed = samples_signed;
    self->len = len;
    self->channel_count = channel_count;
    self->sample_rate = sample_rate;
}

void common_hal_audioio_rawsample_deinit(audioio_rawsample_obj_t *self) {
    self->buffer = NULL;
}
bool common_hal_audioio_rawsample_deinited(audioio_rawsample_obj_t *self) {
    return self->buffer == NULL;
}

uint32_t common_hal_audioio_rawsample_get_sample_rate(audioio_rawsample_obj_t *self) {
    return self->sample_rate;
}
void common_hal_audioio_rawsample_set_sample_rate(audioio_rawsample_obj_t *self,
    uint32_t sample_rate) {
    self->sample_rate = sample_rate;
}
uint8_t common_hal_audioio_rawsample_get_bits_per_sample(audioio_rawsample_obj_t *self) {
    return self->bits_per_sample;
}
uint8_t common_hal_audioio_rawsample_get_channel_count(audioio_rawsample_obj_t *self) {
    return self->channel_count;
}

void audioio_rawsample_reset_buffer(audioio_rawsample_obj_t *self,
    bool single_channel_output,
    uint8_t channel) {
}

audioio_get_buffer_result_t audioio_rawsample_get_buffer(audioio_rawsample_obj_t *self,
    bool single_channel_output,
    uint8_t channel,
    uint8_t **buffer,
    uint32_t *buffer_length) {
    *buffer_length = self->len;
    if (single_channel_output) {
        *buffer = self->buffer + (channel % self->channel_count) * (self->bits_per_sample / 8);
    } else {
        *buffer = self->buffer;
    }
    return GET_BUFFER_DONE;
}

void audioio_rawsample_get_buffer_structure(audioio_rawsample_obj_t *self, bool single_channel_output,
    bool *single_buffer, bool *samples_signed,
    uint32_t *max_buffer_length, uint8_t *spacing) {
    *single_buffer = true;
    *samples_signed = self->samples_signed;
    *max_buffer_length = self->len;
    if (single_channel_output) {
        *spacing = self->channel_count;
    } else {
        *spacing = 1;
    }
}
