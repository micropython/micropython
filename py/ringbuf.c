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

#include "ringbuf.h"

bool ringbuf_init(ringbuf_t *r, uint8_t *buf, size_t capacity) {
    r->buf = buf;
    r->size = capacity;
    r->iget = r->iput = 0;
    return r->buf != NULL;
}

// Dynamic initialization. This should be accessible from a root pointer.
// capacity is the number of bytes the ring buffer can hold. The actual
// size of the buffer is one greater than that, due to how the buffer
// handles empty and full statuses.
bool ringbuf_alloc(ringbuf_t *r, size_t capacity, bool long_lived) {
    r->buf = gc_alloc(capacity + 1, false, long_lived);
    r->size = capacity + 1;
    r->iget = r->iput = 0;
    return r->buf != NULL;
}

void ringbuf_free(ringbuf_t *r) {
    // Free buf by letting gc take care of it. If the VM has finished already,
    // this will be safe.
    r->buf = (uint8_t *)NULL;
    r->size = 0;
    ringbuf_clear(r);
}

size_t ringbuf_capacity(ringbuf_t *r) {
    return r->size - 1;
}

// Returns -1 if buffer is empty, else returns byte fetched.
int ringbuf_get(ringbuf_t *r) {
    if (r->iget == r->iput) {
        return -1;
    }
    uint8_t v = r->buf[r->iget++];
    if (r->iget >= r->size) {
        r->iget = 0;
    }
    return v;
}

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

// Returns -1 if no room in buffer, else returns 0.
int ringbuf_put(ringbuf_t *r, uint8_t v) {
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

void ringbuf_clear(ringbuf_t *r) {
    r->iput = r->iget = 0;
}

// Number of free slots that can be written.
size_t ringbuf_num_empty(ringbuf_t *r) {
    return (r->size + r->iget - r->iput - 1) % r->size;
}

// Number of bytes available to read.
size_t ringbuf_num_filled(ringbuf_t *r) {
    return (r->size + r->iput - r->iget) % r->size;
}

// If the ring buffer fills up, not all bytes will be written.
// Returns how many bytes were successfully written.
size_t ringbuf_put_n(ringbuf_t *r, const uint8_t *buf, size_t bufsize) {
    for (size_t i = 0; i < bufsize; i++) {
        if (ringbuf_put(r, buf[i]) < 0) {
            // If ringbuf is full, give up and return how many bytes
            // we wrote so far.
            return i;
        }
    }
    return bufsize;
}

// Returns how many bytes were fetched.
size_t ringbuf_get_n(ringbuf_t *r, uint8_t *buf, size_t bufsize) {
    for (size_t i = 0; i < bufsize; i++) {
        int b = ringbuf_get(r);
        if (b < 0) {
            return i;
        }
        buf[i] = b;
    }
    return bufsize;
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
