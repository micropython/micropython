#pragma once

#include "py/obj.h"

typedef struct synthio_note_obj synthio_note_obj_t;
extern const mp_obj_type_t synthio_note_type;
typedef enum synthio_bend_mode_e synthio_bend_mode_t;

mp_float_t common_hal_synthio_note_get_frequency(synthio_note_obj_t *self);
void common_hal_synthio_note_set_frequency(synthio_note_obj_t *self, mp_float_t value);

bool common_hal_synthio_note_get_filter(synthio_note_obj_t *self);
void common_hal_synthio_note_set_filter(synthio_note_obj_t *self, bool value);

mp_obj_t common_hal_synthio_note_get_panning(synthio_note_obj_t *self);
void common_hal_synthio_note_set_panning(synthio_note_obj_t *self, mp_obj_t value);

mp_obj_t common_hal_synthio_note_get_amplitude(synthio_note_obj_t *self);
void common_hal_synthio_note_set_amplitude(synthio_note_obj_t *self, mp_obj_t value);

mp_obj_t common_hal_synthio_note_get_bend(synthio_note_obj_t *self);
void common_hal_synthio_note_set_bend(synthio_note_obj_t *self, mp_obj_t value);

mp_obj_t common_hal_synthio_note_get_waveform_obj(synthio_note_obj_t *self);
void common_hal_synthio_note_set_waveform(synthio_note_obj_t *self, mp_obj_t value);

mp_float_t common_hal_synthio_note_get_ring_frequency(synthio_note_obj_t *self);
void common_hal_synthio_note_set_ring_frequency(synthio_note_obj_t *self, mp_float_t value);

mp_obj_t common_hal_synthio_note_get_ring_bend(synthio_note_obj_t *self);
void common_hal_synthio_note_set_ring_bend(synthio_note_obj_t *self, mp_obj_t value);

mp_obj_t common_hal_synthio_note_get_ring_waveform_obj(synthio_note_obj_t *self);
void common_hal_synthio_note_set_ring_waveform(synthio_note_obj_t *self, mp_obj_t value);

mp_obj_t common_hal_synthio_note_get_envelope_obj(synthio_note_obj_t *self);
void common_hal_synthio_note_set_envelope(synthio_note_obj_t *self, mp_obj_t value);
