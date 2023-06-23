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

#define SYNTHIO_BITS_PER_SAMPLE (16)
#define SYNTHIO_BYTES_PER_SAMPLE (SYNTHIO_BITS_PER_SAMPLE / 8)
#define SYNTHIO_MAX_DUR (256)
#define SYNTHIO_SILENCE (mp_const_none)
#define SYNTHIO_NOTE_IS_SIMPLE(note) (mp_obj_is_small_int(note))
#define SYNTHIO_NOTE_IS_PLAYING(synth, i) ((synth)->envelope_state[(i)].state != SYNTHIO_ENVELOPE_STATE_RELEASE)
#define SYNTHIO_FREQUENCY_SHIFT (16)

#include "shared-module/audiocore/__init__.h"

typedef struct {
    uint16_t dur;
    mp_obj_t note_obj[CIRCUITPY_SYNTHIO_MAX_CHANNELS];
} synthio_midi_span_t;

typedef struct {
    // the number of attack or decay steps (signed) per sample
    // therefore the maximum time is 32767 samples or 0.68s at 48kHz
    // provided the level is maximum (this should be increased!)
    int16_t attack_step, decay_step, release_step;
    uint16_t attack_level, sustain_level;
} synthio_envelope_definition_t;

typedef enum {
    SYNTHIO_ENVELOPE_STATE_ATTACK, SYNTHIO_ENVELOPE_STATE_DECAY,
    SYNTHIO_ENVELOPE_STATE_SUSTAIN, SYNTHIO_ENVELOPE_STATE_RELEASE
} envelope_state_e;

typedef struct {
    int16_t level;
    uint16_t substep;
    envelope_state_e state;
} synthio_envelope_state_t;

typedef struct synthio_synth {
    uint32_t sample_rate;
    uint32_t total_envelope;
    int16_t *buffers[2];
    uint8_t channel_count;
    uint16_t buffer_length;
    uint16_t last_buffer_length;
    uint8_t other_channel, buffer_index, other_buffer_index;
    mp_buffer_info_t waveform_bufinfo;
    synthio_envelope_definition_t global_envelope_definition;
    mp_obj_t waveform_obj, filter_obj, envelope_obj;
    synthio_midi_span_t span;
    uint32_t accum[CIRCUITPY_SYNTHIO_MAX_CHANNELS];
    uint32_t ring_accum[CIRCUITPY_SYNTHIO_MAX_CHANNELS];
    synthio_envelope_state_t envelope_state[CIRCUITPY_SYNTHIO_MAX_CHANNELS];
} synthio_synth_t;

typedef struct {
    mp_float_t amplitude, frequency;
} synthio_lfo_descr_t;

typedef struct {
    int32_t amplitude_scaled;
    uint32_t offset_scaled, dds, phase;
} synthio_lfo_state_t;


void synthio_synth_synthesize(synthio_synth_t *synth, uint8_t **buffer, uint32_t *buffer_length, uint8_t channel);
void synthio_synth_deinit(synthio_synth_t *synth);
bool synthio_synth_deinited(synthio_synth_t *synth);
void synthio_synth_init(synthio_synth_t *synth, uint32_t sample_rate, int channel_count, mp_obj_t waveform_obj, mp_obj_t envelope);
void synthio_synth_get_buffer_structure(synthio_synth_t *synth, bool single_channel_output,
    bool *single_buffer, bool *samples_signed, uint32_t *max_buffer_length, uint8_t *spacing);
void synthio_synth_reset_buffer(synthio_synth_t *synth, bool single_channel_output, uint8_t channel);
void synthio_synth_parse_waveform(mp_buffer_info_t *bufinfo_waveform, mp_obj_t waveform_obj);
void synthio_synth_parse_filter(mp_buffer_info_t *bufinfo_filter, mp_obj_t filter_obj);
void synthio_synth_parse_envelope(uint16_t *envelope_sustain_index, mp_buffer_info_t *bufinfo_envelope, mp_obj_t envelope_obj, mp_obj_t envelope_hold_obj);

bool synthio_span_change_note(synthio_synth_t *synth, mp_obj_t old_note, mp_obj_t new_note);

void synthio_envelope_step(synthio_envelope_definition_t *definition, synthio_envelope_state_t *state, int n_samples);
void synthio_envelope_definition_set(synthio_envelope_definition_t *envelope, mp_obj_t obj, uint32_t sample_rate);

uint64_t synthio_frequency_convert_float_to_scaled(mp_float_t frequency_hz);
uint32_t synthio_frequency_convert_float_to_dds(mp_float_t frequency_hz, int32_t sample_rate);
uint32_t synthio_frequency_convert_scaled_to_dds(uint64_t frequency_scaled, int32_t sample_rate);

void synthio_lfo_set(synthio_lfo_state_t *state, const synthio_lfo_descr_t *descr, uint32_t sample_rate);
int synthio_lfo_step(synthio_lfo_state_t *state, uint16_t dur);
int synthio_sweep_step(synthio_lfo_state_t *state, uint16_t dur);
int synthio_sweep_in_step(synthio_lfo_state_t *state, uint16_t dur);

extern mp_float_t synthio_global_rate_scale;
extern uint8_t synthio_global_tick;
void shared_bindings_synthio_lfo_tick(uint32_t sample_rate);
