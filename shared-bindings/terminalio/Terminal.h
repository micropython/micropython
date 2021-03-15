/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_TERMINALIO_TERMINAL_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_TERMINALIO_TERMINAL_H

#include "shared-module/terminalio/Terminal.h"

#include "shared-bindings/displayio/TileGrid.h"

extern const mp_obj_type_t terminalio_terminal_type;

extern void common_hal_terminalio_terminal_construct(terminalio_terminal_obj_t *self,
    displayio_tilegrid_t *tilegrid, const fontio_builtinfont_t *font);

// Write characters. len is in characters NOT bytes!
extern size_t common_hal_terminalio_terminal_write(terminalio_terminal_obj_t *self,
    const uint8_t *data, size_t len, int *errcode);

extern bool common_hal_terminalio_terminal_ready_to_tx(terminalio_terminal_obj_t *self);

#endif  // MICROPY_INCLUDED_SHARED_BINDINGS_TERMINALIO_TERMINAL_H
