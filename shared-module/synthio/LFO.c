// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/synthio/LFO.h"
#include "shared-module/synthio/LFO.h"
#include <math.h>

#define ONE (MICROPY_FLOAT_CONST(1.))
#define ZERO (MICROPY_FLOAT_CONST(0.))

#define ALMOST_ONE (MICROPY_FLOAT_CONST(32767.) / 32768)

mp_float_t common_hal_synthio_lfo_tick(mp_obj_t self_in) {
    synthio_lfo_obj_t *lfo = MP_OBJ_TO_PTR(self_in);

    mp_float_t rate = synthio_block_slot_get(&lfo->rate) * synthio_global_rate_scale;
    mp_float_t phase_offset = synthio_block_slot_get(&lfo->phase_offset);

    mp_float_t accum = lfo->accum + rate + phase_offset;

    if (lfo->once) {
        if (rate > 0) {
            if (accum > ALMOST_ONE) {
                accum = ALMOST_ONE;
            }
        } else if (rate < 0 && accum < ZERO) {
            accum = ZERO;
        }
    } else {
        accum = accum - MICROPY_FLOAT_C_FUN(floor)(accum);
    }
    lfo->accum = accum - phase_offset;

    int len = lfo->waveform_bufinfo.len;

    mp_float_t scaled_accum = accum * (len - lfo->once);
    size_t idx = (size_t)MICROPY_FLOAT_C_FUN(floor)(scaled_accum);
    assert(idx < lfo->waveform_bufinfo.len);

    int16_t *waveform = lfo->waveform_bufinfo.buf;
    mp_float_t value = waveform[idx];

    if (lfo->interpolate) {

        mp_float_t frac = scaled_accum - idx;

        size_t idxp1 = idx + 1;
        if (idxp1 == lfo->waveform_bufinfo.len) {
            idxp1 = lfo->once ? idx : 0;
        }
        value = value * (1 - frac) + waveform[idxp1] * frac;
    }

    mp_float_t scale = synthio_block_slot_get(&lfo->scale);
    mp_float_t offset = synthio_block_slot_get(&lfo->offset);
    value = MICROPY_FLOAT_C_FUN(ldexp)(value, -15) * scale + offset;

    return value;
}

mp_obj_t common_hal_synthio_lfo_get_waveform_obj(synthio_lfo_obj_t *self) {
    return self->waveform_obj;
}

mp_obj_t common_hal_synthio_lfo_get_rate_obj(synthio_lfo_obj_t *self) {
    return self->rate.obj;
}

void common_hal_synthio_lfo_set_rate_obj(synthio_lfo_obj_t *self, mp_obj_t arg) {
    synthio_block_assign_slot(arg, &self->rate, MP_QSTR_rate);
}

mp_obj_t common_hal_synthio_lfo_get_scale_obj(synthio_lfo_obj_t *self) {
    return self->scale.obj;
}
void common_hal_synthio_lfo_set_scale_obj(synthio_lfo_obj_t *self, mp_obj_t arg) {
    synthio_block_assign_slot(arg, &self->scale, MP_QSTR_scale);
}

mp_obj_t common_hal_synthio_lfo_get_phase_offset_obj(synthio_lfo_obj_t *self) {
    return self->phase_offset.obj;
}
void common_hal_synthio_lfo_set_phase_offset_obj(synthio_lfo_obj_t *self, mp_obj_t arg) {
    synthio_block_assign_slot(arg, &self->phase_offset, MP_QSTR_phase_offset);
}

mp_obj_t common_hal_synthio_lfo_get_offset_obj(synthio_lfo_obj_t *self) {
    return self->offset.obj;
}
void common_hal_synthio_lfo_set_offset_obj(synthio_lfo_obj_t *self, mp_obj_t arg) {
    synthio_block_assign_slot(arg, &self->offset, MP_QSTR_offset);
}

bool common_hal_synthio_lfo_get_once(synthio_lfo_obj_t *self) {
    return self->once;
}
void common_hal_synthio_lfo_set_once(synthio_lfo_obj_t *self, bool arg) {
    self->once = arg;
}

bool common_hal_synthio_lfo_get_interpolate(synthio_lfo_obj_t *self) {
    return self->interpolate;
}
void common_hal_synthio_lfo_set_interpolate(synthio_lfo_obj_t *self, bool arg) {
    self->interpolate = arg;
}

mp_float_t common_hal_synthio_lfo_get_value(synthio_lfo_obj_t *self) {
    return self->base.value;
}

mp_float_t common_hal_synthio_lfo_get_phase(synthio_lfo_obj_t *self) {
    return self->accum;
}

void common_hal_synthio_lfo_retrigger(synthio_lfo_obj_t *self) {
    self->accum = 0;
}
