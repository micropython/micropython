/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft
 * Copyright (c) 2019 Jeff Epler for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SHARED_MODULE_AUDIOIO_MP3FILE_H
#define MICROPY_INCLUDED_SHARED_MODULE_AUDIOIO_MP3FILE_H

#include "supervisor/background_callback.h"
#include "extmod/vfs_fat.h"
#include "py/obj.h"

#include "shared-module/audiocore/__init__.h"

typedef struct {
    mp_obj_base_t base;
    struct _MP3DecInfo *decoder;
    background_callback_t inbuf_fill_cb;
    uint8_t *inbuf;
    uint32_t inbuf_length;
    uint32_t inbuf_offset;
    int16_t *buffers[2];
    uint32_t len;
    uint32_t frame_buffer_size;

    uint32_t sample_rate;
    pyb_file_obj_t *file;

    uint8_t buffer_index;
    uint8_t channel_count;
    bool eof;

    int8_t other_channel;
    int8_t other_buffer_index;
} audiomp3_mp3file_obj_t;

// These are not available from Python because it may be called in an interrupt.
void audiomp3_mp3file_reset_buffer(audiomp3_mp3file_obj_t *self,
    bool single_channel_output,
    uint8_t channel);
audioio_get_buffer_result_t audiomp3_mp3file_get_buffer(audiomp3_mp3file_obj_t *self,
    bool single_channel_output,
    uint8_t channel,
    uint8_t **buffer,
    uint32_t *buffer_length);                                                     // length in bytes
void audiomp3_mp3file_get_buffer_structure(audiomp3_mp3file_obj_t *self, bool single_channel_output,
    bool *single_buffer, bool *samples_signed,
    uint32_t *max_buffer_length, uint8_t *spacing);

float audiomp3_mp3file_get_rms_level(audiomp3_mp3file_obj_t *self);

#endif // MICROPY_INCLUDED_SHARED_MODULE_AUDIOIO_MP3FILE_H
