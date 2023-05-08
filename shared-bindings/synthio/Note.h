#pragma once

#include "py/obj.h"

typedef struct synthio_note_obj synthio_note_obj_t;
extern const mp_obj_type_t synthio_note_type;

mp_float_t common_hal_synthio_note_get_frequency(synthio_note_obj_t *self);
void common_hal_synthio_note_set_frequency(synthio_note_obj_t *self, mp_float_t value);

mp_float_t common_hal_synthio_note_get_amplitude(synthio_note_obj_t *self);
void common_hal_synthio_note_set_amplitude(synthio_note_obj_t *self, mp_float_t value);

mp_float_t common_hal_synthio_note_get_tremolo_rate(synthio_note_obj_t *self);
void common_hal_synthio_note_set_tremolo_rate(synthio_note_obj_t *self, mp_float_t value);

mp_float_t common_hal_synthio_note_get_tremolo_depth(synthio_note_obj_t *self);
void common_hal_synthio_note_set_tremolo_depth(synthio_note_obj_t *self, mp_float_t value);

mp_float_t common_hal_synthio_note_get_vibrato_rate(synthio_note_obj_t *self);
void common_hal_synthio_note_set_vibrato_rate(synthio_note_obj_t *self, mp_float_t value);

mp_float_t common_hal_synthio_note_get_vibrato_depth(synthio_note_obj_t *self);
void common_hal_synthio_note_set_vibrato_depth(synthio_note_obj_t *self, mp_float_t value);

mp_obj_t common_hal_synthio_note_get_waveform_obj(synthio_note_obj_t *self);
void common_hal_synthio_note_set_waveform(synthio_note_obj_t *self, mp_obj_t value);

mp_obj_t common_hal_synthio_note_get_envelope_obj(synthio_note_obj_t *self);
void common_hal_synthio_note_set_envelope(synthio_note_obj_t *self, mp_obj_t value);
