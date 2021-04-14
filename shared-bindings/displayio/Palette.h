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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_PALETTE_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_PALETTE_H

#include "shared-module/displayio/Palette.h"

extern const mp_obj_type_t displayio_palette_type;

void common_hal_displayio_palette_construct(displayio_palette_t *self, uint16_t color_count);
void common_hal_displayio_palette_set_color(displayio_palette_t *self, uint32_t palette_index, uint32_t color);
uint32_t common_hal_displayio_palette_get_color(displayio_palette_t *self, uint32_t palette_index);
uint32_t common_hal_displayio_palette_get_len(displayio_palette_t *self);

void common_hal_displayio_palette_make_opaque(displayio_palette_t *self, uint32_t palette_index);
void common_hal_displayio_palette_make_transparent(displayio_palette_t *self, uint32_t palette_index);
bool common_hal_displayio_palette_is_transparent(displayio_palette_t *self, uint32_t palette_index);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_PALETTE_H
