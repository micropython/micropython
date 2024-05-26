// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

typedef struct {
    int32_t a1, a2, b0, b1, b2;
    int32_t x[2], y[2];
} biquad_filter_state;

void synthio_biquad_filter_assign(biquad_filter_state *st, mp_obj_t biquad_obj);
void synthio_biquad_filter_reset(biquad_filter_state *st);
void synthio_biquad_filter_samples(biquad_filter_state *st, int32_t *buffer, size_t n_samples);
