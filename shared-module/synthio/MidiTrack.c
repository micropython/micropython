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

#define LOUDNESS 0x4000  // 0.5
#define BITS_PER_SAMPLE 16
#define BYTES_PER_SAMPLE (BITS_PER_SAMPLE / 8)
#define SILENCE 0x80

STATIC uint8_t parse_note(const uint8_t *buffer, uint32_t len, uint32_t *pos) {
    if (*pos + 1 >= len) {
        mp_raise_ValueError_varg(translate("Error in MIDI stream at position %d"), *pos);
    }
    uint8_t note = buffer[(*pos)++];
    if (note > 127 || buffer[(*pos)++] > 127) {
        mp_raise_ValueError_varg(translate("Error in MIDI stream at position %d"), *pos);
    }
    return note;
}

STATIC void terminate_span(synthio_miditrack_obj_t *self, uint16_t *dur, uint16_t *max_dur) {
    if (*dur) {
        self->track[self->total_spans - 1].dur = *dur;
        if (*dur > *max_dur) {
            *max_dur = *dur;
        }
        *dur = 0;
    } else {
        self->total_spans--;
    }
}

STATIC void add_span(synthio_miditrack_obj_t *self, uint8_t note1, uint8_t note2) {
    synthio_midi_span_t span = { 0, {note1, note2} };
    self->track = m_realloc(self->track,
        (self->total_spans + 1) * sizeof(synthio_midi_span_t));
    self->track[self->total_spans++] = span;
}

void common_hal_synthio_miditrack_construct(synthio_miditrack_obj_t *self,
    const uint8_t *buffer, uint32_t len, uint32_t tempo, uint32_t sample_rate) {

    synthio_midi_span_t initial = { 0, {SILENCE, SILENCE} };
    self->sample_rate = sample_rate;
    self->track = m_malloc(sizeof(synthio_midi_span_t), false);
    self->next_span = 0;
    self->total_spans = 1;
    *self->track = initial;

    uint16_t dur = 0, max_dur = 0;
    uint32_t pos = 0;
    while (pos < len) {
        uint8_t c;
        uint32_t delta = 0;
        do {
            c = buffer[pos++];
            delta <<= 7;
            delta |= c & 0x7f;
        } while ((c & 0x80) && (pos < len));

        if (c & 0x80) {
            mp_raise_ValueError_varg(translate("Error in MIDI stream at position %d"), pos);
        }

        dur += delta * sample_rate / tempo;

        switch (buffer[pos++] >> 4) {
            case 8: { // Note Off
                uint8_t note = parse_note(buffer, len, &pos);

                // Ignore if not a note which is playing
                synthio_midi_span_t last_span = self->track[self->total_spans - 1];
                if (last_span.note[0] == note || last_span.note[1] == note) {
                    terminate_span(self, &dur, &max_dur);
                    if (last_span.note[0] == note) {
                        add_span(self, last_span.note[1], SILENCE);
                    } else {
                        add_span(self, last_span.note[0], SILENCE);
                    }
                }
                break;
            }
            case 9: { // Note On
                uint8_t note = parse_note(buffer, len, &pos);

                // Ignore if two notes are already playing
                synthio_midi_span_t last_span = self->track[self->total_spans - 1];
                if (last_span.note[1] == SILENCE) {
                    terminate_span(self, &dur, &max_dur);
                    if (last_span.note[0] == SILENCE) {
                        add_span(self, note, SILENCE);
                    } else {
                        add_span(self, last_span.note[0], note);
                    }
                }
                break;
            }
            case 10:
            case 11:
            case 14: // two data bytes to ignore
                parse_note(buffer, len, &pos);
                break;
            case 12:
            case 13: // one data byte to ignore
                if (pos >= len || buffer[pos++] > 127) {
                    mp_raise_ValueError_varg(translate("Error in MIDI stream at position %d"), pos);
                }
                break;
            case 15: // the full syntax is too complicated, just assume it's "End of Track" event
                pos = len;
                break;
            default: // invalid event
                mp_raise_ValueError_varg(translate("Error in MIDI stream at position %d"), pos);
        }
    }
    terminate_span(self, &dur, &max_dur);

    self->buffer_length = max_dur * BYTES_PER_SAMPLE;
    self->buffer = m_malloc(self->buffer_length, false);
}

void common_hal_synthio_miditrack_deinit(synthio_miditrack_obj_t *self) {
    m_free(self->buffer);
    self->buffer = NULL;
    m_free(self->track);
    self->track = NULL;
}
bool common_hal_synthio_miditrack_deinited(synthio_miditrack_obj_t *self) {
    return self->buffer == NULL;
}

uint32_t common_hal_synthio_miditrack_get_sample_rate(synthio_miditrack_obj_t *self) {
    return self->sample_rate;
}
uint8_t common_hal_synthio_miditrack_get_bits_per_sample(synthio_miditrack_obj_t *self) {
    return BITS_PER_SAMPLE;
}
uint8_t common_hal_synthio_miditrack_get_channel_count(synthio_miditrack_obj_t *self) {
    return 1;
}

void synthio_miditrack_reset_buffer(synthio_miditrack_obj_t *self,
    bool single_channel_output, uint8_t channel) {

    self->next_span = 0;
}

STATIC const uint16_t notes[] = {8372, 8870, 9397, 9956, 10548, 11175, 11840,
                                 12544, 13290, 14080, 14917, 15804}; // 9th octave

audioio_get_buffer_result_t synthio_miditrack_get_buffer(synthio_miditrack_obj_t *self,
    bool single_channel_output, uint8_t channel, uint8_t **buffer, uint32_t *buffer_length) {

    if (self->next_span >= self->total_spans) {
        *buffer_length = 0;
        return GET_BUFFER_DONE;
    }

    synthio_midi_span_t span = self->track[self->next_span++];
    *buffer_length = span.dur * BYTES_PER_SAMPLE;
    uint8_t octave1 = span.note[0] / 12; // 0..10
    uint8_t octave2 = span.note[1] / 12; // 0..10
    int32_t base_freq1 = notes[span.note[0] % 12];
    int32_t base_freq2 = notes[span.note[1] % 12];
    int32_t sample_rate = self->sample_rate;

    for (uint16_t i = 0; i < span.dur; i++) {
        int16_t semiperiod1 = span.note[0] == SILENCE ? 0 :
            ((base_freq1 * i * 2) / sample_rate) >> (10 - octave1);
        int16_t semiperiod2 = span.note[1] == SILENCE ? semiperiod1 :
            ((base_freq2 * i * 2) / sample_rate) >> (10 - octave2);
        self->buffer[i] = ((semiperiod1 % 2 + semiperiod2 % 2) - 1) * LOUDNESS;
    }
    *buffer = (uint8_t *)self->buffer;

    return self->next_span >= self->total_spans ?
           GET_BUFFER_DONE : GET_BUFFER_MORE_DATA;
}

void synthio_miditrack_get_buffer_structure(synthio_miditrack_obj_t *self, bool single_channel_output,
    bool *single_buffer, bool *samples_signed, uint32_t *max_buffer_length, uint8_t *spacing) {

    *single_buffer = true;
    *samples_signed = true;
    *max_buffer_length = self->buffer_length;
    *spacing = 1;
}
