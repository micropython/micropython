/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Artyom Skrobov
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

#include "py/obj.h"

typedef struct synthio_note_obj synthio_note_obj_t;
extern const mp_obj_type_t synthio_note_type;
typedef enum synthio_bend_mode_e synthio_bend_mode_t;

mp_float_t common_hal_synthio_note_get_frequency(synthio_note_obj_t *self);
void common_hal_synthio_note_set_frequency(synthio_note_obj_t *self, mp_float_t value);

mp_obj_t common_hal_synthio_note_get_filter_obj(synthio_note_obj_t *self);
void common_hal_synthio_note_set_filter(synthio_note_obj_t *self, mp_obj_t biquad);

mp_obj_t common_hal_synthio_note_get_panning(synthio_note_obj_t *self);
void common_hal_synthio_note_set_panning(synthio_note_obj_t *self, mp_obj_t value);

mp_obj_t common_hal_synthio_note_get_amplitude(synthio_note_obj_t *self);
void common_hal_synthio_note_set_amplitude(synthio_note_obj_t *self, mp_obj_t value);

mp_obj_t common_hal_synthio_note_get_bend(synthio_note_obj_t *self);
void common_hal_synthio_note_set_bend(synthio_note_obj_t *self, mp_obj_t value);

mp_obj_t common_hal_synthio_note_get_waveform_obj(synthio_note_obj_t *self);
void common_hal_synthio_note_set_waveform(synthio_note_obj_t *self, mp_obj_t value);

mp_int_t common_hal_synthio_note_get_waveform_loop_start(synthio_note_obj_t *self);
void common_hal_synthio_note_set_waveform_loop_start(synthio_note_obj_t *self, mp_int_t value_in);

mp_int_t common_hal_synthio_note_get_waveform_loop_end(synthio_note_obj_t *self);
void common_hal_synthio_note_set_waveform_loop_end(synthio_note_obj_t *self, mp_int_t value_in);

mp_float_t common_hal_synthio_note_get_ring_frequency(synthio_note_obj_t *self);
void common_hal_synthio_note_set_ring_frequency(synthio_note_obj_t *self, mp_float_t value);

mp_obj_t common_hal_synthio_note_get_ring_bend(synthio_note_obj_t *self);
void common_hal_synthio_note_set_ring_bend(synthio_note_obj_t *self, mp_obj_t value);

mp_obj_t common_hal_synthio_note_get_ring_waveform_obj(synthio_note_obj_t *self);
void common_hal_synthio_note_set_ring_waveform(synthio_note_obj_t *self, mp_obj_t value);

mp_int_t common_hal_synthio_note_get_ring_waveform_loop_start(synthio_note_obj_t *self);
void common_hal_synthio_note_set_ring_waveform_loop_start(synthio_note_obj_t *self, mp_int_t value_in);

mp_int_t common_hal_synthio_note_get_ring_waveform_loop_end(synthio_note_obj_t *self);
void common_hal_synthio_note_set_ring_waveform_loop_end(synthio_note_obj_t *self, mp_int_t value_in);

mp_obj_t common_hal_synthio_note_get_envelope_obj(synthio_note_obj_t *self);
void common_hal_synthio_note_set_envelope(synthio_note_obj_t *self, mp_obj_t value);
