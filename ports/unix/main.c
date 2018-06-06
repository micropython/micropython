/*
 * This file is part of the MicroPython project, http://micropython.org/
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
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>

#include "py/compile.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/stackctrl.h"
#include "py/mphal.h"
#include "py/mpthread.h"
#include "extmod/misc.h"
#include "extmod/vfs.h"
#include "extmod/vfs_posix.h"
#include "genhdr/mpversion.h"
#include "input.h"

// Command line options, with their defaults
STATIC bool compile_only = false;
STATIC uint emit_opt = MP_EMIT_OPT_NONE;

#if MICROPY_ENABLE_GC
// Heap size of GC heap (if enabled)
// Make it larger on a 64 bit machine, because pointers are larger.
long heap_size = 1024*1024 * (sizeof(mp_uint_t) / 4);
#endif

STATIC void stderr_print_strn(void *env, const char *str, size_t len) {
    (void)env;
    ssize_t dummy = write(STDERR_FILENO, str, len);
    mp_uos_dupterm_tx_strn(str, len);
    (void)dummy;
}

const mp_print_t mp_stderr_print = {NULL, stderr_print_strn};

#define FORCED_EXIT (0x100)
// If exc is SystemExit, return value where FORCED_EXIT bit set,
// and lower 8 bits are SystemExit value. For all other exceptions,
// return 1.
STATIC int handle_uncaught_exception(mp_obj_base_t *exc) {
    // check for SystemExit
    if (mp_obj_is_subclass_fast(MP_OBJ_FROM_PTR(exc->type), MP_OBJ_FROM_PTR(&mp_type_SystemExit))) {
        // None is an exit value of 0; an int is its value; anything else is 1
        mp_obj_t exit_val = mp_obj_exception_get_value(MP_OBJ_FROM_PTR(exc));
        mp_int_t val = 0;
        if (exit_val != mp_const_none && !mp_obj_get_int_maybe(exit_val, &val)) {
            val = 1;
        }
        return FORCED_EXIT | (val & 255);
    }

    // Report all other exceptions
    mp_obj_print_exception(&mp_stderr_print, MP_OBJ_FROM_PTR(exc));
    return 1;
}

#define LEX_SRC_STR (1)
#define LEX_SRC_VSTR (2)
#define LEX_SRC_FILENAME (3)
#define LEX_SRC_STDIN (4)

// Returns standard error codes: 0 for success, 1 for all other errors,
// except if FORCED_EXIT bit is set then script raised SystemExit and the
// value of the exit is in the lower 8 bits of the return value
STATIC int execute_from_lexer(int source_kind, const void *source, mp_parse_input_kind_t input_kind, bool is_repl) {
    mp_hal_set_interrupt_char(CHAR_CTRL_C);

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        // create lexer based on source kind
        mp_lexer_t *lex;
        if (source_kind == LEX_SRC_STR) {
            const char *line = source;
            lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, line, strlen(line), false);
        } else if (source_kind == LEX_SRC_VSTR) {
            const vstr_t *vstr = source;
            lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, vstr->buf, vstr->len, false);
        } else if (source_kind == LEX_SRC_FILENAME) {
            lex = mp_lexer_new_from_file((const char*)source);
        } else { // LEX_SRC_STDIN
            lex = mp_lexer_new_from_fd(MP_QSTR__lt_stdin_gt_, 0, false);
        }

        qstr source_name = lex->source_name;

        #if MICROPY_PY___FILE__
        if (input_kind == MP_PARSE_FILE_INPUT) {
            mp_store_global(MP_QSTR___file__, MP_OBJ_NEW_QSTR(source_name));
        }
        #endif

        mp_parse_tree_t parse_tree = mp_parse(lex, input_kind);

        #if defined(MICROPY_UNIX_COVERAGE)
        // allow to print the parse tree in the coverage build
        if (mp_verbose_flag >= 3) {
            printf("----------------\n");
            mp_parse_node_print(parse_tree.root, 0);
            printf("----------------\n");
        }
        #endif

        mp_obj_t module_fun = mp_compile(&parse_tree, source_name, emit_opt, is_repl);

        if (!compile_only) {
            // execute it
            mp_call_function_0(module_fun);
            // check for pending exception
            if (MP_STATE_VM(mp_pending_exception) != MP_OBJ_NULL) {
                mp_obj_t obj = MP_STATE_VM(mp_pending_exception);
                MP_STATE_VM(mp_pending_exception) = MP_OBJ_NULL;
                nlr_raise(obj);
            }
        }

        mp_hal_set_interrupt_char(-1);
        nlr_pop();
        return 0;

    } else {
        // uncaught exception
        mp_hal_set_interrupt_char(-1);
        return handle_uncaught_exception(nlr.ret_val);
    }
}

#if MICROPY_USE_READLINE == 1
#include "lib/mp-readline/readline.h"
#else
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
#endif

STATIC int do_repl(void) {
    mp_hal_stdout_tx_str("MicroPython " MICROPY_GIT_TAG " on " MICROPY_BUILD_DATE "; "
        MICROPY_PY_SYS_PLATFORM " version\nUse Ctrl-D to exit, Ctrl-E for paste mode\n");

    #if MICROPY_USE_READLINE == 1

    // use MicroPython supplied readline

    vstr_t line;
    vstr_init(&line, 16);
    for (;;) {
        mp_hal_stdio_mode_raw();

    input_restart:
        vstr_reset(&line);
        int ret = readline(&line, ">>> ");
        mp_parse_input_kind_t parse_input_kind = MP_PARSE_SINGLE_INPUT;

        if (ret == CHAR_CTRL_C) {
            // cancel input
            mp_hal_stdout_tx_str("\r\n");
            goto input_restart;
        } else if (ret == CHAR_CTRL_D) {
            // EOF
            printf("\n");
            mp_hal_stdio_mode_orig();
            vstr_clear(&line);
            return 0;
        } else if (ret == CHAR_CTRL_E) {
            // paste mode
            mp_hal_stdout_tx_str("\npaste mode; Ctrl-C to cancel, Ctrl-D to finish\n=== ");
            vstr_reset(&line);
            for (;;) {
                char c = mp_hal_stdin_rx_chr();
                if (c == CHAR_CTRL_C) {
                    // cancel everything
                    mp_hal_stdout_tx_str("\n");
                    goto input_restart;
                } else if (c == CHAR_CTRL_D) {
                    // end of input
                    mp_hal_stdout_tx_str("\n");
                    break;
                } else {
                    // add char to buffer and echo
                    vstr_add_byte(&line, c);
                    if (c == '\r') {
                        mp_hal_stdout_tx_str("\n=== ");
                    } else {
                        mp_hal_stdout_tx_strn(&c, 1);
                    }
                }
            }
            parse_input_kind = MP_PARSE_FILE_INPUT;
        } else if (line.len == 0) {
            if (ret != 0) {
                printf("\n");
            }
            goto input_restart;
        } else {
            // got a line with non-zero length, see if it needs continuing
            while (mp_repl_continue_with_input(vstr_null_terminated_str(&line))) {
                vstr_add_byte(&line, '\n');
                ret = readline(&line, "... ");
                if (ret == CHAR_CTRL_C) {
                    // cancel everything
                    printf("\n");
                    goto input_restart;
                } else if (ret == CHAR_CTRL_D) {
                    // stop entering compound statement
                    break;
                }
            }
        }

        mp_hal_stdio_mode_orig();

        ret = execute_from_lexer(LEX_SRC_VSTR, &line, parse_input_kind, true);
        if (ret & FORCED_EXIT) {
            return ret;
        }
    }

    #else

    // use simple readline

    for (;;) {
        char *line = prompt(">>> ");
        if (line == NULL) {
            // EOF
            return 0;
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

        int ret = execute_from_lexer(LEX_SRC_STR, line, MP_PARSE_SINGLE_INPUT, true);
        if (ret & FORCED_EXIT) {
            return ret;
        }
        free(line);
    }

    #endif
}

STATIC int do_file(const char *file) {
    return execute_from_lexer(LEX_SRC_FILENAME, file, MP_PARSE_FILE_INPUT, false);
}

STATIC int do_str(const char *str) {
    return execute_from_lexer(LEX_SRC_STR, str, MP_PARSE_FILE_INPUT, false);
}

STATIC int usage(char **argv) {
    printf(
"usage: %s [<opts>] [-X <implopt>] [-c <command>] [<filename>]\n"
"Options:\n"
"-v : verbose (trace various operations); can be multiple\n"
"-O[N] : apply bytecode optimizations of level N\n"
"\n"
"Implementation specific options (-X):\n", argv[0]
);
    int impl_opts_cnt = 0;
    printf(
"  compile-only                 -- parse and compile only\n"
"  emit={bytecode,native,viper} -- set the default code emitter\n"
);
    impl_opts_cnt++;
#if MICROPY_ENABLE_GC
    printf(
"  heapsize=<n>[w][K|M] -- set the heap size for the GC (default %ld)\n"
, heap_size);
    impl_opts_cnt++;
#endif

    if (impl_opts_cnt == 0) {
        printf("  (none)\n");
    }

    return 1;
}

// Process options which set interpreter init options
STATIC void pre_process_options(int argc, char **argv) {
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
                    char *end;
                    heap_size = strtol(argv[a + 1] + sizeof("heapsize=") - 1, &end, 0);
                    // Don't bring unneeded libc dependencies like tolower()
                    // If there's 'w' immediately after number, adjust it for
                    // target word size. Note that it should be *before* size
                    // suffix like K or M, to avoid confusion with kilowords,
                    // etc. the size is still in bytes, just can be adjusted
                    // for word size (taking 32bit as baseline).
                    bool word_adjust = false;
                    if ((*end | 0x20) == 'w') {
                        word_adjust = true;
                        end++;
                    }
                    if ((*end | 0x20) == 'k') {
                        heap_size *= 1024;
                    } else if ((*end | 0x20) == 'm') {
                        heap_size *= 1024 * 1024;
                    } else {
                        // Compensate for ++ below
                        --end;
                    }
                    if (*++end != 0) {
                        goto invalid_arg;
                    }
                    if (word_adjust) {
                        heap_size = heap_size * BYTES_PER_WORD / 4;
                    }
                    // If requested size too small, we'll crash anyway
                    if (heap_size < 700) {
                        goto invalid_arg;
                    }
#endif
                } else {
invalid_arg:
                    printf("Invalid option\n");
                    exit(usage(argv));
                }
                a++;
            }
        }
    }
}

STATIC void set_sys_argv(char *argv[], int argc, int start_arg) {
    for (int i = start_arg; i < argc; i++) {
        mp_obj_list_append(mp_sys_argv, MP_OBJ_NEW_QSTR(qstr_from_str(argv[i])));
    }
}

#ifdef _WIN32
#define PATHLIST_SEP_CHAR ';'
#else
#define PATHLIST_SEP_CHAR ':'
#endif

MP_NOINLINE int main_(int argc, char **argv);

int main(int argc, char **argv) {
    #if MICROPY_PY_THREAD
    mp_thread_init();
    #endif
    // We should capture stack top ASAP after start, and it should be
    // captured guaranteedly before any other stack variables are allocated.
    // For this, actual main (renamed main_) should not be inlined into
    // this function. main_() itself may have other functions inlined (with
    // their own stack variables), that's why we need this main/main_ split.
    mp_stack_ctrl_init();
    return main_(argc, argv);
}

MP_NOINLINE int main_(int argc, char **argv) {
    #ifdef SIGPIPE
    // Do not raise SIGPIPE, instead return EPIPE. Otherwise, e.g. writing
    // to peer-closed socket will lead to sudden termination of MicroPython
    // process. SIGPIPE is particularly nasty, because unix shell doesn't
    // print anything for it, so the above looks like completely sudden and
    // silent termination for unknown reason. Ignoring SIGPIPE is also what
    // CPython does. Note that this may lead to problems using MicroPython
    // scripts as pipe filters, but again, that's what CPython does. So,
    // scripts which want to follow unix shell pipe semantics (where SIGPIPE
    // means "pipe was requested to terminate, it's not an error"), should
    // catch EPIPE themselves.
    signal(SIGPIPE, SIG_IGN);
    #endif

    mp_stack_set_limit(40000 * (BYTES_PER_WORD / 4));

    pre_process_options(argc, argv);

#if MICROPY_ENABLE_GC
    char *heap = malloc(heap_size);
    gc_init(heap, heap + heap_size);
#endif

    #if MICROPY_ENABLE_PYSTACK
    static mp_obj_t pystack[1024];
    mp_pystack_init(pystack, &pystack[MP_ARRAY_SIZE(pystack)]);
    #endif

    mp_init();

    #if MICROPY_VFS_POSIX
    {
        // Mount the host FS at the root of our internal VFS
        mp_obj_t args[2] = {
            mp_type_vfs_posix.make_new(&mp_type_vfs_posix, 0, 0, NULL),
            MP_OBJ_NEW_QSTR(MP_QSTR__slash_),
        };
        mp_vfs_mount(2, args, (mp_map_t*)&mp_const_empty_map);
        MP_STATE_VM(vfs_cur) = MP_STATE_VM(vfs_mount_table);
    }
    #endif

    char *home = getenv("HOME");
    char *path = getenv("MICROPYPATH");
    if (path == NULL) {
        #ifdef MICROPY_PY_SYS_PATH_DEFAULT
        path = MICROPY_PY_SYS_PATH_DEFAULT;
        #else
        path = "~/.micropython/lib:/usr/lib/micropython";
        #endif
    }
    size_t path_num = 1; // [0] is for current dir (or base dir of the script)
    if (*path == ':') {
        path_num++;
    }
    for (char *p = path; p != NULL; p = strchr(p, PATHLIST_SEP_CHAR)) {
        path_num++;
        if (p != NULL) {
            p++;
        }
    }
    mp_obj_list_init(MP_OBJ_TO_PTR(mp_sys_path), path_num);
    mp_obj_t *path_items;
    mp_obj_list_get(mp_sys_path, &path_num, &path_items);
    path_items[0] = MP_OBJ_NEW_QSTR(MP_QSTR_);
    {
    char *p = path;
    for (mp_uint_t i = 1; i < path_num; i++) {
        char *p1 = strchr(p, PATHLIST_SEP_CHAR);
        if (p1 == NULL) {
            p1 = p + strlen(p);
        }
        if (p[0] == '~' && p[1] == '/' && home != NULL) {
            // Expand standalone ~ to $HOME
            int home_l = strlen(home);
            vstr_t vstr;
            vstr_init(&vstr, home_l + (p1 - p - 1) + 1);
            vstr_add_strn(&vstr, home, home_l);
            vstr_add_strn(&vstr, p + 1, p1 - p - 1);
            path_items[i] = mp_obj_new_str_from_vstr(&mp_type_str, &vstr);
        } else {
            path_items[i] = mp_obj_new_str_via_qstr(p, p1 - p);
        }
        p = p1 + 1;
    }
    }

    mp_obj_list_init(MP_OBJ_TO_PTR(mp_sys_argv), 0);

    #if defined(MICROPY_UNIX_COVERAGE)
    {
        MP_DECLARE_CONST_FUN_OBJ_0(extra_coverage_obj);
        mp_store_global(QSTR_FROM_STR_STATIC("extra_coverage"), MP_OBJ_FROM_PTR(&extra_coverage_obj));
    }
    #endif

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
    bool inspect = false;
    for (int a = 1; a < argc; a++) {
        if (argv[a][0] == '-') {
            if (strcmp(argv[a], "-i") == 0) {
                inspect = true;
            } else if (strcmp(argv[a], "-c") == 0) {
                if (a + 1 >= argc) {
                    return usage(argv);
                }
                ret = do_str(argv[a + 1]);
                if (ret & FORCED_EXIT) {
                    break;
                }
                a += 1;
            } else if (strcmp(argv[a], "-m") == 0) {
                if (a + 1 >= argc) {
                    return usage(argv);
                }
                mp_obj_t import_args[4];
                import_args[0] = mp_obj_new_str(argv[a + 1], strlen(argv[a + 1]));
                import_args[1] = import_args[2] = mp_const_none;
                // Ask __import__ to handle imported module specially - set its __name__
                // to __main__, and also return this leaf module, not top-level package
                // containing it.
                import_args[3] = mp_const_false;
                // TODO: https://docs.python.org/3/using/cmdline.html#cmdoption-m :
                // "the first element of sys.argv will be the full path to
                // the module file (while the module file is being located,
                // the first element will be set to "-m")."
                set_sys_argv(argv, argc, a + 1);

                mp_obj_t mod;
                nlr_buf_t nlr;
                bool subpkg_tried = false;

            reimport:
                if (nlr_push(&nlr) == 0) {
                    mod = mp_builtin___import__(MP_ARRAY_SIZE(import_args), import_args);
                    nlr_pop();
                } else {
                    // uncaught exception
                    return handle_uncaught_exception(nlr.ret_val) & 0xff;
                }

                if (mp_obj_is_package(mod) && !subpkg_tried) {
                    subpkg_tried = true;
                    vstr_t vstr;
                    int len = strlen(argv[a + 1]);
                    vstr_init(&vstr, len + sizeof(".__main__"));
                    vstr_add_strn(&vstr, argv[a + 1], len);
                    vstr_add_strn(&vstr, ".__main__", sizeof(".__main__") - 1);
                    import_args[0] = mp_obj_new_str_from_vstr(&mp_type_str, &vstr);
                    goto reimport;
                }

                ret = 0;
                break;
            } else if (strcmp(argv[a], "-X") == 0) {
                a += 1;
            #if MICROPY_DEBUG_PRINTERS
            } else if (strcmp(argv[a], "-v") == 0) {
                mp_verbose_flag++;
            #endif
            } else if (strncmp(argv[a], "-O", 2) == 0) {
                if (unichar_isdigit(argv[a][2])) {
                    MP_STATE_VM(mp_optimise_value) = argv[a][2] & 0xf;
                } else {
                    MP_STATE_VM(mp_optimise_value) = 0;
                    for (char *p = argv[a] + 1; *p && *p == 'O'; p++, MP_STATE_VM(mp_optimise_value)++);
                }
            } else {
                return usage(argv);
            }
        } else {
            char *pathbuf = malloc(PATH_MAX);
            char *basedir = realpath(argv[a], pathbuf);
            if (basedir == NULL) {
                mp_printf(&mp_stderr_print, "%s: can't open file '%s': [Errno %d] %s\n", argv[0], argv[a], errno, strerror(errno));
                // CPython exits with 2 in such case
                ret = 2;
                break;
            }

            // Set base dir of the script as first entry in sys.path
            char *p = strrchr(basedir, '/');
            path_items[0] = mp_obj_new_str_via_qstr(basedir, p - basedir);
            free(pathbuf);

            set_sys_argv(argv, argc, a);
            ret = do_file(argv[a]);
            break;
        }
    }

    if (ret == NOTHING_EXECUTED || inspect) {
        if (isatty(0)) {
            prompt_read_history();
            ret = do_repl();
            prompt_write_history();
        } else {
            ret = execute_from_lexer(LEX_SRC_STDIN, NULL, MP_PARSE_FILE_INPUT, false);
        }
    }

    #if MICROPY_PY_MICROPYTHON_MEM_INFO
    if (mp_verbose_flag) {
        mp_micropython_mem_info(0, NULL);
    }
    #endif

    mp_deinit();

#if MICROPY_ENABLE_GC && !defined(NDEBUG)
    // We don't really need to free memory since we are about to exit the
    // process, but doing so helps to find memory leaks.
    free(heap);
#endif

    //printf("total bytes = %d\n", m_get_total_bytes_allocated());
    return ret & 0xff;
}

#if !MICROPY_VFS
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
#endif

void nlr_jump_fail(void *val) {
    printf("FATAL: uncaught NLR %p\n", val);
    exit(1);
}
