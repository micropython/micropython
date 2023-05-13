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

bool ringbuf_init(ringbuf_t *r, uint8_t *buf, size_t size) {
    r->buf = buf;
    r->size = size;
    r->used = 0;
    r->next_read = 0;
    r->next_write = 0;
    return r->buf != NULL;
}

// Dynamic initialization. This should be accessible from a root pointer..
bool ringbuf_alloc(ringbuf_t *r, size_t size, bool long_lived) {
    bool result = ringbuf_init(r, gc_alloc(size, false, long_lived), size);
    return result;
}

void ringbuf_deinit(ringbuf_t *r) {
    // Free buf by doing nothing and letting gc take care of it. If the VM has finished already,
    // this will be safe.
    r->buf = (uint8_t *)NULL;
    r->size = 0;
    ringbuf_clear(r);
}

size_t ringbuf_size(ringbuf_t *r) {
    return r->size;
}

// Return -1 if buffer is empty, else return byte fetched.
int ringbuf_get(ringbuf_t *r) {
    if (r->used < 1) {
        return -1;
    }
    uint8_t v = r->buf[r->next_read];
    r->next_read++;
    if (r->next_read >= r->size) {
        r->next_read = 0;
    }
    r->used--;
    return v;
}

int ringbuf_get16(ringbuf_t *r) {
    if (r->used < 2) {
        return -1;
    }
    int high_byte = ringbuf_get(r);
    int low_byte = ringbuf_get(r);
    return (high_byte << 8) | low_byte;
}

// Return -1 if no room in buffer, else return 0.
int ringbuf_put(ringbuf_t *r, uint8_t v) {
    if (r->used >= r->size) {
        return -1;
    }
    r->buf[r->next_write] = v;
    r->next_write++;
    if (r->next_write >= r->size) {
        r->next_write = 0;
    }
    r->used++;
    return 0;
}

int ringbuf_put16(ringbuf_t *r, uint16_t v) {
    if (r->size - r->used < 2) {
        return -1;
    }
    ringbuf_put(r, (v >> 8) & 0xff);
    ringbuf_put(r, v & 0xff);
    return 0;
}

void ringbuf_clear(ringbuf_t *r) {
    r->next_write = 0;
    r->next_read = 0;
    r->used = 0;
}

// Number of free slots that can be written.
size_t ringbuf_num_empty(ringbuf_t *r) {
    return r->size - r->used;
}

// Number of bytes available to read.
size_t ringbuf_num_filled(ringbuf_t *r) {
    return r->used;
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
