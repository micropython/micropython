/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2016 Damien P. George
 * Copyright (c) 2016 Paul Sokolovsky
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

typedef struct _mp_lexer_str32_buf_t {
    const uint32_t *src_cur;
    uint32_t val;
    uint8_t byte_off;
} mp_lexer_str32_buf_t;

static mp_uint_t str32_buf_next_byte(void *sb_in) {
    mp_lexer_str32_buf_t *sb = (mp_lexer_str32_buf_t *)sb_in;
    byte c = sb->val & 0xff;
    if (c == 0) {
        return MP_READER_EOF;
    }

    if (++sb->byte_off > 3) {
        sb->byte_off = 0;
        sb->val = *sb->src_cur++;
    } else {
        sb->val >>= 8;
    }

    return c;
}

static void str32_buf_free(void *sb_in) {
    mp_lexer_str32_buf_t *sb = (mp_lexer_str32_buf_t *)sb_in;
    m_del_obj(mp_lexer_str32_buf_t, sb);
}

mp_lexer_t *mp_lexer_new_from_str32(qstr src_name, const char *str, mp_uint_t len, mp_uint_t free_len) {
    mp_lexer_str32_buf_t *sb = m_new_obj(mp_lexer_str32_buf_t);
    sb->byte_off = (uint32_t)str & 3;
    sb->src_cur = (uint32_t *)(str - sb->byte_off);
    sb->val = *sb->src_cur++ >> sb->byte_off * 8;
    mp_reader_t reader = {sb, str32_buf_next_byte, str32_buf_free};
    return mp_lexer_new(src_name, reader);
}

#endif // MICROPY_ENABLE_COMPILER
