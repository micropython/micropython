// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/displayio/TileGrid.h"

extern const mp_obj_type_t displayio_tilegrid_type;

void common_hal_displayio_tilegrid_construct(displayio_tilegrid_t *self, mp_obj_t bitmap,
    uint16_t bitmap_width_in_tiles, uint16_t bitmap_height_in_tiles,
    mp_obj_t pixel_shader, uint16_t width, uint16_t height,
    uint16_t tile_width, uint16_t tile_height, uint16_t x, uint16_t y, uint8_t default_tile);

bool common_hal_displayio_tilegrid_get_hidden(displayio_tilegrid_t *self);
void common_hal_displayio_tilegrid_set_hidden(displayio_tilegrid_t *self, bool hidden);
mp_int_t common_hal_displayio_tilegrid_get_x(displayio_tilegrid_t *self);
void common_hal_displayio_tilegrid_set_x(displayio_tilegrid_t *self, mp_int_t x);
mp_int_t common_hal_displayio_tilegrid_get_y(displayio_tilegrid_t *self);
void common_hal_displayio_tilegrid_set_y(displayio_tilegrid_t *self, mp_int_t y);
mp_obj_t common_hal_displayio_tilegrid_get_pixel_shader(displayio_tilegrid_t *self);
void common_hal_displayio_tilegrid_set_pixel_shader(displayio_tilegrid_t *self, mp_obj_t pixel_shader);

mp_obj_t common_hal_displayio_tilegrid_get_bitmap(displayio_tilegrid_t *self);
void common_hal_displayio_tilegrid_set_bitmap(displayio_tilegrid_t *self, mp_obj_t bitmap);


bool common_hal_displayio_tilegrid_get_flip_x(displayio_tilegrid_t *self);
void common_hal_displayio_tilegrid_set_flip_x(displayio_tilegrid_t *self, bool flip_x);
bool common_hal_displayio_tilegrid_get_flip_y(displayio_tilegrid_t *self);
void common_hal_displayio_tilegrid_set_flip_y(displayio_tilegrid_t *self, bool flip_y);
bool common_hal_displayio_tilegrid_get_transpose_xy(displayio_tilegrid_t *self);
void common_hal_displayio_tilegrid_set_transpose_xy(displayio_tilegrid_t *self, bool transpose_xy);

bool common_hal_displayio_tilegrid_contains(displayio_tilegrid_t *self, uint16_t x, uint16_t y);

uint16_t common_hal_displayio_tilegrid_get_width(displayio_tilegrid_t *self);
uint16_t common_hal_displayio_tilegrid_get_height(displayio_tilegrid_t *self);

uint16_t common_hal_displayio_tilegrid_get_tile_width(displayio_tilegrid_t *self);
uint16_t common_hal_displayio_tilegrid_get_tile_height(displayio_tilegrid_t *self);

uint8_t common_hal_displayio_tilegrid_get_tile(displayio_tilegrid_t *self, uint16_t x, uint16_t y);
void common_hal_displayio_tilegrid_set_tile(displayio_tilegrid_t *self, uint16_t x, uint16_t y, uint8_t tile_index);

// Private API for scrolling the TileGrid.
void common_hal_displayio_tilegrid_set_top_left(displayio_tilegrid_t *self, uint16_t x, uint16_t y);
void common_hal_displayio_tilegrid_set_all_tiles(displayio_tilegrid_t *self, uint8_t tile_index);
