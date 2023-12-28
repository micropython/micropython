/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include <math.h>
#include "shared-bindings/synthio/Biquad.h"
#include "shared-module/synthio/Biquad.h"

mp_obj_t common_hal_synthio_new_lpf(mp_float_t w0, mp_float_t Q) {
    mp_float_t s = MICROPY_FLOAT_C_FUN(sin)(w0);
    mp_float_t c = MICROPY_FLOAT_C_FUN(cos)(w0);
    mp_float_t alpha = s / (2 * Q);
    mp_float_t a0 = 1 + alpha;
    mp_float_t a1 = -2 * c;
    mp_float_t a2 = 1 - alpha;
    mp_float_t b0 = (1 - c) / 2;
    mp_float_t b1 = 1 - c;
    mp_float_t b2 = (1 - c) / 2;

    mp_obj_t out_args[] = {
        mp_obj_new_float(a1 / a0),
        mp_obj_new_float(a2 / a0),
        mp_obj_new_float(b0 / a0),
        mp_obj_new_float(b1 / a0),
        mp_obj_new_float(b2 / a0),
    };

    return namedtuple_make_new((const mp_obj_type_t *)&synthio_biquad_type_obj, MP_ARRAY_SIZE(out_args), 0, out_args);
}

mp_obj_t common_hal_synthio_new_hpf(mp_float_t w0, mp_float_t Q) {
    mp_float_t s = MICROPY_FLOAT_C_FUN(sin)(w0);
    mp_float_t c = MICROPY_FLOAT_C_FUN(cos)(w0);
    mp_float_t alpha = s / (2 * Q);
    mp_float_t a0 = 1 + alpha;
    mp_float_t a1 = -2 * c;
    mp_float_t a2 = 1 - alpha;
    mp_float_t b0 = (1 + c) / 2;
    mp_float_t b1 = -(1 + c);
    mp_float_t b2 = (1 + c) / 2;

    mp_obj_t out_args[] = {
        mp_obj_new_float(a1 / a0),
        mp_obj_new_float(a2 / a0),
        mp_obj_new_float(b0 / a0),
        mp_obj_new_float(b1 / a0),
        mp_obj_new_float(b2 / a0),
    };

    return namedtuple_make_new((const mp_obj_type_t *)&synthio_biquad_type_obj, MP_ARRAY_SIZE(out_args), 0, out_args);
}

mp_obj_t common_hal_synthio_new_bpf(mp_float_t w0, mp_float_t Q) {
    mp_float_t s = MICROPY_FLOAT_C_FUN(sin)(w0);
    mp_float_t c = MICROPY_FLOAT_C_FUN(cos)(w0);
    mp_float_t alpha = s / (2 * Q);
    mp_float_t a0 = 1 + alpha;
    mp_float_t a1 = -2 * c;
    mp_float_t a2 = 1 - alpha;
    mp_float_t b0 = alpha;
    mp_float_t b1 = 0;
    mp_float_t b2 = -alpha;

    mp_obj_t out_args[] = {
        mp_obj_new_float(a1 / a0),
        mp_obj_new_float(a2 / a0),
        mp_obj_new_float(b0 / a0),
        mp_obj_new_float(b1 / a0),
        mp_obj_new_float(b2 / a0),
    };

    return namedtuple_make_new((const mp_obj_type_t *)&synthio_biquad_type_obj, MP_ARRAY_SIZE(out_args), 0, out_args);
}

#define BIQUAD_SHIFT (15)
STATIC int32_t biquad_scale_arg_obj(mp_obj_t arg) {
    return (int32_t)MICROPY_FLOAT_C_FUN(round)(MICROPY_FLOAT_C_FUN(ldexp)(mp_obj_get_float(arg), BIQUAD_SHIFT));
}
void synthio_biquad_filter_assign(biquad_filter_state *st, mp_obj_t biquad_obj) {
    if (biquad_obj != mp_const_none) {
        mp_arg_validate_type(biquad_obj, (const mp_obj_type_t *)&synthio_biquad_type_obj, MP_QSTR_filter);
        mp_obj_tuple_t *biquad = (mp_obj_tuple_t *)MP_OBJ_TO_PTR(biquad_obj);
        st->a1 = biquad_scale_arg_obj(biquad->items[0]);
        st->a2 = biquad_scale_arg_obj(biquad->items[1]);
        st->b0 = biquad_scale_arg_obj(biquad->items[2]);
        st->b1 = biquad_scale_arg_obj(biquad->items[3]);
        st->b2 = biquad_scale_arg_obj(biquad->items[4]);
    }
}

void synthio_biquad_filter_reset(biquad_filter_state *st) {
    memset(&st->x, 0, 4 * sizeof(int16_t));
}

void synthio_biquad_filter_samples(biquad_filter_state *st, int32_t *buffer, size_t n_samples) {
    int32_t a1 = st->a1;
    int32_t a2 = st->a2;
    int32_t b0 = st->b0;
    int32_t b1 = st->b1;
    int32_t b2 = st->b2;

    int32_t x0 = st->x[0];
    int32_t x1 = st->x[1];
    int32_t y0 = st->y[0];
    int32_t y1 = st->y[1];

    for (size_t n = n_samples; n; --n, ++buffer) {
        int32_t input = *buffer;
        int32_t output = (b0 * input + b1 * x0 + b2 * x1 - a1 * y0 - a2 * y1 + (1 << (BIQUAD_SHIFT - 1))) >> BIQUAD_SHIFT;

        x1 = x0;
        x0 = input;
        y1 = y0;
        y0 = output;
        *buffer = output;
    }
    st->x[0] = x0;
    st->x[1] = x1;
    st->y[0] = y0;
    st->y[1] = y1;
}
