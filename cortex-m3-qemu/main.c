#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>

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

    mp_obj_t module_fun = mp_compile(pn, source_name, MP_EMIT_OPT_ASM_THUMB, is_repl);

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

mp_import_stat_t mp_import_stat(const char *path) {
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

static void do_str(const char *str) {
    mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, str, strlen(str), false);
    execute_from_lexer(lex, MP_PARSE_SINGLE_INPUT, false);
}

void nlr_jump_fail(void *val) {
    printf("FATAL: uncaught exception %p\n", val);
    //__fatal_error("");
}

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
  return NULL;
}

int main() {
    qstr_init();
    mp_init();
    
    //mp_obj_t m_sys = mp_obj_new_module(MP_QSTR_sys);
    //mp_obj_t py_argv = mp_obj_new_list(0, NULL);
    //mp_store_attr(m_sys, MP_QSTR_argv, py_argv);
    
    mp_store_name(qstr_from_str("mem_info"), mp_make_function_n(0, mem_info));
    mp_store_name(qstr_from_str("qstr_info"), mp_make_function_n(0, qstr_info));
    
    do_str("print(123)");
}
