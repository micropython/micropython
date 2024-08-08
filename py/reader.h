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

#define MP_READER_EOF ((uintptr_t)(-1))

// Reader ioctl request codes.
#define MP_READER_CLOSE (1)
#define MP_READER_MEMMAP (2)

// Used as arg for MP_READER_MEMMAP ioctl request.
typedef struct _mp_reader_ioctl_memmap_t {
    size_t len;
    const uint8_t *ptr;
} mp_reader_ioctl_memmap_t;

typedef struct _mp_reader_t {
    // Pointer to the context of this reader, passed as the first argument to the methods below.
    void *data;

    // The readbyte function must return the next byte in the input stream.
    // It must return MP_READER_EOF if end of stream.  It can be called again after returning
    // MP_READER_EOF, and in that case must return MP_READER_EOF.
    uintptr_t (*readbyte)(void *data);

    // Ioctl method for performing various control actions.
    // On error it must return a negative errno value.
    intptr_t (*ioctl)(void *data, uintptr_t request, uintptr_t arg);
} mp_reader_t;

void mp_reader_new_mem(mp_reader_t *reader, const byte *buf, size_t len, size_t free_len);
void mp_reader_new_file(mp_reader_t *reader, qstr filename);
void mp_reader_new_file_from_fd(mp_reader_t *reader, int fd, bool close_fd);

#endif // MICROPY_INCLUDED_PY_READER_H
