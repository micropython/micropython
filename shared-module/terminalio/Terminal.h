// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "py/obj.h"
#include "shared-module/fontio/BuiltinFont.h"
#include "shared-module/displayio/TileGrid.h"

typedef struct  {
    mp_obj_base_t base;
    const fontio_builtinfont_t *font;
    uint16_t cursor_x;
    uint16_t cursor_y;
    displayio_tilegrid_t *scroll_area;
    displayio_tilegrid_t *status_bar;
    uint16_t status_x;
    uint16_t status_y;
    uint16_t first_row;
    uint16_t osc_command;
    bool in_osc_command;
} terminalio_terminal_obj_t;

extern void terminalio_terminal_clear_status_bar(terminalio_terminal_obj_t *self);
