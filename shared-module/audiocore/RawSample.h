// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "shared-module/audiocore/__init__.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t *buffer;
    uint32_t len;
    uint8_t bits_per_sample;
    bool samples_signed;
    uint8_t channel_count;
    uint32_t sample_rate;
    bool single_buffer;
    uint8_t buffer_index;
} audioio_rawsample_obj_t;


// These are not available from Python because it may be called in an interrupt.
void audioio_rawsample_reset_buffer(audioio_rawsample_obj_t *self,
    bool single_channel_output,
    uint8_t channel);
audioio_get_buffer_result_t audioio_rawsample_get_buffer(audioio_rawsample_obj_t *self,
    bool single_channel_output,
    uint8_t channel,
    uint8_t **buffer,
    uint32_t *buffer_length);                                                      // length in bytes
void audioio_rawsample_get_buffer_structure(audioio_rawsample_obj_t *self, bool single_channel_output,
    bool *single_buffer, bool *samples_signed,
    uint32_t *max_buffer_length, uint8_t *spacing);
