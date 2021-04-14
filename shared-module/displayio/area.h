/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SHARED_MODULE_DISPLAYIO_AREA_H
#define MICROPY_INCLUDED_SHARED_MODULE_DISPLAYIO_AREA_H

// Implementations are in __init__.c
typedef struct _displayio_area_t displayio_area_t;

struct _displayio_area_t {
    int16_t x1;
    int16_t y1;
    int16_t x2; // Second point is exclusive.
    int16_t y2;
    const displayio_area_t *next; // Next area in the linked list.
};

typedef struct {
    uint16_t x;
    uint16_t y;
    int8_t dx;
    int8_t dy;
    uint8_t scale;
    uint16_t width;
    uint16_t height;
    bool mirror_x;
    bool mirror_y;
    bool transpose_xy;
} displayio_buffer_transform_t;

extern displayio_buffer_transform_t null_transform;

bool displayio_area_empty(const displayio_area_t *a);
void displayio_area_copy_coords(const displayio_area_t *src, displayio_area_t *dest);
void displayio_area_canon(displayio_area_t *a);
void displayio_area_union(const displayio_area_t *a,
    const displayio_area_t *b,
    displayio_area_t *u);
void displayio_area_copy(const displayio_area_t *src, displayio_area_t *dst);
void displayio_area_scale(displayio_area_t *area, uint16_t scale);
void displayio_area_shift(displayio_area_t *area, int16_t dx, int16_t dy);
bool displayio_area_compute_overlap(const displayio_area_t *a,
    const displayio_area_t *b,
    displayio_area_t *overlap);
uint16_t displayio_area_width(const displayio_area_t *area);
uint16_t displayio_area_height(const displayio_area_t *area);
uint32_t displayio_area_size(const displayio_area_t *area);
bool displayio_area_equal(const displayio_area_t *a, const displayio_area_t *b);
void displayio_area_transform_within(bool mirror_x, bool mirror_y, bool transpose_xy,
    const displayio_area_t *original,
    const displayio_area_t *whole,
    displayio_area_t *transformed);

#endif // MICROPY_INCLUDED_SHARED_MODULE_DISPLAYIO_AREA_H
