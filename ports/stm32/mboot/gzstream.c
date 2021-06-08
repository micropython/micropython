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

#include <string.h>

#include "py/mphal.h"
#include "extmod/uzlib/uzlib.h"
#include "gzstream.h"
#include "mboot.h"

#if MBOOT_FSLOAD || MBOOT_ENABLE_PACKING

#define DICT_SIZE (1 << 15)

typedef struct _gz_stream_t {
    void *stream_data;
    stream_read_t stream_read;
    struct uzlib_uncomp tinf;
    uint8_t buf[512];
    uint8_t dict[DICT_SIZE];
} gz_stream_t;

static gz_stream_t gz_stream SECTION_NOZERO_BSS;

static int gz_stream_read_src(TINF_DATA *tinf) {
    int n = gz_stream.stream_read(gz_stream.stream_data, gz_stream.buf, sizeof(gz_stream.buf));
    if (n < 0) {
        // Stream error
        return -1;
    }
    if (n == 0) {
        // No data / EOF
        return -1;
    }

    tinf->source = gz_stream.buf + 1;
    tinf->source_limit = gz_stream.buf + n;
    return gz_stream.buf[0];
}

int gz_stream_init_from_raw_data(const uint8_t *data, size_t len) {
    memset(&gz_stream.tinf, 0, sizeof(gz_stream.tinf));
    gz_stream.tinf.source = data;
    gz_stream.tinf.source_limit = data + len;

    uzlib_uncompress_init(&gz_stream.tinf, gz_stream.dict, DICT_SIZE);

    return 0;
}

int gz_stream_init_from_stream(void *stream_data, stream_read_t stream_read) {
    gz_stream.stream_data = stream_data;
    gz_stream.stream_read = stream_read;

    memset(&gz_stream.tinf, 0, sizeof(gz_stream.tinf));
    gz_stream.tinf.source_read_cb = gz_stream_read_src;

    int st = uzlib_gzip_parse_header(&gz_stream.tinf);
    if (st != TINF_OK) {
        return -MBOOT_ERRNO_GUNZIP_FAILED;
    }

    uzlib_uncompress_init(&gz_stream.tinf, gz_stream.dict, DICT_SIZE);

    return 0;
}

int gz_stream_read(size_t len, uint8_t *buf) {
    if (gz_stream.tinf.source == NULL && gz_stream.tinf.source_read_cb == NULL) {
        // End of stream.
        return 0;
    }
    gz_stream.tinf.dest = buf;
    gz_stream.tinf.dest_limit = buf + len;
    int st = uzlib_uncompress_chksum(&gz_stream.tinf);
    if (st < 0) {
        return st;
    }
    if (st == TINF_DONE) {
        // Indicate end-of-stream for subsequent calls.
        gz_stream.tinf.source = NULL;
        gz_stream.tinf.source_read_cb = NULL;
    }
    return gz_stream.tinf.dest - buf;
}

#endif // MBOOT_FSLOAD || MBOOT_ENABLE_PACKING
