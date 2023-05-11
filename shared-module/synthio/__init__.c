/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#include "shared-module/synthio/__init__.h"
#include "shared-bindings/synthio/__init__.h"
#include "shared-module/synthio/Note.h"
#include "py/runtime.h"
#include <math.h>
#include <stdlib.h>

STATIC const int16_t square_wave[] = {-32768, 32767};

STATIC const uint16_t notes[] = {8372, 8870, 9397, 9956, 10548, 11175, 11840,
                                 12544, 13290, 14080, 14917, 15804}; // 9th octave

STATIC int32_t round_float_to_int(mp_float_t f) {
    return (int32_t)(f + MICROPY_FLOAT_CONST(0.5));
}

STATIC int64_t round_float_to_int64(mp_float_t f) {
    return (int64_t)(f + MICROPY_FLOAT_CONST(0.5));
}

mp_float_t common_hal_synthio_midi_to_hz_float(mp_float_t arg) {
    return common_hal_synthio_onevo_to_hz_float(arg / 12.);
}

mp_float_t common_hal_synthio_onevo_to_hz_float(mp_float_t octave) {
    return notes[0] * MICROPY_FLOAT_C_FUN(pow)(2., octave - 10);
}

STATIC int16_t convert_time_to_rate(uint32_t sample_rate, mp_obj_t time_in, int16_t difference) {
    mp_float_t time = mp_obj_get_float(time_in);
    int num_samples = (int)MICROPY_FLOAT_C_FUN(round)(time * sample_rate);
    if (num_samples == 0) {
        return 32767;
    }
    int16_t result = MIN(32767, MAX(1, abs(difference * SYNTHIO_MAX_DUR) / num_samples));
    return (difference < 0) ? -result : result;
}

void synthio_envelope_definition_set(synthio_envelope_definition_t *envelope, mp_obj_t obj, uint32_t sample_rate) {
    if (obj == mp_const_none) {
        envelope->attack_level = 32767;
        envelope->sustain_level = 32767;
        envelope->attack_step = 32767;
        envelope->decay_step = -32767;
        envelope->release_step = -32767;
        return;
    }
    mp_arg_validate_type(obj, (mp_obj_type_t *)&synthio_envelope_type_obj, MP_QSTR_envelope);

    size_t len;
    mp_obj_t *fields;
    mp_obj_tuple_get(obj, &len, &fields);

    envelope->attack_level = (int)(32767 * mp_obj_get_float(fields[3]));
    envelope->sustain_level = (int)(32767 * mp_obj_get_float(fields[4]) * mp_obj_get_float(fields[3]));

    envelope->attack_step = convert_time_to_rate(
        sample_rate, fields[0], envelope->attack_level);

    envelope->decay_step = -convert_time_to_rate(
        sample_rate, fields[1], envelope->attack_level - envelope->sustain_level);

    envelope->release_step = -convert_time_to_rate(
        sample_rate, fields[2],
        envelope->sustain_level
            ? envelope->sustain_level
            : envelope->attack_level);
}

STATIC void synthio_envelope_state_step(synthio_envelope_state_t *state, synthio_envelope_definition_t *def, size_t n_steps) {
    state->substep += n_steps;
    while (state->substep >= SYNTHIO_MAX_DUR) {
        // max n_steps should be SYNTHIO_MAX_DUR so this loop executes at most
        // once
        state->substep -= SYNTHIO_MAX_DUR;
        switch (state->state) {
            case SYNTHIO_ENVELOPE_STATE_SUSTAIN:
                break;
            case SYNTHIO_ENVELOPE_STATE_ATTACK:
                state->level = MIN(state->level + def->attack_step, def->attack_level);
                if (state->level == def->attack_level) {
                    state->state = SYNTHIO_ENVELOPE_STATE_DECAY;
                }
                break;
            case SYNTHIO_ENVELOPE_STATE_DECAY:
                state->level = MAX(state->level + def->decay_step, def->sustain_level);
                if (state->level == def->sustain_level) {
                    state->state = SYNTHIO_ENVELOPE_STATE_SUSTAIN;
                }
                break;
            case SYNTHIO_ENVELOPE_STATE_RELEASE:
                state->level = MAX(state->level + def->release_step, 0);
        }
    }
}

STATIC void synthio_envelope_state_init(synthio_envelope_state_t *state, synthio_envelope_definition_t *def) {
    state->level = 0;
    state->substep = 0;
    state->state = SYNTHIO_ENVELOPE_STATE_ATTACK;

    synthio_envelope_state_step(state, def, SYNTHIO_MAX_DUR);
}

STATIC void synthio_envelope_state_release(synthio_envelope_state_t *state, synthio_envelope_definition_t *def) {
    state->state = SYNTHIO_ENVELOPE_STATE_RELEASE;
}

STATIC synthio_envelope_definition_t *synthio_synth_get_note_envelope(synthio_synth_t *synth, mp_obj_t note_obj) {
    synthio_envelope_definition_t *def = &synth->global_envelope_definition;
    if (!mp_obj_is_small_int(note_obj)) {
        synthio_note_obj_t *note = MP_OBJ_TO_PTR(note_obj);
        if (note->envelope_obj != mp_const_none) {
            def = &note->envelope_def;
        }
    }
    return def;
}


#define RANGE_LOW (-28000)
#define RANGE_HIGH (28000)
#define RANGE_SHIFT (16)
#define RANGE_SCALE (0xfffffff / (32768 * CIRCUITPY_SYNTHIO_MAX_CHANNELS - RANGE_HIGH))

// dynamic range compression via a downward compressor with hard knee
//
// When the output value is within the range +-28000 (about 85% of full scale),
// it is unchanged. Otherwise, it undergoes a gain reduction so that the
// largest possible values, (+32768,-32767) * CIRCUITPY_SYNTHIO_MAX_CHANNELS,
// still fit within the output range
//
// This produces a much louder overall volume with multiple voices, without
// much additional processing.
//
// https://en.wikipedia.org/wiki/Dynamic_range_compression
STATIC
int16_t mix_down_sample(int32_t sample) {
    if (sample < RANGE_LOW) {
        sample = (((sample - RANGE_LOW) * RANGE_SCALE) >> RANGE_SHIFT) + RANGE_LOW;
    } else if (sample > RANGE_HIGH) {
        sample = (((sample - RANGE_HIGH) * RANGE_SCALE) >> RANGE_SHIFT) + RANGE_HIGH;
    }
    return sample;
}

void synthio_synth_synthesize(synthio_synth_t *synth, uint8_t **bufptr, uint32_t *buffer_length, uint8_t channel) {

    if (channel == synth->other_channel) {
        *buffer_length = synth->last_buffer_length;
        *bufptr = (uint8_t *)(synth->buffers[synth->other_buffer_index] + channel);
        return;
    }

    synth->buffer_index = !synth->buffer_index;
    synth->other_channel = 1 - channel;
    synth->other_buffer_index = synth->buffer_index;

    uint16_t dur = MIN(SYNTHIO_MAX_DUR, synth->span.dur);
    synth->span.dur -= dur;

    int32_t sample_rate = synth->sample_rate;
    int32_t out_buffer32[dur * synth->channel_count];

    memset(out_buffer32, 0, sizeof(out_buffer32));
    for (int chan = 0; chan < CIRCUITPY_SYNTHIO_MAX_CHANNELS; chan++) {
        mp_obj_t note_obj = synth->span.note_obj[chan];
        if (note_obj == SYNTHIO_SILENCE) {
            synth->accum[chan] = 0;
            continue;
        }

        if (synth->envelope_state[chan].level == 0) {
            // note is truly finished, but we only just noticed
            synth->span.note_obj[chan] = SYNTHIO_SILENCE;
            continue;
        }

        // adjust loudness by envelope
        uint16_t loudness[2] = {synth->envelope_state[chan].level,synth->envelope_state[chan].level};

        uint32_t dds_rate;
        const int16_t *waveform = synth->waveform;
        uint32_t waveform_length = synth->waveform_length;

        uint32_t ring_dds_rate = 0;
        const int16_t *ring_waveform = NULL;
        uint32_t ring_waveform_length = 0;

        if (mp_obj_is_small_int(note_obj)) {
            uint8_t note = mp_obj_get_int(note_obj);
            uint8_t octave = note / 12;
            uint16_t base_freq = notes[note % 12];
            // rate = base_freq * waveform_length
            // den = sample_rate * 2 ^ (10 - octave)
            // den = sample_rate * 2 ^ 10 / 2^octave
            // dds_rate = 2^SHIFT * rate / den
            // dds_rate = 2^(SHIFT-10+octave) * base_freq * waveform_length / sample_rate
            dds_rate = (sample_rate / 2 + ((uint64_t)(base_freq * waveform_length) << (SYNTHIO_FREQUENCY_SHIFT - 10 + octave))) / sample_rate;
        } else {
            synthio_note_obj_t *note = MP_OBJ_TO_PTR(note_obj);
            int32_t frequency_scaled = synthio_note_step(note, sample_rate, dur, loudness);
            if (note->waveform_buf.buf) {
                waveform = note->waveform_buf.buf;
                waveform_length = note->waveform_buf.len / 2;
            }
            dds_rate = synthio_frequency_convert_scaled_to_dds((uint64_t)frequency_scaled * waveform_length, sample_rate);
            if (note->ring_frequency_scaled != 0 && note->ring_waveform_buf.buf) {
                ring_waveform = note->ring_waveform_buf.buf;
                ring_waveform_length = note->ring_waveform_buf.len / 2;
                ring_dds_rate = synthio_frequency_convert_scaled_to_dds((uint64_t)note->ring_frequency_scaled * ring_waveform_length, sample_rate);
                uint32_t lim = ring_waveform_length << SYNTHIO_FREQUENCY_SHIFT;
                if (ring_dds_rate > lim / 2) {
                    ring_dds_rate = 0; // can't ring at that frequency
                }
            }
        }

        int synth_chan = synth->channel_count;
        if (ring_dds_rate) {
            uint32_t lim = waveform_length << SYNTHIO_FREQUENCY_SHIFT;
            uint32_t accum = synth->accum[chan];

            if (dds_rate > lim / 2) {
                // beyond nyquist, can't play note
                continue;
            }

            // can happen if note waveform gets set mid-note, but the expensive modulo is usually avoided
            if (accum > lim) {
                accum %= lim;
            }

            int32_t ring_buffer[dur];
            // first, fill with waveform
            for (uint16_t i = 0; i < dur; i++) {
                accum += dds_rate;
                // because dds_rate is low enough, the subtraction is guaranteed to go back into range, no expensive modulo needed
                if (accum > lim) {
                    accum -= lim;
                }
                int16_t idx = accum >> SYNTHIO_FREQUENCY_SHIFT;
                ring_buffer[i] = waveform[idx];
            }
            synth->accum[chan] = accum;

            // now modulate by ring and accumulate
            accum = synth->ring_accum[chan];
            lim = ring_waveform_length << SYNTHIO_FREQUENCY_SHIFT;

            // can happen if note waveform gets set mid-note, but the expensive modulo is usually avoided
            if (accum > lim) {
                accum %= lim;
            }

            for (uint16_t i = 0, j = 0; i < dur; i++) {
                accum += ring_dds_rate;
                // because dds_rate is low enough, the subtraction is guaranteed to go back into range, no expensive modulo needed
                if (accum > lim) {
                    accum -= lim;
                }
                int16_t idx = accum >> SYNTHIO_FREQUENCY_SHIFT;
                int16_t wi = (ring_waveform[idx] * ring_buffer[i]) / 32768;
                for (int c = 0; c < synth_chan; c++) {
                    out_buffer32[j] += (wi * loudness[c]) / 32768;
                    j++;
                }
            }
            synth->ring_accum[chan] = accum;
        } else {
            uint32_t lim = waveform_length << SYNTHIO_FREQUENCY_SHIFT;
            uint32_t accum = synth->accum[chan];

            if (dds_rate > lim / 2) {
                // beyond nyquist, can't play note
                continue;
            }

            // can happen if note waveform gets set mid-note, but the expensive modulo is usually avoided
            if (accum > lim) {
                accum %= lim;
            }

            for (uint16_t i = 0, j = 0; i < dur; i++) {
                accum += dds_rate;
                // because dds_rate is low enough, the subtraction is guaranteed to go back into range, no expensive modulo needed
                if (accum > lim) {
                    accum -= lim;
                }
                int16_t idx = accum >> SYNTHIO_FREQUENCY_SHIFT;
                int16_t wi = waveform[idx];
                for (int c = 0; c < synth_chan; c++) {
                    out_buffer32[j] += (wi * loudness[c]) / 65536;
                    j++;
                }
            }
            synth->accum[chan] = accum;
        }
    }

    int16_t *out_buffer16 = (int16_t *)(void *)synth->buffers[synth->buffer_index];

    // mix down audio
    for (size_t i = 0; i < MP_ARRAY_SIZE(out_buffer32); i++) {
        int32_t sample = out_buffer32[i];
        out_buffer16[i] = mix_down_sample(sample);
    }

    // advance envelope states
    for (int chan = 0; chan < CIRCUITPY_SYNTHIO_MAX_CHANNELS; chan++) {
        mp_obj_t note_obj = synth->span.note_obj[chan];
        if (note_obj == SYNTHIO_SILENCE) {
            continue;
        }
        synthio_envelope_state_step(&synth->envelope_state[chan], synthio_synth_get_note_envelope(synth, note_obj), dur);
    }

    *buffer_length = synth->last_buffer_length = dur * SYNTHIO_BYTES_PER_SAMPLE * synth->channel_count;
    *bufptr = (uint8_t *)out_buffer16;
}

void synthio_synth_reset_buffer(synthio_synth_t *synth, bool single_channel_output, uint8_t channel) {
    if (single_channel_output && channel == 1) {
        return;
    }
    synth->other_channel = -1;
}

bool synthio_synth_deinited(synthio_synth_t *synth) {
    return synth->buffers[0] == NULL;
}

void synthio_synth_deinit(synthio_synth_t *synth) {
    m_del(uint8_t, synth->buffers[0], synth->buffer_length);
    m_del(uint8_t, synth->buffers[1], synth->buffer_length);
    synth->buffers[0] = NULL;
    synth->buffers[1] = NULL;
}

void synthio_synth_envelope_set(synthio_synth_t *synth, mp_obj_t envelope_obj) {
    synthio_envelope_definition_set(&synth->global_envelope_definition, envelope_obj, synth->sample_rate);
    synth->envelope_obj = envelope_obj;
}

mp_obj_t synthio_synth_envelope_get(synthio_synth_t *synth) {
    return synth->envelope_obj;
}

void synthio_synth_init(synthio_synth_t *synth, uint32_t sample_rate, int channel_count, const int16_t *waveform, uint16_t waveform_length, mp_obj_t envelope_obj) {
    mp_arg_validate_int_range(channel_count, 1, 2, MP_QSTR_channel_count);
    synth->buffer_length = SYNTHIO_MAX_DUR * SYNTHIO_BYTES_PER_SAMPLE * channel_count;
    synth->buffers[0] = m_malloc(synth->buffer_length, false);
    synth->buffers[1] = m_malloc(synth->buffer_length, false);
    synth->channel_count = channel_count;
    synth->other_channel = -1;
    synth->waveform = waveform;
    synth->waveform_length = waveform_length;
    synth->sample_rate = sample_rate;
    synthio_synth_envelope_set(synth, envelope_obj);

    for (size_t i = 0; i < CIRCUITPY_SYNTHIO_MAX_CHANNELS; i++) {
        synth->span.note_obj[i] = SYNTHIO_SILENCE;
    }
}

void synthio_synth_get_buffer_structure(synthio_synth_t *synth, bool single_channel_output,
    bool *single_buffer, bool *samples_signed, uint32_t *max_buffer_length, uint8_t *spacing) {
    *single_buffer = false;
    *samples_signed = true;
    *max_buffer_length = synth->buffer_length;
    if (single_channel_output) {
        *spacing = synth->channel_count;
    } else {
        *spacing = 1;
    }
}

STATIC bool parse_common(mp_buffer_info_t *bufinfo, mp_obj_t o, int16_t what) {
    if (o != mp_const_none) {
        mp_get_buffer_raise(o, bufinfo, MP_BUFFER_READ);
        if (bufinfo->typecode != 'h') {
            mp_raise_ValueError_varg(translate("%q must be array of type 'h'"), what);
        }
        mp_arg_validate_length_range(bufinfo->len / 2, 2, 1024, what);
        return true;
    }
    return false;
}

void synthio_synth_parse_waveform(mp_buffer_info_t *bufinfo_waveform, mp_obj_t waveform_obj) {
    *bufinfo_waveform = ((mp_buffer_info_t) { .buf = (void *)square_wave, .len = 4 });
    parse_common(bufinfo_waveform, waveform_obj, MP_QSTR_waveform);
}

STATIC int find_channel_with_note(synthio_synth_t *synth, mp_obj_t note) {
    for (int i = 0; i < CIRCUITPY_SYNTHIO_MAX_CHANNELS; i++) {
        if (synth->span.note_obj[i] == note) {
            return i;
        }
    }
    int result = -1;
    if (note == SYNTHIO_SILENCE) {
        // replace the releasing note with lowest volume level
        int level = 32768;
        for (int chan = 0; chan < CIRCUITPY_SYNTHIO_MAX_CHANNELS; chan++) {
            if (!SYNTHIO_NOTE_IS_PLAYING(synth, chan)) {
                synthio_envelope_state_t *state = &synth->envelope_state[chan];
                if (state->level < level) {
                    result = chan;
                    level = state->level;
                }
            }
        }
    }
    return result;
}

bool synthio_span_change_note(synthio_synth_t *synth, mp_obj_t old_note, mp_obj_t new_note) {
    int channel;
    if (new_note != SYNTHIO_SILENCE && (channel = find_channel_with_note(synth, new_note)) != -1) {
        // note already playing, re-enter attack phase
        synth->envelope_state[channel].state = SYNTHIO_ENVELOPE_STATE_ATTACK;
        return true;
    }
    channel = find_channel_with_note(synth, old_note);
    if (channel != -1) {
        if (new_note == SYNTHIO_SILENCE) {
            synthio_envelope_state_release(&synth->envelope_state[channel], synthio_synth_get_note_envelope(synth, old_note));
        } else {
            synth->span.note_obj[channel] = new_note;
            synthio_envelope_state_init(&synth->envelope_state[channel], synthio_synth_get_note_envelope(synth, new_note));
            synth->accum[channel] = 0;
        }
        return true;
    }
    return false;
}

uint64_t synthio_frequency_convert_float_to_scaled(mp_float_t val) {
    return round_float_to_int64(val * (1 << SYNTHIO_FREQUENCY_SHIFT));
}

uint32_t synthio_frequency_convert_float_to_dds(mp_float_t frequency_hz, int32_t sample_rate) {
    return synthio_frequency_convert_scaled_to_dds(synthio_frequency_convert_float_to_scaled(frequency_hz), sample_rate);
}

uint32_t synthio_frequency_convert_scaled_to_dds(uint64_t frequency_scaled, int32_t sample_rate) {
    return (sample_rate / 2 + frequency_scaled) / sample_rate;
}

void synthio_lfo_set(synthio_lfo_state_t *state, const synthio_lfo_descr_t *descr, uint32_t sample_rate) {
    state->amplitude_scaled = round_float_to_int(descr->amplitude * 32768);
    state->dds = synthio_frequency_convert_float_to_dds(descr->frequency * 65536, sample_rate);
}

STATIC int synthio_lfo_step_common(synthio_lfo_state_t *state, uint16_t dur) {
    uint32_t phase = state->phase;
    uint16_t whole_phase = phase >> 16;

    // advance the phase accumulator
    state->phase = phase + state->dds * dur;

    return whole_phase;
}
STATIC int synthio_lfo_sweep_common(synthio_lfo_state_t *state, uint16_t dur) {
    uint16_t whole_phase = synthio_lfo_step_common(state, dur);
    if (state->phase < state->dds) {
        state->phase = 0xffffffff;
    }
    return whole_phase;
}

int synthio_sweep_step(synthio_lfo_state_t *state, uint16_t dur) {
    uint16_t whole_phase = synthio_lfo_sweep_common(state, dur);
    return (state->amplitude_scaled * whole_phase) / 65536 + state->offset_scaled;
}

int synthio_sweep_in_step(synthio_lfo_state_t *state, uint16_t dur) {
    uint16_t whole_phase = 65535 - synthio_lfo_sweep_common(state, dur);
    return (state->amplitude_scaled * whole_phase) / 65536 + state->offset_scaled;
}

int synthio_lfo_step(synthio_lfo_state_t *state, uint16_t dur) {
    uint16_t whole_phase = synthio_lfo_step_common(state, dur);
    // create a triangle wave, it's quick and easy
    int v;
    if (whole_phase < 16384) { // ramp from 0 to amplitude
        v = (state->amplitude_scaled * whole_phase);
    } else if (whole_phase < 49152) { // ramp from +amplitude to -amplitude
        v = (state->amplitude_scaled * (32768 - whole_phase));
    } else { // from -amplitude to 0
        v = (state->amplitude_scaled * (whole_phase - 65536));
    }
    return v / 16384 + state->offset_scaled;
}
