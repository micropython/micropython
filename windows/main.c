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
#include "parsehelper.h"
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
        const char * s = strrchr(file, '\\');
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
    mp_init();

    mp_obj_t m_sys = mp_obj_new_module(MP_QSTR_sys);
    mp_obj_t py_argv = mp_obj_new_list(0, NULL);
    mp_store_attr(m_sys, MP_QSTR_argv, py_argv);

    mp_store_name(qstr_from_str("mem_info"), mp_make_function_n(0, mem_info));
    mp_store_name(qstr_from_str("qstr_info"), mp_make_function_n(0, qstr_info));

    file_init();

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
                for (int i = a; i < argc; i++) {
                    mp_obj_list_append(py_argv, MP_OBJ_NEW_QSTR(qstr_from_str(argv[i])));
                }
                do_file(argv[a]);
                break;
            }
        }
    }

    mp_deinit();

    //printf("total bytes = %d\n", m_get_total_bytes_allocated());
    return 0;
}

// for sqrt
#include <math.h>
machine_float_t machine_sqrt(machine_float_t x) {
    return sqrt(x);
}
