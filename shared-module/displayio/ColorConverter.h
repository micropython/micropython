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

#ifndef MICROPY_INCLUDED_SHARED_MODULE_DISPLAYIO_COLORCONVERTER_H
#define MICROPY_INCLUDED_SHARED_MODULE_DISPLAYIO_COLORCONVERTER_H

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"
#include "shared-module/displayio/Palette.h"

typedef struct displayio_colorconverter {
    mp_obj_base_t base;
    bool dither;
    uint8_t input_colorspace;
    uint32_t transparent_color;
} displayio_colorconverter_t;

bool displayio_colorconverter_needs_refresh(displayio_colorconverter_t *self);
void displayio_colorconverter_finish_refresh(displayio_colorconverter_t *self);
void displayio_colorconverter_convert(displayio_colorconverter_t *self, const _displayio_colorspace_t *colorspace, const displayio_input_pixel_t *input_pixel, displayio_output_pixel_t *output_color);

uint32_t displayio_colorconverter_dither_noise_1(uint32_t n);
uint32_t displayio_colorconverter_dither_noise_2(uint32_t x, uint32_t y);

uint16_t displayio_colorconverter_compute_rgb565(uint32_t color_rgb888);
uint8_t displayio_colorconverter_compute_luma(uint32_t color_rgb888);
uint8_t displayio_colorconverter_compute_chroma(uint32_t color_rgb888);
uint8_t displayio_colorconverter_compute_hue(uint32_t color_rgb888);
void displayio_colorconverter_compute_tricolor(const _displayio_colorspace_t *colorspace, uint8_t pixel_hue, uint8_t pixel_luma, uint32_t *color);

#endif // MICROPY_INCLUDED_SHARED_MODULE_DISPLAYIO_COLORCONVERTER_H
