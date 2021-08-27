/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_TILEGRID_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_TILEGRID_H

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


bool common_hal_displayio_tilegrid_get_flip_x(displayio_tilegrid_t *self);
void common_hal_displayio_tilegrid_set_flip_x(displayio_tilegrid_t *self, bool flip_x);
bool common_hal_displayio_tilegrid_get_flip_y(displayio_tilegrid_t *self);
void common_hal_displayio_tilegrid_set_flip_y(displayio_tilegrid_t *self, bool flip_y);
bool common_hal_displayio_tilegrid_get_transpose_xy(displayio_tilegrid_t *self);
void common_hal_displayio_tilegrid_set_transpose_xy(displayio_tilegrid_t *self, bool transpose_xy);

uint16_t common_hal_displayio_tilegrid_get_width(displayio_tilegrid_t *self);
uint16_t common_hal_displayio_tilegrid_get_height(displayio_tilegrid_t *self);

uint8_t common_hal_displayio_tilegrid_get_tile(displayio_tilegrid_t *self, uint16_t x, uint16_t y);
void common_hal_displayio_tilegrid_set_tile(displayio_tilegrid_t *self, uint16_t x, uint16_t y, uint8_t tile_index);

// Private API for scrolling the TileGrid.
void common_hal_displayio_tilegrid_set_top_left(displayio_tilegrid_t *self, uint16_t x, uint16_t y);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_TILEGRID_H
