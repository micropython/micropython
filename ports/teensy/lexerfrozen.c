#include <stdio.h>

#include "py/builtin.h"
#include "py/lexer.h"
#include "py/runtime.h"
#include "py/mperrno.h"

mp_import_stat_t mp_import_stat(const char *path) {
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    mp_raise_OSError(MP_ENOENT);
}
