#include <stdio.h>

#include "py/lexer.h"

mp_import_stat_t mp_import_stat(const char *path) {
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    return NULL;
}
