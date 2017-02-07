/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2017 Damien P. George
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

#include <stdio.h>
#include <string.h>

#include "py/nlr.h"
#include "py/stream.h"
#include "py/reader.h"
#include "extmod/vfs.h"

#if MICROPY_READER_VFS

typedef struct _mp_reader_vfs_t {
    mp_obj_t file;
    uint16_t len;
    uint16_t pos;
    byte buf[24];
} mp_reader_vfs_t;

STATIC mp_uint_t mp_reader_vfs_readbyte(void *data) {
    mp_reader_vfs_t *reader = (mp_reader_vfs_t*)data;
    if (reader->pos >= reader->len) {
        if (reader->len < sizeof(reader->buf)) {
            return MP_READER_EOF;
        } else {
            int errcode;
            reader->len = mp_stream_rw(reader->file, reader->buf, sizeof(reader->buf),
                &errcode, MP_STREAM_RW_READ | MP_STREAM_RW_ONCE);
            if (errcode != 0) {
                // TODO handle errors properly
                return MP_READER_EOF;
            }
            if (reader->len == 0) {
                return MP_READER_EOF;
            }
            reader->pos = 0;
        }
    }
    return reader->buf[reader->pos++];
}

STATIC void mp_reader_vfs_close(void *data) {
    mp_reader_vfs_t *reader = (mp_reader_vfs_t*)data;
    mp_stream_close(reader->file);
    m_del_obj(mp_reader_vfs_t, reader);
}

int mp_reader_new_file(mp_reader_t *reader, const char *filename) {
    mp_reader_vfs_t *rf = m_new_obj_maybe(mp_reader_vfs_t);
    if (rf == NULL) {
        return MP_ENOMEM;
    }
    // TODO we really should just let this function raise a uPy exception
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_obj_t arg = mp_obj_new_str(filename, strlen(filename), false);
        rf->file = mp_vfs_open(1, &arg, (mp_map_t*)&mp_const_empty_map);
        int errcode;
        rf->len = mp_stream_rw(rf->file, rf->buf, sizeof(rf->buf), &errcode, MP_STREAM_RW_READ | MP_STREAM_RW_ONCE);
        nlr_pop();
        if (errcode != 0) {
            return errcode;
        }
    } else {
        return MP_ENOENT; // assume error was "file not found"
    }
    rf->pos = 0;
    reader->data = rf;
    reader->readbyte = mp_reader_vfs_readbyte;
    reader->close = mp_reader_vfs_close;
    return 0; // success
}

#endif // MICROPY_READER_VFS
