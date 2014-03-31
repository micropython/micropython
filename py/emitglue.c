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

typedef enum {
    MP_CODE_UNUSED,
    MP_CODE_RESERVED,
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
STATIC machine_uint_t unique_codes_total = 0; // always >= unique_codes_alloc
STATIC mp_code_t *unique_codes = NULL;

#ifdef WRITE_CODE
FILE *fp_write_code = NULL;
#endif

void mp_emit_glue_init(void) {
    unique_codes_alloc = 0;
    unique_codes_total = 0;
    unique_codes = NULL;

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

    m_del(mp_code_t, unique_codes, unique_codes_alloc);
}

uint mp_emit_glue_get_unique_code_id(void) {
    // look for an existing unused slot
    for (uint i = 0; i < unique_codes_alloc; i++) {
        if (unique_codes[i].kind == MP_CODE_UNUSED) {
            unique_codes[i].kind = MP_CODE_RESERVED;
            return i;
        }
    }
    // no existing slot
    // return next available id, memory will be allocated later
    return unique_codes_total++;
}

STATIC void mp_emit_glue_alloc_unique_codes(void) {
    if (unique_codes_total > unique_codes_alloc) {
        DEBUG_printf("allocate more unique codes: " UINT_FMT " -> %u\n", unique_codes_alloc, unique_codes_total);
        // increase size of unique_codes table (all new entries are already reserved)
        unique_codes = m_renew(mp_code_t, unique_codes, unique_codes_alloc, unique_codes_total);
        for (uint i = unique_codes_alloc; i < unique_codes_total; i++) {
            unique_codes[i].kind = MP_CODE_RESERVED;
        }
        unique_codes_alloc = unique_codes_total;
    }
}

void mp_emit_glue_assign_byte_code(uint unique_code_id, byte *code, uint len, int n_args, int n_locals, uint scope_flags, qstr *arg_names) {
    mp_emit_glue_alloc_unique_codes();

    assert(unique_code_id < unique_codes_alloc && unique_codes[unique_code_id].kind == MP_CODE_RESERVED);
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

    assert(unique_code_id < unique_codes_alloc && unique_codes[unique_code_id].kind == MP_CODE_RESERVED);
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

    assert(unique_code_id < unique_codes_alloc && unique_codes[unique_code_id].kind == MP_CODE_RESERVED);
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

mp_obj_t mp_make_function_from_id(uint unique_code_id, bool free_unique_code, mp_obj_t def_args, mp_obj_t def_kw_args) {
    DEBUG_OP_printf("make_function_from_id %d\n", unique_code_id);
    if (unique_code_id >= unique_codes_total) {
        // illegal code id
        return mp_const_none;
    }

    // TODO implement default kw args
    assert(def_kw_args == MP_OBJ_NULL);

    // make the function, depending on the code kind
    mp_code_t *c = &unique_codes[unique_code_id];
    mp_obj_t fun;
    switch (c->kind) {
        case MP_CODE_BYTE:
            fun = mp_obj_new_fun_bc(c->scope_flags, c->arg_names, c->n_args, def_args, c->u_byte.code);
            break;
        case MP_CODE_NATIVE:
            fun = mp_make_function_n(c->n_args, c->u_native.fun);
            break;
        case MP_CODE_INLINE_ASM:
            fun = mp_obj_new_fun_asm(c->n_args, c->u_inline_asm.fun);
            break;
        default:
            // code id was never assigned (this should not happen)
            assert(0);
            return mp_const_none;
    }

    // check for generator functions and if so wrap in generator object
    if ((c->scope_flags & MP_SCOPE_FLAG_GENERATOR) != 0) {
        fun = mp_obj_new_gen_wrap(fun);
    }

    // in some cases we can free the unique_code slot
    // any dynamically allocade memory is now owned by the fun object
    if (free_unique_code) {
        memset(c, 0, sizeof *c); // make sure all pointers are zeroed
        c->kind = MP_CODE_UNUSED;
    }

    return fun;
}

mp_obj_t mp_make_closure_from_id(uint unique_code_id, mp_obj_t closure_tuple, mp_obj_t def_args, mp_obj_t def_kw_args) {
    DEBUG_OP_printf("make_closure_from_id %d\n", unique_code_id);
    // make function object
    mp_obj_t ffun = mp_make_function_from_id(unique_code_id, false, def_args, def_kw_args);
    // wrap function in closure object
    return mp_obj_new_closure(ffun, closure_tuple);
}
