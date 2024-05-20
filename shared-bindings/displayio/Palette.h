// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/displayio/Palette.h"

extern const mp_obj_type_t displayio_palette_type;

void common_hal_displayio_palette_construct(displayio_palette_t *self, uint16_t color_count, bool dither);
void common_hal_displayio_palette_set_color(displayio_palette_t *self, uint32_t palette_index, uint32_t color);
uint32_t common_hal_displayio_palette_get_color(displayio_palette_t *self, uint32_t palette_index);
uint32_t common_hal_displayio_palette_get_len(displayio_palette_t *self);

void common_hal_displayio_palette_set_dither(displayio_palette_t *self, bool dither);
bool common_hal_displayio_palette_get_dither(displayio_palette_t *self);

void common_hal_displayio_palette_make_opaque(displayio_palette_t *self, uint32_t palette_index);
void common_hal_displayio_palette_make_transparent(displayio_palette_t *self, uint32_t palette_index);
bool common_hal_displayio_palette_is_transparent(displayio_palette_t *self, uint32_t palette_index);
