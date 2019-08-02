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

#ifndef MICROPY_INCLUDED_SHARED_MODULE_DISPLAYIO_EPAPERDISPLAY_H
#define MICROPY_INCLUDED_SHARED_MODULE_DISPLAYIO_EPAPERDISPLAY_H

#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/displayio/Group.h"
#include "shared-bindings/pulseio/PWMOut.h"

#include "shared-module/displayio/area.h"

typedef void (*display_bus_bus_reset)(mp_obj_t bus);
typedef bool (*display_bus_bus_free)(mp_obj_t bus);
typedef bool (*display_bus_begin_transaction)(mp_obj_t bus);
typedef void (*display_bus_send)(mp_obj_t bus, bool command, bool toggle_every_byte, uint8_t *data, uint32_t data_length);
typedef void (*display_bus_end_transaction)(mp_obj_t bus);

typedef struct {
    mp_obj_base_t base;
    mp_obj_t bus;
    displayio_group_t *current_group;
    uint64_t last_refresh;
    display_bus_bus_reset bus_reset;
    display_bus_bus_free bus_free;
    display_bus_begin_transaction begin_transaction;
    display_bus_send send;
    display_bus_end_transaction end_transaction;
    digitalio_digitalinout_obj_t busy;
    uint32_t milliseconds_per_frame;
    uint8_t* start_sequence;
    uint32_t start_sequence_len;
    uint8_t* stop_sequence;
    uint32_t stop_sequence_len;
    displayio_buffer_transform_t transform;
    displayio_area_t area;
    uint16_t width;
    uint16_t height;
    uint16_t ram_width;
    uint16_t ram_height;
    _displayio_colorspace_t colorspace;
    int16_t colstart;
    int16_t rowstart;
    uint16_t set_column_window_command;
    uint16_t set_row_window_command;
    uint16_t set_current_column_command;
    uint16_t set_current_row_command;
    uint16_t write_black_ram_command;
    uint16_t write_color_ram_command;
    uint8_t refresh_display_command;
    uint8_t hue;
    bool busy_state;
    bool black_bits_inverted;
    bool color_bits_inverted;
    bool refresh;
    bool refreshing;
    bool full_refresh; // New group means we need to refresh the whole display.
    bool always_toggle_chip_select;
} displayio_epaperdisplay_obj_t;

bool displayio_epaperdisplay_refresh_area(displayio_epaperdisplay_obj_t* display, const displayio_area_t* area);
void displayio_epaperdisplay_start_refresh(displayio_epaperdisplay_obj_t* self);
const displayio_area_t* displayio_epaperdisplay_get_refresh_areas(displayio_epaperdisplay_obj_t *self);
bool displayio_epaperdisplay_fill_area(displayio_epaperdisplay_obj_t *self, displayio_area_t* area, uint32_t* mask, uint32_t *buffer);
bool displayio_epaperdisplay_clip_area(displayio_epaperdisplay_obj_t *self, const displayio_area_t* area, displayio_area_t* clipped);
bool displayio_epaperdisplay_bus_free(displayio_epaperdisplay_obj_t *self);
void release_epaperdisplay(displayio_epaperdisplay_obj_t* self);

#endif // MICROPY_INCLUDED_SHARED_MODULE_DISPLAYIO_EPAPERDISPLAY_H
