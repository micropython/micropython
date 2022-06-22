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

#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#ifdef RING_BUFFER_INCLUDE_AS_STATIC
// Use this on the ESP8266 (1M) to reduce the code size (by 88 bytes).
// This header will #include ring_buffer.c below with all functions
// declared "static". This is unnecessary on the ESP32.
#define RB_STATIC static
#else
#define RB_STATIC
#define RING_BUFFER_DEBUG
#endif

#include <stdbool.h>
#include <stdint.h>

typedef struct buffer_real_t buffer_real_t;

typedef buffer_real_t *buffer_t;

// Initialise the buffer of requested size
RB_STATIC buffer_t buffer_init(size_t size);

// Release the memory buffer - must be "free"ed by owner
RB_STATIC void buffer_release(buffer_t buffer);

// Copy some data to the buffer - reject if buffer is full
RB_STATIC bool buffer_put(buffer_t buffer, const void *data, size_t len);

// Copy data from the buffer - return false if buffer is empty
RB_STATIC bool buffer_get(buffer_t buffer, void *data, size_t len);

// Copy data from the buffer - wait if buffer is empty up to timeout_ms
RB_STATIC bool buffer_recv(
    buffer_t buffer, void *data, size_t len, int timeout_ms);

#if 0
// Copy data to the buffer - wait if buffer is full up to timeout_ms
RB_STATIC bool buffer_send(
    buffer_t buffer, const void *data, size_t len, int timeout_ms);
#endif

#ifdef RING_BUFFER_DEBUG
// Print the buffer stats
RB_STATIC void buffer_print(char *name, const buffer_t buffer);
#endif

// A simple ring buffer for memcpy data blocks in and out of buffer.
// This method needs to maintain one free byte to ensure lock-less thread
// safety for a single producer and a single consumer.
struct buffer_real_t {
    uint16_t size;
    volatile uint16_t head;
    uint16_t tail;
    uint8_t memory[];
};

// Is the buffer full?
static inline bool buffer_full(const buffer_t buffer) {
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

#endif // RING_BUFFER_H
