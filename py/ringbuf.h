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
#include <string.h>

#include "py/mpconfig.h"

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

static inline int ringbuf_peek(ringbuf_t *r) {
    if (r->iget == r->iput) {
        return -1;
    }
    return r->buf[r->iget];
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

static inline void ringbuf_memcpy_get_internal(ringbuf_t *r, uint8_t *data, size_t data_len) {
    // No bounds / space checking is performed here so ensure available size is checked before running this
    // otherwise data loss or buffer overflow can occur.
    uint32_t iget = r->iget;
    uint32_t iget_a = (iget + data_len) % r->size;
    uint8_t *datap = data;
    if (iget_a < iget) {
        // Copy part of the data from the space left at the end of the buffer
        memcpy(datap, r->buf + iget, r->size - iget);
        datap += (r->size - iget);
        iget = 0;
    }
    memcpy(datap, r->buf + iget, iget_a - iget);
    r->iget = iget_a;
}

static inline void ringbuf_memcpy_put_internal(ringbuf_t *r, const uint8_t *data, size_t data_len) {
    // No bounds / space checking is performed here so ensure free size is checked before running this
    // otherwise data loss or buffer overflow can occur.
    uint32_t iput = r->iput;
    uint32_t iput_a = (iput + data_len) % r->size;
    const uint8_t *datap = data;
    if (iput_a < iput) {
        // Copy part of the data to the end of the buffer
        memcpy(r->buf + iput, datap, r->size - iput);
        datap += (r->size - iput);
        iput = 0;
    }
    memcpy(r->buf + iput, datap, iput_a - iput);
    r->iput = iput_a;
}

// Note: big-endian. No-op if not enough room available for both bytes.
int ringbuf_get16(ringbuf_t *r);
int ringbuf_peek16(ringbuf_t *r);
int ringbuf_put16(ringbuf_t *r, uint16_t v);

int ringbuf_get_bytes(ringbuf_t *r, uint8_t *data, size_t data_len);
int ringbuf_put_bytes(ringbuf_t *r, const uint8_t *data, size_t data_len);

#endif // MICROPY_INCLUDED_PY_RINGBUF_H
