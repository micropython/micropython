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
#include "py/runtime.h"
#include <math.h>
#include <stdlib.h>

STATIC const int16_t square_wave[] = {-32768, 32767};

STATIC const uint16_t notes[] = {8372, 8870, 9397, 9956, 10548, 11175, 11840,
                                 12544, 13290, 14080, 14917, 15804}; // 9th octave

STATIC int16_t convert_time_to_rate(uint32_t sample_rate, mp_obj_t time_in, int16_t difference) {
    mp_float_t time = mp_obj_get_float(time_in);
    int num_samples = (int)MICROPY_FLOAT_C_FUN(round)(time * sample_rate);
    if (num_samples == 0) {
        return 0;
    }
    int16_t result = MIN(32767, MAX(1, abs(difference * SYNTHIO_MAX_DUR) / num_samples));
    return (difference < 0) ? -result : result;
}

STATIC void synthio_envelope_definition_set(synthio_envelope_definition_t *envelope, mp_obj_t obj, uint32_t sample_rate) {
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
    envelope->sustain_level = (int)(32767 * mp_obj_get_float(fields[4]));

    envelope->attack_step = convert_time_to_rate(
        sample_rate, fields[0], envelope->attack_level);

    envelope->decay_step = -convert_time_to_rate(
        sample_rate, fields[1], envelope->attack_level - envelope->sustain_level);

    envelope->release_step = -convert_time_to_rate(
        sample_rate, fields[2],
        envelope->decay_step
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
                if (def->attack_step != 0) {
                    state->level = MIN(state->level + def->attack_step, def->attack_level);
                    if (state->level == def->attack_level) {
                        state->state = SYNTHIO_ENVELOPE_STATE_DECAY;
                    }
                    break;
                }
                state->state = SYNTHIO_ENVELOPE_STATE_DECAY;
                MP_FALLTHROUGH;
            case SYNTHIO_ENVELOPE_STATE_DECAY:
                if (def->decay_step != 0) {
                    state->level = MAX(state->level + def->decay_step, def->sustain_level);
                    assert(state->level >= 0);
                    if (state->level == def->sustain_level) {
                        state->state = SYNTHIO_ENVELOPE_STATE_SUSTAIN;
                    }
                    break;
                }
                state->state = SYNTHIO_ENVELOPE_STATE_RELEASE;
                MP_FALLTHROUGH;
            case SYNTHIO_ENVELOPE_STATE_RELEASE:
                if (def->release_step != 0) {
                    int delta = def->release_step;
                    state->level = MAX(state->level + delta, 0);
                } else {
                    state->level = 0;
                }
                break;
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


STATIC uint32_t synthio_synth_sum_envelope(synthio_synth_t *synth) {
    uint32_t result = 0;
    for (int chan = 0; chan < CIRCUITPY_SYNTHIO_MAX_CHANNELS; chan++) {
        if (synth->span.note[chan] != SYNTHIO_SILENCE) {
            result += synth->envelope_state[chan].level;
        }
    }
    return result;
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
    int16_t *out_buffer = (int16_t *)(void *)synth->buffers[synth->buffer_index];

    uint16_t dur = MIN(SYNTHIO_MAX_DUR, synth->span.dur);
    synth->span.dur -= dur;
    memset(out_buffer, 0, synth->buffer_length);

    int32_t sample_rate = synth->sample_rate;
    uint32_t total_envelope = synthio_synth_sum_envelope(synth);
    const int16_t *waveform = synth->waveform;
    uint32_t waveform_length = synth->waveform_length;
    if (total_envelope > 0) {
        uint16_t ovl_loudness = 0x7fffffff / MAX(0x8000, total_envelope);
        for (int chan = 0; chan < CIRCUITPY_SYNTHIO_MAX_CHANNELS; chan++) {
            if (synth->span.note[chan] == SYNTHIO_SILENCE) {
                synth->accum[chan] = 0;
                continue;
            }
            // adjust loudness by envelope
            uint16_t loudness = (ovl_loudness * synth->envelope_state[chan].level) >> 16;

            if (synth->envelope_state[chan].level == 0) {
                // note is truly finished
                synth->span.note[chan] = SYNTHIO_SILENCE;
            }
            uint8_t octave = synth->span.note[chan] / 12;
            uint16_t base_freq = notes[synth->span.note[chan] % 12];
            uint32_t accum = synth->accum[chan];
#define SHIFT (16)
            // rate = base_freq * waveform_length
            // den = sample_rate * 2 ^ (10 - octave)
            // den = sample_rate * 2 ^ 10 / 2^octave
            // dds_rate = 2^SHIFT * rate / den
            // dds_rate = 2^(SHIFT-10+octave) * base_freq * waveform_length / sample_rate
            uint32_t dds_rate = (sample_rate / 2 + ((uint64_t)(base_freq * waveform_length) << (SHIFT - 10 + octave))) / sample_rate;

            for (uint16_t i = 0; i < dur; i++) {
                accum += dds_rate;
                if (accum > waveform_length << SHIFT) {
                    accum -= waveform_length << SHIFT;
                }
                int16_t idx = accum >> SHIFT;
                out_buffer[i] += (waveform[idx] * loudness) / 65536;
            }
            synth->accum[chan] = accum;
        }
    }

    // advance envelope states
    for (int chan = 0; chan < CIRCUITPY_SYNTHIO_MAX_CHANNELS; chan++) {
        synthio_envelope_state_step(&synth->envelope_state[chan], &synth->envelope_definition, dur);
    }

    *buffer_length = synth->last_buffer_length = dur * SYNTHIO_BYTES_PER_SAMPLE;
    *bufptr = (uint8_t *)out_buffer;
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
    synthio_envelope_definition_set(&synth->envelope_definition, envelope_obj, synth->sample_rate);
    synth->envelope_obj = envelope_obj;
}

mp_obj_t synthio_synth_envelope_get(synthio_synth_t *synth) {
    return synth->envelope_obj;
}

void synthio_synth_init(synthio_synth_t *synth, uint32_t sample_rate, const int16_t *waveform, uint16_t waveform_length, mp_obj_t envelope_obj) {
    synth->buffer_length = SYNTHIO_MAX_DUR * SYNTHIO_BYTES_PER_SAMPLE;
    synth->buffers[0] = m_malloc(synth->buffer_length, false);
    synth->buffers[1] = m_malloc(synth->buffer_length, false);
    synth->other_channel = -1;
    synth->waveform = waveform;
    synth->waveform_length = waveform_length;
    synth->sample_rate = sample_rate;
    synthio_synth_envelope_set(synth, envelope_obj);

    for (size_t i = 0; i < CIRCUITPY_SYNTHIO_MAX_CHANNELS; i++) {
        synth->span.note[i] = SYNTHIO_SILENCE;
    }
}

void synthio_synth_get_buffer_structure(synthio_synth_t *synth, bool single_channel_output,
    bool *single_buffer, bool *samples_signed, uint32_t *max_buffer_length, uint8_t *spacing) {
    *single_buffer = false;
    *samples_signed = true;
    *max_buffer_length = synth->buffer_length;
    *spacing = 1;
}

static bool parse_common(mp_buffer_info_t *bufinfo, mp_obj_t o, int16_t what) {
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

STATIC int find_channel_with_note(synthio_synth_t *synth, uint8_t note) {
    for (int i = 0; i < CIRCUITPY_SYNTHIO_MAX_CHANNELS; i++) {
        if (synth->span.note[i] == note) {
            return i;
        }
    }
    if (note == SYNTHIO_SILENCE) {
        // we need a victim note that is releasing. simple algorithm: lowest numbered slot
        for (int i = 0; i < CIRCUITPY_SYNTHIO_MAX_CHANNELS; i++) {
            if (SYNTHIO_VOICE_IS_RELEASING(synth, i)) {
                return i;
            }
        }
    }
    return -1;
}

bool synthio_span_change_note(synthio_synth_t *synth, uint8_t old_note, uint8_t new_note) {
    int channel;
    if (new_note != SYNTHIO_SILENCE && (channel = find_channel_with_note(synth, new_note)) != -1) {
        // note already playing, re-strike
        synthio_envelope_state_init(&synth->envelope_state[channel], &synth->envelope_definition);
        synth->accum[channel] = 0;
        return true;
    }
    channel = find_channel_with_note(synth, old_note);
    if (channel != -1) {
        if (new_note == SYNTHIO_SILENCE) {
            synthio_envelope_state_release(&synth->envelope_state[channel], &synth->envelope_definition);
        } else {
            synth->span.note[channel] = new_note;
            synthio_envelope_state_init(&synth->envelope_state[channel], &synth->envelope_definition);
            synth->accum[channel] = 0;
        }
        return true;
    }
    return false;
}
