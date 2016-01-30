/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
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

#if MICROPY_HELPER_LEXER_MPFILE

#include <stdio.h>
#include <stdint.h>

#include "py/lexer.h"
#include "py/mpfile.h"

typedef struct _mp_lexer_file_buf_t {
    mp_file_t *file;
    byte buf[20];
    mp_uint_t len;
    mp_uint_t pos;
} mp_lexer_file_buf_t;

STATIC mp_uint_t mp_file_buf_next_byte(void *lexer_fb) {
    mp_lexer_file_buf_t *fb = lexer_fb;
    if (fb->pos >= fb->len) {
        if (fb->len == 0) {
            return MP_LEXER_EOF;
        }

        int n = mp_readinto(fb->file, fb->buf, sizeof(fb->buf));
        if (n <= 0) {
            fb->len = 0;
            return MP_LEXER_EOF;
        }
        fb->len = n;
        fb->pos = 0;
    }
    return fb->buf[fb->pos++];
}

STATIC void mp_file_buf_close(void *lexer_fb) {
    mp_lexer_file_buf_t *fb = lexer_fb;
    mp_close(fb->file);
    fb->file = NULL;
    m_del_obj(mp_lexer_file_buf_t, fb);
}

mp_lexer_t *mp_lexer_new_from_file_obj(qstr filename, mp_obj_t file_obj) {
    mp_lexer_file_buf_t *fb = m_new_obj_maybe(mp_lexer_file_buf_t);
    if (fb == NULL) {
        return NULL;
    }
    fb->file = mp_file_from_file_obj(file_obj);
    int n = mp_readinto(fb->file, fb->buf, sizeof(fb->buf));
    fb->len = n;
    fb->pos = 0;

    return mp_lexer_new(filename, fb, mp_file_buf_next_byte, mp_file_buf_close);
}

#endif // MICROPY_HELPER_LEXER_MPFILE
