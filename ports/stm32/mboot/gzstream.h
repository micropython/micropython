/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Damien P. George
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
#ifndef MICROPY_INCLUDED_STM32_MBOOT_GZSTREAM_H
#define MICROPY_INCLUDED_STM32_MBOOT_GZSTREAM_H

#include <stddef.h>
#include <stdint.h>

typedef int (*stream_open_t)(void *stream, const char *fname);
typedef void (*stream_close_t)(void *stream);
typedef int (*stream_read_t)(void *stream, uint8_t *buf, size_t len);

typedef struct _stream_methods_t {
    stream_open_t open;
    stream_close_t close;
    stream_read_t read;
} stream_methods_t;

int gz_stream_init(void *stream_data, stream_read_t stream_read);
int gz_stream_read(size_t len, uint8_t *buf);

#endif // MICROPY_INCLUDED_STM32_MBOOT_GZSTREAM_H
