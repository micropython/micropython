// This code glues the code emitters to the runtime.

#include <stdlib.h>
#include <assert.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime0.h"
#include "runtime.h"
#include "emitglue.h"

#if 0 // print debugging info
#define DEBUG_PRINT (1)
#define WRITE_CODE (1)
#define DEBUG_printf DEBUG_printf
#define DEBUG_OP_printf(...) DEBUG_printf(__VA_ARGS__)
#else // don't print debugging info
#define DEBUG_printf(...) (void)0
#define DEBUG_OP_printf(...) (void)0
#endif

typedef enum {
    MP_CODE_NONE,
    MP_CODE_BYTE,
    MP_CODE_NATIVE,
    MP_CODE_INLINE_ASM,
} mp_code_kind_t;

typedef struct _mp_code_t {
    mp_code_kind_t kind : 8;
    uint scope_flags : 8;
    uint n_args : 16;
    union {
        struct {
            byte *code;
            uint len;
        } u_byte;
        struct {
            mp_fun_t fun;
        } u_native;
        struct {
            void *fun;
        } u_inline_asm;
    };
    qstr *arg_names;
} mp_code_t;

STATIC machine_uint_t unique_codes_alloc = 0;
STATIC mp_code_t *unique_codes = NULL;
STATIC uint next_unique_code_id;

void mp_emit_glue_init(void) {
    next_unique_code_id = 0;
    unique_codes_alloc = 0;
    unique_codes = NULL;
}

void mp_emit_glue_deinit(void) {
    m_del(mp_code_t, unique_codes, unique_codes_alloc);
}

uint mp_emit_glue_get_unique_code_id(void) {
    return next_unique_code_id++;
}

STATIC void mp_emit_glue_alloc_unique_codes(void) {
    if (next_unique_code_id > unique_codes_alloc) {
        DEBUG_printf("allocate more unique codes: " UINT_FMT " -> %u\n", unique_codes_alloc, next_unique_code_id);
        // increase size of unique_codes table
        unique_codes = m_renew(mp_code_t, unique_codes, unique_codes_alloc, next_unique_code_id);
        for (uint i = unique_codes_alloc; i < next_unique_code_id; i++) {
            unique_codes[i].kind = MP_CODE_NONE;
        }
        unique_codes_alloc = next_unique_code_id;
    }
}

void mp_emit_glue_assign_byte_code(uint unique_code_id, byte *code, uint len, int n_args, int n_locals, uint scope_flags, qstr *arg_names) {
    mp_emit_glue_alloc_unique_codes();

    assert(unique_code_id < next_unique_code_id && unique_codes[unique_code_id].kind == MP_CODE_NONE);
    unique_codes[unique_code_id].kind = MP_CODE_BYTE;
    unique_codes[unique_code_id].scope_flags = scope_flags;
    unique_codes[unique_code_id].n_args = n_args;
    unique_codes[unique_code_id].u_byte.code = code;
    unique_codes[unique_code_id].u_byte.len = len;
    unique_codes[unique_code_id].arg_names = arg_names;

    //printf("byte code: %d bytes\n", len);

#ifdef DEBUG_PRINT
    DEBUG_printf("assign byte code: id=%d code=%p len=%u n_args=%d n_locals=%d\n", unique_code_id, code, len, n_args, n_locals);
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

void mp_emit_glue_assign_native_code(uint unique_code_id, void *fun, uint len, int n_args) {
    mp_emit_glue_alloc_unique_codes();

    assert(unique_code_id < next_unique_code_id && unique_codes[unique_code_id].kind == MP_CODE_NONE);
    unique_codes[unique_code_id].kind = MP_CODE_NATIVE;
    unique_codes[unique_code_id].scope_flags = 0;
    unique_codes[unique_code_id].n_args = n_args;
    unique_codes[unique_code_id].u_native.fun = fun;

    //printf("native code: %d bytes\n", len);

#ifdef DEBUG_PRINT
    DEBUG_printf("assign native code: id=%d fun=%p len=%u n_args=%d\n", unique_code_id, fun, len, n_args);
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

void mp_emit_glue_assign_inline_asm_code(uint unique_code_id, void *fun, uint len, int n_args) {
    mp_emit_glue_alloc_unique_codes();

    assert(unique_code_id < next_unique_code_id && unique_codes[unique_code_id].kind == MP_CODE_NONE);
    unique_codes[unique_code_id].kind = MP_CODE_INLINE_ASM;
    unique_codes[unique_code_id].scope_flags = 0;
    unique_codes[unique_code_id].n_args = n_args;
    unique_codes[unique_code_id].u_inline_asm.fun = fun;

#ifdef DEBUG_PRINT
    DEBUG_printf("assign inline asm code: id=%d fun=%p len=%u n_args=%d\n", unique_code_id, fun, len, n_args);
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
    }
#endif
#endif
}

mp_obj_t rt_make_function_from_id(int unique_code_id, mp_obj_t def_args) {
    DEBUG_OP_printf("make_function_from_id %d\n", unique_code_id);
    if (unique_code_id >= next_unique_code_id) {
        // illegal code id
        return mp_const_none;
    }

    // make the function, depending on the code kind
    mp_code_t *c = &unique_codes[unique_code_id];
    mp_obj_t fun;
    switch (c->kind) {
        case MP_CODE_BYTE:
            fun = mp_obj_new_fun_bc(c->scope_flags, c->arg_names, c->n_args, def_args, c->u_byte.code);
            break;
        case MP_CODE_NATIVE:
            fun = rt_make_function_n(c->n_args, c->u_native.fun);
            break;
        case MP_CODE_INLINE_ASM:
            fun = mp_obj_new_fun_asm(c->n_args, c->u_inline_asm.fun);
            break;
        default:
            assert(0);
            fun = mp_const_none;
    }

    // check for generator functions and if so wrap in generator object
    if ((c->scope_flags & MP_SCOPE_FLAG_GENERATOR) != 0) {
        fun = mp_obj_new_gen_wrap(fun);
    }

    return fun;
}

mp_obj_t rt_make_closure_from_id(int unique_code_id, mp_obj_t closure_tuple, mp_obj_t def_args) {
    DEBUG_OP_printf("make_closure_from_id %d\n", unique_code_id);
    // make function object
    mp_obj_t ffun = rt_make_function_from_id(unique_code_id, def_args);
    // wrap function in closure object
    return mp_obj_new_closure(ffun, closure_tuple);
}
