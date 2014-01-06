mp_lexer_t *mp_lexer_new_from_str_len(const char *src_name, const char *str, uint len, bool free_str);
mp_lexer_t *mp_lexer_new_from_file(const char *filename);

void mp_import_set_directory(const char *dir);
