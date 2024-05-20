// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

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
