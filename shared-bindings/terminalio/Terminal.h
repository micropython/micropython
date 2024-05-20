// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/terminalio/Terminal.h"

#include "shared-bindings/displayio/TileGrid.h"

extern const mp_obj_type_t terminalio_terminal_type;

extern void common_hal_terminalio_terminal_construct(terminalio_terminal_obj_t *self,
    displayio_tilegrid_t *scroll_area, const fontio_builtinfont_t *font, displayio_tilegrid_t *status_bar);

// Write characters. len is in characters NOT bytes!
extern size_t common_hal_terminalio_terminal_write(terminalio_terminal_obj_t *self,
    const uint8_t *data, size_t len, int *errcode);

extern bool common_hal_terminalio_terminal_ready_to_tx(terminalio_terminal_obj_t *self);
