// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 DeanM for Adafruit Industries
//
// SPDX-License-Identifier: MIT
#include "shared-bindings/audiomixer/Mixer.h"
#include "shared-bindings/audiomixer/MixerVoice.h"
#include "shared-module/audiomixer/MixerVoice.h"

#include <stdint.h>

#include "py/runtime.h"
#include "shared-module/audiomixer/__init__.h"
#include "shared-module/audiocore/RawSample.h"

void common_hal_audiomixer_mixervoice_construct(audiomixer_mixervoice_obj_t *self) {
    self->sample = NULL;
    common_hal_audiomixer_mixervoice_set_level(self, mp_obj_new_float(1.0));
    common_hal_audiomixer_mixervoice_set_panning(self, mp_obj_new_float(0.0));
}

void common_hal_audiomixer_mixervoice_set_parent(audiomixer_mixervoice_obj_t *self, audiomixer_mixer_obj_t *parent) {
    self->parent = parent;
}

mp_obj_t common_hal_audiomixer_mixervoice_get_level(audiomixer_mixervoice_obj_t *self) {
    #if CIRCUITPY_SYNTHIO
    return self->level.obj;
    #else
    return mp_obj_new_float((mp_float_t)self->level / (1 << 15));
    #endif
}

void common_hal_audiomixer_mixervoice_set_level(audiomixer_mixervoice_obj_t *self, mp_obj_t arg) {
    #if CIRCUITPY_SYNTHIO
    synthio_block_assign_slot(arg, &self->level, MP_QSTR_level);
    #else
    self->level = (uint16_t)(mp_arg_validate_obj_float_range(arg, 0, 1, MP_QSTR_level) * (1 << 15));
    #endif
}

mp_obj_t common_hal_audiomixer_mixervoice_get_panning(audiomixer_mixervoice_obj_t *self) {
    #if CIRCUITPY_SYNTHIO
    return self->panning.obj;
    #else
    return mp_obj_new_float((mp_float_t)self->panning / ((1 << 15) - 1));
    #endif
}

void common_hal_audiomixer_mixervoice_set_panning(audiomixer_mixervoice_obj_t *self, mp_obj_t arg) {
    #if CIRCUITPY_SYNTHIO
    synthio_block_assign_slot(arg, &self->panning, MP_QSTR_panning);
    #else
    self->panning = (int16_t)(mp_arg_validate_obj_float_range(arg, -1, 1, MP_QSTR_panning) * ((1 << 15) - 1));
    #endif
}

bool common_hal_audiomixer_mixervoice_get_loop(audiomixer_mixervoice_obj_t *self) {
    return self->loop;
}

void common_hal_audiomixer_mixervoice_set_loop(audiomixer_mixervoice_obj_t *self, bool loop) {
    self->loop = loop;
}

void common_hal_audiomixer_mixervoice_play(audiomixer_mixervoice_obj_t *self, mp_obj_t sample_in, bool loop) {
    audiosample_must_match(&self->parent->base, sample_in, true);
    // cast is safe, checked by must_match
    audiosample_base_t *sample = MP_OBJ_TO_PTR(sample_in);
    self->sample = sample;
    self->loop = loop;

    common_hal_audiomixer_mixervoice_reset_buffer(self);
}

void common_hal_audiomixer_mixervoice_reset_buffer(audiomixer_mixervoice_obj_t *self) {
    if (self->sample != NULL) {
        self->snap_level = true;
        audiosample_reset_buffer(self->sample, false, 0);
        audioio_get_buffer_result_t result = audiosample_get_buffer(self->sample, false, 0, (uint8_t **)&self->remaining_buffer, &self->buffer_length);
        // Track length in terms of words.
        self->buffer_length /= sizeof(uint32_t);
        self->more_data = result == GET_BUFFER_MORE_DATA;
    }
}

bool common_hal_audiomixer_mixervoice_get_playing(audiomixer_mixervoice_obj_t *self) {
    return self->sample != NULL;
}

void common_hal_audiomixer_mixervoice_stop(audiomixer_mixervoice_obj_t *self) {
    self->sample = NULL;
}

void common_hal_audiomixer_mixervoice_end(audiomixer_mixervoice_obj_t *self) {
    if (self->sample != NULL) {
        self->loop = false;
    }
}
