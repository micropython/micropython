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
#include "py/mphal.h"

#include "ring_buffer.h"

// A simple ring buffer to memcpy data blocks in and out of buffer.
// This implementation:
//  - maintains one free byte to ensure lock-less thread safety
//    (for a single producer and a single consumer).
//  - prioritises efficient memory usage at the expense of additional
//    memcpy()s (eg. on buffer wrap).

// Number of milliseconds to wait (mp_hal_wait_ms()) in each loop
// while waiting for send or receive packet.
// Needs to be >15ms to permit yield to other tasks.
#ifndef BUSY_WAIT_MS
#define BUSY_WAIT_MS (25)
#endif

// Initialise a buffer of the requested size
// Will allocate an additional 9 bytes for buffer overhead
RB_STATIC buffer_t buffer_init(size_t size) {
    assert(size);

    // Allocate one extra byte to ensure thread safety
    buffer_t buffer = m_malloc0(size + sizeof(buffer_real_t) + 1);
    assert(buffer);

    buffer->size = size + 1;
    #ifdef RING_BUFFER_USE
    buffer->free = 1;
    #endif

    assert(buffer_empty(buffer));

    return buffer;
}

// Release and free the memory buffer
RB_STATIC void buffer_release(buffer_t buffer) {
    assert(buffer);
    buffer->size = buffer->head = buffer->tail = 0;
    m_free(buffer);
}

// Copy some data to the buffer - reject if buffer is full
RB_STATIC bool buffer_put(buffer_t buffer, const void *data, size_t len) {
    assert(buffer && buffer->memory && data);

    if (buffer_free(buffer) < len) {
        return false;
    }

    size_t end = (buffer->head + len) % buffer->size;
    const uint8_t *datap = data;
    if (end < buffer->head) {
        // Copy part of the data into the space left at the end of the buffer
        memcpy(buffer->memory + buffer->head, data, buffer->size - buffer->head);
        datap += (buffer->size - buffer->head);
        buffer->head = 0;
    }
    memcpy(buffer->memory + buffer->head, datap, end - buffer->head);
    buffer->head = end;

    return true;
}

// Copy data from the buffer - return -1 if error else end index
static int do_buffer_peek(const buffer_t buffer, void *data, size_t len) {
    assert(buffer && buffer->memory && data);

    if (buffer_used(buffer) < len) {
        return -1;
    }

    int tail = buffer->tail;
    int end = (tail + len) % buffer->size;
    uint8_t *datap = data;
    if (end < tail) {
        // Copy part of the data from the space left at the end of the buffer
        memcpy(data, buffer->memory + tail, buffer->size - tail);
        datap += (buffer->size - tail);
        tail = 0;
    }
    memcpy(datap, buffer->memory + tail, end - tail);

    return end;
}

// Copy data from the buffer - return false if buffer is empty
RB_STATIC bool buffer_get(buffer_t buffer, void *data, size_t len) {
    int end = do_buffer_peek(buffer, data, len);
    if (end < 0) {
        return false;
    }
    buffer->tail = end;
    return true;
}

#ifdef RING_BUFFER_DEBUG
// Print the current buffer state
RB_STATIC void buffer_print(char *name, const buffer_t buffer) {
    printf("%s: alloc=%3d size=%3d head=%3d, tail=%3d, used=%3d, free=%3d, start=%p\n",
        name,
        (int)buffer->size + sizeof(buffer_real_t),
        (int)buffer->size, (int)buffer->head, (int)buffer->tail,
        (int)buffer_used(buffer), (int)buffer_free(buffer), buffer->memory
        );
}
#endif

// Copy data from the buffer - wait if buffer is empty up to timeout_ms
RB_STATIC bool buffer_recv(
    buffer_t buffer, void *data, size_t len, int timeout_ms) {

    int64_t start = mp_hal_ticks_ms();
    while (!buffer_get(buffer, data, len)) {
        if (timeout_ms >= 0 && mp_hal_ticks_ms() - start >= timeout_ms) {
            return false;
        }
        mp_hal_delay_ms(BUSY_WAIT_MS);
    }
    return true;
}

#if 0
// Copy data to the buffer - wait if buffer is full up to timeout_ms
RB_STATIC bool buffer_send(
    buffer_t buffer, const void *data, size_t len, int timeout_ms) {

    int64_t start = mp_hal_ticks_ms();
    while (!buffer_put(buffer, data, len)) {
        if (timeout_ms >= 0 && mp_hal_ticks_ms() - start >= timeout_ms) {
            return false;
        }
        mp_hal_delay_ms(BUSY_WAIT_MS);
    }
    return true;
}
#endif
