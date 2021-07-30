/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Dan Halbert for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SHARED_MODULE_AUDIOCORE__INIT__H
#define MICROPY_INCLUDED_SHARED_MODULE_AUDIOCORE__INIT__H

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"
#include "py/proto.h"

typedef enum {
    GET_BUFFER_DONE,            // No more data to read
    GET_BUFFER_MORE_DATA,       // More data to read.
    GET_BUFFER_ERROR,           // Error while reading data.
} audioio_get_buffer_result_t;

typedef uint32_t (*audiosample_sample_rate_fun)(mp_obj_t);
typedef uint8_t (*audiosample_bits_per_sample_fun)(mp_obj_t);
typedef uint8_t (*audiosample_channel_count_fun)(mp_obj_t);
typedef void (*audiosample_reset_buffer_fun)(mp_obj_t,
    bool single_channel_output, uint8_t audio_channel);
typedef audioio_get_buffer_result_t (*audiosample_get_buffer_fun)(mp_obj_t,
    bool single_channel_output, uint8_t channel, uint8_t **buffer,
    uint32_t *buffer_length);
typedef void (*audiosample_get_buffer_structure_fun)(mp_obj_t,
    bool single_channel_output, bool *single_buffer,
    bool *samples_signed, uint32_t *max_buffer_length,
    uint8_t *spacing);

typedef struct _audiosample_p_t {
    MP_PROTOCOL_HEAD // MP_QSTR_protocol_audiosample
    audiosample_sample_rate_fun sample_rate;
    audiosample_bits_per_sample_fun bits_per_sample;
    audiosample_channel_count_fun channel_count;
    audiosample_reset_buffer_fun reset_buffer;
    audiosample_get_buffer_fun get_buffer;
    audiosample_get_buffer_structure_fun get_buffer_structure;
} audiosample_p_t;

uint32_t audiosample_sample_rate(mp_obj_t sample_obj);
uint8_t audiosample_bits_per_sample(mp_obj_t sample_obj);
uint8_t audiosample_channel_count(mp_obj_t sample_obj);
void audiosample_reset_buffer(mp_obj_t sample_obj, bool single_channel_output, uint8_t audio_channel);
audioio_get_buffer_result_t audiosample_get_buffer(mp_obj_t sample_obj,
    bool single_channel_output,
    uint8_t channel,
    uint8_t **buffer, uint32_t *buffer_length);
void audiosample_get_buffer_structure(mp_obj_t sample_obj, bool single_channel_output,
    bool *single_buffer, bool *samples_signed,
    uint32_t *max_buffer_length, uint8_t *spacing);

void audiosample_convert_u8m_s16s(int16_t *buffer_out, const uint8_t *buffer_in, size_t nframes);
void audiosample_convert_u8s_s16s(int16_t *buffer_out, const uint8_t *buffer_in, size_t nframes);
void audiosample_convert_s8m_s16s(int16_t *buffer_out, const int8_t *buffer_in, size_t nframes);
void audiosample_convert_s8s_s16s(int16_t *buffer_out, const int8_t *buffer_in, size_t nframes);
void audiosample_convert_u16m_s16s(int16_t *buffer_out, const uint16_t *buffer_in, size_t nframes);
void audiosample_convert_u16s_s16s(int16_t *buffer_out, const uint16_t *buffer_in, size_t nframes);
void audiosample_convert_s16m_s16s(int16_t *buffer_out, const int16_t *buffer_in, size_t nframes);

#endif  // MICROPY_INCLUDED_SHARED_MODULE_AUDIOCORE__INIT__H
