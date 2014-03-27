#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "lexer.h"
#include "lexerunix.h"
#include "parse.h"
#include "obj.h"
#include "parsehelper.h"
#include "compile.h"
#include "runtime.h"

void do_file(const char *file) {
    mp_lexer_t *lex = mp_lexer_new_from_file(file);
    if (lex == NULL) {
        return;
    }

    if (0) {
        // just tokenise
        while (!mp_lexer_is_kind(lex, MP_TOKEN_END)) {
            mp_token_show(mp_lexer_cur(lex));
            mp_lexer_to_next(lex);
        }
        mp_lexer_free(lex);

    } else {
        // parse
        mp_parse_error_kind_t parse_error_kind;
        mp_parse_node_t pn = mp_parse(lex, MP_PARSE_FILE_INPUT, &parse_error_kind);

        if (pn == MP_PARSE_NODE_NULL) {
            // parse error
            mp_parse_show_exception(lex, parse_error_kind);
            mp_lexer_free(lex);
            return;
        }

        mp_lexer_free(lex);

        if (pn != MP_PARSE_NODE_NULL) {
            //printf("----------------\n");
            //mp_parse_node_print(pn, 0);
            //printf("----------------\n");

            // compile
            mp_obj_t module_fun = mp_compile(pn, 0, false);

            //printf("----------------\n");

            if (module_fun == mp_const_none) {
                printf("compile error\n");
            }
        }
    }
}

int main(int argc, char **argv) {
    qstr_init();
    rt_init();

    if (argc == 2) {
        do_file(argv[1]);
    } else {
        printf("usage: py [<file>]\n");
        return 1;
    }
    rt_deinit();

    return 0;
}

// for sqrt
#include <math.h>
machine_float_t machine_sqrt(machine_float_t x) {
    return sqrt(x);
}

mp_import_stat_t mp_import_stat(const char *path) {
    return MP_IMPORT_STAT_NO_EXIST;
}
