/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft
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

#ifndef MICROPY_INCLUDED_SHARED_MODULE_AUDIOIO_WAVEFILE_H
#define MICROPY_INCLUDED_SHARED_MODULE_AUDIOIO_WAVEFILE_H

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

#endif // MICROPY_INCLUDED_SHARED_MODULE_AUDIOIO_WAVEFILE_H
