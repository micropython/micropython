#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "mpconfig.h"
#include "nlr.h"
#include "misc.h"
#include "qstr.h"
#include "lexer.h"
#include "parse.h"
#include "obj.h"
#include "parsehelper.h"
#include "compile.h"
#include "runtime0.h"
#include "runtime.h"
#include "repl.h"

#include "tinytest.h"
#include "tinytest_macros.h"

void do_str(const char *src);
inline void do_str(const char *src) {
    mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
    if (lex == NULL) {
        tt_abort_msg("Lexer initialization error");
    }

    mp_parse_error_kind_t parse_error_kind;
    mp_parse_node_t pn = mp_parse(lex, MP_PARSE_FILE_INPUT, &parse_error_kind);

    if (pn == MP_PARSE_NODE_NULL) {
        mp_parse_show_exception(lex, parse_error_kind);
        mp_lexer_free(lex);
        tt_abort_msg("Parser error");
    }

    // parse okay
    qstr source_name = mp_lexer_source_name(lex);
    mp_lexer_free(lex);
    mp_obj_t module_fun = mp_compile(pn, source_name, MP_EMIT_OPT_NONE, true);
    mp_parse_node_free(pn);

    if (module_fun == mp_const_none) {
        tt_abort_msg("Computer error");
    }

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        mp_obj_print_exception((mp_obj_t)nlr.ret_val);
        tt_abort_msg("Uncaught exception");
    }
end:
    ;
}

#include "genhdr/tests.h"

int main() {
    const char a[] = {"sim"};
    mp_init();
    int r = tinytest_main(1, (const char **) a, groups);
    mp_deinit();
    printf( "status: %i\n", r);
    return r;
}

void gc_collect(void) {
}

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    return NULL;
}

mp_import_stat_t mp_import_stat(const char *path) {
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(uint n_args, const mp_obj_t *args) {
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_open_obj, 1, 2, mp_builtin_open);

void nlr_jump_fail(void *val) {
}
