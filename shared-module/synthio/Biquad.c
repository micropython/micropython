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
