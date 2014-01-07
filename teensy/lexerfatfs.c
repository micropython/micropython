#include <stdint.h>
#include <stdio.h>

#include "misc.h"
#include "lexer.h"
typedef int FIL;
#include "../stm/lexerfatfs.h"

mp_lexer_t *mp_lexer_new_from_file(const char *filename, mp_lexer_file_buf_t *fb) {
  printf("import not implemented\n");
  return NULL;
}

mp_lexer_t *mp_import_open_file(qstr mod_name) {
    printf("import not implemented\n");
    return NULL;
}
