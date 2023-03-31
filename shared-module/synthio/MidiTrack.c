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

#define BITS_PER_SAMPLE (16)
#define BYTES_PER_SAMPLE (BITS_PER_SAMPLE / 8)
#define MAX_DUR (512)
#define SILENCE (0x80)

STATIC NORETURN void raise_midi_stream_error(uint32_t pos) {
    mp_raise_ValueError_varg(translate("Error in MIDI stream at position %d"), pos);
}

STATIC uint8_t parse_note(const uint8_t *buffer, uint32_t len, uint32_t *pos) {
    if (*pos + 1 >= len) {
        raise_midi_stream_error(*pos);
    }
    uint8_t note = buffer[(*pos)++];
    if (note > 127 || buffer[(*pos)++] > 127) {
        raise_midi_stream_error(*pos);
    }
    return note;
}

STATIC void terminate_span(synthio_miditrack_obj_t *self, uint16_t *dur) {
    if (*dur) {
        self->track[self->total_spans - 1].dur = *dur;
        *dur = 0;
    } else {
        self->total_spans--;
    }
}

STATIC void add_span(synthio_miditrack_obj_t *self, const synthio_midi_span_t *span) {
    self->track = m_realloc(self->track,
        (self->total_spans + 1) * sizeof(synthio_midi_span_t));
    self->track[self->total_spans++] = *span;
}

STATIC int find_channel_with_note(const synthio_midi_span_t *span, uint8_t note) {
    for (int i = 0; i < CIRCUITPY_SYNTHIO_MAX_CHANNELS; i++) {
        if (span->note[i] == note) {
            return i;
        }
    }
    return -1;
}

STATIC void change_span_note(synthio_miditrack_obj_t *self, uint8_t old_note, uint8_t new_note, uint16_t *dur) {
    synthio_midi_span_t span = self->track[self->total_spans - 1];
    int channel = find_channel_with_note(&span, old_note);
    if (channel != -1) {
        terminate_span(self, dur);
        span.note[channel] = new_note;
        add_span(self, &span);
        *dur = 0;
    }
}

void common_hal_synthio_miditrack_construct(synthio_miditrack_obj_t *self,
    const uint8_t *buffer, uint32_t len, uint32_t tempo, uint32_t sample_rate) {

    synthio_midi_span_t initial = { 0, {[0 ... (CIRCUITPY_SYNTHIO_MAX_CHANNELS - 1)] = SILENCE} };
    self->sample_rate = sample_rate;
    self->track = m_malloc(sizeof(synthio_midi_span_t), false);
    self->next_span = 0;
    self->total_spans = 1;
    *self->track = initial;

    uint16_t dur = 0;
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
            raise_midi_stream_error(pos);
        }

        // dur is carried over here so that if a note on/off message doesn't actually produce a change, the
        // underlying "span" is extended. Otherwise, it is zeroed out in the call to `terminate_span`.
        dur += delta * sample_rate / tempo;

        switch (buffer[pos++] >> 4) {
            case 8: { // Note Off
                uint8_t note = parse_note(buffer, len, &pos);
                change_span_note(self, note, SILENCE, &dur);
                break;
            }
            case 9: { // Note On
                uint8_t note = parse_note(buffer, len, &pos);
                change_span_note(self, SILENCE, note, &dur);
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
                    raise_midi_stream_error(pos);
                }
                break;
            case 15: // the full syntax is too complicated, just assume it's "End of Track" event
                pos = len;
                break;
            default: // invalid event
                raise_midi_stream_error(pos);
        }
    }
    terminate_span(self, &dur);

    uint16_t max_dur = 0;
    for (int i = 0; i < self->total_spans; i++) {
        max_dur = MAX(self->track[i].dur, max_dur);
    }
    self->buffer_length = MIN(MAX_DUR, max_dur) * BYTES_PER_SAMPLE;
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

    self->remaining_dur = 0;
    self->next_span = 0;
}

STATIC const uint16_t notes[] = {8372, 8870, 9397, 9956, 10548, 11175, 11840,
                                 12544, 13290, 14080, 14917, 15804}; // 9th octave

static int count_active_channels(synthio_midi_span_t *span) {
    int result = 0;
    for (int i = 0; i < CIRCUITPY_SYNTHIO_MAX_CHANNELS; i++) {
        if (span->note[i] != SILENCE) {
            result += 1;
        }
    }
    return result;
}

audioio_get_buffer_result_t synthio_miditrack_get_buffer(synthio_miditrack_obj_t *self,
    bool single_channel_output, uint8_t channel, uint8_t **buffer, uint32_t *buffer_length) {
    synthio_midi_span_t span = self->track[self->next_span - !!self->remaining_dur];

    if (self->remaining_dur == 0) {
        if (self->next_span >= self->total_spans) {
            *buffer_length = 0;
            return GET_BUFFER_DONE;
        }
        self->next_span++;
        self->remaining_dur = span.dur;
    }

    uint16_t dur = MIN(MAX_DUR, self->remaining_dur);
    self->remaining_dur -= dur;
    *buffer_length = dur * BYTES_PER_SAMPLE;
    memset(self->buffer, 0, *buffer_length);

    int32_t sample_rate = self->sample_rate;
    int active_channels = count_active_channels(&span);
    if (active_channels) {
        int16_t loudness = 0x3fff / (1 + active_channels);
        for (int chan = 0; chan < CIRCUITPY_SYNTHIO_MAX_CHANNELS; chan++) {
            if (span.note[chan] == SILENCE) {
                self->accum[chan] = 0;
                continue;
            }
            uint8_t octave = span.note[chan] / 12;
            uint16_t base_freq = notes[span.note[chan] % 12];
            uint32_t accum = self->accum[chan];
            uint32_t rate = base_freq * 2;
            for (uint16_t i = 0; i < dur; i++) {
                accum += rate;
                int16_t semiperiod = (accum / sample_rate) >> (10 - octave);
                self->buffer[i] += semiperiod % 2 ? loudness : -loudness;
            }
            self->accum[chan] = accum;
        }
    }

    *buffer = (uint8_t *)self->buffer;

    return (self->remaining_dur == 0 && self->next_span >= self->total_spans) ?
           GET_BUFFER_DONE : GET_BUFFER_MORE_DATA;
}

void synthio_miditrack_get_buffer_structure(synthio_miditrack_obj_t *self, bool single_channel_output,
    bool *single_buffer, bool *samples_signed, uint32_t *max_buffer_length, uint8_t *spacing) {

    *single_buffer = true;
    *samples_signed = true;
    *max_buffer_length = self->buffer_length;
    *spacing = 1;
}
