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


mp_float_t synthio_lfo_obj_tick(synthio_lfo_slot_t *slot) {
    mp_obj_t obj = slot->obj;

    if (!mp_obj_is_type(obj, &synthio_lfo_type)) {
        return slot->value;
    }

    synthio_lfo_obj_t *lfo = MP_OBJ_TO_PTR(obj);
    if (lfo->last_tick == synthio_global_tick) {
        slot->value = lfo->value;
        return slot->value;
    }

    lfo->last_tick = synthio_global_tick;

    mp_float_t rate = synthio_lfo_obj_tick(&lfo->rate) * synthio_global_rate_scale;
    ;

    mp_float_t accum = lfo->accum + rate;
    int len = lfo->waveform_bufinfo.len / 2;
    mp_float_t frac = accum - MICROPY_FLOAT_C_FUN(floor)(accum);
    size_t idx = (int)(frac * len);

    if (lfo->once) {
        if (rate > 0) {
            if (accum >= ONE) {
                frac = ONE;
                idx = len - 1;
            }
        } else if (rate < 0 && accum < ZERO) {
            frac = ZERO;
            idx = 0;
        }
    }

    lfo->accum = frac;

    int16_t *waveform = lfo->waveform_bufinfo.buf;
    assert(idx < lfo->waveform_bufinfo.len / 2);
    mp_float_t scale = synthio_lfo_obj_tick(&lfo->scale);
    mp_float_t offset = synthio_lfo_obj_tick(&lfo->offset);
    mp_float_t value = MICROPY_FLOAT_C_FUN(ldexp)(waveform[idx], -15) * scale + offset;

    lfo->value = slot->value = value;
    return value;
}

mp_float_t synthio_lfo_obj_tick_limited(synthio_lfo_slot_t *lfo_slot, mp_float_t lo, mp_float_t hi) {
    mp_float_t value = synthio_lfo_obj_tick(lfo_slot);
    if (value < lo) {
        return lo;
    }
    if (value > hi) {
        return hi;
    }
    return value;
}

int32_t synthio_lfo_obj_tick_scaled(synthio_lfo_slot_t *lfo_slot, mp_float_t lo, mp_float_t hi) {
    mp_float_t value = synthio_lfo_obj_tick_limited(lfo_slot, lo, hi);
    return (int32_t)MICROPY_FLOAT_C_FUN(round)(MICROPY_FLOAT_C_FUN(ldexp)(value, 15));
}

void synthio_lfo_assign_input(mp_obj_t obj, synthio_lfo_slot_t *slot, qstr arg_name) {
    if (mp_obj_is_type(obj, &synthio_lfo_type)) {
        slot->obj = obj;
        return;
    }

    if (!mp_obj_get_float_maybe(obj, &slot->value)) {
        mp_raise_TypeError_varg(translate("%q must be of type %q or %q, not %q"), arg_name, MP_QSTR_Lfo, MP_QSTR_float,  mp_obj_get_type_qstr(obj));
    }

    slot->value = mp_obj_get_float(obj);
    slot->obj = obj;
}

mp_obj_t common_hal_synthio_lfo_get_waveform_obj(synthio_lfo_obj_t *self) {
    return self->waveform_obj;
}

mp_obj_t common_hal_synthio_lfo_get_rate_obj(synthio_lfo_obj_t *self) {
    return self->rate.obj;
}

void common_hal_synthio_lfo_set_rate_obj(synthio_lfo_obj_t *self, mp_obj_t arg) {
    synthio_lfo_assign_input(arg, &self->rate, MP_QSTR_rate);
}

mp_obj_t common_hal_synthio_lfo_get_scale_obj(synthio_lfo_obj_t *self) {
    return self->scale.obj;
}
void common_hal_synthio_lfo_set_scale_obj(synthio_lfo_obj_t *self, mp_obj_t arg) {
    synthio_lfo_assign_input(arg, &self->scale, MP_QSTR_scale);
}

mp_obj_t common_hal_synthio_lfo_get_offset_obj(synthio_lfo_obj_t *self) {
    return self->offset.obj;
}
void common_hal_synthio_lfo_set_offset_obj(synthio_lfo_obj_t *self, mp_obj_t arg) {
    synthio_lfo_assign_input(arg, &self->offset, MP_QSTR_offset);
}

bool common_hal_synthio_lfo_get_once(synthio_lfo_obj_t *self) {
    return self->once;
}
void common_hal_synthio_lfo_set_once(synthio_lfo_obj_t *self, bool arg) {
    self->once = arg;
}

mp_float_t common_hal_synthio_lfo_get_value(synthio_lfo_obj_t *self) {
    return self->value;
}

mp_float_t common_hal_synthio_lfo_get_phase(synthio_lfo_obj_t *self) {
    return self->accum;
}

void common_hal_synthio_lfo_retrigger(synthio_lfo_obj_t *self) {
    self->accum = 0;
}
