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

#include "shared-bindings/displayio/Bitmap.h"

#include <string.h>

#include "py/runtime.h"

void common_hal_displayio_bitmap_construct(displayio_bitmap_t *self, uint32_t width,
    uint32_t height, uint32_t bits_per_value) {
    uint32_t row_width = width * bits_per_value;
    // align to size_t
    uint8_t align_bits = 8 * sizeof(size_t);
    if (row_width % align_bits != 0) {
        self->stride = (row_width / align_bits + 1);
    } else {
        self->stride = row_width / align_bits;
    }
    self->width = width;
    self->height = height;
    self->data = m_malloc(self->stride * height * sizeof(size_t), false);
    self->read_only = false;
    self->bits_per_value = bits_per_value;

    if (bits_per_value > 8 && bits_per_value != 16 && bits_per_value != 32) {
        mp_raise_NotImplementedError(translate("Invalid bits per value"));
    }

    // Division and modulus can be slow because it has to handle any integer. We know bits_per_value
    // is a power of two. We divide and mod by bits_per_value to compute the offset into the byte
    // array. So, we can the offset computation to simplify to a shift for division and mask for mod.

    self->x_shift = 0; // Used to divide the index by the number of pixels per word. Its used in a
                       // shift which effectively divides by 2 ** x_shift.
    uint32_t power_of_two = 1;
    while (power_of_two < align_bits / bits_per_value) {
        self->x_shift++;
        power_of_two <<= 1;
    }
    self->x_mask = (1 << self->x_shift) - 1; // Used as a modulus on the x value
    self->bitmask = (1 << bits_per_value) - 1;

    self->dirty_area.x1 = 0;
    self->dirty_area.x2 = width;
    self->dirty_area.y1 = 0;
    self->dirty_area.y2 = height;
}

uint16_t common_hal_displayio_bitmap_get_height(displayio_bitmap_t *self) {
    return self->height;
}

uint16_t common_hal_displayio_bitmap_get_width(displayio_bitmap_t *self) {
    return self->width;
}

uint32_t common_hal_displayio_bitmap_get_bits_per_value(displayio_bitmap_t *self) {
    return self->bits_per_value;
}

uint32_t common_hal_displayio_bitmap_get_pixel(displayio_bitmap_t *self, int16_t x, int16_t y) {
    if (x >= self->width || x < 0 || y >= self->height || y < 0) {
        return 0;
    }
    int32_t row_start = y * self->stride;
    uint32_t bytes_per_value = self->bits_per_value / 8;
    if (bytes_per_value < 1) {
        size_t word = self->data[row_start + (x >> self->x_shift)];

        return (word >> (sizeof(size_t) * 8 - ((x & self->x_mask) + 1) * self->bits_per_value)) & self->bitmask;
    } else {
        size_t *row = self->data + row_start;
        if (bytes_per_value == 1) {
            return ((uint8_t *)row)[x];
        } else if (bytes_per_value == 2) {
            return ((uint16_t *)row)[x];
        } else if (bytes_per_value == 4) {
            return ((uint32_t *)row)[x];
        }
    }
    return 0;
}

void displayio_bitmap_set_dirty_area(displayio_bitmap_t *self, const displayio_area_t *dirty_area) {
    if (self->read_only) {
        mp_raise_RuntimeError(translate("Read-only object"));
    }

    displayio_area_t area = *dirty_area;
    displayio_area_canon(&area);
    displayio_area_union(&area, &self->dirty_area, &area);
    displayio_area_t bitmap_area = {0, 0, self->width, self->height};
    displayio_area_compute_overlap(&area, &bitmap_area, &self->dirty_area);
}

void displayio_bitmap_write_pixel(displayio_bitmap_t *self, int16_t x, int16_t y, uint32_t value) {
    // Writes the color index value into a pixel position
    // Must update the dirty area separately

    // Update one pixel of data
    int32_t row_start = y * self->stride;
    uint32_t bytes_per_value = self->bits_per_value / 8;
    if (bytes_per_value < 1) {
        uint32_t bit_position = (sizeof(size_t) * 8 - ((x & self->x_mask) + 1) * self->bits_per_value);
        uint32_t index = row_start + (x >> self->x_shift);
        uint32_t word = self->data[index];
        word &= ~(self->bitmask << bit_position);
        word |= (value & self->bitmask) << bit_position;
        self->data[index] = word;
    } else {
        size_t *row = self->data + row_start;
        if (bytes_per_value == 1) {
            ((uint8_t *)row)[x] = value;
        } else if (bytes_per_value == 2) {
            ((uint16_t *)row)[x] = value;
        } else if (bytes_per_value == 4) {
            ((uint32_t *)row)[x] = value;
        }
    }
}

void common_hal_displayio_bitmap_blit(displayio_bitmap_t *self, int16_t x, int16_t y, displayio_bitmap_t *source,
    int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint32_t skip_index, bool skip_index_none) {
    // Copy region of "source" bitmap into "self" bitmap at location x,y in the "self"
    // If skip_value is encountered in the source bitmap, it will not be copied.
    // If skip_value is `None`, then all pixels are copied.
    // This function assumes input checks were performed for pixel index entries.

    // Update the dirty area
    int16_t dirty_x_max = (x + (x2 - x1));
    if (dirty_x_max > self->width) {
        dirty_x_max = self->width;
    }
    int16_t dirty_y_max = y + (y2 - y1);
    if (dirty_y_max > self->height) {
        dirty_y_max = self->height;
    }

    displayio_area_t a = { x, y, dirty_x_max, dirty_y_max};
    displayio_bitmap_set_dirty_area(self, &a);

    bool x_reverse = false;
    bool y_reverse = false;

    // Add reverse direction option to protect blitting of self bitmap back into self bitmap
    if (x > x1) {
        x_reverse = true;
    }
    if (y > y1) {
        y_reverse = true;
    }

    // simplest version - use internal functions for get/set pixels
    for (int16_t i = 0; i < (x2 - x1); i++) {

        const int xs_index = x_reverse ? ((x2) - i - 1) : x1 + i; // x-index into the source bitmap
        const int xd_index = x_reverse ? ((x + (x2 - x1)) - i - 1) : x + i; // x-index into the destination bitmap

        if ((xd_index >= 0) && (xd_index < self->width)) {
            for (int16_t j = 0; j < (y2 - y1); j++) {

                const int ys_index = y_reverse ? ((y2) - j - 1) : y1 + j;  // y-index into the source bitmap
                const int yd_index = y_reverse ? ((y + (y2 - y1)) - j - 1) : y + j; // y-index into the destination bitmap

                if ((yd_index >= 0) && (yd_index < self->height)) {
                    uint32_t value = common_hal_displayio_bitmap_get_pixel(source, xs_index, ys_index);
                    if ((skip_index_none) || (value != skip_index)) {   // write if skip_value_none is True
                        displayio_bitmap_write_pixel(self, xd_index, yd_index, value);
                    }
                }
            }
        }
    }
}

void common_hal_displayio_bitmap_set_pixel(displayio_bitmap_t *self, int16_t x, int16_t y, uint32_t value) {
    // update the dirty region
    displayio_area_t a = {x, y, x + 1, y + 1};
    displayio_bitmap_set_dirty_area(self, &a);

    // write the pixel
    displayio_bitmap_write_pixel(self, x, y, value);

}

displayio_area_t *displayio_bitmap_get_refresh_areas(displayio_bitmap_t *self, displayio_area_t *tail) {
    if (self->dirty_area.x1 == self->dirty_area.x2) {
        return tail;
    }
    self->dirty_area.next = tail;
    return &self->dirty_area;
}

void displayio_bitmap_finish_refresh(displayio_bitmap_t *self) {
    self->dirty_area.x1 = 0;
    self->dirty_area.x2 = 0;
}

void common_hal_displayio_bitmap_fill(displayio_bitmap_t *self, uint32_t value) {
    displayio_area_t a = {0, 0, self->width, self->height};
    displayio_bitmap_set_dirty_area(self, &a);

    // build the packed word
    uint32_t word = 0;
    for (uint8_t i = 0; i < 32 / self->bits_per_value; i++) {
        word |= (value & self->bitmask) << (32 - ((i + 1) * self->bits_per_value));
    }
    // copy it in
    for (uint32_t i = 0; i < self->stride * self->height; i++) {
        self->data[i] = word;
    }
}

int common_hal_displayio_bitmap_get_buffer(displayio_bitmap_t *self, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    if ((flags & MP_BUFFER_WRITE) && self->read_only) {
        return 1;
    }
    bufinfo->len = self->stride * self->height * sizeof(size_t);
    bufinfo->buf = self->data;
    switch (self->bits_per_value) {
        case 32:
            bufinfo->typecode = 'I';
            break;
        case 16:
            bufinfo->typecode = 'H';
            break;
        default:
            bufinfo->typecode = 'B';
            break;
    }
    return 0;
}
