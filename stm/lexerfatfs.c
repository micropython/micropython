#include <stdint.h>
#include <stdio.h>

#include "ff.h"

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "lexer.h"
#include "lexerfatfs.h"

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
    return mp_lexer_new(filename, fb, (mp_lexer_stream_next_char_t)file_buf_next_char, (mp_lexer_stream_close_t)file_buf_close);
}

/******************************************************************************/
// implementation of import

#include "ff.h"

mp_lexer_t *mp_import_open_file(qstr mod_name) {
    vstr_t *vstr = vstr_new();
    FRESULT res;

    // look for module in src/
    vstr_printf(vstr, "0:/src/%s.py", qstr_str(mod_name));
    res = f_stat(vstr_str(vstr), NULL);
    if (res == FR_OK) {
        // found file
        return mp_lexer_new_from_file(vstr_str(vstr)); // TODO does lexer need to copy the string? can we free it here?
    }

    // look for module in /
    vstr_reset(vstr);
    vstr_printf(vstr, "0:/%s.py", qstr_str(mod_name));
    res = f_stat(vstr_str(vstr), NULL);
    if (res == FR_OK) {
        // found file
        return mp_lexer_new_from_file(vstr_str(vstr)); // TODO does lexer need to copy the string? can we free it here?
    }

    // could not find file
    vstr_free(vstr);
    printf("import %s: could not find file in src/ or /\n", qstr_str(mod_name));

    return NULL;
}
