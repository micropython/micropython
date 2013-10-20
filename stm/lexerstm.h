typedef struct _py_lexer_str_buf_t {
    bool free;                  // free src_beg when done
    const char *src_beg;        // beginning of source
    const char *src_cur;        // current location in source
    const char *src_end;        // end (exclusive) of source
} py_lexer_str_buf_t;

typedef struct _py_lexer_file_buf_t {
    FIL fp;
    char buf[20];
    uint16_t len;
    uint16_t pos;
} py_lexer_file_buf_t;

py_lexer_t *py_lexer_new_from_str_len(const char *src_name, const char *str, uint len, bool free_str, py_lexer_str_buf_t *sb);
py_lexer_t *py_lexer_new_from_file(const char *filename, py_lexer_file_buf_t *fb);
