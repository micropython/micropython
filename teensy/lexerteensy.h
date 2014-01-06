typedef struct _py_lexer_str_buf_t {
    bool free;                  // free src_beg when done
    const char *src_beg;        // beginning of source
    const char *src_cur;        // current location in source
    const char *src_end;        // end (exclusive) of source
} mp_lexer_str_buf_t;

mp_lexer_t *mp_lexer_new_from_str_len(const char *src_name, const char *str, uint len, bool free_str, mp_lexer_str_buf_t *sb);
