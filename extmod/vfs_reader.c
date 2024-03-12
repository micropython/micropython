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
#include <stdlib.h>

#include "py/runtime.h"
#include "py/stream.h"
#include "py/reader.h"
#include "extmod/vfs.h"

#if MICROPY_READER_VFS

#ifndef MICROPY_READER_VFS_DEFAULT_BUFFER_SIZE
#define MICROPY_READER_VFS_DEFAULT_BUFFER_SIZE (2 * MICROPY_BYTES_PER_GC_BLOCK - offsetof(mp_reader_vfs_t, buf))
#endif
#define MICROPY_READER_VFS_MIN_BUFFER_SIZE (MICROPY_BYTES_PER_GC_BLOCK - offsetof(mp_reader_vfs_t, buf))
#define MICROPY_READER_VFS_MAX_BUFFER_SIZE (255)

typedef struct _mp_reader_vfs_t {
    mp_obj_t file;
    uint8_t bufpos;
    uint8_t buflen;
    uint8_t bufsize;
    byte buf[];
} mp_reader_vfs_t;

static mp_uint_t mp_reader_vfs_readbyte(void *data) {
    mp_reader_vfs_t *reader = (mp_reader_vfs_t *)data;
    if (reader->bufpos >= reader->buflen) {
        if (reader->buflen < reader->bufsize) {
            return MP_READER_EOF;
        } else {
            int errcode;
            reader->buflen = mp_stream_rw(reader->file, reader->buf, reader->bufsize, &errcode, MP_STREAM_RW_READ | MP_STREAM_RW_ONCE);
            if (errcode != 0) {
                // TODO handle errors properly
                return MP_READER_EOF;
            }
            if (reader->buflen == 0) {
                return MP_READER_EOF;
            }
            reader->bufpos = 0;
        }
    }
    return reader->buf[reader->bufpos++];
}

static void mp_reader_vfs_close(void *data) {
    mp_reader_vfs_t *reader = (mp_reader_vfs_t *)data;
    mp_stream_close(reader->file);
    m_del_obj(mp_reader_vfs_t, reader);
}

void mp_reader_new_file(mp_reader_t *reader, qstr filename) {
    mp_obj_t args[2] = {
        MP_OBJ_NEW_QSTR(filename),
        MP_OBJ_NEW_QSTR(MP_QSTR_rb),
    };
    mp_obj_t file = mp_vfs_open(MP_ARRAY_SIZE(args), &args[0], (mp_map_t *)&mp_const_empty_map);

    const mp_stream_p_t *stream_p = mp_get_stream(file);
    int errcode = 0;
    mp_uint_t bufsize = stream_p->ioctl(file, MP_STREAM_GET_BUFFER_SIZE, 0, &errcode);
    if (bufsize == MP_STREAM_ERROR || bufsize == 0) {
        // bufsize == 0 is included here to support mpremote v1.21 and older where mount file ioctl
        // returned 0 by default.
        bufsize = MICROPY_READER_VFS_DEFAULT_BUFFER_SIZE;
    } else {
        bufsize = MIN(MICROPY_READER_VFS_MAX_BUFFER_SIZE, MAX(MICROPY_READER_VFS_MIN_BUFFER_SIZE, bufsize));
    }

    mp_reader_vfs_t *rf = m_new_obj_var(mp_reader_vfs_t, buf, byte, bufsize);
    rf->file = file;
    rf->bufsize = bufsize;
    rf->buflen = mp_stream_rw(rf->file, rf->buf, rf->bufsize, &errcode, MP_STREAM_RW_READ | MP_STREAM_RW_ONCE);
    if (errcode != 0) {
        mp_raise_OSError(errcode);
    }
    rf->bufpos = 0;
    reader->data = rf;
    reader->readbyte = mp_reader_vfs_readbyte;
    reader->close = mp_reader_vfs_close;
}

#endif // MICROPY_READER_VFS
