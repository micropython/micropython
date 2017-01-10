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

typedef struct _mp_reader_mem_t {
    size_t free_len; // if >0 mem is freed on close by: m_free(beg, free_len)
    const byte *beg;
    const byte *cur;
    const byte *end;
} mp_reader_mem_t;

STATIC mp_uint_t mp_reader_mem_readbyte(void *data) {
    mp_reader_mem_t *reader = (mp_reader_mem_t*)data;
    if (reader->cur < reader->end) {
        return *reader->cur++;
    } else {
        return MP_READER_EOF;
    }
}

STATIC void mp_reader_mem_close(void *data) {
    mp_reader_mem_t *reader = (mp_reader_mem_t*)data;
    if (reader->free_len > 0) {
        m_del(char, (char*)reader->beg, reader->free_len);
    }
    m_del_obj(mp_reader_mem_t, reader);
}

bool mp_reader_new_mem(mp_reader_t *reader, const byte *buf, size_t len, size_t free_len) {
    mp_reader_mem_t *rm = m_new_obj_maybe(mp_reader_mem_t);
    if (rm == NULL) {
        return false;
    }
    rm->free_len = free_len;
    rm->beg = buf;
    rm->cur = buf;
    rm->end = buf + len;
    reader->data = rm;
    reader->readbyte = mp_reader_mem_readbyte;
    reader->close = mp_reader_mem_close;
    return true;
}

#if MICROPY_READER_POSIX

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

typedef struct _mp_reader_posix_t {
    bool close_fd;
    int fd;
    size_t len;
    size_t pos;
    byte buf[20];
} mp_reader_posix_t;

STATIC mp_uint_t mp_reader_posix_readbyte(void *data) {
    mp_reader_posix_t *reader = (mp_reader_posix_t*)data;
    if (reader->pos >= reader->len) {
        if (reader->len == 0) {
            return MP_READER_EOF;
        } else {
            int n = read(reader->fd, reader->buf, sizeof(reader->buf));
            if (n <= 0) {
                reader->len = 0;
                return MP_READER_EOF;
            }
            reader->len = n;
            reader->pos = 0;
        }
    }
    return reader->buf[reader->pos++];
}

STATIC void mp_reader_posix_close(void *data) {
    mp_reader_posix_t *reader = (mp_reader_posix_t*)data;
    if (reader->close_fd) {
        close(reader->fd);
    }
    m_del_obj(mp_reader_posix_t, reader);
}

int mp_reader_new_file_from_fd(mp_reader_t *reader, int fd, bool close_fd) {
    mp_reader_posix_t *rp = m_new_obj_maybe(mp_reader_posix_t);
    if (rp == NULL) {
        if (close_fd) {
            close(fd);
        }
        return MP_ENOMEM;
    }
    rp->close_fd = close_fd;
    rp->fd = fd;
    int n = read(rp->fd, rp->buf, sizeof(rp->buf));
    if (n == -1) {
        if (close_fd) {
            close(fd);
        }
        return errno;
    }
    rp->len = n;
    rp->pos = 0;
    reader->data = rp;
    reader->readbyte = mp_reader_posix_readbyte;
    reader->close = mp_reader_posix_close;
    return 0; // success
}

int mp_reader_new_file(mp_reader_t *reader, const char *filename) {
    int fd = open(filename, O_RDONLY, 0644);
    if (fd < 0) {
        return errno;
    }
    return mp_reader_new_file_from_fd(reader, fd, true);
}

#endif
