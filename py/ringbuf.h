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

#include <stdint.h>

typedef struct _ringbuf_t {
    uint8_t *buf;
    uint16_t size;
    uint16_t iget;
    uint16_t iput;
} ringbuf_t;

// Static initialization:
// byte buf_array[N];
// ringbuf_t buf = {buf_array, sizeof(buf_array)};

// Dynamic initialization. This creates root pointer!
#define ringbuf_alloc(r, sz, long_lived)                   \
{ \
    (r)->buf = gc_alloc(sz, false, long_lived);   \
    (r)->size = sz; \
    (r)->iget = (r)->iput = 0; \
}

static inline int ringbuf_get(ringbuf_t *r) {
    if (r->iget == r->iput) {
        return -1;
    }
    uint8_t v = r->buf[r->iget++];
    if (r->iget >= r->size) {
        r->iget = 0;
    }
    return v;
}

static inline int ringbuf_put(ringbuf_t *r, uint8_t v) {
    uint32_t iput_new = r->iput + 1;
    if (iput_new >= r->size) {
        iput_new = 0;
    }
    if (iput_new == r->iget) {
        return -1;
    }
    r->buf[r->iput] = v;
    r->iput = iput_new;
    return 0;
}

static inline uint16_t ringbuf_count(ringbuf_t *r)
{
    volatile int count = r->iput - r->iget;
    if ( count < 0 ) {
        count += r->size;
    }

    return (uint16_t) count;
}

static inline void ringbuf_clear(ringbuf_t *r)
{
    r->iput = r->iget = 0;
}

// will overwrite old data
static inline void ringbuf_put_n(ringbuf_t* r, uint8_t* buf, uint8_t bufsize)
{
    for(uint8_t i=0; i < bufsize; i++) {
        if ( ringbuf_put(r, buf[i]) < 0 ) {
            // if full overwrite old data
            (void) ringbuf_get(r);
            ringbuf_put(r, buf[i]);
        }
    }
}
#endif // MICROPY_INCLUDED_PY_RINGBUF_H
