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
#include "lib/uzlib/uzlib.h"
#include "gzstream.h"
#include "mboot.h"

#if MBOOT_FSLOAD || MBOOT_ENABLE_PACKING

#define DICT_SIZE (1 << 15)

typedef struct _gz_stream_t {
    void *stream_data;
    stream_read_t stream_read;
    uzlib_uncomp_t decomp;
    uint8_t buf[512];
    uint8_t dict[DICT_SIZE];
} gz_stream_t;

static gz_stream_t gz_stream SECTION_NOZERO_BSS;

static int gz_stream_read_src(void *data) {
    uzlib_uncomp_t *decomp = data;
    int n = gz_stream.stream_read(gz_stream.stream_data, gz_stream.buf, sizeof(gz_stream.buf));
    if (n < 0) {
        // Stream error
        return -1;
    }
    if (n == 0) {
        // No data / EOF
        return -1;
    }

    decomp->source = gz_stream.buf + 1;
    decomp->source_limit = gz_stream.buf + n;
    return gz_stream.buf[0];
}

int gz_stream_init_from_raw_data(const uint8_t *data, size_t len) {
    memset(&gz_stream.decomp, 0, sizeof(gz_stream.decomp));
    gz_stream.decomp.source = data;
    gz_stream.decomp.source_limit = data + len;

    uzlib_uncompress_init(&gz_stream.decomp, gz_stream.dict, DICT_SIZE);

    return 0;
}

int gz_stream_init_from_stream(void *stream_data, stream_read_t stream_read) {
    gz_stream.stream_data = stream_data;
    gz_stream.stream_read = stream_read;

    memset(&gz_stream.decomp, 0, sizeof(gz_stream.decomp));
    gz_stream.decomp.source_read_data = &gz_stream.decomp;
    gz_stream.decomp.source_read_cb = gz_stream_read_src;

    int header_wbits;
    int st = uzlib_parse_zlib_gzip_header(&gz_stream.decomp, &header_wbits);
    if (st != UZLIB_HEADER_GZIP) {
        return -MBOOT_ERRNO_GUNZIP_FAILED;
    }

    uzlib_uncompress_init(&gz_stream.decomp, gz_stream.dict, DICT_SIZE);

    return 0;
}

int gz_stream_read(size_t len, uint8_t *buf) {
    if (gz_stream.decomp.source == NULL && gz_stream.decomp.source_read_cb == NULL) {
        // End of stream.
        return 0;
    }
    gz_stream.decomp.dest = buf;
    gz_stream.decomp.dest_limit = buf + len;
    int st = uzlib_uncompress_chksum(&gz_stream.decomp);
    if (st < 0) {
        return st;
    }
    if (st == UZLIB_DONE) {
        // Indicate end-of-stream for subsequent calls.
        gz_stream.decomp.source = NULL;
        gz_stream.decomp.source_read_cb = NULL;
    }
    return gz_stream.decomp.dest - buf;
}

#endif // MBOOT_FSLOAD || MBOOT_ENABLE_PACKING
