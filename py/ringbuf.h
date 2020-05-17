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

#include <stddef.h>
#include <stdint.h>

#ifdef _MSC_VER
#include "py/mpconfig.h" // For inline.
#endif

typedef struct _ringbuf_t {
    uint8_t *buf;
    uint16_t size;
    uint16_t iget;
    uint16_t iput;
} ringbuf_t;

// Static initialization:
// byte buf_array[N];
// ringbuf_t buf = {buf_array, sizeof(buf_array)};

// Dynamic initialization. This needs to become findable as a root pointer!
#define ringbuf_alloc(r, sz) \
    { \
        (r)->buf = m_new(uint8_t, sz); \
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

static inline size_t ringbuf_free(ringbuf_t *r) {
    return (r->size + r->iget - r->iput - 1) % r->size;
}

static inline size_t ringbuf_avail(ringbuf_t *r) {
    return (r->size + r->iput - r->iget) % r->size;
}

// Note: big-endian. No-op if not enough room available for both bytes.
int ringbuf_get16(ringbuf_t *r);
int ringbuf_peek16(ringbuf_t *r);
int ringbuf_put16(ringbuf_t *r, uint16_t v);

#endif // MICROPY_INCLUDED_PY_RINGBUF_H
