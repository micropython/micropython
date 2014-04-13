// This code glues the code emitters to the runtime.

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime0.h"
#include "runtime.h"
#include "emitglue.h"
#include "bc.h"

#if 0 // print debugging info
#define DEBUG_PRINT (1)
#define WRITE_CODE (1)
#define DEBUG_printf DEBUG_printf
#define DEBUG_OP_printf(...) DEBUG_printf(__VA_ARGS__)
#else // don't print debugging info
#define DEBUG_printf(...) (void)0
#define DEBUG_OP_printf(...) (void)0
#endif

#ifdef WRITE_CODE
FILE *fp_write_code = NULL;
#endif

void mp_emit_glue_init(void) {
#ifdef WRITE_CODE
    fp_write_code = fopen("out-code", "wb");
#endif
}

void mp_emit_glue_deinit(void) {
#ifdef WRITE_CODE
    if (fp_write_code != NULL) {
        fclose(fp_write_code);
    }
#endif
}

mp_raw_code_t *mp_emit_glue_new_raw_code(void) {
    mp_raw_code_t *rc = m_new0(mp_raw_code_t, 1);
    rc->kind = MP_CODE_RESERVED;
    return rc;
}

void mp_emit_glue_assign_byte_code(mp_raw_code_t *rc, byte *code, uint len, int n_args, int n_locals, uint scope_flags, qstr *arg_names) {
    rc->kind = MP_CODE_BYTE;
    rc->scope_flags = scope_flags;
    rc->n_args = n_args;
    rc->u_byte.code = code;
    rc->u_byte.len = len;
    rc->arg_names = arg_names;

#ifdef DEBUG_PRINT
    DEBUG_printf("assign byte code: code=%p len=%u n_args=%d n_locals=%d\n", code, len, n_args, n_locals);
    for (int i = 0; i < 128 && i < len; i++) {
        if (i > 0 && i % 16 == 0) {
            DEBUG_printf("\n");
        }
        DEBUG_printf(" %02x", code[i]);
    }
    DEBUG_printf("\n");
#if MICROPY_DEBUG_PRINTERS
    mp_byte_code_print(code, len);
#endif
#endif
}

void mp_emit_glue_assign_native_code(mp_raw_code_t *rc, void *fun, uint len, int n_args) {
    rc->kind = MP_CODE_NATIVE;
    rc->scope_flags = 0;
    rc->n_args = n_args;
    rc->u_native.fun = fun;

#ifdef DEBUG_PRINT
    DEBUG_printf("assign native code: fun=%p len=%u n_args=%d\n", fun, len, n_args);
    byte *fun_data = (byte*)(((machine_uint_t)fun) & (~1)); // need to clear lower bit in case it's thumb code
    for (int i = 0; i < 128 && i < len; i++) {
        if (i > 0 && i % 16 == 0) {
            DEBUG_printf("\n");
        }
        DEBUG_printf(" %02x", fun_data[i]);
    }
    DEBUG_printf("\n");

#ifdef WRITE_CODE
    if (fp_write_code != NULL) {
        fwrite(fun_data, len, 1, fp_write_code);
        fflush(fp_write_code);
    }
#endif
#endif
}

void mp_emit_glue_assign_inline_asm_code(mp_raw_code_t *rc, void *fun, uint len, int n_args) {
    rc->kind = MP_CODE_INLINE_ASM;
    rc->scope_flags = 0;
    rc->n_args = n_args;
    rc->u_inline_asm.fun = fun;

#ifdef DEBUG_PRINT
    DEBUG_printf("assign inline asm code: fun=%p len=%u n_args=%d\n", fun, len, n_args);
    byte *fun_data = (byte*)(((machine_uint_t)fun) & (~1)); // need to clear lower bit in case it's thumb code
    for (int i = 0; i < 128 && i < len; i++) {
        if (i > 0 && i % 16 == 0) {
            DEBUG_printf("\n");
        }
        DEBUG_printf(" %02x", fun_data[i]);
    }
    DEBUG_printf("\n");

#ifdef WRITE_CODE
    if (fp_write_code != NULL) {
        fwrite(fun_data, len, 1, fp_write_code);
        fflush(fp_write_code);
    }
#endif
#endif
}

mp_obj_t mp_make_function_from_raw_code(mp_raw_code_t *rc, mp_obj_t def_args, mp_obj_t def_kw_args) {
    DEBUG_OP_printf("make_function_from_raw_code %p\n", rc);
    assert(rc != NULL);

    // def_args must be MP_OBJ_NULL or a tuple
    assert(def_args == MP_OBJ_NULL || MP_OBJ_IS_TYPE(def_args, &mp_type_tuple));

    // TODO implement default kw args
    assert(def_kw_args == MP_OBJ_NULL);

    // make the function, depending on the raw code kind
    mp_obj_t fun;
    switch (rc->kind) {
        case MP_CODE_BYTE:
            fun = mp_obj_new_fun_bc(rc->scope_flags, rc->arg_names, rc->n_args, def_args, rc->u_byte.code);
            break;
        case MP_CODE_NATIVE:
            fun = mp_make_function_n(rc->n_args, rc->u_native.fun);
            break;
        case MP_CODE_INLINE_ASM:
            fun = mp_obj_new_fun_asm(rc->n_args, rc->u_inline_asm.fun);
            break;
        default:
            // raw code was never set (this should not happen)
            assert(0);
            return mp_const_none;
    }

    // check for generator functions and if so wrap in generator object
    if ((rc->scope_flags & MP_SCOPE_FLAG_GENERATOR) != 0) {
        fun = mp_obj_new_gen_wrap(fun);
    }

    return fun;
}

mp_obj_t mp_make_closure_from_raw_code(mp_raw_code_t *rc, mp_obj_t closure_tuple, mp_obj_t def_args, mp_obj_t def_kw_args) {
    DEBUG_OP_printf("make_closure_from_raw_code %p\n", rc);
    // make function object
    mp_obj_t ffun = mp_make_function_from_raw_code(rc, def_args, def_kw_args);
    // wrap function in closure object
    return mp_obj_new_closure(ffun, closure_tuple);
}
