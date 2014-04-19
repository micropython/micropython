#include "misc.h"
#include "mpconfig.h"

#if MICROPY_ENABLE_LEXER_UNIX

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "qstr.h"
#include "lexer.h"
#include "lexerunix.h"

typedef struct _mp_lexer_file_buf_t {
    int fd;
    char buf[20];
    uint len;
    uint pos;
} mp_lexer_file_buf_t;

STATIC unichar file_buf_next_char(mp_lexer_file_buf_t *fb) {
    if (fb->pos >= fb->len) {
        if (fb->len < sizeof(fb->buf)) {
            return MP_LEXER_CHAR_EOF;
        } else {
            int n = read(fb->fd, fb->buf, sizeof(fb->buf));
            if (n <= 0) {
                return MP_LEXER_CHAR_EOF;
            }
            fb->len = n;
            fb->pos = 0;
        }
    }
    return fb->buf[fb->pos++];
}

STATIC void file_buf_close(mp_lexer_file_buf_t *fb) {
    close(fb->fd);
    m_del_obj(mp_lexer_file_buf_t, fb);
}

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    mp_lexer_file_buf_t *fb = m_new_obj(mp_lexer_file_buf_t);
    fb->fd = open(filename, O_RDONLY);
    if (fb->fd < 0) {
        m_del_obj(mp_lexer_file_buf_t, fb);
        return NULL;
    }
    int n = read(fb->fd, fb->buf, sizeof(fb->buf));
    fb->len = n;
    fb->pos = 0;
    return mp_lexer_new(qstr_from_str(filename), fb, (mp_lexer_stream_next_char_t)file_buf_next_char, (mp_lexer_stream_close_t)file_buf_close);
}

#endif // MICROPY_ENABLE_LEXER_UNIX
