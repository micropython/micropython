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
    while (power_of_two < align_bits / bits_per_value ) {
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
        size_t* row = self->data + row_start;
        if (bytes_per_value == 1) {
            return ((uint8_t*) row)[x];
        } else if (bytes_per_value == 2) {
            return ((uint16_t*) row)[x];
        } else if (bytes_per_value == 4) {
            return ((uint32_t*) row)[x];
        }
    }
    return 0;
}

void common_hal_displayio_bitmap_blit(displayio_bitmap_t *self, int16_t x, int16_t y, displayio_bitmap_t *source,
                            int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint32_t skip_index, bool skip_index_none) {
    // Copy complete "source" bitmap into "self" bitmap at location x,y in the "self"
    // Add a boolean to determine if all values are copied, or only if non-zero
    // If skip_value is encountered in the source bitmap, it will not be copied.
    // If skip_value is `None`, then all pixels are copied.

    if (self->read_only) {
        mp_raise_RuntimeError(translate("Read-only object"));
    }

    // simplest version - use internal functions for get/set pixels
    for (int16_t i=0; i < (x2-x1) ; i++) {
        if ( (x+i >= 0) && (x+i < self->width) ) {
            for (int16_t j=0; j < (y2-y1) ; j++){
                if ((y+j >= 0) && (y+j < self->height) ) {
                    uint32_t value = common_hal_displayio_bitmap_get_pixel(source, x1+i, y1+j);
                    if ( (skip_index_none) || (value != skip_index) ) { // write if skip_value_none is True
                            common_hal_displayio_bitmap_set_pixel(self, x+i, y+j, value);
                    }
                }
            }
        }
    }

}

void common_hal_displayio_bitmap_set_pixel(displayio_bitmap_t *self, int16_t x, int16_t y, uint32_t value) {
    if (self->read_only) {
        mp_raise_RuntimeError(translate("Read-only object"));
    }
    // Update the dirty area.
    if (self->dirty_area.x1 == self->dirty_area.x2) {
        self->dirty_area.x1 = x;
        self->dirty_area.x2 = x + 1;
        self->dirty_area.y1 = y;
        self->dirty_area.y2 = y + 1;
    } else {
        if (x < self->dirty_area.x1) {
            self->dirty_area.x1 = x;
        } else if (x >= self->dirty_area.x2) {
            self->dirty_area.x2 = x + 1;
        }
        if (y < self->dirty_area.y1) {
            self->dirty_area.y1 = y;
        } else if (y >= self->dirty_area.y2) {
            self->dirty_area.y2 = y + 1;
        }
    }

    // Update our data
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
        size_t* row = self->data + row_start;
        if (bytes_per_value == 1) {
            ((uint8_t*) row)[x] = value;
        } else if (bytes_per_value == 2) {
            ((uint16_t*) row)[x] = value;
        } else if (bytes_per_value == 4) {
            ((uint32_t*) row)[x] = value;
        }
    }
}

displayio_area_t* displayio_bitmap_get_refresh_areas(displayio_bitmap_t *self, displayio_area_t* tail) {
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
    if (self->read_only) {
        mp_raise_RuntimeError(translate("Read-only object"));
    }
    // Update the dirty area.
    self->dirty_area.x1 = 0;
    self->dirty_area.x2 = self->width;
    self->dirty_area.y1 = 0;
    self->dirty_area.y2 = self->height;

    // build the packed word
    uint32_t word = 0;
    for (uint8_t i=0; i<32 / self->bits_per_value; i++) {
        word |= (value & self->bitmask) << (32 - ((i+1)*self->bits_per_value));
    }
    // copy it in
    for (uint32_t i=0; i<self->stride * self->height; i++) {
        self->data[i] = word;
    }
}
