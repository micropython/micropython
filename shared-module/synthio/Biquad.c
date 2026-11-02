
// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <math.h>
#include "shared-bindings/synthio/Biquad.h"
#include "shared-module/synthio/Biquad.h"
#include "shared-module/synthio/block.h"
#include "shared-bindings/synthio/__init__.h"

typedef struct {
    mp_float_t s, c;
} sincos_result_t;

#include <stdint.h>  // uint32_t

// The famous Quake approximate square root function
static mp_float_t Q_rsqrt(mp_float_t number_in) {
    float number = (float)number_in;
    union {
        float f;
        uint32_t i;
    } conv = { .f = (float)number };
    conv.i = 0x5f3759df - (conv.i >> 1);
    conv.f *= 1.5F - (number * 0.5F * conv.f * conv.f);
    return (mp_float_t)conv.f;
}

static mp_float_t fast_sqrt(mp_float_t number) {
    return number * Q_rsqrt(number);
}

// Sine and cosine of theta, evaluated together. theta is the filter's angle
// 2 * pi * f / sample_rate, which is in [0, pi] up to Nyquist.
//
// Reflect into [0, pi / 2] (sin(pi - t) = sin(t), cos(pi - t) = -cos(t)), then
// into [0, pi / 4] by swapping the two (sin(pi / 2 - t) = cos(t)). On that
// interval a four-term polynomial for each, fitted to minimise the worst-case
// error, is accurate to the last bit or two of a float.
static void fast_sincos(mp_float_t theta, sincos_result_t *result) {
    bool negate_cos = theta > (M_PI / 2);
    mp_float_t t = negate_cos ? (M_PI - theta) : theta;
    bool swap = t > (M_PI / 4);
    if (swap) {
        t = (M_PI / 2) - t;
    }
    mp_float_t t2 = t * t;
    mp_float_t s = t * (1 + t2 * (-0.16666651 + t2 * (0.0083319786 + t2 * -0.00019495635)));
    mp_float_t c = 1 + t2 * (-0.5 + t2 * (0.041666647 + t2 * (-0.0013887368 + t2 * 0.000024438452)));
    mp_float_t sin_theta = swap ? c : s;
    mp_float_t cos_theta = swap ? s : c;
    result->s = sin_theta;
    result->c = negate_cos ? -cos_theta : cos_theta;
}

mp_obj_t common_hal_synthio_biquad_new(synthio_filter_mode mode) {
    synthio_biquad_t *self = mp_obj_malloc(synthio_biquad_t, &synthio_biquad_type_obj);
    self->mode = mode;
    self->shift = BIQUAD_DEFAULT_SHIFT;
    return MP_OBJ_FROM_PTR(self);
}

synthio_filter_mode common_hal_synthio_biquad_get_mode(synthio_biquad_t *self) {
    return self->mode;
}

mp_obj_t common_hal_synthio_biquad_get_Q(synthio_biquad_t *self) {
    return self->Q.obj;
}

void common_hal_synthio_biquad_set_Q(synthio_biquad_t *self, mp_obj_t Q) {
    synthio_block_assign_slot(Q, &self->Q, MP_QSTR_Q);
}

mp_obj_t common_hal_synthio_biquad_get_A(synthio_biquad_t *self) {
    return self->A.obj;
}

void common_hal_synthio_biquad_set_A(synthio_biquad_t *self, mp_obj_t A) {
    synthio_block_assign_slot(A, &self->A, MP_QSTR_A);
}

mp_obj_t common_hal_synthio_biquad_get_frequency(synthio_biquad_t *self) {
    return self->f0.obj;
}

void common_hal_synthio_biquad_set_frequency(synthio_biquad_t *self, mp_obj_t frequency) {
    synthio_block_assign_slot(frequency, &self->f0, MP_QSTR_frequency);
}

static int32_t biquad_scale_arg_float(mp_float_t arg, int shift) {
    mp_float_t scaled = MICROPY_FLOAT_C_FUN(round)(MICROPY_FLOAT_C_FUN(ldexp)(arg, shift));
    // biquad_choose_shift() leaves room for this; the clamp is so that a
    // rounding tick at the very top of the range cannot wrap the sign.
    if (scaled >= 2147483648.0) {
        return INT32_MAX;
    }
    if (scaled <= -2147483648.0) {
        return INT32_MIN;
    }
    return (int32_t)scaled;
}

// The most fractional bits that all five coefficients can share and still fit
// in an int32_t, up to BIQUAD_MAX_SHIFT. A plain low-pass tops out just under 2
// (that is a1) and gets 30; a shelf with 24 dB of gain reaches almost 32 and gets 26.
static int biquad_choose_shift(const mp_float_t *coefficients, size_t count) {
    mp_float_t largest = 1;
    for (size_t i = 0; i < count; i++) {
        mp_float_t magnitude = MICROPY_FLOAT_C_FUN(fabs)(coefficients[i]);
        if (magnitude > largest) {
            largest = magnitude;
        }
    }
    int exponent;
    MICROPY_FLOAT_C_FUN(frexp)(largest, &exponent); // largest < 2 ** exponent
    int shift = 31 - exponent;
    if (shift > BIQUAD_MAX_SHIFT) {
        shift = BIQUAD_MAX_SHIFT;
    }
    // The rounding term in the filter needs at least one bit.
    return shift < 1 ? 1 : shift;
}

static int float_equal_or_update(
    mp_float_t *cached,
    mp_float_t new) {
    // uses memcmp to avoid error about equality float comparison
    if (memcmp(cached, &new, sizeof(mp_float_t))) {
        *cached = new;
        return false;
    }
    return true;
}

void common_hal_synthio_biquad_tick(mp_obj_t self_in) {
    synthio_biquad_t *self = MP_OBJ_TO_PTR(self_in);

    mp_float_t W0 = synthio_block_slot_get(&self->f0) * synthio_global_W_scale;
    mp_float_t Q = synthio_block_slot_get(&self->Q);
    mp_float_t A =
        (self->mode >= SYNTHIO_PEAKING_EQ) ? synthio_block_slot_get(&self->A) : 0;

    // n.b., assumes that the `mode` field is read-only
    // n.b., use of `&` is deliberate, avoids short-circuiting behavior
    if (float_equal_or_update(&self->cached_W0, W0)
        & float_equal_or_update(&self->cached_Q, Q)
        & float_equal_or_update(&self->cached_A, A)) {
        return;
    }

    sincos_result_t sc;
    fast_sincos(W0, &sc);

    mp_float_t alpha = sc.s / (2 * Q);

    mp_float_t a0, a1, a2, b0, b1, b2;

    switch (self->mode) {
        default:
            a0 = 1 + alpha;
            a1 = -2 * sc.c;
            a2 = 1 - alpha;

            switch (self->mode) {
                default:
                case SYNTHIO_LOW_PASS:
                    b2 = b0 = (1 - sc.c) * .5;
                    b1 = 1 - sc.c;
                    break;

                case SYNTHIO_HIGH_PASS:
                    b2 = b0 = (1 + sc.c) * .5;
                    b1 = -(1 + sc.c);
                    break;

                case SYNTHIO_BAND_PASS:
                    b0 = alpha;
                    b1 = 0;
                    b2 = -b0;
                    break;

                case SYNTHIO_NOTCH:
                    b0 = 1;
                    b1 = -2 * sc.c;
                    b2 = 1;
            }

            break;

        case SYNTHIO_PEAKING_EQ:
            b0 = 1 + alpha * A;
            b1 = -2 * sc.c;
            b2 = 1 - alpha * A;
            a0 = 1 + alpha / A;
            a1 = -2 * sc.c;
            a2 = 1 - alpha / A;
            break;

        case SYNTHIO_LOW_SHELF: {
            mp_float_t sqrt_A = fast_sqrt(A);
            b0 = A * ((A + 1) - (A - 1) * sc.c + 2 * sqrt_A * alpha);
            b1 = 2 * A * ((A - 1) - (A + 1) * sc.c);
            b2 = A * ((A + 1) - (A - 1) * sc.c - 2 * sqrt_A * alpha);
            a0 = (A + 1) + (A - 1) * sc.c + 2 * sqrt_A * alpha;
            a1 = -2 * ((A - 1) + (A + 1) * sc.c);
            a2 = (A + 1) + (A - 1) * sc.c - 2 * sqrt_A * alpha;
        }
        break;

        case SYNTHIO_HIGH_SHELF: {
            mp_float_t sqrt_A = fast_sqrt(A);
            b0 = A * ((A + 1) + (A - 1) * sc.c + 2 * sqrt_A * alpha);
            b1 = -2 * A * ((A - 1) + (A + 1) * sc.c);
            b2 = A * ((A + 1) + (A - 1) * sc.c - 2 * sqrt_A * alpha);
            a0 = (A + 1) - (A - 1) * sc.c + 2 * sqrt_A * alpha;
            a1 = 2 * ((A - 1) - (A + 1) * sc.c);
            a2 = (A + 1) - (A - 1) * sc.c - 2 * sqrt_A * alpha;
        }
        break;
    }
    mp_float_t recip_a0 = 1 / a0;

    a1 *= recip_a0;
    a2 *= recip_a0;
    b0 *= recip_a0;
    b1 *= recip_a0;
    b2 *= recip_a0;

    mp_float_t coefficients[] = {a1, a2, b0, b1, b2};
    int shift = biquad_choose_shift(coefficients, MP_ARRAY_SIZE(coefficients));
    self->shift = shift;
    self->a1 = biquad_scale_arg_float(a1, shift);
    self->a2 = biquad_scale_arg_float(a2, shift);
    self->b0 = biquad_scale_arg_float(b0, shift);
    self->b1 = biquad_scale_arg_float(b1, shift);
    self->b2 = biquad_scale_arg_float(b2, shift);
}

void synthio_biquad_filter_reset(biquad_filter_state *st) {
    memset(st, 0, sizeof(*st));
}

// Returns the new feedback state: the output with BIQUAD_STATE_SHIFT bits
// below the sample. Near 0 Hz a biquad turns each unit of error in its feedback
// into thousands of units of output, so the feedback must not be rounded to
// whole samples. The coefficients can have up to 30 fractional bits, so the
// sum of products needs 64 bits.
static inline int32_t biquad_filter_sample(int32_t input, int32_t a1, int32_t a2, int32_t b0, int32_t b1, int32_t b2, int shift, int32_t x0, int32_t x1, int32_t y0, int32_t y1) {
    int64_t acc = ((int64_t)b0 * input + (int64_t)b1 * x0 + (int64_t)b2 * x1) * (1 << BIQUAD_STATE_SHIFT)
        - (int64_t)a1 * y0 - (int64_t)a2 * y1;
    // Add half a step, then shift, to round to nearest.
    acc = (acc + ((int64_t)1 << (shift - 1))) >> shift;
    if (acc > BIQUAD_STATE_MAX) {
        return BIQUAD_STATE_MAX;
    }
    if (acc < BIQUAD_STATE_MIN) {
        return BIQUAD_STATE_MIN;
    }
    return acc;
}

// The state is already within the range of a sample, so rounding is all it needs.
static inline int32_t biquad_state_to_sample(int32_t y) {
    return (y + (1 << (BIQUAD_STATE_SHIFT - 1))) >> BIQUAD_STATE_SHIFT;
}

void synthio_biquad_filter_samples(mp_obj_t self_in, biquad_filter_state *st, int32_t *buffer, size_t n_samples) {
    synthio_biquad_t *self = MP_OBJ_TO_PTR(self_in);

    int32_t a1 = self->a1;
    int32_t a2 = self->a2;
    int32_t b0 = self->b0;
    int32_t b1 = self->b1;
    int32_t b2 = self->b2;
    int shift = self->shift;

    int32_t x0 = st->x[0];
    int32_t x1 = st->x[1];
    int32_t y0 = st->y[0];
    int32_t y1 = st->y[1];

    for (size_t n = n_samples; n; --n, ++buffer) {
        int32_t input = *buffer;
        int32_t y = biquad_filter_sample(input, a1, a2, b0, b1, b2, shift, x0, x1, y0, y1);

        x1 = x0;
        x0 = input;
        y1 = y0;
        y0 = y;
        *buffer = biquad_state_to_sample(y);
    }
    st->x[0] = x0;
    st->x[1] = x1;
    st->y[0] = y0;
    st->y[1] = y1;
}

int32_t synthio_biquad_filter_sample(mp_obj_t self_in, biquad_filter_state *st, int32_t input) {
    synthio_biquad_t *self = MP_OBJ_TO_PTR(self_in);

    int32_t y = biquad_filter_sample(input, self->a1, self->a2, self->b0, self->b1, self->b2, self->shift, st->x[0], st->x[1], st->y[0], st->y[1]);

    st->x[1] = st->x[0];
    st->x[0] = input;
    st->y[1] = st->y[0];
    st->y[0] = y;

    return biquad_state_to_sample(y);
}
