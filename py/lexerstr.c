
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "lexer.h"

typedef struct _mp_lexer_str_buf_t {
    uint free_len;              // if > 0, src_beg will be freed when done by: m_free(src_beg, free_len)
    const char *src_beg;        // beginning of source
    const char *src_cur;        // current location in source
    const char *src_end;        // end (exclusive) of source
} mp_lexer_str_buf_t;

STATIC unichar str_buf_next_char(mp_lexer_str_buf_t *sb) {
    if (sb->src_cur < sb->src_end) {
        return *sb->src_cur++;
    } else {
        return MP_LEXER_CHAR_EOF;
    }
}

STATIC void str_buf_free(mp_lexer_str_buf_t *sb) {
    if (sb->free_len > 0) {
        m_free((char*)sb->src_beg, sb->free_len);
    }
    m_del_obj(mp_lexer_str_buf_t, sb);
}

mp_lexer_t *mp_lexer_new_from_str_len(qstr src_name, const char *str, uint len, uint free_len) {
    mp_lexer_str_buf_t *sb = m_new_obj(mp_lexer_str_buf_t);
    sb->free_len = free_len;
    sb->src_beg = str;
    sb->src_cur = str;
    sb->src_end = str + len;
    return mp_lexer_new(src_name, sb, (mp_lexer_stream_next_char_t)str_buf_next_char, (mp_lexer_stream_close_t)str_buf_free);
}
