// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft
// SPDX-FileCopyrightText: Copyright (c) 2019 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "supervisor/background_callback.h"
#include "extmod/vfs_fat.h"
#include "py/obj.h"

#include "shared-module/audiocore/__init__.h"

typedef struct {
    mp_obj_base_t base;
    struct _MP3DecInfo *decoder;
    background_callback_t inbuf_fill_cb;
    uint8_t *inbuf;
    int32_t inbuf_length;
    int32_t inbuf_offset;
    int16_t *buffers[2];
    uint32_t len;
    uint32_t frame_buffer_size;

    uint32_t sample_rate;
    mp_obj_t stream;

    uint8_t buffer_index;
    uint8_t channel_count;
    bool eof;

    int8_t other_channel;
    int8_t other_buffer_index;

    uint32_t samples_decoded;
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

uint32_t common_hal_audiomp3_mp3file_get_samples_decoded(audiomp3_mp3file_obj_t *self);
