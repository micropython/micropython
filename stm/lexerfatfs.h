typedef struct _py_lexer_file_buf_t {
    FIL fp;
    char buf[20];
    uint16_t len;
    uint16_t pos;
} mp_lexer_file_buf_t;

mp_lexer_t *mp_lexer_new_from_file(const char *filename, mp_lexer_file_buf_t *fb);
