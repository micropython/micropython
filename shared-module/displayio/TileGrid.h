// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"
#include "shared-module/displayio/area.h"
#include "shared-module/displayio/Palette.h"

typedef struct {
    mp_obj_base_t base;
    mp_obj_t bitmap;
    mp_obj_t pixel_shader;
    int16_t x;
    int16_t y;
    uint16_t pixel_width;
    uint16_t pixel_height;
    uint16_t bitmap_width_in_tiles;
    ;
    uint16_t tiles_in_bitmap;
    uint16_t width_in_tiles;
    uint16_t height_in_tiles;
    uint16_t tile_width;
    uint16_t tile_height;
    uint16_t top_left_x;
    uint16_t top_left_y;
    uint8_t *tiles;
    const displayio_buffer_transform_t *absolute_transform;
    displayio_area_t dirty_area; // Stored as a relative area until the refresh area is fetched.
    displayio_area_t previous_area; // Stored as an absolute area.
    displayio_area_t current_area; // Stored as an absolute area so it applies across frames.
    bool partial_change : 1;
    bool full_change : 1;
    bool moved : 1;
    bool inline_tiles : 1;
    bool in_group : 1;
    bool flip_x : 1;
    bool flip_y : 1;
    bool transpose_xy  : 1;
    bool hidden : 1;
    bool hidden_by_parent : 1;
    bool rendered_hidden : 1;
    uint8_t padding : 6;
} displayio_tilegrid_t;

void displayio_tilegrid_set_hidden_by_parent(displayio_tilegrid_t *self, bool hidden);

// Updating the screen is a three stage process.

// The first stage is used to determine i
displayio_area_t *displayio_tilegrid_get_refresh_areas(displayio_tilegrid_t *self, displayio_area_t *tail);

// Area is always in absolute screen coordinates. Update transform is used to inform TileGrids how
// they relate to it.
bool displayio_tilegrid_fill_area(displayio_tilegrid_t *self, const _displayio_colorspace_t *colorspace, const displayio_area_t *area, uint32_t *mask, uint32_t *buffer);
void displayio_tilegrid_update_transform(displayio_tilegrid_t *group, const displayio_buffer_transform_t *parent_transform);

// Fills in area with the maximum bounds of all related pixels in the last rendered frame. Returns
// false if the tilegrid wasn't rendered in the last frame.
bool displayio_tilegrid_get_previous_area(displayio_tilegrid_t *self, displayio_area_t *area);
void displayio_tilegrid_finish_refresh(displayio_tilegrid_t *self);

bool displayio_tilegrid_get_rendered_hidden(displayio_tilegrid_t *self);
