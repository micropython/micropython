#pragma once

#include "py/obj.h"

typedef struct synthio_note_obj synthio_note_obj_t;
extern const mp_obj_type_t synthio_note_type;
typedef enum synthio_bend_mode_e synthio_bend_mode_t;

mp_float_t common_hal_synthio_note_get_frequency(synthio_note_obj_t *self);
void common_hal_synthio_note_set_frequency(synthio_note_obj_t *self, mp_float_t value);

bool common_hal_synthio_note_get_filter(synthio_note_obj_t *self);
void common_hal_synthio_note_set_filter(synthio_note_obj_t *self, bool value);

mp_float_t common_hal_synthio_note_get_ring_frequency(synthio_note_obj_t *self);
void common_hal_synthio_note_set_ring_frequency(synthio_note_obj_t *self, mp_float_t value);

mp_float_t common_hal_synthio_note_get_panning(synthio_note_obj_t *self);
void common_hal_synthio_note_set_panning(synthio_note_obj_t *self, mp_float_t value);

mp_float_t common_hal_synthio_note_get_tremolo_rate(synthio_note_obj_t *self);
void common_hal_synthio_note_set_tremolo_rate(synthio_note_obj_t *self, mp_float_t value);

mp_float_t common_hal_synthio_note_get_tremolo_depth(synthio_note_obj_t *self);
void common_hal_synthio_note_set_tremolo_depth(synthio_note_obj_t *self, mp_float_t value);

synthio_bend_mode_t common_hal_synthio_note_get_bend_mode(synthio_note_obj_t *self);
void common_hal_synthio_note_set_bend_mode(synthio_note_obj_t *self, synthio_bend_mode_t value);

mp_float_t common_hal_synthio_note_get_bend_rate(synthio_note_obj_t *self);
void common_hal_synthio_note_set_bend_rate(synthio_note_obj_t *self, mp_float_t value);

mp_float_t common_hal_synthio_note_get_bend_depth(synthio_note_obj_t *self);
void common_hal_synthio_note_set_bend_depth(synthio_note_obj_t *self, mp_float_t value);

mp_obj_t common_hal_synthio_note_get_waveform_obj(synthio_note_obj_t *self);
void common_hal_synthio_note_set_waveform(synthio_note_obj_t *self, mp_obj_t value);

mp_obj_t common_hal_synthio_note_get_ring_waveform_obj(synthio_note_obj_t *self);
void common_hal_synthio_note_set_ring_waveform(synthio_note_obj_t *self, mp_obj_t value);

mp_obj_t common_hal_synthio_note_get_envelope_obj(synthio_note_obj_t *self);
void common_hal_synthio_note_set_envelope(synthio_note_obj_t *self, mp_obj_t value);
