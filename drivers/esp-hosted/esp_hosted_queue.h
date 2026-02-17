/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (C) Arduino SRL
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
 *
 * A simple, fixed, FIFO queue for ESP hosted frames.
 */

#ifndef MICROPY_INCLUDED_DRIVERS_ESP_HOSTED_QUEUE_H
#define MICROPY_INCLUDED_DRIVERS_ESP_HOSTED_QUEUE_H

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

typedef struct {
    size_t head;
    size_t tail;
    size_t count;
    size_t size;
    size_t el_size;
    uint8_t *data;
} esp_hosted_queue_t;

static inline void esp_hosted_queue_init(esp_hosted_queue_t *q, void *buf, size_t el_size, size_t size) {
    q->head = 0;
    q->tail = 0;
    q->count = 0;
    q->size = size;
    q->el_size = el_size;
    q->data = (uint8_t *)buf;
}

static inline bool esp_hosted_queue_full(esp_hosted_queue_t *q) {
    return q->count >= q->size;
}

static inline bool esp_hosted_queue_empty(esp_hosted_queue_t *q) {
    return q->count == 0;
}

static inline int esp_hosted_queue_put(esp_hosted_queue_t *q, const void *elem) {
    if (q->count >= q->size) {
        return -1;
    }
    memcpy(q->data + q->tail * q->el_size, elem, q->el_size);
    q->tail = (q->tail + 1) % q->size;
    q->count++;
    return 0;
}

static inline void *esp_hosted_queue_get(esp_hosted_queue_t *q) {
    if (q->count == 0) {
        return NULL;
    }
    void *ptr = q->data + q->head * q->el_size;
    q->head = (q->head + 1) % q->size;
    q->count--;
    return ptr;
}

static inline void *esp_hosted_queue_peek(esp_hosted_queue_t *q) {
    if (q->count == 0) {
        return NULL;
    }
    return q->data + q->head * q->el_size;
}
#endif // MICROPY_INCLUDED_DRIVERS_ESP_HOSTED_QUEUE_H
