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

#include "py/lexer.h"

#if MICROPY_ENABLE_COMPILER

typedef struct _mp_lexer_str_buf_t {
    mp_uint_t free_len;         // if > 0, src_beg will be freed when done by: m_free(src_beg, free_len)
    const char *src_beg;        // beginning of source
    const char *src_cur;        // current location in source
    const char *src_end;        // end (exclusive) of source
} mp_lexer_str_buf_t;

STATIC mp_uint_t str_buf_next_byte(mp_lexer_str_buf_t *sb) {
    if (sb->src_cur < sb->src_end) {
        return *sb->src_cur++;
    } else {
        return MP_LEXER_EOF;
    }
}

STATIC void str_buf_free(mp_lexer_str_buf_t *sb) {
    if (sb->free_len > 0) {
        m_del(char, (char*)sb->src_beg, sb->free_len);
    }
    m_del_obj(mp_lexer_str_buf_t, sb);
}

mp_lexer_t *mp_lexer_new_from_str_len(qstr src_name, const char *str, mp_uint_t len, mp_uint_t free_len) {
    mp_lexer_str_buf_t *sb = m_new_obj_maybe(mp_lexer_str_buf_t);
    if (sb == NULL) {
        return NULL;
    }
    sb->free_len = free_len;
    sb->src_beg = str;
    sb->src_cur = str;
    sb->src_end = str + len;
    return mp_lexer_new(src_name, sb, (mp_lexer_stream_next_byte_t)str_buf_next_byte, (mp_lexer_stream_close_t)str_buf_free);
}

#endif // MICROPY_ENABLE_COMPILER
