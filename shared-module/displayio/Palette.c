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

#include "shared-bindings/displayio/Palette.h"

void common_hal_displayio_palette_construct(displayio_palette_t* self, uint16_t color_count) {
    self->color_count = color_count;
    self->colors = (uint32_t *) m_malloc(color_count * sizeof(uint16_t), false);
    uint32_t opaque_byte_count = color_count / 8;
    if (color_count % 8 > 0) {
        opaque_byte_count += 1;
    }
    self->opaque = (uint32_t *) m_malloc(opaque_byte_count, false);
}

void common_hal_displayio_palette_make_opaque(displayio_palette_t* self, uint32_t palette_index) {
    self->opaque[palette_index / 32] &= ~(0x1 << (palette_index % 32));
}

void common_hal_displayio_palette_make_transparent(displayio_palette_t* self, uint32_t palette_index) {
    self->opaque[palette_index / 32] |= (0x1 << (palette_index % 32));
}

void common_hal_displayio_palette_set_color(displayio_palette_t* self, uint32_t palette_index, uint32_t color) {
    uint32_t shift = (palette_index % 2) * 16;
    uint32_t masked = self->colors[palette_index / 2] & ~(0xffff << shift);
    uint32_t r5 = (color >> 19);
    uint32_t g6 = (color >> 10) & 0x3f;
    uint32_t b5 = (color >> 3) & 0x1f;
    uint32_t packed = r5 << 11 | g6 << 5 | b5;
    // swap bytes
    packed = __builtin_bswap16(packed);
    self->colors[palette_index / 2] = masked | packed << shift;
    self->needs_refresh = true;
}

bool displayio_palette_get_color(displayio_palette_t *self, uint32_t palette_index, uint16_t* color) {
    if (palette_index > self->color_count) {
        return false;
    }
    if ((self->opaque[palette_index / 32] & (0x1 << (palette_index % 32))) != 0) {
        return false;
    }
    *color = (self->colors[palette_index / 2] >> (16 * (palette_index % 2))) & 0xffff;

    return true;
}

bool displayio_palette_needs_refresh(displayio_palette_t *self) {
    return self->needs_refresh;
}

void displayio_palette_finish_refresh(displayio_palette_t *self) {
    self->needs_refresh = false;
}
