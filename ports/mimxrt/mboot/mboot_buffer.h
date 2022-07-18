/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Philipp Ebensberger
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

#ifndef MICROPY_INCLUDED_MIMXRT_MBOOT_BUFFER_H
#define MICROPY_INCLUDED_MIMXRT_MBOOT_BUFFER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// --------------------------------------------------------------------+
// Defines and Macros
// --------------------------------------------------------------------+
#ifndef MICROPY_BOOT_BUFFER_SIZE
#define MICROPY_BOOT_BUFFER_SIZE (32 * 1024)  // buffer size must be multiple of 4k!
#endif

// --------------------------------------------------------------------+
// Global Function Declarations
// --------------------------------------------------------------------+
void mboot_buffer_init(void);
size_t mboot_buffer_append(const uint8_t *data, size_t length);
void mboot_buffer_pad(void);
void mboot_buffer_reset(void);
uint8_t *mboot_buffer_data_ptr(void);
bool mboot_buffer_is_full(void);
size_t mboot_buffer_len(void);

#endif // MICROPY_INCLUDED_MIMXRT_MBOOT_BUFFER_H
