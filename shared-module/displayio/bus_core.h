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

#pragma once

#include "shared-bindings/displayio/__init__.h"
#include "shared-bindings/displayio/Group.h"

#include "shared-module/displayio/area.h"
#include "shared-module/displayio/display_core.h"

#define NO_COMMAND 0x100

typedef struct {
    mp_obj_t bus;
    display_bus_bus_reset bus_reset;
    display_bus_bus_free bus_free;
    display_bus_begin_transaction begin_transaction;
    display_bus_send send;
    display_bus_end_transaction end_transaction;
    display_bus_collect_ptrs collect_ptrs;
    uint16_t ram_width;
    uint16_t ram_height;
    int16_t colstart;
    int16_t rowstart;

    // Refresh area related.
    uint16_t column_command;
    uint16_t row_command;
    uint16_t set_current_column_command;
    uint16_t set_current_row_command;
    bool data_as_commands;
    bool always_toggle_chip_select;
    bool SH1107_addressing;
    bool address_little_endian;
} displayio_display_bus_t;

void displayio_display_bus_construct(displayio_display_bus_t *self,
    mp_obj_t bus, uint16_t ram_width, uint16_t ram_height, int16_t colstart, int16_t rowstart,
    uint16_t column_command, uint16_t row_command, uint16_t set_current_column_command, uint16_t set_current_row_command,
    bool data_as_commands, bool always_toggle_chip_select, bool SH1107_addressing, bool address_little_endian);

bool displayio_display_bus_is_free(displayio_display_bus_t *self);
bool displayio_display_bus_begin_transaction(displayio_display_bus_t *self);
void displayio_display_bus_end_transaction(displayio_display_bus_t *self);

void displayio_display_bus_set_region_to_update(displayio_display_bus_t *self, displayio_display_core_t *display, displayio_area_t *area);

void release_display_bus(displayio_display_bus_t *self);

void displayio_display_bus_collect_ptrs(displayio_display_bus_t *self);
