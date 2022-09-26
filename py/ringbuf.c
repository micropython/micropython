/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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

#include "ringbuf.h"

int ringbuf_get16(ringbuf_t *r) {
    int v = ringbuf_peek16(r);
    if (v == -1) {
        return v;
    }
    r->iget += 2;
    if (r->iget >= r->size) {
        r->iget -= r->size;
    }
    return v;
}

int ringbuf_peek16(ringbuf_t *r) {
    if (r->iget == r->iput) {
        return -1;
    }
    uint32_t iget_a = r->iget + 1;
    if (iget_a == r->size) {
        iget_a = 0;
    }
    if (iget_a == r->iput) {
        return -1;
    }
    return (r->buf[r->iget] << 8) | (r->buf[iget_a]);
}

int ringbuf_put16(ringbuf_t *r, uint16_t v) {
    uint32_t iput_a = r->iput + 1;
    if (iput_a == r->size) {
        iput_a = 0;
    }
    if (iput_a == r->iget) {
        return -1;
    }
    uint32_t iput_b = iput_a + 1;
    if (iput_b == r->size) {
        iput_b = 0;
    }
    if (iput_b == r->iget) {
        return -1;
    }
    r->buf[r->iput] = (v >> 8) & 0xff;
    r->buf[iput_a] = v & 0xff;
    r->iput = iput_b;
    return 0;
}

// Returns:
//    0: Success
//   -1: Not enough data available to complete read (try again later)
//   -2: Requested read is larger than buffer - will never succeed
int ringbuf_get_bytes(ringbuf_t *r, uint8_t *data, size_t data_len) {
    if (ringbuf_avail(r) < data_len) {
        return (r->size <= data_len) ? -2 : -1;
    }
    ringbuf_memcpy_get_internal(r, data, data_len);
    return 0;
}

// Returns:
//    0: Success
//   -1: Not enough free space available to complete write (try again later)
//   -2: Requested write is larger than buffer - will never succeed
int ringbuf_put_bytes(ringbuf_t *r, const uint8_t *data, size_t data_len) {
    if (ringbuf_free(r) < data_len) {
        return (r->size <= data_len) ? -2 : -1;
    }
    ringbuf_memcpy_put_internal(r, data, data_len);
    return 0;
}
