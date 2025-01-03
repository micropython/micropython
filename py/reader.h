/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2016 Damien P. George
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
#ifndef MICROPY_INCLUDED_PY_READER_H
#define MICROPY_INCLUDED_PY_READER_H

#include "py/obj.h"

// Pass to the `free_len` argument to `mp_reader_new_mem` to indicate that the data is in ROM.
// This means that the data is addressable and will remain valid at least until a soft reset.
#define MP_READER_IS_ROM ((size_t)-1)

// the readbyte function must return the next byte in the input stream
// it must return MP_READER_EOF if end of stream
// it can be called again after returning MP_READER_EOF, and in that case must return MP_READER_EOF
#define MP_READER_EOF ((mp_uint_t)(-1))

typedef struct _mp_reader_t {
    void *data;
    mp_uint_t (*readbyte)(void *data);
    void (*close)(void *data);
} mp_reader_t;

void mp_reader_new_mem(mp_reader_t *reader, const byte *buf, size_t len, size_t free_len);
void mp_reader_new_file(mp_reader_t *reader, qstr filename);
void mp_reader_new_file_from_fd(mp_reader_t *reader, int fd, bool close_fd);

// Try to efficiently read the given number of bytes from a ROM-based reader.
// Returns a valid, non-NULL pointer to the requseted data if the reader points to ROM.
// Returns NULL if the reader does not point to ROM.
const uint8_t *mp_reader_try_read_rom(mp_reader_t *reader, size_t len);

#endif // MICROPY_INCLUDED_PY_READER_H
