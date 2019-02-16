/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2017 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SUPERVISOR_PORT_H
#define MICROPY_INCLUDED_SUPERVISOR_PORT_H

#include "py/mpconfig.h"

#include "supervisor/shared/safe_mode.h"

// Provided by the linker;
extern uint32_t _ezero;

// This file defines core methods that must be implemented by a port.
extern uint32_t _estack;

// Stored at the end of the bss section (which includes the heap).
extern uint32_t _ebss;

safe_mode_t port_init(void);

// Reset the microcontroller completely.
void reset_cpu(void);

// Reset the microcontroller state.
void reset_port(void);

// Reset the rest of the board.
void reset_board(void);

// Reset to the bootloader
void reset_to_bootloader(void);

// Save and retrieve a word from memory that is preserved over reset. Used for safe mode.
void port_set_saved_word(uint32_t);
uint32_t port_get_saved_word(void);

#endif  // MICROPY_INCLUDED_SUPERVISOR_PORT_H
