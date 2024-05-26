// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"
#include "shared-module/displayio/Palette.h"

typedef struct displayio_colorconverter {
    mp_obj_base_t base;
    bool dither;
    uint8_t input_colorspace;
    _displayio_colorspace_t output_colorspace;
    uint32_t transparent_color;

    // Cache the last computed color in case the are the same.
    const _displayio_colorspace_t *cached_colorspace;
    uint32_t cached_input_pixel;
    uint32_t cached_output_color;
} displayio_colorconverter_t;

bool displayio_colorconverter_needs_refresh(displayio_colorconverter_t *self);
void displayio_colorconverter_finish_refresh(displayio_colorconverter_t *self);
void displayio_colorconverter_convert(displayio_colorconverter_t *self, const _displayio_colorspace_t *colorspace, const displayio_input_pixel_t *input_pixel, displayio_output_pixel_t *output_color);

uint32_t displayio_colorconverter_dither_noise_1(uint32_t n);
uint32_t displayio_colorconverter_dither_noise_2(uint32_t x, uint32_t y);

// Convert version that doesn't require a colorconverter object.
void displayio_convert_color(const _displayio_colorspace_t *colorspace, bool dither, const displayio_input_pixel_t *input_pixel, displayio_output_pixel_t *output_color);

uint16_t displayio_colorconverter_compute_rgb565(uint32_t color_rgb888);
uint8_t displayio_colorconverter_compute_rgb332(uint32_t color_rgb888);
uint8_t displayio_colorconverter_compute_rgbd(uint32_t color_rgb888);
uint8_t displayio_colorconverter_compute_luma(uint32_t color_rgb888);
uint8_t displayio_colorconverter_compute_chroma(uint32_t color_rgb888);
uint8_t displayio_colorconverter_compute_hue(uint32_t color_rgb888);
uint8_t displayio_colorconverter_compute_sevencolor(uint32_t color_rgb888);
void displayio_colorconverter_compute_tricolor(const _displayio_colorspace_t *colorspace, uint8_t pixel_hue, uint32_t *color);
