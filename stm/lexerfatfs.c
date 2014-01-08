#include <stdint.h>
#include <stdio.h>

#include "ff.h"

#include "misc.h"
#include "lexer.h"
#include "lexerfatfs.h"

unichar file_buf_next_char(mp_lexer_file_buf_t *fb) {
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

void file_buf_close(mp_lexer_file_buf_t *fb) {
    f_close(&fb->fp);
}

mp_lexer_t *mp_lexer_new_from_file(const char *filename, mp_lexer_file_buf_t *fb) {
    FRESULT res = f_open(&fb->fp, filename, FA_READ);
    if (res != FR_OK) {
        return NULL;
    }
    UINT n;
    f_read(&fb->fp, fb->buf, sizeof(fb->buf), &n);
    fb->len = n;
    fb->pos = 0;
    return mp_lexer_new(filename, fb, (mp_lexer_stream_next_char_t)file_buf_next_char, (mp_lexer_stream_close_t)file_buf_close);
}

mp_lexer_t *mp_import_open_file(qstr mod_name) {
    printf("import not implemented\n");
    return NULL;
}
