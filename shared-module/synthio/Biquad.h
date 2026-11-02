
// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-bindings/synthio/Biquad.h"
#include "shared-module/synthio/block.h"

// The coefficients are fixed point with a per-filter number of fractional bits,
// kept in `shift`. It is chosen when the filter is updated, between 1 and this.
#define BIQUAD_MAX_SHIFT (30)
#define BIQUAD_DEFAULT_SHIFT (15)

// The feedback state `y` has this many fractional bits below the output sample.
#define BIQUAD_STATE_SHIFT (12)
#define BIQUAD_STATE_MAX (32767 * (1 << BIQUAD_STATE_SHIFT))
#define BIQUAD_STATE_MIN (-32768 * (1 << BIQUAD_STATE_SHIFT))

typedef struct synthio_biquad {
    mp_obj_base_t base;
    synthio_filter_mode mode;
    synthio_block_slot_t f0, Q, A;
    mp_float_t cached_W0, cached_Q, cached_A;
    int32_t a1, a2, b0, b1, b2;
    uint8_t shift;
} synthio_biquad_t;

typedef struct {
    int32_t x[2];
    int32_t y[2]; // in units of 1 / (1 << BIQUAD_STATE_SHIFT) of a sample
} biquad_filter_state;

void common_hal_synthio_biquad_tick(mp_obj_t self_in);
void synthio_biquad_filter_reset(biquad_filter_state *st);
void synthio_biquad_filter_samples(mp_obj_t self_in, biquad_filter_state *st, int32_t *buffer, size_t n_samples);
int32_t synthio_biquad_filter_sample(mp_obj_t self_in, biquad_filter_state *st, int32_t input);
