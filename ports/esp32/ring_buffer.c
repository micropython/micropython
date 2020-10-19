/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Glenn Moloney @glenn20
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

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "py/mpconfig.h"
#include "py/misc.h"

#include "ring_buffer.h"

// A simple ring buffer to memcpy data blocks in and out of buffer.
// This implementation:
//  - maintains one free byte to ensure lock-less thread safety
//    (for a single producer and a single consumer).
//  - prioritises efficient memory usage at the expense of additional
//    memcpy()s (eg. on buffer wrap).

// Initialise a buffer of the requested size
// Will allocate an additional 9 bytes for buffer overhead
buffer_t buffer_init(size_t size) {
    assert(size);

    // Allocate one extra byte to ensure threadsafety
    buffer_t buffer = m_malloc0(size + sizeof(buffer_real_t) + 1);
    assert(buffer);

    buffer->size = size + 1;
    buffer->free = 1;
    buffer_reset(buffer);

    assert(buffer_empty(buffer));

    return buffer;
}

// Use the provided memory as buffer
buffer_t buffer_use(uint8_t *buf, size_t size) {
    assert(size > sizeof(buffer_real_t) + 16);

    buffer_t buffer = (buffer_t)buf;
    assert(buffer);

    buffer->size = size - sizeof(buffer_real_t);
    buffer->free = 0;
    buffer_reset(buffer);

    assert(buffer_empty(buffer));

    return buffer;
}

// Release and free the memory buffer
void buffer_release(buffer_t buffer) {
    assert(buffer);
    buffer->size = buffer->head = buffer->tail = 0;
    if (buffer->free) {
        m_free(buffer);
    }
}

// Reset the buffer pointers discarding any data in the buffer
void buffer_reset(buffer_t buffer) {
    assert(buffer);

    buffer->head = buffer->tail = 0;
}

// Copy some data to the buffer - reject if buffer is full
bool buffer_put(buffer_t buffer, const void *data, size_t len) {
    assert(buffer && buffer->memory && data);

    if (buffer_free(buffer) < len) {
        return false;
    }

    size_t end = (buffer->head + len) % buffer->size;
    if (end < buffer->head) {
        // Copy part of the data into the space left at the end of the buffer
        memcpy(buffer->memory + buffer->head, data, buffer->size - buffer->head);
        data += (buffer->size - buffer->head);
        buffer->head = 0;
    }
    memcpy(buffer->memory + buffer->head, data, end - buffer->head);
    buffer->head = end;

    return true;
}

// Copy data from the buffer - return -1 if error else end index
STATIC int do_buffer_peek(buffer_t buffer, void *data, size_t len) {
    assert(buffer && buffer->memory && data);

    if (buffer_used(buffer) < len) {
        return -1;
    }

    int tail = buffer->tail;
    int end = (tail + len) % buffer->size;
    if (end < tail) {
        // Copy part of the data from the space left at the end of the buffer
        memcpy(data, buffer->memory + tail, buffer->size - tail);
        data += (buffer->size - tail);
        tail = 0;
    }
    memcpy(data, buffer->memory + tail, end - tail);

    return end;
}

// Peek data from the buffer - return false if buffer is empty
bool buffer_peek(buffer_t buffer, void *data, size_t len) {
    return do_buffer_peek(buffer, data, len) >= 0;
}

// Copy data from the buffer - return false if buffer is empty
bool buffer_get(buffer_t buffer, void *data, size_t len) {
    int end = do_buffer_peek(buffer, data, len);
    if (end < 0) {
        return false;
    }
    buffer->tail = end;
    return true;
}

// Print the current buffer state
void buffer_print(char *name, buffer_t buffer) {
    printf("%s: alloc=%3d size=%3d head=%3d, tail=%3d, used=%3d, free=%3d, start=%p\n",
        name,
        (int)buffer->size + sizeof(buffer_real_t),
        (int)buffer->size, (int)buffer->head, (int)buffer->tail,
        (int)buffer_used(buffer), (int)buffer_free(buffer), buffer->memory
        );
}
