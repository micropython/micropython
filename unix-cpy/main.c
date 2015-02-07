/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "py/nlr.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/pfenv.h"

void do_file(const char *file) {
    mp_lexer_t *lex = mp_lexer_new_from_file(file);
    if (lex == NULL) {
        return;
    }

    if (0) {
        // just tokenise
        while (lex->tok_kind != MP_TOKEN_END) {
            mp_lexer_show_token(lex);
            mp_lexer_to_next(lex);
        }
        mp_lexer_free(lex);

    } else {
        // parse
        mp_parse_node_t pn = mp_parse(lex, MP_PARSE_FILE_INPUT);

        if (pn != MP_PARSE_NODE_NULL) {
            //printf("----------------\n");
            //mp_parse_node_print(pn, 0);
            //printf("----------------\n");

            // compile
            mp_compile(pn, 0, MP_EMIT_OPT_NONE, false);

            //printf("----------------\n");
        }
    }
}

int main(int argc, char **argv) {
    mp_init();

    if (argc == 2) {
        do_file(argv[1]);
    } else {
        printf("usage: py [<file>]\n");
        return 1;
    }
    mp_deinit();

    return 0;
}

mp_import_stat_t mp_import_stat(const char *path) {
    return MP_IMPORT_STAT_NO_EXIST;
}

void nlr_jump_fail(void *val) {
    printf("FATAL: uncaught NLR %p\n", val);
    exit(1);
}
