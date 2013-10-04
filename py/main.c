#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "misc.h"
#include "lexer.h"
#include "machine.h"
#include "parse.h"
#include "compile.h"
#include "runtime.h"

int main(int argc, char **argv) {
    qstr_init();
    rt_init();

    if (argc != 2) {
        printf("usage: py <file>\n");
        return 1;
    }
    py_lexer_t *lex = py_lexer_from_file(argv[1]);
    //const char *pysrc = "def f():\n  x=x+1\n  print(42)\n";
    //py_lexer_t *lex = py_lexer_from_str_len("<>", pysrc, strlen(pysrc), false);
    if (lex == NULL) {
        return 1;
    }

    if (0) {
        while (!py_lexer_is_kind(lex, PY_TOKEN_END)) {
            py_token_show(py_lexer_cur(lex));
            py_lexer_to_next(lex);
        }
    } else {
        py_parse_node_t pn = py_parse(lex, 0);
        //printf("----------------\n");
        //parse_node_show(pn, 0);
        //printf("----------------\n");
        py_compile(pn);
        //printf("----------------\n");
    }

    py_lexer_free(lex);

    if (1) {
        // execute it
        py_obj_t module_fun = rt_make_function_from_id(1);
        if (module_fun != py_const_none) {
            py_obj_t ret = rt_call_function_0(module_fun);
            printf("done! got: ");
            py_obj_print(ret);
            printf("\n");
        }
    }

    rt_deinit();

    //printf("total bytes = %d\n", m_get_total_bytes_allocated());
    return 0;
}
