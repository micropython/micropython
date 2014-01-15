#include <stdint.h>
#include <stdio.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "lexer.h"
typedef int FIL;
#include "../stm/lexerfatfs.h"

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
  printf("import not implemented\n");
  return NULL;
}

mp_lexer_t *mp_import_open_file(qstr mod_name) {
    printf("import not implemented\n");
    return NULL;
}

mp_import_stat_t mp_import_stat(const char *path) {
    // TODO implement me!
    return MP_IMPORT_STAT_NO_EXIST;
}
