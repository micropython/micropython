/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include <math.h>
#include "py/runtime.h"
#include "shared-module/synthio/Note.h"
#include "shared-bindings/synthio/Note.h"
#include "shared-bindings/synthio/__init__.h"

static int32_t round_float_to_int(mp_float_t f) {
    return (int32_t)(f + MICROPY_FLOAT_CONST(0.5));
}

mp_float_t common_hal_synthio_note_get_frequency(synthio_note_obj_t *self) {
    return self->frequency;
}

void common_hal_synthio_note_set_frequency(synthio_note_obj_t *self, mp_float_t value_in) {
    mp_float_t val = mp_arg_validate_float_range(value_in, 0, 32767, MP_QSTR_frequency);
    self->frequency = val;
    self->frequency_scaled = synthio_frequency_convert_float_to_scaled(val);
}

bool common_hal_synthio_note_get_filter(synthio_note_obj_t *self) {
    return self->filter;
}

void common_hal_synthio_note_set_filter(synthio_note_obj_t *self, bool value_in) {
    self->filter = value_in;
}

mp_float_t common_hal_synthio_note_get_ring_frequency(synthio_note_obj_t *self) {
    return self->ring_frequency;
}

void common_hal_synthio_note_set_ring_frequency(synthio_note_obj_t *self, mp_float_t value_in) {
    mp_float_t val = mp_arg_validate_float_range(value_in, 0, 32767, MP_QSTR_ring_frequency);
    self->ring_frequency = val;
    self->ring_frequency_scaled = synthio_frequency_convert_float_to_scaled(val);
}

mp_float_t common_hal_synthio_note_get_panning(synthio_note_obj_t *self) {
    return self->panning;
}

void common_hal_synthio_note_set_panning(synthio_note_obj_t *self, mp_float_t value_in) {
    mp_float_t val = mp_arg_validate_float_range(value_in, -1, 1, MP_QSTR_panning);
    self->panning = val;
    if (val >= 0) {
        self->left_panning_scaled = 32768;
        self->right_panning_scaled = 32768 - round_float_to_int(val * 32768);
    } else {
        self->right_panning_scaled = 32768;
        self->left_panning_scaled = 32768 + round_float_to_int(val * 32768);
    }
}

mp_float_t common_hal_synthio_note_get_tremolo_depth(synthio_note_obj_t *self) {
    return self->tremolo_descr.amplitude;
}

void common_hal_synthio_note_set_tremolo_depth(synthio_note_obj_t *self, mp_float_t value_in) {
    mp_float_t val = mp_arg_validate_float_range(value_in, 0, 1, MP_QSTR_tremolo_depth);
    self->tremolo_descr.amplitude = val;
    self->tremolo_state.amplitude_scaled = round_float_to_int(val * 32767);
}

mp_float_t common_hal_synthio_note_get_tremolo_rate(synthio_note_obj_t *self) {
    return self->tremolo_descr.frequency;
}

void common_hal_synthio_note_set_tremolo_rate(synthio_note_obj_t *self, mp_float_t value_in) {
    mp_float_t val = mp_arg_validate_float_range(value_in, 0, 60, MP_QSTR_tremolo_rate);
    self->tremolo_descr.frequency = val;
    if (self->sample_rate != 0) {
        self->tremolo_state.dds = synthio_frequency_convert_float_to_dds(val, self->sample_rate);
    }
}

mp_float_t common_hal_synthio_note_get_bend_depth(synthio_note_obj_t *self) {
    return self->bend_descr.amplitude;
}

void common_hal_synthio_note_set_bend_depth(synthio_note_obj_t *self, mp_float_t value_in) {
    mp_float_t val = mp_arg_validate_float_range(value_in, -1, 1, MP_QSTR_bend_depth);
    self->bend_descr.amplitude = val;
    self->bend_state.amplitude_scaled = round_float_to_int(val * 32767);
}

mp_float_t common_hal_synthio_note_get_bend_rate(synthio_note_obj_t *self) {
    return self->bend_descr.frequency;
}

synthio_bend_mode_t common_hal_synthio_note_get_bend_mode(synthio_note_obj_t *self) {
    return self->bend_mode;
}

void common_hal_synthio_note_set_bend_mode(synthio_note_obj_t *self, synthio_bend_mode_t value) {
    self->bend_mode = value;
}


void common_hal_synthio_note_set_bend_rate(synthio_note_obj_t *self, mp_float_t value_in) {
    mp_float_t val = mp_arg_validate_float_range(value_in, 0, 60, MP_QSTR_bend_rate);
    self->bend_descr.frequency = val;
    if (self->sample_rate != 0) {
        self->bend_state.dds = synthio_frequency_convert_float_to_dds(val, self->sample_rate);
    }
}

mp_obj_t common_hal_synthio_note_get_envelope_obj(synthio_note_obj_t *self) {
    return self->envelope_obj;
}

void common_hal_synthio_note_set_envelope(synthio_note_obj_t *self, mp_obj_t envelope_in) {
    if (envelope_in != mp_const_none) {
        mp_arg_validate_type(envelope_in, (mp_obj_type_t *)&synthio_envelope_type_obj, MP_QSTR_envelope);
        if (self->sample_rate != 0) {
            synthio_envelope_definition_set(&self->envelope_def, envelope_in, self->sample_rate);
        }
    }
    self->envelope_obj = envelope_in;
}

mp_obj_t common_hal_synthio_note_get_waveform_obj(synthio_note_obj_t *self) {
    return self->waveform_obj;
}

void common_hal_synthio_note_set_waveform(synthio_note_obj_t *self, mp_obj_t waveform_in) {
    if (waveform_in == mp_const_none) {
        memset(&self->waveform_buf, 0, sizeof(self->waveform_buf));
    } else {
        mp_buffer_info_t bufinfo_waveform;
        synthio_synth_parse_waveform(&bufinfo_waveform, waveform_in);
        self->waveform_buf = bufinfo_waveform;
    }
    self->waveform_obj = waveform_in;
}

mp_obj_t common_hal_synthio_note_get_ring_waveform_obj(synthio_note_obj_t *self) {
    return self->ring_waveform_obj;
}

void common_hal_synthio_note_set_ring_waveform(synthio_note_obj_t *self, mp_obj_t ring_waveform_in) {
    if (ring_waveform_in == mp_const_none) {
        memset(&self->ring_waveform_buf, 0, sizeof(self->ring_waveform_buf));
    } else {
        mp_buffer_info_t bufinfo_ring_waveform;
        synthio_synth_parse_waveform(&bufinfo_ring_waveform, ring_waveform_in);
        self->ring_waveform_buf = bufinfo_ring_waveform;
    }
    self->ring_waveform_obj = ring_waveform_in;
}

void synthio_note_recalculate(synthio_note_obj_t *self, int32_t sample_rate) {
    if (sample_rate == self->sample_rate) {
        return;
    }
    self->sample_rate = sample_rate;

    if (self->envelope_obj != mp_const_none) {
        synthio_envelope_definition_set(&self->envelope_def, self->envelope_obj, sample_rate);
    }

    synthio_lfo_set(&self->tremolo_state, &self->tremolo_descr, sample_rate);
    self->tremolo_state.offset_scaled = 32768 - self->tremolo_state.amplitude_scaled;
    synthio_lfo_set(&self->bend_state, &self->bend_descr, sample_rate);
    self->bend_state.offset_scaled = 32768;
}

void synthio_note_start(synthio_note_obj_t *self, int32_t sample_rate) {
    synthio_note_recalculate(self, sample_rate);
    if (self->bend_mode != SYNTHIO_BEND_MODE_VIBRATO) {
        self->bend_state.phase = 0;
    }
}

uint32_t synthio_note_envelope(synthio_note_obj_t *self) {
    return self->amplitude_scaled;
}

// Perform a pitch bend operation
//
// bend_value is in the range [0, 65535]. "no change" is 32768. The bend unit is 32768/octave.
//
// compare to (frequency_scaled * pow(2, (bend_value-32768)/32768))
// a 13-entry pitch table
#define BEND_SCALE (32768)
#define BEND_OFFSET (BEND_SCALE)

STATIC uint16_t pitch_bend_table[] = { 0, 1948, 4013, 6200, 8517, 10972, 13573, 16329, 19248, 22341, 25618, 29090, 32768 };

STATIC uint32_t pitch_bend(uint32_t frequency_scaled, uint16_t bend_value) {
    bool down = (bend_value < 32768);
    if (!down) {
        bend_value -= 32768;
    }
    uint32_t bend_value_semitone = (uint32_t)bend_value * 24; // 65536/semitone
    uint32_t semitone = bend_value_semitone >> 16;
    uint32_t fractone = bend_value_semitone & 0xffff;
    uint32_t f_lo = pitch_bend_table[semitone];
    uint32_t f_hi = pitch_bend_table[semitone + 1]; // table has 13 entries, indexing with semitone=12 is OK
    uint32_t f = ((f_lo * (65535 - fractone) + f_hi * fractone) >> 16) + BEND_OFFSET;
    return (frequency_scaled * (uint64_t)f) >> (15 + down);
}

STATIC int synthio_bend_value(synthio_note_obj_t *self, int16_t dur) {
    switch (self->bend_mode) {
        case SYNTHIO_BEND_MODE_STATIC:
            return self->bend_state.amplitude_scaled + self->bend_state.offset_scaled;
        case SYNTHIO_BEND_MODE_VIBRATO:
            return synthio_lfo_step(&self->bend_state, dur);
        case SYNTHIO_BEND_MODE_SWEEP:
            return synthio_sweep_step(&self->bend_state, dur);
        case SYNTHIO_BEND_MODE_SWEEP_IN:
            return synthio_sweep_in_step(&self->bend_state, dur);
        default:
            return 32768;
    }
}

uint32_t synthio_note_step(synthio_note_obj_t *self, int32_t sample_rate, int16_t dur, uint16_t loudness[2]) {
    int tremolo_value = synthio_lfo_step(&self->tremolo_state, dur);
    loudness[0] = (((loudness[0] * tremolo_value) >> 15) * self->left_panning_scaled) >> 15;
    loudness[1] = (((loudness[1] * tremolo_value) >> 15) * self->right_panning_scaled) >> 15;
    int bend_value = synthio_bend_value(self, dur);
    uint32_t frequency_scaled = pitch_bend(self->frequency_scaled, bend_value);
    return frequency_scaled;
}
