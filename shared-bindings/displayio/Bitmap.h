// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/displayio/Bitmap.h"

extern const mp_obj_type_t displayio_bitmap_type;

void common_hal_displayio_bitmap_construct(displayio_bitmap_t *self, uint32_t width,
    uint32_t height, uint32_t bits_per_value);
void common_hal_displayio_bitmap_construct_from_buffer(displayio_bitmap_t *self, uint32_t width,
    uint32_t height, uint32_t bits_per_value, uint32_t *data, bool read_only);

void common_hal_displayio_bitmap_load_row(displayio_bitmap_t *self, uint16_t y, uint8_t *data,
    uint16_t len);
uint16_t common_hal_displayio_bitmap_get_height(displayio_bitmap_t *self);
uint16_t common_hal_displayio_bitmap_get_width(displayio_bitmap_t *self);
uint32_t common_hal_displayio_bitmap_get_bits_per_value(displayio_bitmap_t *self);
void common_hal_displayio_bitmap_set_pixel(displayio_bitmap_t *bitmap, int16_t x, int16_t y, uint32_t value);
uint32_t common_hal_displayio_bitmap_get_pixel(displayio_bitmap_t *bitmap, int16_t x, int16_t y);
void common_hal_displayio_bitmap_fill(displayio_bitmap_t *bitmap, uint32_t value);
int common_hal_displayio_bitmap_get_buffer(displayio_bitmap_t *self, mp_buffer_info_t *bufinfo, mp_uint_t flags);
void common_hal_displayio_bitmap_deinit(displayio_bitmap_t *self);
bool common_hal_displayio_bitmap_deinited(displayio_bitmap_t *self);
