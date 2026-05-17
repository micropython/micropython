/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Damien P. George
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

#include "lexer_dedent.h"

typedef struct _mp_reader_mem_dedent_t {
    size_t free_len; // if >0 mem is freed on close by: m_free(beg, free_len)
    const byte *beg;
    const byte *cur;
    const byte *end;
    size_t dedent_prefix;
} mp_reader_mem_dedent_t;

// Work out the amount of common whitespace among all non-empty lines.
static size_t dedent(const byte *text, size_t len) {
    size_t min_prefix = -1;
    size_t cur_prefix = 0;
    bool start_of_line = true;
    for (const byte *t = text; t < text + len; ++t) {
        if (*t == '\n') {
            start_of_line = true;
            cur_prefix = 0;
        } else if (start_of_line) {
            if (unichar_isspace(*t)) {
                ++cur_prefix;
            } else {
                if (cur_prefix < min_prefix) {
                    min_prefix = cur_prefix;
                    if (min_prefix == 0) {
                        return min_prefix;
                    }
                }
                start_of_line = false;
            }
        }
    }
    return min_prefix;
}

static mp_uint_t mp_reader_mem_dedent_readbyte(void *data) {
    mp_reader_mem_dedent_t *reader = (mp_reader_mem_dedent_t *)data;
    if (reader->cur < reader->end) {
        byte c = *reader->cur++;
        if (c == '\n') {
            for (size_t i = 0; i < reader->dedent_prefix; ++i) {
                if (*reader->cur == '\n') {
                    break;
                }
                ++reader->cur;
            }
        }
        return c;
    } else {
        return MP_READER_EOF;
    }
}

static void mp_reader_mem_dedent_close(void *data) {
    mp_reader_mem_dedent_t *reader = (mp_reader_mem_dedent_t *)data;
    if (reader->free_len > 0) {
        m_del(char, (char *)reader->beg, reader->free_len);
    }
    m_del_obj(mp_reader_mem_dedent_t, reader);
}

static void mp_reader_new_mem_dedent(mp_reader_t *reader, const byte *buf, size_t len, size_t free_len) {
    mp_reader_mem_dedent_t *rm = m_new_obj(mp_reader_mem_dedent_t);
    rm->free_len = free_len;
    rm->beg = buf;
    rm->cur = buf;
    rm->end = buf + len;
    rm->dedent_prefix = dedent(buf, len);
    reader->data = rm;
    reader->readbyte = mp_reader_mem_dedent_readbyte;
    reader->close = mp_reader_mem_dedent_close;
}

mp_lexer_t *mp_lexer_new_from_str_len_dedent(qstr src_name, const char *str, size_t len, size_t free_len) {
    mp_reader_t reader;
    mp_reader_new_mem_dedent(&reader, (const byte *)str, len, free_len);
    return mp_lexer_new(src_name, reader);
}
