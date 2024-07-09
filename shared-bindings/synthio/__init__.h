// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Artyom Skrobov
//
// SPDX-License-Identifier: MIT

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
