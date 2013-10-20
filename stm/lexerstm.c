#include <stdint.h>
#include <stdio.h>

#include "ff.h"

#include "misc.h"
#include "lexer.h"
#include "lexerstm.h"

unichar str_buf_next_char(py_lexer_str_buf_t *sb) {
    if (sb->src_cur < sb->src_end) {
        return *sb->src_cur++;
    } else {
        return PY_LEXER_CHAR_EOF;
    }
}

void str_buf_free(py_lexer_str_buf_t *sb) {
    if (sb->free) {
        m_free((char*)sb->src_beg);
    }
}

py_lexer_t *py_lexer_new_from_str_len(const char *src_name, const char *str, uint len, bool free_str, py_lexer_str_buf_t *sb) {
    sb->free = free_str;
    sb->src_beg = str;
    sb->src_cur = str;
    sb->src_end = str + len;
    return py_lexer_new(src_name, sb, (py_lexer_stream_next_char_t)str_buf_next_char, (py_lexer_stream_close_t)str_buf_free);
}

unichar file_buf_next_char(py_lexer_file_buf_t *fb) {
    if (fb->pos >= fb->len) {
        if (fb->len < sizeof(fb->buf)) {
            return PY_LEXER_CHAR_EOF;
        } else {
            UINT n;
            f_read(&fb->fp, fb->buf, sizeof(fb->buf), &n);
            fb->len = n;
            fb->pos = 0;
        }
    }
    return fb->buf[fb->pos++];
}

void file_buf_close(py_lexer_file_buf_t *fb) {
    f_close(&fb->fp);
}

py_lexer_t *py_lexer_new_from_file(const char *filename, py_lexer_file_buf_t *fb) {
    FRESULT res = f_open(&fb->fp, filename, FA_READ);
    if (res != FR_OK) {
        return NULL;
    }
    UINT n;
    f_read(&fb->fp, fb->buf, sizeof(fb->buf), &n);
    fb->len = n;
    fb->pos = 0;
    return py_lexer_new(filename, fb, (py_lexer_stream_next_char_t)file_buf_next_char, (py_lexer_stream_close_t)file_buf_close);
}
