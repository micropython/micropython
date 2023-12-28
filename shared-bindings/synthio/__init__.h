/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Artyom Skrobov
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

#include "py/objnamedtuple.h"
#include "py/enum.h"

#define SYNTHIO_WAVEFORM_SIZE 16384

typedef enum {
    SYNTHIO_ENVELOPE_STATE_ATTACK, SYNTHIO_ENVELOPE_STATE_DECAY,
    SYNTHIO_ENVELOPE_STATE_SUSTAIN, SYNTHIO_ENVELOPE_STATE_RELEASE
} envelope_state_e;

typedef enum synthio_bend_mode_e {
    SYNTHIO_BEND_MODE_STATIC, SYNTHIO_BEND_MODE_VIBRATO, SYNTHIO_BEND_MODE_SWEEP, SYNTHIO_BEND_MODE_SWEEP_IN
} synthio_bend_mode_t;

extern const mp_obj_type_t synthio_note_state_type;
extern const cp_enum_obj_t bend_mode_VIBRATO_obj;
extern const mp_obj_type_t synthio_bend_mode_type;
typedef struct synthio_synth synthio_synth_t;
extern int16_t shared_bindings_synthio_square_wave[];
extern const mp_obj_namedtuple_type_t synthio_envelope_type_obj;
void synthio_synth_envelope_set(synthio_synth_t *synth, mp_obj_t envelope_obj);
mp_obj_t synthio_synth_envelope_get(synthio_synth_t *synth);
mp_float_t common_hal_synthio_midi_to_hz_float(mp_float_t note);
mp_float_t common_hal_synthio_voct_to_hz_float(mp_float_t note);
