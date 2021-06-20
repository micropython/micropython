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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_SHAPE_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_SHAPE_H

#include "shared-module/displayio/Shape.h"

extern const mp_obj_type_t displayio_shape_type;

void common_hal_displayio_shape_construct(displayio_shape_t *self, uint32_t width,
    uint32_t height, bool mirror_x, bool mirror_y);

void common_hal_displayio_shape_set_boundary(displayio_shape_t *self, uint16_t y, uint16_t start_x,
    uint16_t end_x);
uint32_t common_hal_displayio_shape_get_pixel(void *shape, int16_t x, int16_t y);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_SHAPE_H
