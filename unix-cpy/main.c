#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "lexer.h"
#include "lexerunix.h"
#include "parse.h"
#include "obj.h"
#include "compile.h"
#include "runtime0.h"
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
        // compile

        mp_parse_node_t pn = mp_parse(lex, MP_PARSE_FILE_INPUT);
        mp_lexer_free(lex);

        if (pn != MP_PARSE_NODE_NULL) {
            //printf("----------------\n");
            //parse_node_show(pn, 0);
            //printf("----------------\n");
            mp_obj_t module_fun = mp_compile(pn, MP_FALSE);
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
