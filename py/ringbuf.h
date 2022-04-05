/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Paul Sokolovsky
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

#include "py/gc.h"

#include <stddef.h>
#include <stdint.h>

typedef struct _ringbuf_t {
    uint8_t *buf;
    // Allocated size; capacity is one less. Don't reference this directly.
    uint32_t size;
    uint32_t iget;
    uint32_t iput;
} ringbuf_t;

// Note that the capacity of the buffer is N-1!

// For static initialization use ringbuf_init()
bool ringbuf_init(ringbuf_t *r, uint8_t *buf, size_t capacity);
bool ringbuf_alloc(ringbuf_t *r, size_t capacity, bool long_lived);
void ringbuf_free(ringbuf_t *r);
size_t ringbuf_capacity(ringbuf_t *r);
int ringbuf_get(ringbuf_t *r);
int ringbuf_put(ringbuf_t *r, uint8_t v);
void ringbuf_clear(ringbuf_t *r);
size_t ringbuf_num_empty(ringbuf_t *r);
size_t ringbuf_num_filled(ringbuf_t *r);
size_t ringbuf_put_n(ringbuf_t *r, const uint8_t *buf, size_t bufsize);
size_t ringbuf_get_n(ringbuf_t *r, uint8_t *buf, size_t bufsize);

// Note: big-endian. No-op if not enough room available for both bytes.
int ringbuf_get16(ringbuf_t *r);
int ringbuf_peek16(ringbuf_t *r);
int ringbuf_put16(ringbuf_t *r, uint16_t v);

#endif // MICROPY_INCLUDED_PY_RINGBUF_H
