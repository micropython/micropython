// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "extmod/vfs_fat.h"
#include "py/obj.h"

#include "shared-module/audiocore/__init__.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t *buffer;
    uint32_t buffer_length;
    uint8_t *second_buffer;
    uint32_t second_buffer_length;
    uint32_t file_length; // In bytes
    uint16_t data_start; // Where the data values start
    uint8_t bits_per_sample;
    uint16_t buffer_index;
    uint32_t bytes_remaining;

    uint8_t channel_count;
    uint32_t sample_rate;

    uint32_t len;
    pyb_file_obj_t *file;

    uint32_t read_count;
    uint32_t left_read_count;
    uint32_t right_read_count;
} audioio_wavefile_obj_t;

// These are not available from Python because it may be called in an interrupt.
void audioio_wavefile_reset_buffer(audioio_wavefile_obj_t *self,
    bool single_channel_output,
    uint8_t channel);
audioio_get_buffer_result_t audioio_wavefile_get_buffer(audioio_wavefile_obj_t *self,
    bool single_channel_output,
    uint8_t channel,
    uint8_t **buffer,
    uint32_t *buffer_length);                                                     // length in bytes
void audioio_wavefile_get_buffer_structure(audioio_wavefile_obj_t *self, bool single_channel_output,
    bool *single_buffer, bool *samples_signed,
    uint32_t *max_buffer_length, uint8_t *spacing);
