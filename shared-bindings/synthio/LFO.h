// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

typedef struct synthio_lfo_obj synthio_lfo_obj_t;
extern const mp_obj_type_t synthio_lfo_type;

mp_obj_t common_hal_synthio_lfo_get_waveform_obj(synthio_lfo_obj_t *self);
void common_hal_synthio_lfo_set_waveform_obj(synthio_lfo_obj_t *self, mp_obj_t arg);

mp_obj_t common_hal_synthio_lfo_get_rate_obj(synthio_lfo_obj_t *self);
void common_hal_synthio_lfo_set_rate_obj(synthio_lfo_obj_t *self, mp_obj_t arg);

mp_obj_t common_hal_synthio_lfo_get_scale_obj(synthio_lfo_obj_t *self);
void common_hal_synthio_lfo_set_scale_obj(synthio_lfo_obj_t *self, mp_obj_t arg);

mp_obj_t common_hal_synthio_lfo_get_phase_offset_obj(synthio_lfo_obj_t *self);
void common_hal_synthio_lfo_set_phase_offset_obj(synthio_lfo_obj_t *self, mp_obj_t arg);

mp_obj_t common_hal_synthio_lfo_get_offset_obj(synthio_lfo_obj_t *self);
void common_hal_synthio_lfo_set_offset_obj(synthio_lfo_obj_t *self, mp_obj_t arg);

bool common_hal_synthio_lfo_get_once(synthio_lfo_obj_t *self);
void common_hal_synthio_lfo_set_once(synthio_lfo_obj_t *self, bool arg);

bool common_hal_synthio_lfo_get_interpolate(synthio_lfo_obj_t *self);
void common_hal_synthio_lfo_set_interpolate(synthio_lfo_obj_t *self, bool arg);

mp_float_t common_hal_synthio_lfo_get_value(synthio_lfo_obj_t *self);

mp_float_t common_hal_synthio_lfo_get_phase(synthio_lfo_obj_t *self);

void common_hal_synthio_lfo_retrigger(synthio_lfo_obj_t *self);
mp_float_t common_hal_synthio_lfo_tick(mp_obj_t self_in);
