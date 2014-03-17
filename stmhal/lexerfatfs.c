#include <stdint.h>
#include <stdio.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "lexer.h"
#include "lexerfatfs.h"
#include "ff.h"

typedef struct _mp_lexer_file_buf_t {
    FIL fp;
    char buf[20];
    uint16_t len;
    uint16_t pos;
} mp_lexer_file_buf_t;

static unichar file_buf_next_char(mp_lexer_file_buf_t *fb) {
    if (fb->pos >= fb->len) {
        if (fb->len < sizeof(fb->buf)) {
            return MP_LEXER_CHAR_EOF;
        } else {
            UINT n;
            f_read(&fb->fp, fb->buf, sizeof(fb->buf), &n);
            if (n == 0) {
                return MP_LEXER_CHAR_EOF;
            }
            fb->len = n;
            fb->pos = 0;
        }
    }
    return fb->buf[fb->pos++];
}

static void file_buf_close(mp_lexer_file_buf_t *fb) {
    f_close(&fb->fp);
    m_del_obj(mp_lexer_file_buf_t, fb);
}

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    mp_lexer_file_buf_t *fb = m_new_obj(mp_lexer_file_buf_t);
    FRESULT res = f_open(&fb->fp, filename, FA_READ);
    if (res != FR_OK) {
        m_del_obj(mp_lexer_file_buf_t, fb);
        return NULL;
    }
    UINT n;
    f_read(&fb->fp, fb->buf, sizeof(fb->buf), &n);
    fb->len = n;
    fb->pos = 0;
    return mp_lexer_new(qstr_from_str(filename), fb, (mp_lexer_stream_next_char_t)file_buf_next_char, (mp_lexer_stream_close_t)file_buf_close);
}
