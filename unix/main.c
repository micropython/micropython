#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

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
#include "runtime0.h"
#include "runtime.h"
#include "repl.h"
#include "gc.h"

#if MICROPY_USE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

#if MICROPY_ENABLE_GC
// Heap size of GC heap (if enabled)
long heap_size = 128*1024;
#endif

// Stack top at the start of program
void *stack_top;

void file_init();
void microsocket_init();
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

    mp_parse_error_kind_t parse_error_kind;
    mp_parse_node_t pn = mp_parse(lex, input_kind, &parse_error_kind);

    if (pn == MP_PARSE_NODE_NULL) {
        // parse error
        mp_parse_show_exception(lex, parse_error_kind);
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
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        // uncaught exception
        mp_obj_print_exception((mp_obj_t)nlr.ret_val);
    }
}

static char *strjoin(const char *s1, int sep_char, const char *s2) {
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
                char *line3 = strjoin(line, '\n', line2);
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

static const mp_map_elem_t test_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_get), (mp_obj_t)&test_get_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_set), (mp_obj_t)&test_set_obj },
};

STATIC MP_DEFINE_CONST_DICT(test_locals_dict, test_locals_dict_table);

static const mp_obj_type_t test_type = {
    { &mp_type_type },
    .name = MP_QSTR_Test,
    .print = test_print,
    .locals_dict = (mp_obj_t)&test_locals_dict,
};

mp_obj_t test_obj_new(int value) {
    test_obj_t *o = m_new_obj(test_obj_t);
    o->base.type = &test_type;
    o->value = value;
    return o;
}

int usage(char **argv) {
    printf(
"usage: %s [-X <opt>] [-c <command>] [<filename>]\n"
"\n"
"Implementation specific options:\n", argv[0]
);
    int impl_opts_cnt = 0;
#if MICROPY_ENABLE_GC
    printf(
"  heapsize=<n> -- set the heap size for the GC\n"
);
    impl_opts_cnt++;
#endif

    if (impl_opts_cnt == 0) {
        printf("  (none)\n");
    }

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

#if MICROPY_ENABLE_GC
// TODO: this doesn't belong here
static mp_obj_t pyb_gc(void) {
    gc_collect();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(pyb_gc_obj, pyb_gc);
#endif

// Process options which set interpreter init options
void pre_process_options(int argc, char **argv) {
    for (int a = 1; a < argc; a++) {
        if (argv[a][0] == '-') {
            if (strcmp(argv[a], "-X") == 0) {
                if (a + 1 >= argc) {
                    exit(usage(argv));
                }
                if (0) {
#if MICROPY_ENABLE_GC
                } else if (strncmp(argv[a + 1], "heapsize=", sizeof("heapsize=") - 1) == 0) {
                    heap_size = strtol(argv[a + 1] + sizeof("heapsize=") - 1, NULL, 0);
#endif
                } else {
                    exit(usage(argv));
                }
                a++;
            }
        }
    }
}

int main(int argc, char **argv) {
    volatile int stack_dummy;
    stack_top = (void*)&stack_dummy;

    pre_process_options(argc, argv);

#if MICROPY_ENABLE_GC
    char *heap = malloc(heap_size);
    gc_init(heap, heap + heap_size);
#endif

    qstr_init();
    mp_init();

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
    mp_sys_path = mp_obj_new_list(path_num, NULL);
    mp_obj_t *path_items;
    mp_obj_list_get(mp_sys_path, &path_num, &path_items);
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
    mp_store_attr(m_sys, MP_QSTR_path, mp_sys_path);
    mp_obj_t py_argv = mp_obj_new_list(0, NULL);
    mp_store_attr(m_sys, MP_QSTR_argv, py_argv);

    mp_store_name(qstr_from_str("test"), test_obj_new(42));
    mp_store_name(qstr_from_str("mem_info"), mp_make_function_n(0, mem_info));
    mp_store_name(qstr_from_str("qstr_info"), mp_make_function_n(0, qstr_info));
#if MICROPY_ENABLE_GC
    mp_store_name(qstr_from_str("gc"), (mp_obj_t)&pyb_gc_obj);
#endif

    file_init();
    microsocket_init();
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
    test_class_type = mp_obj_new_type(QSTR_FROM_STR_STATIC("TestClass"), mp_const_empty_tuple, mp_obj_new_dict(0));
    mp_store_name(QSTR_FROM_STR_STATIC("test_obj"), test_class_instance = mp_call_function_0(test_class_type));
    mp_store_attr(test_class_instance, QSTR_FROM_STR_STATIC("attr"), mp_obj_new_int(42));

    /*
    printf("bytes:\n");
    printf("    total %d\n", m_get_total_bytes_allocated());
    printf("    cur   %d\n", m_get_current_bytes_allocated());
    printf("    peak  %d\n", m_get_peak_bytes_allocated());
    */

    bool executed = false;
    for (int a = 1; a < argc; a++) {
        if (argv[a][0] == '-') {
            if (strcmp(argv[a], "-c") == 0) {
                if (a + 1 >= argc) {
                    return usage(argv);
                }
                do_str(argv[a + 1]);
                executed = true;
                a += 1;
            } else if (strcmp(argv[a], "-X") == 0) {
                a += 1;
            } else {
                return usage(argv);
            }
        } else {
            char *basedir = realpath(argv[a], NULL);
            if (basedir == NULL) {
                fprintf(stderr, "%s: can't open file '%s': [Errno %d] ", argv[0], argv[1], errno);
                perror("");
                // CPython exits with 2 in such case
                exit(2);
            }

            // Set base dir of the script as first entry in sys.path
            char *p = strrchr(basedir, '/');
            path_items[0] = MP_OBJ_NEW_QSTR(qstr_from_strn(basedir, p - basedir));
            free(basedir);

            for (int i = a; i < argc; i++) {
                mp_obj_list_append(py_argv, MP_OBJ_NEW_QSTR(qstr_from_str(argv[i])));
            }
            do_file(argv[a]);
            executed = true;
            break;
        }
    }

    if (!executed) {
        do_repl();
    }

    mp_deinit();

    //printf("total bytes = %d\n", m_get_total_bytes_allocated());
    return 0;
}

uint mp_import_stat(const char *path) {
    struct stat st;
    if (stat(path, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            return MP_IMPORT_STAT_DIR;
        } else if (S_ISREG(st.st_mode)) {
            return MP_IMPORT_STAT_FILE;
        }
    }
    return MP_IMPORT_STAT_NO_EXIST;
}

int DEBUG_printf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = vfprintf(stderr, fmt, ap);
    va_end(ap);
    return ret;
}
