#include <stdint.h>
#include <stdio.h>

#include "misc.h"
#include "lexer.h"
#include "lexerstr.h"

unichar str_buf_next_char(mp_lexer_str_buf_t *sb) {
    if (sb->src_cur < sb->src_end) {
        return *sb->src_cur++;
    } else {
        return MP_LEXER_CHAR_EOF;
    }
}

void str_buf_free(mp_lexer_str_buf_t *sb) {
    if (sb->free) {
        m_del(char, (char*)sb->src_beg, 0 /* don't know allocated size of src */);
    }
}

mp_lexer_t *mp_lexer_new_from_str_len(const char *src_name, const char *str, uint len, bool free_str, mp_lexer_str_buf_t *sb) {
    sb->free = free_str;
    sb->src_beg = str;
    sb->src_cur = str;
    sb->src_end = str + len;
    return mp_lexer_new(src_name, sb, (mp_lexer_stream_next_char_t)str_buf_next_char, (mp_lexer_stream_close_t)str_buf_free);
}
