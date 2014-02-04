#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
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
void file_init();
void rawsocket_init();
void time_init();
void ffi_init();

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

    qstr parse_exc_id;
    const char *parse_exc_msg;
    mp_parse_node_t pn = mp_parse(lex, input_kind, &parse_exc_id, &parse_exc_msg);

    if (pn == MP_PARSE_NODE_NULL) {
        // parse error
        mp_lexer_show_error_pythonic_prefix(lex);
        printf("%s: %s\n", qstr_str(parse_exc_id), parse_exc_msg);
        mp_lexer_free(lex);
        return;
    }

    qstr source_name = mp_lexer_source_name(lex);
    mp_lexer_free(lex);

    /*
    printf("----------------\n");
    mp_parse_node_print(pn, 0);
    printf("----------------\n");
    */

    mp_obj_t module_fun = mp_compile(pn, source_name, is_repl);

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
        mp_obj_print_exception((mp_obj_t)nlr.ret_val);
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
    char *line = malloc(l);
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

        mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, line, strlen(line), false);
        execute_from_lexer(lex, MP_PARSE_SINGLE_INPUT, true);
        free(line);
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
    mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, str, strlen(str), false);
    execute_from_lexer(lex, MP_PARSE_SINGLE_INPUT, false);
}

typedef struct _test_obj_t {
    mp_obj_base_t base;
    int value;
} test_obj_t;

static void test_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
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

mp_obj_t mem_info(void) {
    printf("mem: total=%d, current=%d, peak=%d\n", m_get_total_bytes_allocated(), m_get_current_bytes_allocated(), m_get_peak_bytes_allocated());
    return mp_const_none;
}

mp_obj_t qstr_info(void) {
    uint n_pool, n_qstr, n_str_data_bytes, n_total_bytes;
    qstr_pool_info(&n_pool, &n_qstr, &n_str_data_bytes, &n_total_bytes);
    printf("qstr pool: n_pool=%u, n_qstr=%u, n_str_data_bytes=%u, n_total_bytes=%u\n", n_pool, n_qstr, n_str_data_bytes, n_total_bytes);
    return mp_const_none;
}

int main(int argc, char **argv) {
    qstr_init();
    rt_init();

    char *home = getenv("HOME");
    char *path = getenv("MICROPYPATH");
    if (path == NULL) {
        path = "~/.micropython/lib:/usr/lib/micropython";
    }
    uint path_num = 1; // [0] is for current dir (or base dir of the script)
    for (char *p = path; p != NULL; p = strchr(p, ':')) {
        path_num++;
        if (p != NULL) {
            p++;
        }
    }
    sys_path = mp_obj_new_list(path_num, NULL);
    mp_obj_t *path_items;
    mp_obj_list_get(sys_path, &path_num, &path_items);
    path_items[0] = MP_OBJ_NEW_QSTR(MP_QSTR_);
    char *p = path;
    for (int i = 1; i < path_num; i++) {
        char *p1 = strchr(p, ':');
        if (p1 == NULL) {
            p1 = p + strlen(p);
        }
        if (p[0] == '~' && p[1] == '/' && home != NULL) {
            // Expand standalone ~ to $HOME
            CHECKBUF(buf, PATH_MAX);
            CHECKBUF_APPEND(buf, home, strlen(home));
            CHECKBUF_APPEND(buf, p + 1, p1 - p - 1);
            path_items[i] = MP_OBJ_NEW_QSTR(qstr_from_strn(buf, CHECKBUF_LEN(buf)));
        } else {
            path_items[i] = MP_OBJ_NEW_QSTR(qstr_from_strn(p, p1 - p));
        }
        p = p1 + 1;
    }

    mp_obj_t m_sys = mp_obj_new_module(MP_QSTR_sys);
    rt_store_attr(m_sys, MP_QSTR_path, sys_path);
    mp_obj_t py_argv = mp_obj_new_list(0, NULL);
    rt_store_attr(m_sys, MP_QSTR_argv, py_argv);

    rt_store_name(qstr_from_str("test"), test_obj_new(42));
    rt_store_name(qstr_from_str("mem_info"), rt_make_function_n(0, mem_info));
    rt_store_name(qstr_from_str("qstr_info"), rt_make_function_n(0, qstr_info));

    file_init();
    rawsocket_init();
#if MICROPY_MOD_TIME
    time_init();
#endif
#if MICROPY_MOD_FFI
    ffi_init();
#endif

    // Here is some example code to create a class and instance of that class.
    // First is the Python, then the C code.
    //
    // class TestClass:
    //     pass
    // test_obj = TestClass()
    // test_obj.attr = 42
    mp_obj_t test_class_type, test_class_instance;
    test_class_type = mp_obj_new_type("TestClass", mp_const_empty_tuple, mp_obj_new_dict(0));
    rt_store_name(QSTR_FROM_STR_STATIC("test_obj"), test_class_instance = rt_call_function_0(test_class_type));
    rt_store_attr(test_class_instance, QSTR_FROM_STR_STATIC("attr"), mp_obj_new_int(42));

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
                // Set base dir of the script as first entry in sys.path
                char *basedir = realpath(argv[a], NULL);
                if (basedir != NULL) {
                    char *p = strrchr(basedir, '/');
                    path_items[0] = MP_OBJ_NEW_QSTR(qstr_from_strn(basedir, p - basedir));
                    free(basedir);
                }
                for (int i = a; i < argc; i++) {
                    rt_list_append(py_argv, MP_OBJ_NEW_QSTR(qstr_from_str(argv[i])));
                }
                do_file(argv[a]);
                break;
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
