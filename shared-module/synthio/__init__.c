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
#include "py/runtime.h"

STATIC const int16_t square_wave[] = {-32768, 32767};

STATIC const uint16_t notes[] = {8372, 8870, 9397, 9956, 10548, 11175, 11840,
                                 12544, 13290, 14080, 14917, 15804}; // 9th octave

int synthio_span_count_active_channels(synthio_midi_span_t *span) {
    int result = 0;
    for (int i = 0; i < CIRCUITPY_SYNTHIO_MAX_CHANNELS; i++) {
        if (span->note[i] != SYNTHIO_SILENCE) {
            result += 1;
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
    int active_channels = synthio_span_count_active_channels(&synth->span);
    const int16_t *waveform = synth->waveform;
    uint32_t waveform_length = synth->waveform_length;
    if (active_channels) {
        int16_t loudness = 0xffff / (1 + 2 * active_channels);
        for (int chan = 0; chan < CIRCUITPY_SYNTHIO_MAX_CHANNELS; chan++) {
            if (synth->span.note[chan] == SYNTHIO_SILENCE) {
                synth->accum[chan] = 0;
                continue;
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

void synthio_synth_init(synthio_synth_t *synth, uint16_t max_dur) {
    synth->buffer_length = MIN(SYNTHIO_MAX_DUR, max_dur) * SYNTHIO_BYTES_PER_SAMPLE;
    synth->buffers[0] = m_malloc(synth->buffer_length, false);
    synth->buffers[1] = m_malloc(synth->buffer_length, false);
    synth->other_channel = -1;
}

void synthio_synth_get_buffer_structure(synthio_synth_t *synth, bool single_channel_output,
    bool *single_buffer, bool *samples_signed, uint32_t *max_buffer_length, uint8_t *spacing) {
    *single_buffer = false;
    *samples_signed = true;
    *max_buffer_length = synth->buffer_length;
    *spacing = 1;
}

void synthio_synth_parse_waveform(mp_buffer_info_t *bufinfo_waveform, mp_obj_t waveform_obj) {
    *bufinfo_waveform = ((mp_buffer_info_t) { .buf = (void *)square_wave, .len = 4 });

    if (waveform_obj != mp_const_none) {
        mp_get_buffer_raise(waveform_obj, bufinfo_waveform, MP_BUFFER_READ);
        if (bufinfo_waveform->typecode != 'h') {
            mp_raise_ValueError_varg(translate("%q must be array of type 'h'"), MP_QSTR_waveform);
        }
    }
    mp_arg_validate_length_range(bufinfo_waveform->len / 2, 2, 1024, MP_QSTR_waveform);
}

void synthio_span_init(synthio_midi_span_t *span) {
    span->dur = 0;
    for (size_t i = 0; i < CIRCUITPY_SYNTHIO_MAX_CHANNELS; i++) { span->note[i] = SYNTHIO_SILENCE;
    }
}

STATIC int find_channel_with_note(const synthio_midi_span_t *span, uint8_t note) {
    for (int i = 0; i < CIRCUITPY_SYNTHIO_MAX_CHANNELS; i++) {
        if (span->note[i] == note) {
            return i;
        }
    }
    return -1;
}

bool synthio_span_change_note(synthio_midi_span_t *span, uint8_t old_note, uint8_t new_note) {
    if (new_note != SYNTHIO_SILENCE && find_channel_with_note(span, new_note) != -1) {
        return false; // note already pressed, do nothing
    }
    int channel = find_channel_with_note(span, old_note);
    if (channel != -1) {
        span->note[channel] = new_note;
        return true;
    }
    return false;
}
