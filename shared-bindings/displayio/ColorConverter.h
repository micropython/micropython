// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-bindings/displayio/__init__.h"
#include "shared-module/displayio/ColorConverter.h"

#include "shared-module/displayio/Palette.h"

extern const mp_obj_type_t displayio_colorconverter_type;

void common_hal_displayio_colorconverter_construct(displayio_colorconverter_t *self, bool dither, displayio_colorspace_t input_colorspace);
void common_hal_displayio_colorconverter_convert(displayio_colorconverter_t *colorconverter, const _displayio_colorspace_t *colorspace, uint32_t input_color, uint32_t *output_color);
uint32_t displayio_colorconverter_convert_pixel(displayio_colorspace_t colorspace, uint32_t pixel);

void common_hal_displayio_colorconverter_set_dither(displayio_colorconverter_t *self, bool dither);
bool common_hal_displayio_colorconverter_get_dither(displayio_colorconverter_t *self);

void common_hal_displayio_colorconverter_make_transparent(displayio_colorconverter_t *self, uint32_t transparent_color);
void common_hal_displayio_colorconverter_make_opaque(displayio_colorconverter_t *self, uint32_t transparent_color);
