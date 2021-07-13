/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SHARED_MODULE_DISPLAYIO_PALETTE_H
#define MICROPY_INCLUDED_SHARED_MODULE_DISPLAYIO_PALETTE_H

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"

typedef struct {
    uint8_t depth;
    uint8_t bytes_per_cell;
    uint8_t tricolor_hue;
    uint8_t tricolor_luma;
    uint8_t grayscale_bit; // The lowest grayscale bit. Normally 8 - depth.
    bool grayscale;
    bool tricolor;
    bool pixels_in_byte_share_row;
    bool reverse_pixels_in_byte;
    bool reverse_bytes_in_word;
    bool dither;
} _displayio_colorspace_t;

typedef struct {
    uint32_t rgb888;
    uint16_t rgb565;
    uint8_t luma;
    uint8_t hue;
    uint8_t chroma;
    bool transparent; // This may have additional bits added later for blending.
} _displayio_color_t;

typedef struct {
    uint32_t pixel;
    uint16_t x;
    uint16_t y;
    uint8_t tile;
    uint16_t tile_x;
    uint16_t tile_y;
} displayio_input_pixel_t;

typedef struct {
    uint32_t pixel;
    bool opaque;
} displayio_output_pixel_t;

typedef struct displayio_palette {
    mp_obj_base_t base;
    _displayio_color_t *colors;
    uint32_t color_count;
    bool needs_refresh;
} displayio_palette_t;

// Returns false if color fetch did not succeed (out of range or transparent).
// Returns true if color is opaque, and sets color.
bool displayio_palette_get_color(displayio_palette_t *palette, const _displayio_colorspace_t *colorspace, uint32_t palette_index, uint32_t *color);
bool displayio_palette_needs_refresh(displayio_palette_t *self);
void displayio_palette_finish_refresh(displayio_palette_t *self);

#endif // MICROPY_INCLUDED_SHARED_MODULE_DISPLAYIO_PALLETE_H
