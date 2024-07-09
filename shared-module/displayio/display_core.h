// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-bindings/displayio/__init__.h"
#include "shared-bindings/displayio/Group.h"

#include "shared-module/displayio/area.h"

#define NO_COMMAND 0x100

typedef struct {
    displayio_group_t *current_group;
    uint64_t last_refresh;
    displayio_buffer_transform_t transform;
    displayio_area_t area;
    uint16_t width;
    uint16_t height;
    uint16_t rotation;
    _displayio_colorspace_t colorspace;

    bool full_refresh; // New group means we need to refresh the whole display.
    bool refresh_in_progress;
} displayio_display_core_t;

void displayio_display_core_construct(displayio_display_core_t *self,
    uint16_t width, uint16_t height, uint16_t rotation,
    uint16_t color_depth, bool grayscale, bool pixels_in_byte_share_row, uint8_t bytes_per_cell, bool reverse_pixels_in_byte, bool reverse_bytes_in_word);

bool displayio_display_core_set_root_group(displayio_display_core_t *self, displayio_group_t *root_group);

uint16_t displayio_display_core_get_width(displayio_display_core_t *self);
uint16_t displayio_display_core_get_height(displayio_display_core_t *self);

void displayio_display_core_set_dither(displayio_display_core_t *self, bool dither);
bool displayio_display_core_get_dither(displayio_display_core_t *self);

void displayio_display_core_set_rotation(displayio_display_core_t *self, int rotation);

void release_display_core(displayio_display_core_t *self);

bool displayio_display_core_start_refresh(displayio_display_core_t *self);
void displayio_display_core_finish_refresh(displayio_display_core_t *self);

void displayio_display_core_collect_ptrs(displayio_display_core_t *self);

bool displayio_display_core_fill_area(displayio_display_core_t *self, displayio_area_t *area, uint32_t *mask, uint32_t *buffer);

bool displayio_display_core_clip_area(displayio_display_core_t *self, const displayio_area_t *area, displayio_area_t *clipped);
