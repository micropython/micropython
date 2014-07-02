#include <stdint.h>
#include <stdio.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "lexer.h"
typedef int FIL;
#include "../stmhal/lexerfatfs.h"

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
  printf("import not implemented\n");
  return NULL;
}

