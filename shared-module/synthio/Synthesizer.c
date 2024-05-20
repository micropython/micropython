// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Artyom Skrobov
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"
#include "shared-bindings/synthio/LFO.h"
#include "shared-bindings/synthio/Note.h"
#include "shared-bindings/synthio/Synthesizer.h"
#include "shared-module/synthio/Note.h"



void common_hal_synthio_synthesizer_construct(synthio_synthesizer_obj_t *self,
    uint32_t sample_rate, int channel_count, mp_obj_t waveform_obj,
    mp_obj_t envelope_obj) {

    synthio_synth_init(&self->synth, sample_rate, channel_count, waveform_obj, envelope_obj);
    self->blocks = mp_obj_new_list(0, NULL);
}

void common_hal_synthio_synthesizer_deinit(synthio_synthesizer_obj_t *self) {
    synthio_synth_deinit(&self->synth);
}
bool common_hal_synthio_synthesizer_deinited(synthio_synthesizer_obj_t *self) {
    return synthio_synth_deinited(&self->synth);
}

uint32_t common_hal_synthio_synthesizer_get_sample_rate(synthio_synthesizer_obj_t *self) {
    return self->synth.sample_rate;
}
uint8_t common_hal_synthio_synthesizer_get_bits_per_sample(synthio_synthesizer_obj_t *self) {
    return SYNTHIO_BITS_PER_SAMPLE;
}
uint8_t common_hal_synthio_synthesizer_get_channel_count(synthio_synthesizer_obj_t *self) {
    return self->synth.channel_count;
}

void synthio_synthesizer_reset_buffer(synthio_synthesizer_obj_t *self,
    bool single_channel_output, uint8_t channel) {
    synthio_synth_reset_buffer(&self->synth, single_channel_output, channel);
}

audioio_get_buffer_result_t synthio_synthesizer_get_buffer(synthio_synthesizer_obj_t *self,
    bool single_channel_output, uint8_t channel, uint8_t **buffer, uint32_t *buffer_length) {
    if (common_hal_synthio_synthesizer_deinited(self)) {
        *buffer_length = 0;
        return GET_BUFFER_ERROR;
    }
    self->synth.span.dur = SYNTHIO_MAX_DUR;


    synthio_synth_synthesize(&self->synth, buffer, buffer_length, single_channel_output ? channel : 0);

    // free-running LFOs
    mp_obj_iter_buf_t iter_buf;
    mp_obj_t iterable = mp_getiter(self->blocks, &iter_buf);
    mp_obj_t item;
    while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        if (!synthio_obj_is_block(item)) {
            continue;
        }
        synthio_block_slot_t slot = { item };
        (void)synthio_block_slot_get(&slot);
    }
    return GET_BUFFER_MORE_DATA;
}

void synthio_synthesizer_get_buffer_structure(synthio_synthesizer_obj_t *self, bool single_channel_output,
    bool *single_buffer, bool *samples_signed, uint32_t *max_buffer_length, uint8_t *spacing) {
    return synthio_synth_get_buffer_structure(&self->synth, single_channel_output, single_buffer, samples_signed, max_buffer_length, spacing);
}

void common_hal_synthio_synthesizer_release_all(synthio_synthesizer_obj_t *self) {
    for (size_t i = 0; i < CIRCUITPY_SYNTHIO_MAX_CHANNELS; i++) {
        if (self->synth.span.note_obj[i] != SYNTHIO_SILENCE) {
            synthio_span_change_note(&self->synth, self->synth.span.note_obj[i], SYNTHIO_SILENCE);
        }
    }
}

static bool is_note(mp_obj_t note_in) {
    return mp_obj_is_small_int(note_in) || mp_obj_is_type(note_in, &synthio_note_type);
}

static mp_obj_t validate_note(mp_obj_t note_in) {
    if (mp_obj_is_small_int(note_in)) {
        mp_arg_validate_int_range(mp_obj_get_int(note_in), 0, 127, MP_QSTR_note);
    } else {
        const mp_obj_type_t *note_type = mp_obj_get_type(note_in);
        if (note_type != &synthio_note_type) {
            mp_raise_TypeError_varg(MP_ERROR_TEXT("%q must be of type %q or %q, not %q"), MP_QSTR_note, MP_QSTR_int, MP_QSTR_Note, note_type->name);
        }
    }
    return note_in;
}

void common_hal_synthio_synthesizer_release(synthio_synthesizer_obj_t *self, mp_obj_t to_release) {
    if (is_note(to_release)) {
        synthio_span_change_note(&self->synth, validate_note(to_release), SYNTHIO_SILENCE);
        return;
    }

    mp_obj_iter_buf_t iter_buf;
    mp_obj_t iterable = mp_getiter(to_release, &iter_buf);
    mp_obj_t item;
    while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        synthio_span_change_note(&self->synth, validate_note(item), SYNTHIO_SILENCE);
    }
}

void common_hal_synthio_synthesizer_press(synthio_synthesizer_obj_t *self, mp_obj_t to_press) {
    if (is_note(to_press)) {
        if (!mp_obj_is_small_int(to_press)) {
            synthio_note_obj_t *note = MP_OBJ_TO_PTR(to_press);
            synthio_note_start(note, self->synth.sample_rate);
        }
        synthio_span_change_note(&self->synth, SYNTHIO_SILENCE, validate_note(to_press));
        return;
    }

    mp_obj_iter_buf_t iter_buf;
    mp_obj_t iterable = mp_getiter(to_press, &iter_buf);
    mp_obj_t note_obj;
    while ((note_obj = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        note_obj = validate_note(note_obj);
        if (!mp_obj_is_small_int(note_obj)) {
            synthio_note_obj_t *note = MP_OBJ_TO_PTR(note_obj);
            synthio_note_start(note, self->synth.sample_rate);
        }
        synthio_span_change_note(&self->synth, SYNTHIO_SILENCE, note_obj);
    }
}

void common_hal_synthio_synthesizer_retrigger(synthio_synthesizer_obj_t *self, mp_obj_t to_retrigger) {
    if (mp_obj_is_type(to_retrigger, &synthio_lfo_type)) {
        synthio_lfo_obj_t *lfo = MP_OBJ_TO_PTR(mp_arg_validate_type(to_retrigger, &synthio_lfo_type, MP_QSTR_retrigger));
        common_hal_synthio_lfo_retrigger(lfo);
        return;
    }

    mp_obj_iter_buf_t iter_buf;
    mp_obj_t iterable = mp_getiter(to_retrigger, &iter_buf);
    mp_obj_t lfo_obj;
    while ((lfo_obj = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        synthio_lfo_obj_t *lfo = MP_OBJ_TO_PTR(mp_arg_validate_type(lfo_obj, &synthio_lfo_type, MP_QSTR_retrigger));
        common_hal_synthio_lfo_retrigger(lfo);
    }
}

mp_obj_t common_hal_synthio_synthesizer_get_pressed_notes(synthio_synthesizer_obj_t *self) {
    int count = 0;
    for (int chan = 0; chan < CIRCUITPY_SYNTHIO_MAX_CHANNELS; chan++) {
        if (self->synth.span.note_obj[chan] != SYNTHIO_SILENCE && SYNTHIO_NOTE_IS_PLAYING(&self->synth, chan)) {
            count += 1;
        }
    }
    mp_obj_tuple_t *result = MP_OBJ_TO_PTR(mp_obj_new_tuple(count, NULL));
    for (size_t chan = 0, j = 0; chan < CIRCUITPY_SYNTHIO_MAX_CHANNELS; chan++) {
        if (self->synth.span.note_obj[chan] != SYNTHIO_SILENCE && SYNTHIO_NOTE_IS_PLAYING(&self->synth, chan)) {
            result->items[j++] = self->synth.span.note_obj[chan];
        }
    }
    return MP_OBJ_FROM_PTR(result);
}

envelope_state_e common_hal_synthio_synthesizer_note_info(synthio_synthesizer_obj_t *self, mp_obj_t note, mp_float_t *vol_out) {
    for (int chan = 0; chan < CIRCUITPY_SYNTHIO_MAX_CHANNELS; chan++) {
        if (self->synth.span.note_obj[chan] == note) {
            *vol_out = self->synth.envelope_state[chan].level / 32767.;
            return self->synth.envelope_state[chan].state;
        }
    }
    return (envelope_state_e) - 1;
}


mp_obj_t common_hal_synthio_synthesizer_get_blocks(synthio_synthesizer_obj_t *self) {
    return self->blocks;
}
