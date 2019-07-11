/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Paul Sokolovsky
 * Copyright (c) 2019 Jim Mussared
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
#ifndef MICROPY_INCLUDED_PY_RINGBUF_H
#define MICROPY_INCLUDED_PY_RINGBUF_H

#include "py/mpconfig.h"
#include "py/misc.h"

// In order to avoid wasting a byte in the ringbuffer, and to reduce code
// size, the two indicies (iget & iput) do not wrap around at size. Instead
// they rely on unsigned integer overflow.
// This means that the empty state (iget == iput) can be distinguished from
// the full state (iput == iget + size).
// However, this means that the size must divide evenly into 2^16. Most uses
// of the ringbuffer use small power-of-two sizes anyway.

typedef struct _ringbuf_t {
    uint8_t *buf;
    uint16_t size;
    uint16_t iget;
    uint16_t iput;
} ringbuf_t;

// Static initialization:
// byte buf_array[N]; // N must be divisible into 2^16.
// ringbuf_t buf = {buf_array, sizeof(buf_array)};

// Dynamic initialization.
#define ringbuf_alloc(r, sz) \
{ \
    (r)->size = ringbuf_fix_len(sz); \
    (r)->buf = m_new(uint8_t, (r)->size); \
    (r)->iget = (r)->iput = 0; \
}

// Rounds len to the nearest power of two (to ensure divisibility into 2^16).
// Use this if dynamically allocating buf.
uint16_t ringbuf_fix_len(uint16_t len);

static inline bool ringbuf_is_empty(ringbuf_t *r) {
    return r->iput == r->iget;
}

// Returns the next byte, or -1 if empty.
int ringbuf_get(ringbuf_t *r);

// Returns -1 if full, 0 otherwise.
int ringbuf_put(ringbuf_t *r, uint8_t v);

#endif // MICROPY_INCLUDED_PY_RINGBUF_H
