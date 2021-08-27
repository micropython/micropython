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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_BITMAP_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_BITMAP_H

#include "shared-module/displayio/Bitmap.h"

extern const mp_obj_type_t displayio_bitmap_type;

void common_hal_displayio_bitmap_construct(displayio_bitmap_t *self, uint32_t width,
    uint32_t height, uint32_t bits_per_value);

void common_hal_displayio_bitmap_load_row(displayio_bitmap_t *self, uint16_t y, uint8_t *data,
    uint16_t len);
uint16_t common_hal_displayio_bitmap_get_height(displayio_bitmap_t *self);
uint16_t common_hal_displayio_bitmap_get_width(displayio_bitmap_t *self);
uint32_t common_hal_displayio_bitmap_get_bits_per_value(displayio_bitmap_t *self);
void common_hal_displayio_bitmap_set_pixel(displayio_bitmap_t *bitmap, int16_t x, int16_t y, uint32_t value);
void common_hal_displayio_bitmap_blit(displayio_bitmap_t *self, int16_t x, int16_t y, displayio_bitmap_t *source,
    int16_t x1, int16_t y1, int16_t x2, int16_t y2,
    uint32_t skip_index, bool skip_index_none);
uint32_t common_hal_displayio_bitmap_get_pixel(displayio_bitmap_t *bitmap, int16_t x, int16_t y);
void common_hal_displayio_bitmap_fill(displayio_bitmap_t *bitmap, uint32_t value);
int common_hal_displayio_bitmap_get_buffer(displayio_bitmap_t *self, mp_buffer_info_t *bufinfo, mp_uint_t flags);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_BITMAP_H
