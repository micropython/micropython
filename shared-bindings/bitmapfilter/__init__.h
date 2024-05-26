// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/displayio/Bitmap.h"

void shared_module_bitmapfilter_morph(
    displayio_bitmap_t *bitmap,
    displayio_bitmap_t *mask,
    const int ksize,
    const int *krn,
    const mp_float_t m,
    const mp_float_t b,
    bool threshold,
    int offset,
    bool invert);

void shared_module_bitmapfilter_morph9(
    displayio_bitmap_t *bitmap,
    displayio_bitmap_t *mask,
    const int ksize,
    int krn[9 * (2 * ksize + 1) * (2 * ksize + 1)], // Note: modifies krn[]
    const mp_float_t m[3],
    const mp_float_t b[3],
    bool threshold,
    int offset,
    bool invert);

void shared_module_bitmapfilter_mix(
    displayio_bitmap_t *bitmap,
    displayio_bitmap_t *mask,
    const mp_float_t weights[12]);

void shared_module_bitmapfilter_solarize(
    displayio_bitmap_t *bitmap,
    displayio_bitmap_t *mask,
    const mp_float_t threshold);

typedef struct {
    uint8_t r[32], g[64], b[32];
} bitmapfilter_lookup_table_t;

void shared_module_bitmapfilter_lookup(
    displayio_bitmap_t *bitmap,
    displayio_bitmap_t *mask,
    const bitmapfilter_lookup_table_t *table);

void shared_module_bitmapfilter_false_color(
    displayio_bitmap_t *bitmap,
    displayio_bitmap_t *mask,
    _displayio_color_t palette[256]);

void shared_module_bitmapfilter_blend_precompute(mp_obj_t fun, uint8_t lookup[4096]);

void shared_module_bitmapfilter_blend(
    displayio_bitmap_t *dest,
    displayio_bitmap_t *src1,
    displayio_bitmap_t *src2,
    displayio_bitmap_t *mask,
    const uint8_t lookup[4096]);
