#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "nlr.h"
#include "misc.h"
#include "mpyconfig.h"
#include "lexer.h"
#include "lexerunix.h"
#include "parse.h"
#include "compile.h"
#include "runtime.h"
#include "repl.h"

#include <readline/readline.h>

char *str_join(const char *s1, int sep_char, const char *s2) {
    int l1 = strlen(s1);
    int l2 = strlen(s2);
    char *s = m_new(char, l1 + l2 + 2);
    memcpy(s, s1, l1);
    if (sep_char != 0) {
        s[l1] = sep_char;
        l1 += 1;
    }
    memcpy(s + l1, s2, l2);
    s[l1 + l2] = 0;
    return s;
}

void do_repl(void) {
    for (;;) {
        char *line = readline(">>> ");
        if (line == NULL) {
            // EOF
            return;
        }
        if (py_repl_is_compound_stmt(line)) {
            for (;;) {
                char *line2 = readline("... ");
                if (line2 == NULL || strlen(line2) == 0) {
                    break;
                }
                char *line3 = str_join(line, '\n', line2);
                m_free(line);
                m_free(line2);
                line = line3;
            }
        }

        py_lexer_t *lex = py_lexer_new_from_str_len("<stdin>", line, strlen(line), false);
        py_parse_node_t pn = py_parse(lex, PY_PARSE_SINGLE_INPUT);
        py_lexer_free(lex);

        if (pn != PY_PARSE_NODE_NULL) {
            //py_parse_node_show(pn, 0);
            bool comp_ok = py_compile(pn, true);
            if (comp_ok) {
                py_obj_t module_fun = rt_make_function_from_id(1);
                if (module_fun != py_const_none) {
                    nlr_buf_t nlr;
                    if (nlr_push(&nlr) == 0) {
                        rt_call_function_0(module_fun);
                        nlr_pop();
                    } else {
                        // uncaught exception
                        py_obj_print((py_obj_t)nlr.ret_val);
                        printf("\n");
                    }
                }
            }
        }
    }
}

void do_file(const char *file) {
    py_lexer_t *lex = py_lexer_new_from_file(file);
    //const char *pysrc = "def f():\n  x=x+1\n  print(42)\n";
    //py_lexer_t *lex = py_lexer_from_str_len("<>", pysrc, strlen(pysrc), false);
    if (lex == NULL) {
        return;
    }

    if (0) {
        // just tokenise
        while (!py_lexer_is_kind(lex, PY_TOKEN_END)) {
            py_token_show(py_lexer_cur(lex));
            py_lexer_to_next(lex);
        }
        py_lexer_free(lex);

    } else {
        // compile

        py_parse_node_t pn = py_parse(lex, PY_PARSE_FILE_INPUT);
        py_lexer_free(lex);

        if (pn != PY_PARSE_NODE_NULL) {
            //printf("----------------\n");
            //parse_node_show(pn, 0);
            //printf("----------------\n");
            bool comp_ok = py_compile(pn, false);
            //printf("----------------\n");

#if MICROPY_EMIT_CPYTHON
            if (!comp_ok) {
                printf("compile error\n");
            }
#else
            if (1 && comp_ok) {
                // execute it
                py_obj_t module_fun = rt_make_function_from_id(1);
                if (module_fun != py_const_none) {
                    nlr_buf_t nlr;
                    if (nlr_push(&nlr) == 0) {
                        py_obj_t ret = rt_call_function_0(module_fun);
                        printf("done! got: ");
                        py_obj_print(ret);
                        printf("\n");
                        nlr_pop();
                    } else {
                        // uncaught exception
                        printf("exception: ");
                        py_obj_print((py_obj_t)nlr.ret_val);
                        printf("\n");
                    }
                }
            }
#endif
        }
    }
}

void test_print(py_obj_t o_in) {
    printf("<test>");
}

py_obj_t test_get(py_obj_t o_in) {
    py_obj_t d1;
    py_obj_t d2;
    py_user_get_data(o_in, (machine_uint_t*)&d1, (machine_uint_t*)&d2);
    return d1;
}

py_obj_t test_set(py_obj_t o_in, py_obj_t arg) {
    py_user_set_data(o_in, (machine_uint_t)arg, (machine_uint_t)arg);
    return py_const_none;
}

const py_user_info_t test_obj_info = {
    "Test",
    test_print,
    {
        { "get", 0, test_get },
        { "set", 1, test_set },
        { NULL, 0, NULL },
    }
};

int main(int argc, char **argv) {
    qstr_init();
    rt_init();

    rt_store_name(qstr_from_str_static("test"), py_obj_new_user(&test_obj_info, (machine_uint_t)py_obj_new_int(42), 0));

    if (argc == 1) {
        do_repl();
    } else if (argc == 2) {
        do_file(argv[1]);
    } else {
        printf("usage: py [<file>]\n");
        return 1;
    }
    rt_deinit();

    //printf("total bytes = %d\n", m_get_total_bytes_allocated());
    return 0;
}

// for sqrt
#include <math.h>
machine_float_t machine_sqrt(machine_float_t x) {
    return sqrt(x);
}
