/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2016 Damien P. George
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
#include <assert.h>

#include "py/mperrno.h"
#include "py/reader.h"

#if MICROPY_READER_FATFS

#include "lib/fatfs/ff.h"
#include "extmod/vfs_fat_file.h"

typedef struct _mp_reader_fatfs_t {
    FIL fp;
    uint16_t len;
    uint16_t pos;
    byte buf[20];
} mp_reader_fatfs_t;

STATIC mp_uint_t mp_reader_fatfs_readbyte(void *data) {
    mp_reader_fatfs_t *reader = (mp_reader_fatfs_t*)data;
    if (reader->pos >= reader->len) {
        if (reader->len < sizeof(reader->buf)) {
            return MP_READER_EOF;
        } else {
            UINT n;
            f_read(&reader->fp, reader->buf, sizeof(reader->buf), &n);
            if (n == 0) {
                return MP_READER_EOF;
            }
            reader->len = n;
            reader->pos = 0;
        }
    }
    return reader->buf[reader->pos++];
}

STATIC void mp_reader_fatfs_close(void *data) {
    mp_reader_fatfs_t *reader = (mp_reader_fatfs_t*)data;
    f_close(&reader->fp);
    m_del_obj(mp_reader_fatfs_t, reader);
}

int mp_reader_new_file(mp_reader_t *reader, const char *filename) {
    mp_reader_fatfs_t *rf = m_new_obj_maybe(mp_reader_fatfs_t);
    if (rf == NULL) {
        return MP_ENOMEM;
    }
    FRESULT res = f_open(&rf->fp, filename, FA_READ);
    if (res != FR_OK) {
        return fresult_to_errno_table[res];
    }
    UINT n;
    f_read(&rf->fp, rf->buf, sizeof(rf->buf), &n);
    rf->len = n;
    rf->pos = 0;
    reader->data = rf;
    reader->readbyte = mp_reader_fatfs_readbyte;
    reader->close = mp_reader_fatfs_close;
    return 0; // success
}

#endif
