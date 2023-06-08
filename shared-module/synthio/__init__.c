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
#include "shared-module/synthio/Biquad.h"
#include "shared-module/synthio/Note.h"
#include "py/runtime.h"
#include <math.h>
#include <stdlib.h>

mp_float_t synthio_global_rate_scale;
uint8_t synthio_global_tick;

STATIC const int16_t square_wave[] = {-32768, 32767};

STATIC const uint16_t notes[] = {8372, 8870, 9397, 9956, 10548, 11175, 11840,
                                 12544, 13290, 14080, 14917, 15804}; // 9th octave


STATIC int64_t round_float_to_int64(mp_float_t f) {
    return (int64_t)(f + MICROPY_FLOAT_CONST(0.5));
}

mp_float_t common_hal_synthio_midi_to_hz_float(mp_float_t arg) {
    return common_hal_synthio_voct_to_hz_float(arg / 12. - 3);
}

mp_float_t common_hal_synthio_voct_to_hz_float(mp_float_t octave) {
    return notes[0] * MICROPY_FLOAT_C_FUN(pow)(2., octave - 7);
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

static bool synth_note_into_buffer(synthio_synth_t *synth, int chan, int32_t *out_buffer32, int16_t dur, uint16_t loudness[2]) {
    mp_obj_t note_obj = synth->span.note_obj[chan];

    int32_t sample_rate = synth->sample_rate;


    uint32_t dds_rate;
    const int16_t *waveform = synth->waveform_bufinfo.buf;
    uint32_t waveform_length = synth->waveform_bufinfo.len;

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
            waveform_length = note->waveform_buf.len;
        }
        dds_rate = synthio_frequency_convert_scaled_to_dds((uint64_t)frequency_scaled * waveform_length, sample_rate);
        if (note->ring_frequency_scaled != 0 && note->ring_waveform_buf.buf) {
            ring_waveform = note->ring_waveform_buf.buf;
            ring_waveform_length = note->ring_waveform_buf.len;
            ring_dds_rate = synthio_frequency_convert_scaled_to_dds((uint64_t)note->ring_frequency_bent * ring_waveform_length, sample_rate);
            uint32_t lim = ring_waveform_length << SYNTHIO_FREQUENCY_SHIFT;
            if (ring_dds_rate > lim / sizeof(int16_t)) {
                ring_dds_rate = 0; // can't ring at that frequency
            }
        }
    }

    uint32_t lim = waveform_length << SYNTHIO_FREQUENCY_SHIFT;
    uint32_t accum = synth->accum[chan];

    if (dds_rate > lim / 2) {
        // beyond nyquist, can't play note
        return false;
    }

    // can happen if note waveform gets set mid-note, but the expensive modulo is usually avoided
    if (accum > lim) {
        accum %= lim;
    }

    // first, fill with waveform
    for (uint16_t i = 0; i < dur; i++) {
        accum += dds_rate;
        // because dds_rate is low enough, the subtraction is guaranteed to go back into range, no expensive modulo needed
        if (accum > lim) {
            accum -= lim;
        }
        int16_t idx = accum >> SYNTHIO_FREQUENCY_SHIFT;
        out_buffer32[i] = waveform[idx];
    }
    synth->accum[chan] = accum;

    if (ring_dds_rate) {
        if (ring_dds_rate > lim / 2) {
            // beyond nyquist, can't play ring (but did synth main sound so
            // return true)
            return true;
        }

        // now modulate by ring and accumulate
        accum = synth->ring_accum[chan];
        lim = ring_waveform_length << SYNTHIO_FREQUENCY_SHIFT;

        // can happen if note waveform gets set mid-note, but the expensive modulo is usually avoided
        if (accum > lim) {
            accum %= lim;
        }

        for (uint16_t i = 0; i < dur; i++) {
            accum += ring_dds_rate;
            // because dds_rate is low enough, the subtraction is guaranteed to go back into range, no expensive modulo needed
            if (accum > lim) {
                accum -= lim;
            }
            int16_t idx = accum >> SYNTHIO_FREQUENCY_SHIFT;
            int16_t wi = (ring_waveform[idx] * out_buffer32[i]) / 32768;
            out_buffer32[i] = wi;
        }
        synth->ring_accum[chan] = accum;
    }
    return true;
}

STATIC mp_obj_t synthio_synth_get_note_filter(mp_obj_t note_obj) {
    if (note_obj == mp_const_none) {
        return mp_const_none;
    }
    if (!mp_obj_is_small_int(note_obj)) {
        synthio_note_obj_t *note = MP_OBJ_TO_PTR(note_obj);
        return note->filter_obj;
    }
    return mp_const_none;
}

STATIC void sum_with_loudness(int32_t *out_buffer32, int32_t *tmp_buffer32, uint16_t loudness[2], size_t dur, int synth_chan) {
    if (synth_chan == 1) {
        for (size_t i = 0; i < dur; i++) {
            *out_buffer32++ += (*tmp_buffer32++ *loudness[0]) >> 16;
        }
    } else {
        for (size_t i = 0; i < dur; i++) {
            *out_buffer32++ += (*tmp_buffer32 * loudness[0]) >> 16;
            *out_buffer32++ += (*tmp_buffer32++ *loudness[1]) >> 16;
        }
    }
}

void synthio_synth_synthesize(synthio_synth_t *synth, uint8_t **bufptr, uint32_t *buffer_length, uint8_t channel) {

    if (channel == synth->other_channel) {
        *buffer_length = synth->last_buffer_length;
        *bufptr = (uint8_t *)(synth->buffers[synth->other_buffer_index] + channel);
        return;
    }

    shared_bindings_synthio_lfo_tick(synth->sample_rate);

    synth->buffer_index = !synth->buffer_index;
    synth->other_channel = 1 - channel;
    synth->other_buffer_index = synth->buffer_index;

    uint16_t dur = MIN(SYNTHIO_MAX_DUR, synth->span.dur);
    synth->span.dur -= dur;

    int32_t out_buffer32[SYNTHIO_MAX_DUR * synth->channel_count];
    int32_t tmp_buffer32[SYNTHIO_MAX_DUR];
    memset(out_buffer32, 0, synth->channel_count * dur * sizeof(int32_t));

    for (int chan = 0; chan < CIRCUITPY_SYNTHIO_MAX_CHANNELS; chan++) {
        mp_obj_t note_obj = synth->span.note_obj[chan];
        if (note_obj == SYNTHIO_SILENCE) {
            continue;
        }

        if (synth->envelope_state[chan].level == 0) {
            // note is truly finished, but we only just noticed
            synth->span.note_obj[chan] = SYNTHIO_SILENCE;
            continue;
        }

        uint16_t loudness[2] = {synth->envelope_state[chan].level,synth->envelope_state[chan].level};

        if (!synth_note_into_buffer(synth, chan, tmp_buffer32, dur, loudness)) {
            // for some other reason, such as being above nyquist, note
            // couldn't be synthed, so don't filter or sum it in
            continue;
        }

        mp_obj_t filter_obj = synthio_synth_get_note_filter(note_obj);
        if (filter_obj != mp_const_none) {
            synthio_note_obj_t *note = MP_OBJ_TO_PTR(note_obj);
            synthio_biquad_filter_samples(&note->filter_state, tmp_buffer32, dur);
        }

        // adjust loudness by envelope
        sum_with_loudness(out_buffer32, tmp_buffer32, loudness, dur, synth->channel_count);
    }

    int16_t *out_buffer16 = (int16_t *)(void *)synth->buffers[synth->buffer_index];

    // mix down audio
    for (size_t i = 0; i < dur * synth->channel_count; i++) {
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

void synthio_synth_init(synthio_synth_t *synth, uint32_t sample_rate, int channel_count, mp_obj_t waveform_obj, mp_obj_t envelope_obj) {
    synthio_synth_parse_waveform(&synth->waveform_bufinfo, waveform_obj);
    mp_arg_validate_int_range(channel_count, 1, 2, MP_QSTR_channel_count);
    synth->buffer_length = SYNTHIO_MAX_DUR * SYNTHIO_BYTES_PER_SAMPLE * channel_count;
    synth->buffers[0] = m_malloc(synth->buffer_length, false);
    synth->buffers[1] = m_malloc(synth->buffer_length, false);
    synth->channel_count = channel_count;
    synth->other_channel = -1;
    synth->waveform_obj = waveform_obj;
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

STATIC void parse_common(mp_buffer_info_t *bufinfo, mp_obj_t o, int16_t what, mp_int_t max_len) {
    if (o != mp_const_none) {
        mp_get_buffer_raise(o, bufinfo, MP_BUFFER_READ);
        if (bufinfo->typecode != 'h') {
            mp_raise_ValueError_varg(translate("%q must be array of type 'h'"), what);
        }
        bufinfo->len /= 2;
        mp_arg_validate_length_range(bufinfo->len, 2, max_len, what);
    }
}

void synthio_synth_parse_waveform(mp_buffer_info_t *bufinfo_waveform, mp_obj_t waveform_obj) {
    *bufinfo_waveform = ((mp_buffer_info_t) { .buf = (void *)square_wave, .len = 2 });
    parse_common(bufinfo_waveform, waveform_obj, MP_QSTR_waveform, 16384);
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

void shared_bindings_synthio_lfo_tick(uint32_t sample_rate) {
    synthio_global_rate_scale = (mp_float_t)SYNTHIO_MAX_DUR / sample_rate;
    synthio_global_tick++;
}

mp_float_t synthio_block_slot_get(synthio_block_slot_t *slot) {
    // all numbers (and None!) previously converted to float in synthio_block_assign_slot
    if (mp_obj_is_float(slot->obj)) {
        return mp_obj_get_float(slot->obj);
    }

    synthio_block_base_t *block = MP_OBJ_TO_PTR(slot->obj);
    if (block->last_tick == synthio_global_tick) {
        return block->value;
    }

    block->last_tick = synthio_global_tick;
    // previously verified by call to mp_proto_get in synthio_block_assign_slot
    const synthio_block_proto_t *p = mp_type_get_protocol_slot(mp_obj_get_type(slot->obj));
    mp_float_t value = p->tick(slot->obj);
    block->value = value;
    return value;
}

mp_float_t synthio_block_slot_get_limited(synthio_block_slot_t *lfo_slot, mp_float_t lo, mp_float_t hi) {
    mp_float_t value = synthio_block_slot_get(lfo_slot);
    if (value < lo) {
        return lo;
    }
    if (value > hi) {
        return hi;
    }
    return value;
}

int32_t synthio_block_slot_get_scaled(synthio_block_slot_t *lfo_slot, mp_float_t lo, mp_float_t hi) {
    mp_float_t value = synthio_block_slot_get_limited(lfo_slot, lo, hi);
    return (int32_t)MICROPY_FLOAT_C_FUN(round)(MICROPY_FLOAT_C_FUN(ldexp)(value, 15));
}

bool synthio_block_assign_slot_maybe(mp_obj_t obj, synthio_block_slot_t *slot) {
    if (synthio_obj_is_block(obj)) {
        slot->obj = obj;
        return true;
    }

    mp_float_t value = MICROPY_FLOAT_CONST(0.);
    if (obj != mp_const_none && !mp_obj_get_float_maybe(obj, &value)) {
        return false;
    }

    slot->obj = mp_obj_new_float(value);
    return true;
}

void synthio_block_assign_slot(mp_obj_t obj, synthio_block_slot_t *slot, qstr arg_name) {
    if (!synthio_block_assign_slot_maybe(obj, slot)) {
        mp_raise_TypeError_varg(translate("%q must be of type %q, not %q"), arg_name, MP_QSTR_BlockInput, mp_obj_get_type_qstr(obj));
    }
}

bool synthio_obj_is_block(mp_obj_t obj) {
    return mp_proto_get(MP_QSTR_synthio_block, obj);
}
