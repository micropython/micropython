/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Jeff Epler for Adafruit Industries
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

#include "shared-bindings/synthio/LFO.h"
#include "shared-module/synthio/LFO.h"
#include <math.h>

#define ONE (MICROPY_FLOAT_CONST(1.))
#define ZERO (MICROPY_FLOAT_CONST(0.))

mp_float_t common_hal_synthio_lfo_tick(mp_obj_t self_in) {
    synthio_lfo_obj_t *lfo = MP_OBJ_TO_PTR(self_in);

    mp_float_t rate = synthio_block_slot_get(&lfo->rate) * synthio_global_rate_scale;
    mp_float_t phase_offset = synthio_block_slot_get(&lfo->phase_offset);

    mp_float_t accum = lfo->accum + rate + phase_offset;

    if (lfo->once) {
        if (rate > 0) {
            if (accum >= ONE) {
                accum = ONE;
            }
        } else if (rate < 0 && accum < ZERO) {
            accum = ZERO;
        }
    } else {
        accum = accum - MICROPY_FLOAT_C_FUN(floor)(accum);
    }

    int len = lfo->waveform_bufinfo.len;
    size_t idx = (int)(accum * len); // rounds down towards zero
    assert(idx < lfo->waveform_bufinfo.len);

    mp_float_t scale = synthio_block_slot_get(&lfo->scale);
    mp_float_t offset = synthio_block_slot_get(&lfo->offset);
    int16_t *waveform = lfo->waveform_bufinfo.buf;
    mp_float_t value = MICROPY_FLOAT_C_FUN(ldexp)(waveform[idx], -15) * scale + offset;

    lfo->accum = accum - phase_offset;
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

mp_float_t common_hal_synthio_lfo_get_value(synthio_lfo_obj_t *self) {
    return self->base.value;
}

mp_float_t common_hal_synthio_lfo_get_phase(synthio_lfo_obj_t *self) {
    return self->accum;
}

void common_hal_synthio_lfo_retrigger(synthio_lfo_obj_t *self) {
    self->accum = 0;
}
