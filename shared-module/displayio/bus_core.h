// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

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
    // Optional. Like send, but may return before the data is sent: the data must not change and
    // the bus must not be used until flush returns. NULL when the bus has no such mode; only set
    // and read when CIRCUITPY_BUSIO_SPI_ASYNC.
    display_bus_send send_async;
    display_bus_end_transaction end_transaction;
    display_bus_flush flush;
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

// Raise ValueError if bus isn't a supported display bus type. Call it before allocating a
// display, so a bad bus doesn't leave a half-built display in its slot.
void displayio_display_bus_validate(mp_obj_t bus);

void displayio_display_bus_construct(displayio_display_bus_t *self,
    mp_obj_t bus, uint16_t ram_width, uint16_t ram_height, int16_t colstart, int16_t rowstart,
    uint16_t column_command, uint16_t row_command, uint16_t set_current_column_command, uint16_t set_current_row_command,
    bool data_as_commands, bool always_toggle_chip_select, bool SH1107_addressing, bool address_little_endian);

bool displayio_display_bus_is_free(displayio_display_bus_t *self);
bool displayio_display_bus_begin_transaction(displayio_display_bus_t *self);
void displayio_display_bus_end_transaction(displayio_display_bus_t *self);

void displayio_display_bus_set_region_to_update(displayio_display_bus_t *self, displayio_display_core_t *display, displayio_area_t *area);

// Drain any pending asynchronous transfers on the bus.
// No-op for synchronous buses (FourWire, I2C, ParallelBus).
void displayio_display_bus_flush(displayio_display_bus_t *self);

void release_display_bus(displayio_display_bus_t *self);

void displayio_display_bus_collect_ptrs(displayio_display_bus_t *self);
