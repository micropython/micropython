#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
#include "repl.h"

#if MICROPY_USE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

extern const mp_obj_fun_native_t mp_builtin_open_obj;

static void execute_from_lexer(mp_lexer_t *lex, mp_parse_input_kind_t input_kind, bool is_repl) {
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
        return;
    }

    mp_parse_node_t pn = mp_parse(lex, input_kind);
    mp_lexer_free(lex);

    if (pn == MP_PARSE_NODE_NULL) {
        // parse error
        return;
    }

    //printf("----------------\n");
    //parse_node_show(pn, 0);
    //printf("----------------\n");

    mp_obj_t module_fun = mp_compile(pn, is_repl);

    if (module_fun == mp_const_none) {
        // compile error
        return;
    }

    // execute it
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

static char *prompt(char *p) {
#if MICROPY_USE_READLINE
    char *line = readline(p);
    if (line) {
        add_history(line);
    }
#else
    static char buf[256];
    fputs(p, stdout);
    char *s = fgets(buf, sizeof(buf), stdin);
    if (!s) {
        return NULL;
    }
    int l = strlen(buf);
    if (buf[l - 1] == '\n') {
        buf[l - 1] = 0;
    } else {
        l++;
    }
    char *line = m_new(char, l);
    memcpy(line, buf, l);
#endif
    return line;
}

static void do_repl(void) {
    for (;;) {
        char *line = prompt(">>> ");
        if (line == NULL) {
            // EOF
            return;
        }
        if (mp_repl_is_compound_stmt(line)) {
            for (;;) {
                char *line2 = prompt("... ");
                if (line2 == NULL || strlen(line2) == 0) {
                    break;
                }
                char *line3 = str_join(line, '\n', line2);
                free(line);
                free(line2);
                line = line3;
            }
        }

        mp_lexer_t *lex = mp_lexer_new_from_str_len("<stdin>", line, strlen(line), false);
        execute_from_lexer(lex, MP_PARSE_SINGLE_INPUT, true);
    }
}

static void do_file(const char *file) {
    // hack: set dir for import based on where this file is
    {
        const char * s = strrchr(file, '/');
        if (s != NULL) {
            int len = s - file;
            char *dir = m_new(char, len + 1);
            memcpy(dir, file, len);
            dir[len] = '\0';
            mp_import_set_directory(dir);
        }
    }

    mp_lexer_t *lex = mp_lexer_new_from_file(file);
    execute_from_lexer(lex, MP_PARSE_FILE_INPUT, false);
}

static void do_str(const char *str) {
    mp_lexer_t *lex = mp_lexer_new_from_str_len("<stdin>", str, strlen(str), false);
    execute_from_lexer(lex, MP_PARSE_SINGLE_INPUT, false);
}

typedef struct _test_obj_t {
    mp_obj_base_t base;
    int value;
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

static const mp_method_t test_methods[] = {
    { "get", &test_get_obj },
    { "set", &test_set_obj },
    { NULL, NULL },
};

static const mp_obj_type_t test_type = {
    { &mp_const_type },
    "Test",
    .print = test_print,
    .methods = test_methods,
};

mp_obj_t test_obj_new(int value) {
    test_obj_t *o = m_new_obj(test_obj_t);
    o->base.type = &test_type;
    o->value = value;
    return o;
}

int usage(void) {
    printf("usage: py [-c <command>] [<filename>]\n");
    return 1;
}

int main(int argc, char **argv) {
    qstr_init();
    rt_init();

    rt_store_name(qstr_from_str_static("test"), test_obj_new(42));
    rt_store_name(qstr_from_str_static("open"), (mp_obj_t)&mp_builtin_open_obj);

    // Here is some example code to create a class and instance of that class.
    // First is the Python, then the C code.
    //
    // class TestClass:
    //     pass
    // test_obj = TestClass()
    // test_obj.attr = 42
    mp_obj_t test_class_type, test_class_instance;
    test_class_type = mp_obj_new_type(qstr_from_str_static("TestClass"), mp_obj_new_dict(0));
    rt_store_name(qstr_from_str_static("test_obj"), test_class_instance = rt_call_function_0(test_class_type));
    rt_store_attr(test_class_instance, qstr_from_str_static("attr"), mp_obj_new_int(42));

    /*
    printf("bytes:\n");
    printf("    total %d\n", m_get_total_bytes_allocated());
    printf("    cur   %d\n", m_get_current_bytes_allocated());
    printf("    peak  %d\n", m_get_peak_bytes_allocated());
    */

    if (argc == 1) {
        do_repl();
    } else {
        for (int a = 1; a < argc; a++) {
            if (argv[a][0] == '-') {
                if (strcmp(argv[a], "-c") == 0) {
                    if (a + 1 >= argc) {
                        return usage();
                    }
                    do_str(argv[a + 1]);
                    a += 1;
                } else {
                    return usage();
                }
            } else {
                do_file(argv[a]);
            }
        }
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
