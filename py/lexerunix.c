#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "misc.h"
#include "lexer.h"

typedef struct _str_buf_t {
    bool free;                  // free src_beg when done
    const char *src_beg;        // beginning of source
    const char *src_cur;        // current location in source
    const char *src_end;        // end (exclusive) of source
} str_buf_t;

unichar str_buf_next_char(str_buf_t *sb) {
    if (sb->src_cur < sb->src_end) {
        return *sb->src_cur++;
    } else {
        return MP_LEXER_CHAR_EOF;
    }
}

void str_buf_free(str_buf_t *sb) {
    if (sb) {
        if (sb->free) {
            m_del(char, (char*)sb->src_beg, 0 /* unknown size of src_beg */);
        }
        m_del_obj(str_buf_t, sb);
    }
}

mp_lexer_t *mp_lexer_new_from_str_len(const char *src_name, const char *str, uint len, bool free_str) {
    str_buf_t *sb = m_new(str_buf_t, 1);
    sb->free = free_str;
    sb->src_beg = str;
    sb->src_cur = str;
    sb->src_end = str + len;
    return mp_lexer_new(src_name, sb, (mp_lexer_stream_next_char_t)str_buf_next_char, (mp_lexer_stream_close_t)str_buf_free);
}

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        printf("cannot open file %s\n", filename);
        return NULL;
    }
    uint size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    char *data = m_new(char, size);
    int read_size = read(fd, data, size);
    close(fd);
    if (read_size != size) {
        printf("error reading file %s\n", filename);
        m_del(char, data, size);
        return NULL;
    }

    return mp_lexer_new_from_str_len(filename, data, size, true);
}
