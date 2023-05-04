/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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

#include "shared-module/synthio/__init__.h"

typedef struct synthio_note_obj {
    mp_obj_base_t base;

    mp_float_t frequency;
    mp_float_t amplitude;
    mp_obj_t waveform_obj, envelope_obj;

    int32_t sample_rate;

    int32_t frequency_scaled;
    int32_t amplitude_scaled;
    synthio_lfo_descr_t tremolo_descr, vibrato_descr;
    synthio_lfo_state_t tremolo_state, vibrato_state;

    mp_buffer_info_t waveform_buf;
    synthio_envelope_definition_t envelope_def;
} synthio_note_obj_t;

void synthio_note_recalculate(synthio_note_obj_t *self, int32_t sample_rate);
uint32_t synthio_note_step(synthio_note_obj_t *self, int32_t sample_rate, int16_t dur, uint16_t *loudness);
void synthio_note_start(synthio_note_obj_t *self, int32_t sample_rate);
bool synthio_note_playing(synthio_note_obj_t *self);
uint32_t synthio_note_envelope(synthio_note_obj_t *self);
