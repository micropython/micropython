/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SUPERVISOR_SERIAL_H
#define MICROPY_INCLUDED_SUPERVISOR_SERIAL_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "py/mpconfig.h"

#ifdef CIRCUITPY_BOOT_OUTPUT_FILE
#include "py/misc.h"

extern vstr_t *boot_output;
#endif


void serial_early_init(void);
void serial_init(void);
void serial_write(const char *text);
// Only writes up to given length. Does not check for null termination at all.
void serial_write_substring(const char *text, uint32_t length);
char serial_read(void);
bool serial_bytes_available(void);
bool serial_connected(void);

// Used for temporarily suppressing output to the console or display.
bool serial_console_write_disable(bool disabled);
bool serial_display_write_disable(bool disabled);

// These have no-op versions that are weak and the port can override. They work
// in tandem with the cross-port mechanics like USB and BLE.
void port_serial_early_init(void);
void port_serial_init(void);
bool port_serial_connected(void);
char port_serial_read(void);
bool port_serial_bytes_available(void);
void port_serial_write_substring(const char *text, uint32_t length);

int console_uart_printf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

#endif  // MICROPY_INCLUDED_SUPERVISOR_SERIAL_H
