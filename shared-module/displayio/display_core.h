/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#ifndef MICROPY_INCLUDED_SHARED_MODULE_DISPLAYIO_DISPLAY_CORE_H
#define MICROPY_INCLUDED_SHARED_MODULE_DISPLAYIO_DISPLAY_CORE_H

#include "shared-bindings/displayio/__init__.h"
#include "shared-bindings/displayio/Group.h"

#include "shared-module/displayio/area.h"

#define NO_COMMAND 0x100

typedef struct {
    mp_obj_t bus;
    displayio_group_t *current_group;
    uint64_t last_refresh;
    display_bus_bus_reset bus_reset;
    display_bus_bus_free bus_free;
    display_bus_begin_transaction begin_transaction;
    display_bus_send send;
    display_bus_end_transaction end_transaction;
    displayio_buffer_transform_t transform;
    displayio_area_t area;
    uint16_t width;
    uint16_t height;
    uint16_t rotation;
    uint16_t ram_width;
    uint16_t ram_height;
    _displayio_colorspace_t colorspace;
    int16_t colstart;
    int16_t rowstart;
    bool full_refresh; // New group means we need to refresh the whole display.
    bool refresh_in_progress;
} displayio_display_core_t;

void displayio_display_core_construct(displayio_display_core_t *self,
    mp_obj_t bus, uint16_t width, uint16_t height, uint16_t ram_width, uint16_t ram_height, int16_t colstart, int16_t rowstart, uint16_t rotation,
    uint16_t color_depth, bool grayscale, bool pixels_in_byte_share_row, uint8_t bytes_per_cell, bool reverse_pixels_in_byte, bool reverse_bytes_in_word);

bool displayio_display_core_show(displayio_display_core_t *self, displayio_group_t *root_group);

uint16_t displayio_display_core_get_width(displayio_display_core_t *self);
uint16_t displayio_display_core_get_height(displayio_display_core_t *self);

void displayio_display_core_set_dither(displayio_display_core_t *self, bool dither);
bool displayio_display_core_get_dither(displayio_display_core_t *self);

void displayio_display_core_set_rotation(displayio_display_core_t *self, int rotation);

bool displayio_display_core_bus_free(displayio_display_core_t *self);
bool displayio_display_core_begin_transaction(displayio_display_core_t *self);
void displayio_display_core_end_transaction(displayio_display_core_t *self);

void displayio_display_core_set_region_to_update(displayio_display_core_t *self, uint8_t column_command,
    uint8_t row_command, uint16_t set_current_column_command, uint16_t set_current_row_command,
    bool data_as_commands, bool always_toggle_chip_select,
    displayio_area_t *area, bool SH1107_addressing);

void release_display_core(displayio_display_core_t *self);

bool displayio_display_core_start_refresh(displayio_display_core_t *self);
void displayio_display_core_finish_refresh(displayio_display_core_t *self);

void displayio_display_core_collect_ptrs(displayio_display_core_t *self);

bool displayio_display_core_fill_area(displayio_display_core_t *self, displayio_area_t *area, uint32_t *mask, uint32_t *buffer);

bool displayio_display_core_clip_area(displayio_display_core_t *self, const displayio_area_t *area, displayio_area_t *clipped);

#endif // MICROPY_INCLUDED_SHARED_MODULE_DISPLAYIO_DISPLAY_CORE_H
