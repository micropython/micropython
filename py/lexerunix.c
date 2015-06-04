/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#include "py/mpconfig.h"

#if MICROPY_HELPER_LEXER_UNIX

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "py/lexer.h"

typedef struct _mp_lexer_file_buf_t {
    int fd;
    bool close_fd;
    byte buf[20];
    mp_uint_t len;
    mp_uint_t pos;
} mp_lexer_file_buf_t;

STATIC mp_uint_t file_buf_next_byte(mp_lexer_file_buf_t *fb) {
    if (fb->pos >= fb->len) {
        if (fb->len == 0) {
            return MP_LEXER_EOF;
        } else {
            int n = read(fb->fd, fb->buf, sizeof(fb->buf));
            if (n <= 0) {
                fb->len = 0;
                return MP_LEXER_EOF;
            }
            fb->len = n;
            fb->pos = 0;
        }
    }
    return fb->buf[fb->pos++];
}

STATIC void file_buf_close(mp_lexer_file_buf_t *fb) {
    if (fb->close_fd) {
        close(fb->fd);
    }
    m_del_obj(mp_lexer_file_buf_t, fb);
}

mp_lexer_t *mp_lexer_new_from_fd(qstr filename, int fd, bool close_fd) {
    mp_lexer_file_buf_t *fb = m_new_obj_maybe(mp_lexer_file_buf_t);
    if (fb == NULL) {
        if (close_fd) {
            close(fd);
        }
        return NULL;
    }
    fb->fd = fd;
    fb->close_fd = close_fd;
    int n = read(fb->fd, fb->buf, sizeof(fb->buf));
    fb->len = n;
    fb->pos = 0;
    return mp_lexer_new(filename, fb, (mp_lexer_stream_next_byte_t)file_buf_next_byte, (mp_lexer_stream_close_t)file_buf_close);
}

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        return NULL;
    }
    return mp_lexer_new_from_fd(qstr_from_str(filename), fd, true);
}

#endif // MICROPY_HELPER_LEXER_UNIX
