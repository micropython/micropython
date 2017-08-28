/**
 * @file lexer.c
 *
 * @section License
 * Copyright (C) 2016, Erik Moqvist
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * This file is part of the Pumbaa project.
 */

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

#include "pumbaa.h"

struct mp_lexer_file_buf_t {
    struct fs_file_t file;
    byte buf[20];
    mp_uint_t len;
    mp_uint_t pos;
};

static mp_uint_t file_buf_next_byte(struct mp_lexer_file_buf_t *fb_p)
{
    int n;

    if (fb_p->pos >= fb_p->len) {
        if (fb_p->len == 0) {
            return (MP_LEXER_EOF);
        } else {
            n = fs_read(&fb_p->file, fb_p->buf, sizeof(fb_p->buf));

            if (n <= 0) {
                fb_p->len = 0;
                return (MP_LEXER_EOF);
            }

            fb_p->len = n;
            fb_p->pos = 0;
        }
    }

    return (fb_p->buf[fb_p->pos++]);
}

static void file_buf_close(struct mp_lexer_file_buf_t *fb_p)
{
    fs_close(&fb_p->file);
    m_del_obj(struct mp_lexer_file_buf_t, fb_p);
}

mp_lexer_t *mp_lexer_new_from_file(const char *filename_p)
{
    struct mp_lexer_file_buf_t *fb_p;
    int n;

    fb_p = m_new_obj_maybe(struct mp_lexer_file_buf_t);

    if (fb_p == NULL) {
        return (NULL);
    }

    if (fs_open(&fb_p->file, filename_p, FS_READ) != 0) {
        m_del_obj(struct mp_lexer_file_buf_t, fb_p);
        return (NULL);
    }

    n = fs_read(&fb_p->file, fb_p->buf, sizeof(fb_p->buf));
    fb_p->len = n;
    fb_p->pos = 0;

    return (mp_lexer_new(0,
                         fb_p,
                         (mp_lexer_stream_next_byte_t)file_buf_next_byte,
                         (mp_lexer_stream_close_t)file_buf_close));
}
