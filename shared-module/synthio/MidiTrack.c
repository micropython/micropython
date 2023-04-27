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
    self->track = m_renew(synthio_midi_span_t, self->track, self->total_spans, self->total_spans + 1);
    self->track[self->total_spans++] = *span;
}

STATIC void change_span_note(synthio_miditrack_obj_t *self, uint8_t old_note, uint8_t new_note, uint16_t *dur) {
    synthio_midi_span_t span = self->track[self->total_spans - 1];
    if (synthio_span_change_note(&span, old_note, new_note)) {
        terminate_span(self, dur);
        add_span(self, &span);
        *dur = 0;
    }
}

void common_hal_synthio_miditrack_construct(synthio_miditrack_obj_t *self,
    const uint8_t *buffer, uint32_t len, uint32_t tempo, uint32_t sample_rate,
    const int16_t *waveform, uint16_t waveform_length) {

    self->synth.sample_rate = sample_rate;
    self->track = m_malloc(sizeof(synthio_midi_span_t), false);
    synthio_span_init(self->track);
    self->next_span = 0;
    self->total_spans = 1;
    self->synth.waveform = waveform;
    self->synth.waveform_length = waveform_length;

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
                change_span_note(self, note, SYNTHIO_SILENCE, &dur);
                break;
            }
            case 9: { // Note On
                uint8_t note = parse_note(buffer, len, &pos);
                change_span_note(self, SYNTHIO_SILENCE, note, &dur);
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
    synthio_synth_init(&self->synth, max_dur);
}

void common_hal_synthio_miditrack_deinit(synthio_miditrack_obj_t *self) {
    synthio_synth_deinit(&self->synth);
    m_del(synthio_midi_span_t, self->track, self->total_spans + 1);
    self->track = NULL;
}
bool common_hal_synthio_miditrack_deinited(synthio_miditrack_obj_t *self) {
    return synthio_synth_deinited(&self->synth);
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
    self->synth.span.dur = 0;
    self->next_span = 0;
}

audioio_get_buffer_result_t synthio_miditrack_get_buffer(synthio_miditrack_obj_t *self,
    bool single_channel_output, uint8_t channel, uint8_t **buffer, uint32_t *buffer_length) {

    if (self->synth.span.dur == 0) {
        if (self->next_span >= self->total_spans) {
            *buffer_length = 0;
            return GET_BUFFER_DONE;
        }
        self->synth.span = self->track[self->next_span++];
    }

    synthio_synth_synthesize(&self->synth, buffer, buffer_length, single_channel_output ? 0 : channel);

    return (self->synth.span.dur == 0 && self->next_span >= self->total_spans) ?
           GET_BUFFER_DONE : GET_BUFFER_MORE_DATA;
}

void synthio_miditrack_get_buffer_structure(synthio_miditrack_obj_t *self, bool single_channel_output,
    bool *single_buffer, bool *samples_signed, uint32_t *max_buffer_length, uint8_t *spacing) {
    return synthio_synth_get_buffer_structure(&self->synth, single_channel_output, single_buffer, samples_signed, max_buffer_length, spacing);
}
