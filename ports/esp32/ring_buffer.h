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

#include <stdbool.h>
#include <stdint.h>

typedef struct buffer_real_t buffer_real_t;

typedef buffer_real_t *buffer_t;

// Initialise the buffer of requested size
buffer_t buffer_init(size_t size);

// Use an existing buffer - don't allocate any new memory
buffer_t buffer_use(uint8_t *buf, size_t size);

// Release the memory buffer - must be "free"ed by owner
void buffer_release(buffer_t buffer);

// Reset the buffer pointers discarding any data in the buffer
void buffer_reset(buffer_t buffer);

// Copy some data to the buffer - reject if buffer is full
bool buffer_put(buffer_t buffer, const void *data, size_t len);

// Copy data from the buffer - return -1 if buffer is empty
bool buffer_get(buffer_t buffer, void *data, size_t len);

// Copy data from the buffer - return -1 if buffer is empty
bool buffer_peek(buffer_t buffer, void *data, size_t len);

// Print the buffer stats
void buffer_print(char *name, buffer_t buffer);

// A simple ring buffer for memcpy data blocks in and out of buffer.
// This method needs to maintain one free byte to ensure lock-less thread
// safety for a single producer and a single consumer.
struct buffer_real_t {
    uint16_t size;
    volatile uint16_t head;
    uint16_t tail;
    uint16_t free;
    uint8_t memory[];
};

// Is the buffer full?
static inline bool buffer_full(buffer_t buffer) {
    assert(buffer);

    return (buffer->head + 1) % buffer->size == buffer->tail;
}

// Is the buffer empty?
static inline bool buffer_empty(const buffer_t buffer) {
    assert(buffer);

    return buffer->head == buffer->tail;
}

// Total size (capacity) of the buffer
static inline size_t buffer_size(const buffer_t buffer) {
    assert(buffer);

    return buffer->size - 1;
}

// Number of bytes available to be read from the buffer
static inline size_t buffer_used(const buffer_t buffer) {
    assert(buffer);

    int diff = buffer->head - buffer->tail;
    return (diff >= 0)
            ? diff
            : (buffer->size + diff);
}

// Number of bytes available to be written to the buffer
static inline size_t buffer_free(const buffer_t buffer) {
    assert(buffer);

    int diff = buffer->head - buffer->tail;
    return (diff >= 0)
            ? buffer->size - diff - 1
            : -diff - 1;
}

// Flush the buffer by setting tail = head.
static inline void buffer_flush(const buffer_t buffer) {
    assert(buffer);

    buffer->tail = buffer->head;
}
