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

mp_float_t common_hal_synthio_note_get_frequency(synthio_note_obj_t *self) {
    return self->frequency;
}

void common_hal_synthio_note_set_frequency(synthio_note_obj_t *self, mp_float_t value_in) {
    mp_float_t val = mp_arg_validate_float_range(value_in, 0, 32767, MP_QSTR_frequency);
    self->frequency = val;
    self->frequency_scaled = synthio_frequency_convert_float_to_scaled(val);
}

mp_obj_t common_hal_synthio_note_get_filter_obj(synthio_note_obj_t *self) {
    return self->filter_obj;
}

void common_hal_synthio_note_set_filter(synthio_note_obj_t *self, mp_obj_t filter_in) {
    synthio_biquad_filter_assign(&self->filter_state, filter_in);
    self->filter_obj = filter_in;
}

mp_float_t common_hal_synthio_note_get_ring_frequency(synthio_note_obj_t *self) {
    return self->ring_frequency;
}

void common_hal_synthio_note_set_ring_frequency(synthio_note_obj_t *self, mp_float_t value_in) {
    mp_float_t val = mp_arg_validate_float_range(value_in, 0, 32767, MP_QSTR_ring_frequency);
    self->ring_frequency = val;
    self->ring_frequency_scaled = synthio_frequency_convert_float_to_scaled(val);
}

mp_obj_t common_hal_synthio_note_get_panning(synthio_note_obj_t *self) {
    return self->panning.obj;
}

void common_hal_synthio_note_set_panning(synthio_note_obj_t *self, mp_obj_t value_in) {
    synthio_block_assign_slot(value_in, &self->panning, MP_QSTR_panning);
}

mp_obj_t common_hal_synthio_note_get_amplitude(synthio_note_obj_t *self) {
    return self->amplitude.obj;
}

void common_hal_synthio_note_set_amplitude(synthio_note_obj_t *self, mp_obj_t value_in) {
    synthio_block_assign_slot(value_in, &self->amplitude, MP_QSTR_amplitude);
}

mp_obj_t common_hal_synthio_note_get_bend(synthio_note_obj_t *self) {
    return self->bend.obj;
}

void common_hal_synthio_note_set_bend(synthio_note_obj_t *self, mp_obj_t value_in) {
    synthio_block_assign_slot(value_in, &self->bend, MP_QSTR_bend);
}

mp_obj_t common_hal_synthio_note_get_ring_bend(synthio_note_obj_t *self) {
    return self->ring_bend.obj;
}

void common_hal_synthio_note_set_ring_bend(synthio_note_obj_t *self, mp_obj_t value_in) {
    synthio_block_assign_slot(value_in, &self->ring_bend, MP_QSTR_ring_bend);
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
}

void synthio_note_start(synthio_note_obj_t *self, int32_t sample_rate) {
    synthio_note_recalculate(self, sample_rate);
    synthio_biquad_filter_reset(&self->filter_state);
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

STATIC uint32_t pitch_bend(uint32_t frequency_scaled, int32_t bend_value) {
    int octave = bend_value >> 15;
    bend_value &= 0x7fff;
    uint32_t bend_value_semitone = (uint32_t)bend_value * 24; // 65536/semitone
    uint32_t semitone = bend_value_semitone >> 16;
    uint32_t fractone = bend_value_semitone & 0xffff;
    uint32_t f_lo = pitch_bend_table[semitone];
    uint32_t f_hi = pitch_bend_table[semitone + 1]; // table has 13 entries, indexing with semitone=12 is OK
    uint32_t f = ((f_lo * (65535 - fractone) + f_hi * fractone) >> 16) + BEND_OFFSET;
    return (frequency_scaled * (uint64_t)f) >> (15 - octave);
}

#define ZERO MICROPY_FLOAT_CONST(0.)
#define ONE MICROPY_FLOAT_CONST(1.)
#define ALMOST_ONE (MICROPY_FLOAT_CONST(32767.) / 32768)

uint32_t synthio_note_step(synthio_note_obj_t *self, int32_t sample_rate, int16_t dur, uint16_t loudness[2]) {
    int panning = synthio_block_slot_get_scaled(&self->panning, -ALMOST_ONE, ALMOST_ONE);
    int left_panning_scaled, right_panning_scaled;
    if (panning >= 0) {
        left_panning_scaled = 32768;
        right_panning_scaled = 32767 - panning;
    } else {
        right_panning_scaled = 32768;
        left_panning_scaled = 32767 + panning;
    }

    int amplitude = synthio_block_slot_get_scaled(&self->amplitude, ZERO, ALMOST_ONE);
    left_panning_scaled = (left_panning_scaled * amplitude) >> 15;
    right_panning_scaled = (right_panning_scaled * amplitude) >> 15;
    loudness[0] = (loudness[0] * left_panning_scaled) >> 15;
    loudness[1] = (loudness[1] * right_panning_scaled) >> 15;

    if (self->ring_frequency_scaled != 0) {
        int ring_bend_value = synthio_block_slot_get_scaled(&self->ring_bend, -12, 12);
        self->ring_frequency_bent = pitch_bend(self->ring_frequency_scaled, ring_bend_value);
    }

    int bend_value = synthio_block_slot_get_scaled(&self->bend, -12, 12);
    uint32_t frequency_scaled = pitch_bend(self->frequency_scaled, bend_value);
    return frequency_scaled;

}
