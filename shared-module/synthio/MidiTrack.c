/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include "py/runtime.h"
#include "shared-bindings/synthio/MidiTrack.h"


STATIC void record_midi_stream_error(synthio_miditrack_obj_t *self) {
    self->error_location = self->pos;
    self->pos = self->track.len;
}

STATIC mp_obj_t parse_note(synthio_miditrack_obj_t *self) {
    uint8_t *buffer = self->track.buf;
    size_t len = self->track.len;
    if (self->pos + 1 >= len) {
        record_midi_stream_error(self);
    }
    uint8_t note = buffer[(self->pos)++];
    if (note > 127 || buffer[(self->pos)++] > 127) {
        record_midi_stream_error(self);
    }
    return MP_OBJ_NEW_SMALL_INT(note);
}

static int decode_duration(synthio_miditrack_obj_t *self) {
    uint8_t *buffer = self->track.buf;
    size_t len = self->track.len;
    uint8_t c;
    uint32_t delta = 0;
    do {
        c = buffer[self->pos++];
        delta <<= 7;
        delta |= c & 0x7f;
    } while ((c & 0x80) && (self->pos < len));

    // errors cannot be raised from the background task, so simply end the track.
    if (c & 0x80) {
        self->pos = self->track.len;
        record_midi_stream_error(self);
    }
    return delta * self->synth.sample_rate / self->tempo;
}

// invariant: pointing at a MIDI message
static void decode_until_pause(synthio_miditrack_obj_t *self) {
    uint8_t *buffer = self->track.buf;
    size_t len = self->track.len;
    do {
        switch (buffer[self->pos++] >> 4) {
            case 8: { // Note Off
                mp_obj_t note = parse_note(self);
                synthio_span_change_note(&self->synth, note, SYNTHIO_SILENCE);
                break;
            }
            case 9: { // Note On
                mp_obj_t note = parse_note(self);
                synthio_span_change_note(&self->synth, SYNTHIO_SILENCE, note);
                break;
            }
            case 10:
            case 11:
            case 14: // two data bytes to ignore
                parse_note(self);
                break;
            case 12:
            case 13: // one data byte to ignore
                if (self->pos >= len || buffer[self->pos++] > 127) {
                    record_midi_stream_error(self);
                }
                break;
            case 15: // the full syntax is too complicated, just assume it's "End of Track" event
                self->pos = len;
                break;
            default: // invalid event
                record_midi_stream_error(self);
        }
        if (self->pos < len) {
            self->synth.span.dur = decode_duration(self);
        }
    } while (self->pos < len && self->synth.span.dur == 0);
}

STATIC void start_parse(synthio_miditrack_obj_t *self) {
    self->pos = 0;
    self->error_location = -1;
    self->synth.span.dur = decode_duration(self);
    if (self->synth.span.dur == 0) {
        // the usual case: the file starts with some MIDI event, not a delay
        decode_until_pause(self);
    }
}

void common_hal_synthio_miditrack_construct(synthio_miditrack_obj_t *self,
    const uint8_t *buffer, uint32_t len, uint32_t tempo, uint32_t sample_rate,
    mp_obj_t waveform_obj, mp_obj_t filter_obj, mp_obj_t envelope_obj) {

    self->tempo = tempo;
    self->track.buf = (void *)buffer;
    self->track.len = len;

    synthio_synth_init(&self->synth, sample_rate, 1, waveform_obj, envelope_obj);

    start_parse(self);
}

void common_hal_synthio_miditrack_deinit(synthio_miditrack_obj_t *self) {
    synthio_synth_deinit(&self->synth);
}

bool common_hal_synthio_miditrack_deinited(synthio_miditrack_obj_t *self) {
    return synthio_synth_deinited(&self->synth);
}

mp_int_t common_hal_synthio_miditrack_get_error_location(synthio_miditrack_obj_t *self) {
    return self->error_location;
}

uint32_t common_hal_synthio_miditrack_get_sample_rate(synthio_miditrack_obj_t *self) {
    return self->synth.sample_rate;
}
uint8_t common_hal_synthio_miditrack_get_bits_per_sample(synthio_miditrack_obj_t *self) {
    return SYNTHIO_BITS_PER_SAMPLE;
}
uint8_t common_hal_synthio_miditrack_get_channel_count(synthio_miditrack_obj_t *self) {
    return 1;
}

void synthio_miditrack_reset_buffer(synthio_miditrack_obj_t *self,
    bool single_channel_output, uint8_t channel) {
    synthio_synth_reset_buffer(&self->synth, single_channel_output, channel);
    start_parse(self);
}

audioio_get_buffer_result_t synthio_miditrack_get_buffer(synthio_miditrack_obj_t *self,
    bool single_channel_output, uint8_t channel, uint8_t **buffer, uint32_t *buffer_length) {
    if (common_hal_synthio_miditrack_deinited(self)) {
        *buffer_length = 0;
        return GET_BUFFER_ERROR;
    }

    synthio_synth_synthesize(&self->synth, buffer, buffer_length, single_channel_output ? 0 : channel);
    if (self->synth.span.dur == 0) {
        if (self->pos == self->track.len) {
            return GET_BUFFER_DONE;
        } else {
            decode_until_pause(self);
        }
    }
    return GET_BUFFER_MORE_DATA;
}

void synthio_miditrack_get_buffer_structure(synthio_miditrack_obj_t *self, bool single_channel_output,
    bool *single_buffer, bool *samples_signed, uint32_t *max_buffer_length, uint8_t *spacing) {
    return synthio_synth_get_buffer_structure(&self->synth, single_channel_output, single_buffer, samples_signed, max_buffer_length, spacing);
}
