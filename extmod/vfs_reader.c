// SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
// SPDX-FileCopyrightText: Copyright (c) 2013-2017 Damien P. George
//
// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <string.h>

#include "py/runtime.h"
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
    mp_reader_vfs_t *reader = (mp_reader_vfs_t *)data;
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
    mp_reader_vfs_t *reader = (mp_reader_vfs_t *)data;
    mp_stream_close(reader->file);
    m_del_obj(mp_reader_vfs_t, reader);
}

void mp_reader_new_file(mp_reader_t *reader, const char *filename) {
    mp_reader_vfs_t *rf = m_new_obj(mp_reader_vfs_t);
    mp_obj_t args[2] = {
        mp_obj_new_str(filename, strlen(filename)),
        MP_OBJ_NEW_QSTR(MP_QSTR_rb),
    };
    rf->file = mp_vfs_open(MP_ARRAY_SIZE(args), &args[0], (mp_map_t *)&mp_const_empty_map);
    int errcode;
    rf->len = mp_stream_rw(rf->file, rf->buf, sizeof(rf->buf), &errcode, MP_STREAM_RW_READ | MP_STREAM_RW_ONCE);
    if (errcode != 0) {
        mp_raise_OSError(errcode);
    }
    rf->pos = 0;
    reader->data = rf;
    reader->readbyte = mp_reader_vfs_readbyte;
    reader->close = mp_reader_vfs_close;
}

#endif // MICROPY_READER_VFS
