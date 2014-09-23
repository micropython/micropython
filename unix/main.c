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

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#include "mpconfig.h"
#include "nlr.h"
#include "misc.h"
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
#include "genhdr/py-version.h"
#include "input.h"
#include "stackctrl.h"

// Command line options, with their defaults
STATIC bool compile_only = false;
STATIC uint emit_opt = MP_EMIT_OPT_NONE;
mp_uint_t mp_verbose_flag = 0;

#if MICROPY_ENABLE_GC
// Heap size of GC heap (if enabled)
// Make it larger on a 64 bit machine, because pointers are larger.
long heap_size = 128*1024 * (sizeof(mp_uint_t) / 4);
#endif

void microsocket_init();
void time_init();
void ffi_init();

// returns standard error codes: 0 for success, 1 for all other errors
STATIC int execute_from_lexer(mp_lexer_t *lex, mp_parse_input_kind_t input_kind, bool is_repl) {
    if (lex == NULL) {
        return 1;
    }

    if (0) {
        // just tokenise
        while (!mp_lexer_is_kind(lex, MP_TOKEN_END)) {
            mp_token_show(mp_lexer_cur(lex));
            mp_lexer_to_next(lex);
        }
        mp_lexer_free(lex);
        return 0;
    }

    mp_parse_error_kind_t parse_error_kind;
    mp_parse_node_t pn = mp_parse(lex, input_kind, &parse_error_kind);

    if (pn == MP_PARSE_NODE_NULL) {
        // parse error
        mp_parse_show_exception(lex, parse_error_kind);
        mp_lexer_free(lex);
        return 1;
    }

    qstr source_name = mp_lexer_source_name(lex);
    #if MICROPY_PY___FILE__
    if (input_kind == MP_PARSE_FILE_INPUT) {
        mp_store_global(MP_QSTR___file__, MP_OBJ_NEW_QSTR(source_name));
    }
    #endif
    mp_lexer_free(lex);

    /*
    printf("----------------\n");
    mp_parse_node_print(pn, 0);
    printf("----------------\n");
    */

    mp_obj_t module_fun = mp_compile(pn, source_name, emit_opt, is_repl);

    if (module_fun == mp_const_none) {
        // compile error
        return 1;
    }

    if (compile_only) {
        return 0;
    }

    // execute it
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_call_function_0(module_fun);
        nlr_pop();
        return 0;
    } else {
        // uncaught exception
        // check for SystemExit
        mp_obj_t exc = (mp_obj_t)nlr.ret_val;
        if (mp_obj_is_subclass_fast(mp_obj_get_type(exc), &mp_type_SystemExit)) {
            // None is an exit value of 0; an int is its value; anything else is 1
            mp_obj_t exit_val = mp_obj_exception_get_value(exc);
            mp_int_t val = 0;
            if (exit_val != mp_const_none && !mp_obj_get_int_maybe(exit_val, &val)) {
                val = 1;
            }
            exit(val);
        }
        mp_obj_print_exception((mp_obj_t)nlr.ret_val);
        return 1;
    }
}

STATIC char *strjoin(const char *s1, int sep_char, const char *s2) {
    int l1 = strlen(s1);
    int l2 = strlen(s2);
    char *s = malloc(l1 + l2 + 2);
    memcpy(s, s1, l1);
    if (sep_char != 0) {
        s[l1] = sep_char;
        l1 += 1;
    }
    memcpy(s + l1, s2, l2);
    s[l1 + l2] = 0;
    return s;
}

STATIC void do_repl(void) {
    printf("Micro Python " MICROPY_GIT_TAG " on " MICROPY_BUILD_DATE "; " MICROPY_PY_SYS_PLATFORM " version\n");

    for (;;) {
        char *line = prompt(">>> ");
        if (line == NULL) {
            // EOF
            return;
        }
        while (mp_repl_continue_with_input(line)) {
            char *line2 = prompt("... ");
            if (line2 == NULL) {
                break;
            }
            char *line3 = strjoin(line, '\n', line2);
            free(line);
            free(line2);
            line = line3;
        }

        mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, line, strlen(line), false);
        execute_from_lexer(lex, MP_PARSE_SINGLE_INPUT, true);
        free(line);
    }
}

STATIC int do_file(const char *file) {
    mp_lexer_t *lex = mp_lexer_new_from_file(file);
    return execute_from_lexer(lex, MP_PARSE_FILE_INPUT, false);
}

STATIC int do_str(const char *str) {
    mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, str, strlen(str), false);
    return execute_from_lexer(lex, MP_PARSE_SINGLE_INPUT, false);
}

int usage(char **argv) {
    printf(
"usage: %s [<opts>] [-X <implopt>] [-c <command>] [<filename>]\n"
"Options:\n"
"-v : verbose (trace various operations); can be multiple\n"
"-O[N] : apply bytecode optimizations of level N\n"
"\n"
"Implementation specific options:\n", argv[0]
);
    int impl_opts_cnt = 0;
    printf(
"  compile-only                 -- parse and compile only\n"
"  emit={bytecode,native,viper} -- set the default code emitter\n"
);
    impl_opts_cnt++;
#if MICROPY_ENABLE_GC
    printf(
"  heapsize=<n> -- set the heap size for the GC (default %ld)\n"
, heap_size);
    impl_opts_cnt++;
#endif

    if (impl_opts_cnt == 0) {
        printf("  (none)\n");
    }

    return 1;
}

#if MICROPY_MEM_STATS
STATIC mp_obj_t mem_info(void) {
    printf("mem: total=" UINT_FMT ", current=" UINT_FMT ", peak=" UINT_FMT "\n",
        m_get_total_bytes_allocated(), m_get_current_bytes_allocated(), m_get_peak_bytes_allocated());
    printf("stack: " UINT_FMT "\n", mp_stack_usage());
#if MICROPY_ENABLE_GC
    gc_dump_info();
#endif
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mem_info_obj, mem_info);
#endif

STATIC mp_obj_t qstr_info(void) {
    uint n_pool, n_qstr, n_str_data_bytes, n_total_bytes;
    qstr_pool_info(&n_pool, &n_qstr, &n_str_data_bytes, &n_total_bytes);
    printf("qstr pool: n_pool=%u, n_qstr=%u, n_str_data_bytes=%u, n_total_bytes=%u\n", n_pool, n_qstr, n_str_data_bytes, n_total_bytes);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(qstr_info_obj, qstr_info);

// Process options which set interpreter init options
void pre_process_options(int argc, char **argv) {
    for (int a = 1; a < argc; a++) {
        if (argv[a][0] == '-') {
            if (strcmp(argv[a], "-X") == 0) {
                if (a + 1 >= argc) {
                    exit(usage(argv));
                }
                if (0) {
                } else if (strcmp(argv[a + 1], "compile-only") == 0) {
                    compile_only = true;
                } else if (strcmp(argv[a + 1], "emit=bytecode") == 0) {
                    emit_opt = MP_EMIT_OPT_BYTECODE;
                } else if (strcmp(argv[a + 1], "emit=native") == 0) {
                    emit_opt = MP_EMIT_OPT_NATIVE_PYTHON;
                } else if (strcmp(argv[a + 1], "emit=viper") == 0) {
                    emit_opt = MP_EMIT_OPT_VIPER;
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

#ifdef _WIN32
#define PATHLIST_SEP_CHAR ';'
#else
#define PATHLIST_SEP_CHAR ':'
#endif

int main(int argc, char **argv) {
    mp_stack_set_limit(32768);

    pre_process_options(argc, argv);

#if MICROPY_ENABLE_GC
    char *heap = malloc(heap_size);
    gc_init(heap, heap + heap_size);
#endif

    mp_init();

    char *home = getenv("HOME");
    char *path = getenv("MICROPYPATH");
    if (path == NULL) {
        path = "~/.micropython/lib:/usr/lib/micropython";
    }
    mp_uint_t path_num = 1; // [0] is for current dir (or base dir of the script)
    for (char *p = path; p != NULL; p = strchr(p, PATHLIST_SEP_CHAR)) {
        path_num++;
        if (p != NULL) {
            p++;
        }
    }
    mp_obj_list_init(mp_sys_path, path_num);
    mp_obj_t *path_items;
    mp_obj_list_get(mp_sys_path, &path_num, &path_items);
    path_items[0] = MP_OBJ_NEW_QSTR(MP_QSTR_);
    char *p = path;
    for (int i = 1; i < path_num; i++) {
        char *p1 = strchr(p, PATHLIST_SEP_CHAR);
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

    mp_obj_list_init(mp_sys_argv, 0);

    #if MICROPY_MEM_STATS
    mp_store_name(qstr_from_str("mem_info"), (mp_obj_t*)&mem_info_obj);
    #endif
    mp_store_name(qstr_from_str("qstr_info"), (mp_obj_t*)&qstr_info_obj);

    // Here is some example code to create a class and instance of that class.
    // First is the Python, then the C code.
    //
    // class TestClass:
    //     pass
    // test_obj = TestClass()
    // test_obj.attr = 42
    //
    // mp_obj_t test_class_type, test_class_instance;
    // test_class_type = mp_obj_new_type(QSTR_FROM_STR_STATIC("TestClass"), mp_const_empty_tuple, mp_obj_new_dict(0));
    // mp_store_name(QSTR_FROM_STR_STATIC("test_obj"), test_class_instance = mp_call_function_0(test_class_type));
    // mp_store_attr(test_class_instance, QSTR_FROM_STR_STATIC("attr"), mp_obj_new_int(42));

    /*
    printf("bytes:\n");
    printf("    total %d\n", m_get_total_bytes_allocated());
    printf("    cur   %d\n", m_get_current_bytes_allocated());
    printf("    peak  %d\n", m_get_peak_bytes_allocated());
    */

    const int NOTHING_EXECUTED = -2;
    int ret = NOTHING_EXECUTED;
    for (int a = 1; a < argc; a++) {
        if (argv[a][0] == '-') {
            if (strcmp(argv[a], "-c") == 0) {
                if (a + 1 >= argc) {
                    return usage(argv);
                }
                ret = do_str(argv[a + 1]);
                a += 1;
            } else if (strcmp(argv[a], "-X") == 0) {
                a += 1;
            } else if (strcmp(argv[a], "-v") == 0) {
                mp_verbose_flag++;
            } else if (strncmp(argv[a], "-O", 2) == 0) {
                if (isdigit(argv[a][2])) {
                    mp_optimise_value = argv[a][2] & 0xf;
                } else {
                    mp_optimise_value = 0;
                    for (char *p = argv[a] + 1; *p && *p == 'O'; p++, mp_optimise_value++);
                }
            } else {
                return usage(argv);
            }
        } else {
            char *pathbuf = malloc(PATH_MAX);
            char *basedir = realpath(argv[a], pathbuf);
            if (basedir == NULL) {
                fprintf(stderr, "%s: can't open file '%s': [Errno %d] ", argv[0], argv[a], errno);
                perror("");
                // CPython exits with 2 in such case
                ret = 2;
                break;
            }

            // Set base dir of the script as first entry in sys.path
            char *p = strrchr(basedir, '/');
            path_items[0] = MP_OBJ_NEW_QSTR(qstr_from_strn(basedir, p - basedir));
            free(pathbuf);

            for (int i = a; i < argc; i++) {
                mp_obj_list_append(mp_sys_argv, MP_OBJ_NEW_QSTR(qstr_from_str(argv[i])));
            }
            ret = do_file(argv[a]);
            break;
        }
    }

    if (ret == NOTHING_EXECUTED) {
        do_repl();
        ret = 0;
    }

    mp_deinit();

    //printf("total bytes = %d\n", m_get_total_bytes_allocated());
    return ret;
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

void nlr_jump_fail(void *val) {
    printf("FATAL: uncaught NLR %p\n", val);
    exit(1);
}
