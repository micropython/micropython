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

#include "shared-bindings/displayio/Shape.h"

#include <string.h>

#include "py/runtime.h"

void common_hal_displayio_shape_construct(displayio_shape_t *self, uint32_t width,
    uint32_t height, bool mirror_x, bool mirror_y) {
    self->mirror_x = mirror_x;
    self->mirror_y = mirror_y;
    self->width = width;
    if (self->mirror_x) {
        width /= 2;
        width += self->width % 2 - 1;
    }
    self->half_width = width;

    self->height = height;
    if (self->mirror_y) {
        height /= 2;
        height += self->height % 2 - 1;
    }
    self->half_height = height;

    self->data = m_malloc(height * sizeof(uint32_t), false);
    for (uint16_t i = 0; i <= height; i++) {
        self->data[2 * i] = 0;
        self->data[2 * i + 1] = width;
    }
}

void common_hal_displayio_shape_set_boundary(displayio_shape_t *self, uint16_t y, uint16_t start_x, uint16_t end_x) {
    if (y < 0 || y >= self->height || (self->mirror_y && y > self->half_height)) {
        mp_raise_ValueError(translate("y value out of bounds"));
    }
    if (start_x < 0 || start_x > self->width || end_x < 0 || end_x > self->width) {
        mp_raise_ValueError(translate("x value out of bounds"));
    }
    uint16_t half_width = self->width / 2 - 1 + self->width % 2;
    if (self->mirror_x && (start_x > half_width || end_x > half_width)) {
        mp_raise_ValueError_varg(translate("Maximum x value when mirrored is %d"), half_width);
    }
    self->data[2 * y] = start_x;
    self->data[2 * y + 1] = end_x;
}

uint32_t common_hal_displayio_shape_get_pixel(void *obj, int16_t x, int16_t y) {
    displayio_shape_t *self = obj;
    if (x >= self->width || x < 0 || y >= self->height || y < 0) {
        return 0;
    }
    if (self->mirror_x && x > self->half_width) {
        x = self->width - 1 - x;
    }
    if (self->mirror_y && y > self->half_height) {
        y = self->height - y - 1;
    }
    uint16_t start_x = self->data[2 * y];
    uint16_t end_x = self->data[2 * y + 1];
    if (x < start_x || x > end_x) {
        return 0;
    }
    return 1;
}
