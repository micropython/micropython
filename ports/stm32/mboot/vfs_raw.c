/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Damien P. George
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

#include "mboot.h"
#include "vfs.h"

#if MBOOT_FSLOAD && MBOOT_VFS_RAW

#ifndef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif

int vfs_raw_mount(vfs_raw_context_t *ctx, mboot_addr_t seg0_base_addr, mboot_addr_t seg0_byte_len, mboot_addr_t seg1_base_addr, mboot_addr_t seg1_byte_len) {
    ctx->seg0_base_addr = seg0_base_addr;
    ctx->seg0_byte_len = seg0_byte_len;
    ctx->seg1_base_addr = seg1_base_addr;
    ctx->seg1_byte_len = seg1_byte_len;
    return 0;
}

static int vfs_raw_stream_open(void *stream_in, const char *fname) {
    vfs_raw_context_t *stream = stream_in;
    (void)fname;
    stream->file_pos = 0;
    return 0;
}

static void vfs_raw_stream_close(void *stream_in) {
    (void)stream_in;
}

static int vfs_raw_stream_read(void *stream_in, uint8_t *buf, size_t len) {
    vfs_raw_context_t *stream = stream_in;
    size_t orig_len = len;
    while (len) {
        mboot_addr_t addr;
        mboot_addr_t remain;
        if (stream->file_pos < stream->seg0_byte_len) {
            // Reading from segment 0.
            mboot_addr_t seg0_pos = stream->file_pos;
            addr = stream->seg0_base_addr + seg0_pos;
            remain = stream->seg0_byte_len - seg0_pos;
        } else {
            // Reading from segment 1.
            mboot_addr_t seg1_pos = stream->file_pos - stream->seg0_byte_len;
            addr = stream->seg1_base_addr + seg1_pos;
            remain = stream->seg1_byte_len - seg1_pos;
            if (!remain) {
                // At the end of segment 1.
                break;
            }
        }
        size_t l = MIN(len, remain);
        hw_read(addr, l, buf);
        stream->file_pos += l;
        buf += l;
        len -= l;
    }
    return orig_len - len;
}

const stream_methods_t vfs_raw_stream_methods = {
    vfs_raw_stream_open,
    vfs_raw_stream_close,
    vfs_raw_stream_read,
};

#endif // MBOOT_FSLOAD && MBOOT_VFS_RAW
