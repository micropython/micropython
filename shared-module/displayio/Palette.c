// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/displayio/Palette.h"

#include "shared-module/displayio/ColorConverter.h"

void common_hal_displayio_palette_construct(displayio_palette_t *self, uint16_t color_count, bool dither) {
    self->color_count = color_count;
    self->colors = (_displayio_color_t *)m_malloc(color_count * sizeof(_displayio_color_t));
    self->dither = dither;
}

void common_hal_displayio_palette_set_dither(displayio_palette_t *self, bool dither) {
    self->dither = dither;
}

bool common_hal_displayio_palette_get_dither(displayio_palette_t *self) {
    return self->dither;
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
    self->colors[palette_index].cached_colorspace = NULL;
    self->needs_refresh = true;
}

uint32_t common_hal_displayio_palette_get_color(displayio_palette_t *self, uint32_t palette_index) {
    return self->colors[palette_index].rgb888;
}

void displayio_palette_get_color(displayio_palette_t *self, const _displayio_colorspace_t *colorspace, const displayio_input_pixel_t *input_pixel, displayio_output_pixel_t *output_color) {
    uint32_t palette_index = input_pixel->pixel;
    if (palette_index > self->color_count || self->colors[palette_index].transparent) {
        output_color->opaque = false;
        return;
    }

    // Cache results when not dithering.
    _displayio_color_t *color = &self->colors[palette_index];
    // Check the grayscale settings because EPaperDisplay will change them on
    // the same object.
    if (!self->dither &&
        color->cached_colorspace == colorspace &&
        color->cached_colorspace_grayscale_bit == colorspace->grayscale_bit &&
        color->cached_colorspace_grayscale == colorspace->grayscale) {
        output_color->pixel = self->colors[palette_index].cached_color;
        return;
    }

    displayio_input_pixel_t rgb888_pixel = *input_pixel;
    rgb888_pixel.pixel = self->colors[palette_index].rgb888;
    displayio_convert_color(colorspace, self->dither, &rgb888_pixel, output_color);
    if (!self->dither) {
        color->cached_colorspace = colorspace;
        color->cached_color = output_color->pixel;
        color->cached_colorspace_grayscale = colorspace->grayscale;
        color->cached_colorspace_grayscale_bit = colorspace->grayscale_bit;
    }
}

bool displayio_palette_needs_refresh(displayio_palette_t *self) {
    return self->needs_refresh;
}

void displayio_palette_finish_refresh(displayio_palette_t *self) {
    self->needs_refresh = false;
}
