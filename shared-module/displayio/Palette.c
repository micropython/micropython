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

#include "shared-module/displayio/ColorConverter.h"

void common_hal_displayio_palette_construct(displayio_palette_t *self, uint16_t color_count) {
    self->color_count = color_count;
    self->colors = (_displayio_color_t *)m_malloc(color_count * sizeof(_displayio_color_t), false);
}

void common_hal_displayio_palette_make_opaque(displayio_palette_t *self, uint32_t palette_index) {
    self->colors[palette_index].transparent = false;
    self->needs_refresh = true;
}

void common_hal_displayio_palette_make_transparent(displayio_palette_t *self, uint32_t palette_index) {
    self->colors[palette_index].transparent = true;
    self->needs_refresh = true;
}

bool common_hal_displayio_palette_is_transparent(displayio_palette_t *self, uint32_t palette_index) {
    return self->colors[palette_index].transparent;
}

uint32_t common_hal_displayio_palette_get_len(displayio_palette_t *self) {
    return self->color_count;
}

void common_hal_displayio_palette_set_color(displayio_palette_t *self, uint32_t palette_index, uint32_t color) {
    if (self->colors[palette_index].rgb888 == color) {
        return;
    }
    self->colors[palette_index].rgb888 = color;
    self->colors[palette_index].luma = displayio_colorconverter_compute_luma(color);
    self->colors[palette_index].rgb565 = displayio_colorconverter_compute_rgb565(color);

    uint8_t chroma = displayio_colorconverter_compute_chroma(color);
    self->colors[palette_index].chroma = chroma;
    self->colors[palette_index].hue = displayio_colorconverter_compute_hue(color);
    self->needs_refresh = true;
}

uint32_t common_hal_displayio_palette_get_color(displayio_palette_t *self, uint32_t palette_index) {
    return self->colors[palette_index].rgb888;
}

bool displayio_palette_get_color(displayio_palette_t *self, const _displayio_colorspace_t *colorspace, uint32_t palette_index, uint32_t *color) {
    if (palette_index > self->color_count || self->colors[palette_index].transparent) {
        return false; // returns opaque
    }

    if (colorspace->tricolor) {
        uint8_t luma = self->colors[palette_index].luma;
        *color = luma >> (8 - colorspace->depth);
        // Chroma 0 means the color is a gray and has no hue so never color based on it.
        if (self->colors[palette_index].chroma <= 16) {
            if (!colorspace->grayscale) {
                *color = 0;
            }
            return true;
        }
        uint8_t pixel_hue = self->colors[palette_index].hue;
        displayio_colorconverter_compute_tricolor(colorspace, pixel_hue, luma, color);
    } else if (colorspace->grayscale) {
        size_t bitmask = (1 << colorspace->depth) - 1;
        *color = (self->colors[palette_index].luma >> colorspace->grayscale_bit) & bitmask;
    } else {
        uint16_t packed = self->colors[palette_index].rgb565;
        if (colorspace->reverse_bytes_in_word) {
            // swap bytes
            packed = __builtin_bswap16(packed);
        }
        *color = packed;
    }

    return true;
}

bool displayio_palette_needs_refresh(displayio_palette_t *self) {
    return self->needs_refresh;
}

void displayio_palette_finish_refresh(displayio_palette_t *self) {
    self->needs_refresh = false;
}
