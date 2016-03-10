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
// *_ADHOC part is for cc3200 port which doesn't use general uPy
// infrastructure and instead duplicates code. TODO: Resolve.
#if MICROPY_VFS_FAT || MICROPY_FSUSERMOUNT || MICROPY_FSUSERMOUNT_ADHOC

#include "py/lexer.h"
#include "lib/fatfs/ff.h"

typedef struct _mp_lexer_file_buf_t {
    FIL fp;
    byte buf[20];
    uint16_t len;
    uint16_t pos;
} mp_lexer_file_buf_t;

STATIC mp_uint_t file_buf_next_byte(mp_lexer_file_buf_t *fb) {
    if (fb->pos >= fb->len) {
        if (fb->len < sizeof(fb->buf)) {
            return MP_LEXER_EOF;
        } else {
            UINT n;
            f_read(&fb->fp, fb->buf, sizeof(fb->buf), &n);
            if (n == 0) {
                return MP_LEXER_EOF;
            }
            fb->len = n;
            fb->pos = 0;
        }
    }
    return fb->buf[fb->pos++];
}

STATIC void file_buf_close(mp_lexer_file_buf_t *fb) {
    f_close(&fb->fp);
    m_del_obj(mp_lexer_file_buf_t, fb);
}

mp_lexer_t *fat_vfs_lexer_new_from_file(const char *filename);

mp_lexer_t *fat_vfs_lexer_new_from_file(const char *filename) {
    mp_lexer_file_buf_t *fb = m_new_obj_maybe(mp_lexer_file_buf_t);
    if (fb == NULL) {
        return NULL;
    }
    FRESULT res = f_open(&fb->fp, filename, FA_READ);
    if (res != FR_OK) {
        m_del_obj(mp_lexer_file_buf_t, fb);
        return NULL;
    }
    UINT n;
    f_read(&fb->fp, fb->buf, sizeof(fb->buf), &n);
    fb->len = n;
    fb->pos = 0;
    return mp_lexer_new(qstr_from_str(filename), fb, (mp_lexer_stream_next_byte_t)file_buf_next_byte, (mp_lexer_stream_close_t)file_buf_close);
}

#endif // MICROPY_VFS_FAT
