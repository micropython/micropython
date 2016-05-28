#include <setjmp.h>

void micropy_gc_collect(mp_state_ctx_t *mp) {
    micropy_gc_collect_start(mp);
    jmp_buf regs;
    setjmp(regs);
    // GC stack (and regs because we captured them)
    void **regs_ptr = (void**)(void*)&regs;
    micropy_gc_collect_root(mp, regs_ptr, ((mp_uint_t)mp->vm.stack_top - (mp_uint_t)&regs) / sizeof(mp_uint_t));
    micropy_gc_collect_end(mp);
}

uint micropy_import_stat(mp_state_ctx_t *mp, const char *path) {
    return MP_IMPORT_STAT_NO_EXIST;
}

void micropy_nlr_jump_fail(mp_state_ctx_t *mp, void *val) {
    micropy_printf(mp, &mp_plat_print, "FATAL: uncaught NLR %p\n", val);
    exit(1);
}

/*
mp_obj_t mp_builtin_open(mp_state_ctx_t *mp, mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    micropy_nlr_raise(mp, micropy_obj_new_exception_msg(mp, &mp_type_OSError, "open() not implemented"));
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);
*/

mp_state_ctx_t *micropy_create(void *heap, size_t heap_size) {
    mp_state_ctx_t *mp = heap;
    micropy_stack_ctrl_init(mp);
    micropy_stack_set_limit(mp, 40000 * (BYTES_PER_WORD / 4));
    heap += sizeof(mp_state_ctx_t);
    heap_size -= sizeof(mp_state_ctx_t);
    micropy_gc_init(mp, heap, heap + heap_size);
    micropy_init(mp);
    return mp;
}

void micropy_destroy(mp_state_ctx_t *mp) {
    micropy_deinit(mp);
}

STATIC int micropy_execute_from_lexer(mp_state_ctx_t *mp, mp_lexer_t *lex, mp_parse_input_kind_t input_kind, bool is_repl) {
    if (lex == NULL) {
        micropy_printf(mp, &mp_plat_print, "MemoryError: lexer could not allocate memory\n");
        return 1;
    }

    nlr_buf_t nlr;
    if (micropy_nlr_push(mp, &nlr) == 0) {
        qstr source_name = lex->source_name;

        #if MICROPY_PY___FILE__
        if (input_kind == MP_PARSE_FILE_INPUT) {
            micropy_store_global(mp, MP_QSTR___file__, MP_OBJ_NEW_QSTR(source_name));
        }
        #endif

        mp_parse_tree_t pt = micropy_parse(mp, lex, input_kind);
        mp_obj_t module_fun = micropy_compile(mp, &pt, source_name, MP_EMIT_OPT_NONE, is_repl);
        micropy_call_function_0(mp, module_fun);

        micropy_nlr_pop(mp);
        return 0;

    } else {
        // uncaught exception
        mp_obj_t exc = nlr.ret_val;
        // check for SystemExit
        if (micropy_obj_is_subclass_fast(mp, micropy_obj_get_type(mp, exc), &mp_type_SystemExit)) {
            // None is an exit value of 0; an int is its value; anything else is 1
            mp_obj_t exit_val = micropy_obj_exception_get_value(mp, exc);
            mp_int_t val = 0;
            if (exit_val != mp_const_none && !micropy_obj_get_int_maybe(mp, exit_val, &val)) {
                val = 1;
            }
            return 0x100 | (val & 255);
        }

        // Report all other exceptions
        micropy_obj_print_exception(mp, &mp_plat_print, exc);
        return 1;
    }
}

int micropy_exec_str(mp_state_ctx_t *mp, const char *str) {
    mp_lexer_t *lex = micropy_lexer_new_from_str_len(mp, MP_QSTR__lt_stdin_gt_, str, strlen(str), false);
    return micropy_execute_from_lexer(mp, lex, MP_PARSE_FILE_INPUT, false);
}
