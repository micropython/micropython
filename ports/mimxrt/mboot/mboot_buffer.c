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

#include <string.h>
#include "mboot_buffer.h"
#include "mboot_memory.h"

// --------------------------------------------------------------------+
// Local Variables
// --------------------------------------------------------------------+
MEM_AT_SECTION(".fw_buffer") uint8_t fw_buffer[MICROPY_BOOT_BUFFER_SIZE];
static size_t buffer_length;

// --------------------------------------------------------------------+
// Global Function Definitions
// --------------------------------------------------------------------+
void mboot_buffer_init(void) {
    buffer_length = 0UL;
    memset(&fw_buffer[0], 0, MICROPY_BOOT_BUFFER_SIZE);
}

size_t mboot_buffer_append(const uint8_t *data, size_t length) {
    size_t bytes_remaining = (size_t)MICROPY_BOOT_BUFFER_SIZE - buffer_length;
    size_t bytes_to_buffer = (bytes_remaining >= length) ? length : bytes_remaining;

    memcpy((void *)&fw_buffer[buffer_length], (void *)data, bytes_to_buffer);
    buffer_length += bytes_to_buffer;

    return length - bytes_to_buffer;
}

void mboot_buffer_pad(void) {
    if (buffer_length < MICROPY_BOOT_BUFFER_SIZE) {
        memset((void *)&fw_buffer[buffer_length], 0, (MICROPY_BOOT_BUFFER_SIZE - buffer_length));
        buffer_length = MICROPY_BOOT_BUFFER_SIZE;
    }
}

void mboot_buffer_reset(void) {
    buffer_length = 0UL;
}

uint8_t *mboot_buffer_data_ptr(void) {
    return &fw_buffer[0];
}

bool mboot_buffer_is_full(void) {
    return (size_t)buffer_length >= MICROPY_BOOT_BUFFER_SIZE;
}

size_t mboot_buffer_len(void) {
    return (size_t)buffer_length;
}
