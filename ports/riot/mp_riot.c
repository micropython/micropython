#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "py/nlr.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"

#include "thread.h"

void mp_do_str(const char *src, size_t len) {
    mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, len, 0);
    if (lex == NULL) {
        printf("MemoryError: lexer could not allocate memory\n");
        return;
    }

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        qstr source_name = lex->source_name;
        mp_parse_tree_t parse_tree = mp_parse(lex, MP_PARSE_FILE_INPUT);
        mp_obj_t module_fun = mp_compile(&parse_tree, source_name, MP_EMIT_OPT_NONE, false);
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        // uncaught exception
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
}

void mp_riot_init(char* heap, size_t heap_size) {
    #if MICROPY_ENABLE_GC
    gc_init(heap, heap + heap_size);
    #endif

    mp_init();
}

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    puts("mp_lexer_new_from_file() stub");
    return NULL;
}

mp_import_stat_t mp_import_stat(const char *path) {
    puts("mp_import_stat() stub");
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(uint n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    puts("mp_builtin_open() stub");
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

void nlr_jump_fail(void *val) {
    puts("nlr_jump_fail() stub");
    while (1) {};
}
