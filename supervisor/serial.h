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
#include "lib/oofatfs/ff.h"

extern FIL *boot_output_file;
#endif

void serial_early_init(void);
void serial_init(void);
void serial_write(const char *text);
// Only writes up to given length. Does not check for null termination at all.
void serial_write_substring(const char *text, uint32_t length);
char serial_read(void);
bool serial_bytes_available(void);
bool serial_connected(void);

// XXX  used in nrf52-sleep debug
int dbg_printf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

#endif  // MICROPY_INCLUDED_SUPERVISOR_SERIAL_H
