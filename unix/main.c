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

#include <readline/readline.h>

bool str_startswith_word(const char *str, const char *head) {
    int i;
    for (i = 0; str[i] && head[i]; i++) {
        if (str[i] != head[i]) {
            return false;
        }
    }
    return head[i] == '\0' && (str[i] == '\0' || !g_unichar_isalpha(str[i]));
}

bool is_compound_stmt(const char *line) {
    // TODO also "compound" if unmatched open bracket
    return
           str_startswith_word(line, "if")
        || str_startswith_word(line, "while")
        || str_startswith_word(line, "for")
        || str_startswith_word(line, "true")
        || str_startswith_word(line, "with")
        || str_startswith_word(line, "def")
        || str_startswith_word(line, "class")
        || str_startswith_word(line, "@");
}

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
    return s;
}

void do_repl() {
    for (;;) {
        char *line = readline(">>> ");
        if (line == NULL) {
            // EOF
            return;
        }
        if (is_compound_stmt(line)) {
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

int main(int argc, char **argv) {
    qstr_init();
    rt_init();

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
