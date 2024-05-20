// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Paul Sokolovsky
// SPDX-FileCopyrightText: Copyright (c) 2019 Jim Mussared
// SPDX-FileCopyrightText: Copyright (c) 2020 Dan Halbert for Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT
#pragma once

#include "py/gc.h"

#include <stddef.h>
#include <stdint.h>

// CIRCUITPY-CHANGE: thoroughly reworked

typedef struct _ringbuf_t {
    uint8_t *buf;
    uint32_t size;
    uint32_t used;
    uint32_t next_read;
    uint32_t next_write;
} ringbuf_t;

// For static initialization with an existing buffer, use ringbuf_init().
bool ringbuf_init(ringbuf_t *r, uint8_t *buf, size_t capacity);

// For allocation of a buffer on the heap, use ringbuf_alloc().
bool ringbuf_alloc(ringbuf_t *r, size_t capacity);

// Mark ringbuf as no longer in use, and allow any heap storage to be freed by gc.
void ringbuf_deinit(ringbuf_t *r);

// Note: Ringbuf operations are not atomic.
size_t ringbuf_size(ringbuf_t *r);
int ringbuf_get(ringbuf_t *r);
int ringbuf_put(ringbuf_t *r, uint8_t v);
void ringbuf_clear(ringbuf_t *r);
size_t ringbuf_num_empty(ringbuf_t *r);
size_t ringbuf_num_filled(ringbuf_t *r);
size_t ringbuf_put_n(ringbuf_t *r, const uint8_t *buf, size_t bufsize);
size_t ringbuf_get_n(ringbuf_t *r, uint8_t *buf, size_t bufsize);

// Note: big-endian. Return -1 if can't read or write two bytes.
int ringbuf_get16(ringbuf_t *r);
int ringbuf_put16(ringbuf_t *r, uint16_t v);

int ringbuf_get_bytes(ringbuf_t *r, uint8_t *data, size_t data_len);
int ringbuf_put_bytes(ringbuf_t *r, const uint8_t *data, size_t data_len);
