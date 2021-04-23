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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_COLORCONVERTER_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_COLORCONVERTER_H

#include "shared-bindings/displayio/__init__.h"
#include "shared-module/displayio/ColorConverter.h"

#include "shared-module/displayio/Palette.h"

extern const mp_obj_type_t displayio_colorconverter_type;

void common_hal_displayio_colorconverter_construct(displayio_colorconverter_t *self, bool dither, displayio_colorspace_t input_colorspace);
void common_hal_displayio_colorconverter_convert(displayio_colorconverter_t *colorconverter, const _displayio_colorspace_t *colorspace, uint32_t input_color, uint32_t *output_color);

void common_hal_displayio_colorconverter_set_dither(displayio_colorconverter_t *self, bool dither);
bool common_hal_displayio_colorconverter_get_dither(displayio_colorconverter_t *self);

void common_hal_displayio_colorconverter_make_transparent(displayio_colorconverter_t *self, uint32_t transparent_color);
void common_hal_displayio_colorconverter_make_opaque(displayio_colorconverter_t *self, uint32_t transparent_color);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_COLORCONVERTER_H
