// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT
#pragma once

#include "py/obj.h"

#include "shared-module/audiocore/__init__.h"

extern const mp_obj_type_t audiotap_tap_type;

typedef struct {
    audiosample_base_t base;

    // Played while there is no audio to pass through. Allocated, like the
    // ring, when the first sample is played, which also sets the format.
    int8_t *silence;
    uint32_t silence_len; // in bytes

    // The part of the source's buffer not passed through yet. It stays the
    // source's until the source is asked for its next buffer.
    uint8_t *sample_remaining_buffer;
    uint32_t sample_buffer_length; // in samples

    bool loop;
    bool more_data;

    mp_obj_t sample;

    // A copy of the audio that passed through, for readinto(). Whatever plays
    // the tap writes it, from a background task or an interrupt, and the VM
    // reads it, so each side moves only its own position. Both positions run
    // from 0 to 2 * capacity, which tells a full ring from an empty one.
    mp_float_t capacity_seconds;
    uint8_t *ring;
    uint32_t capacity; // in samples, a whole number of frames
    volatile uint32_t write_pos;
    volatile uint32_t read_pos;
    // Audio was dropped because the ring was full. Cleared when read.
    volatile bool overflow;

    // Levels of the audio since each was last read. Reading one asks for its
    // reset, and the writer does it, so that only the writer changes them.
    volatile uint32_t peak; // largest magnitude, 0 to 32768
    volatile bool peak_reset;
    volatile float sum_squares; // of samples scaled to -1.0 to 1.0
    volatile uint32_t level_count; // samples summed
    volatile bool rms_reset;
} audiotap_tap_obj_t;

void audiotap_tap_reset_buffer(audiotap_tap_obj_t *self,
    bool single_channel_output,
    uint8_t channel);

audioio_get_buffer_result_t audiotap_tap_get_buffer(audiotap_tap_obj_t *self,
    bool single_channel_output,
    uint8_t channel,
    uint8_t **buffer,
    uint32_t *buffer_length);  // length in bytes
