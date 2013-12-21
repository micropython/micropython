#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "lexer.h"
#include "lexerunix.h"
#include "parse.h"
#include "compile.h"
#include "obj.h"
#include "runtime0.h"
#include "runtime.h"
#include "repl.h"

#include <readline/readline.h>

static char *str_join(const char *s1, int sep_char, const char *s2) {
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

static void do_repl(void) {
    for (;;) {
        char *line = readline(">>> ");
        if (line == NULL) {
            // EOF
            return;
        }
        if (mp_repl_is_compound_stmt(line)) {
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

        mp_lexer_t *lex = mp_lexer_new_from_str_len("<stdin>", line, strlen(line), false);
        mp_parse_node_t pn = mp_parse(lex, MP_PARSE_SINGLE_INPUT);
        mp_lexer_free(lex);

        if (pn != MP_PARSE_NODE_NULL) {
            //mp_parse_node_show(pn, 0);
            bool comp_ok = mp_compile(pn, true);
            if (comp_ok) {
                mp_obj_t module_fun = rt_make_function_from_id(1);
                if (module_fun != mp_const_none) {
                    nlr_buf_t nlr;
                    if (nlr_push(&nlr) == 0) {
                        rt_call_function_0(module_fun);
                        nlr_pop();
                    } else {
                        // uncaught exception
                        mp_obj_print((mp_obj_t)nlr.ret_val);
                        printf("\n");
                    }
                }
            }
        }
    }
}

void do_file(const char *file) {
    mp_lexer_t *lex = mp_lexer_new_from_file(file);
    //const char *pysrc = "def f():\n  x=x+1\n  print(42)\n";
    //mp_lexer_t *lex = mp_lexer_from_str_len("<>", pysrc, strlen(pysrc), false);
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
            bool comp_ok = mp_compile(pn, false);
            //printf("----------------\n");

#if MICROPY_EMIT_CPYTHON
            if (!comp_ok) {
                printf("compile error\n");
            }
#else
            if (1 && comp_ok) {
                // execute it
                mp_obj_t module_fun = rt_make_function_from_id(1);
                if (module_fun != mp_const_none) {
                    nlr_buf_t nlr;
                    if (nlr_push(&nlr) == 0) {
                        mp_obj_t ret = rt_call_function_0(module_fun);
                        printf("done! got: ");
                        mp_obj_print(ret);
                        printf("\n");
                        nlr_pop();
                    } else {
                        // uncaught exception
                        printf("exception: ");
                        mp_obj_print((mp_obj_t)nlr.ret_val);
                        printf("\n");
                    }
                }
            }
#endif
        }
    }
}

typedef struct _test_obj_t {
    mp_obj_base_t base;
    bool value;
} test_obj_t;

static void test_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in) {
    test_obj_t *self = self_in;
    print(env, "<test %d>", self->value);
}

static mp_obj_t test_get(mp_obj_t self_in) {
    test_obj_t *self = self_in;
    return mp_obj_new_int(self->value);
}

static mp_obj_t test_set(mp_obj_t self_in, mp_obj_t arg) {
    test_obj_t *self = self_in;
    self->value = mp_obj_get_int(arg);
    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_1(test_get_obj, test_get);
static MP_DEFINE_CONST_FUN_OBJ_2(test_set_obj, test_set);

static const mp_obj_type_t test_type = {
    { &mp_const_type },
    "Test",
    test_print, // print
    NULL, // call_n
    NULL, // unary_op
    NULL, // binary_op
    NULL, // getiter
    NULL, // iternext
    { // method list
        { "get", &test_get_obj },
        { "set", &test_set_obj },
        { NULL, NULL },
    }
};

mp_obj_t test_obj_new(int value) {
    test_obj_t *o = m_new_obj(test_obj_t);
    o->base.type = &test_type;
    o->value = value;
    return o;
}

int main(int argc, char **argv) {
    qstr_init();
    rt_init();

    rt_store_name(qstr_from_str_static("test"), test_obj_new(42));

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
