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

// Essentially normal Python has 1 type: Python objects
// Viper has more than 1 type, and is just a more complicated (a superset of) Python.
// If you declare everything in Viper as a Python object (ie omit type decls) then
// it should in principle be exactly the same as Python native.
// Having types means having more opcodes, like binary_op_nat_nat, binary_op_nat_obj etc.
// In practice we won't have a VM but rather do this in asm which is actually very minimal.

// Because it breaks strict Python equivalence it should be a completely separate
// decorator.  It breaks equivalence because overflow on integers wraps around.
// It shouldn't break equivalence if you don't use the new types, but since the
// type decls might be used in normal Python for other reasons, it's probably safest,
// cleanest and clearest to make it a separate decorator.

// Actually, it does break equivalence because integers default to native integers,
// not Python objects.

// for x in l[0:8]: can be compiled into a native loop if l has pointer type

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "py/emit.h"
#include "py/bc.h"

#if MICROPY_DEBUG_VERBOSE // print debugging info
#define DEBUG_PRINT (1)
#define DEBUG_printf DEBUG_printf
#else // don't print debugging info
#define DEBUG_printf(...) (void)0
#endif

// wrapper around everything in this file
#if N_X64 || N_X86 || N_THUMB || N_ARM || N_XTENSA

// number of arguments to viper functions are limited to this value
#define REG_ARG_NUM (4)

// define additional generic helper macros
#define ASM_MOV_LOCAL_IMM_VIA(as, local_num, imm, reg_temp) \
    do { \
        ASM_MOV_REG_IMM((as), (reg_temp), (imm)); \
        ASM_MOV_LOCAL_REG((as), (local_num), (reg_temp)); \
    } while (false)

#define EMIT_NATIVE_VIPER_TYPE_ERROR(emit, ...) do { \
        *emit->error_slot = mp_obj_new_exception_msg_varg(&mp_type_ViperTypeError, __VA_ARGS__); \
    } while (0)

typedef enum {
    STACK_VALUE,
    STACK_REG,
    STACK_IMM,
} stack_info_kind_t;

// these enums must be distinct and the bottom 4 bits
// must correspond to the correct MP_NATIVE_TYPE_xxx value
typedef enum {
    VTYPE_PYOBJ = 0x00 | MP_NATIVE_TYPE_OBJ,
    VTYPE_BOOL = 0x00 | MP_NATIVE_TYPE_BOOL,
    VTYPE_INT = 0x00 | MP_NATIVE_TYPE_INT,
    VTYPE_UINT = 0x00 | MP_NATIVE_TYPE_UINT,
    VTYPE_PTR = 0x00 | MP_NATIVE_TYPE_PTR,
    VTYPE_PTR8 = 0x00 | MP_NATIVE_TYPE_PTR8,
    VTYPE_PTR16 = 0x00 | MP_NATIVE_TYPE_PTR16,
    VTYPE_PTR32 = 0x00 | MP_NATIVE_TYPE_PTR32,

    VTYPE_PTR_NONE = 0x50 | MP_NATIVE_TYPE_PTR,

    VTYPE_UNBOUND = 0x60 | MP_NATIVE_TYPE_OBJ,
    VTYPE_BUILTIN_CAST = 0x70 | MP_NATIVE_TYPE_OBJ,
} vtype_kind_t;

STATIC qstr vtype_to_qstr(vtype_kind_t vtype) {
    switch (vtype) {
        case VTYPE_PYOBJ: return MP_QSTR_object;
        case VTYPE_BOOL: return MP_QSTR_bool;
        case VTYPE_INT: return MP_QSTR_int;
        case VTYPE_UINT: return MP_QSTR_uint;
        case VTYPE_PTR: return MP_QSTR_ptr;
        case VTYPE_PTR8: return MP_QSTR_ptr8;
        case VTYPE_PTR16: return MP_QSTR_ptr16;
        case VTYPE_PTR32: return MP_QSTR_ptr32;
        case VTYPE_PTR_NONE: default: return MP_QSTR_None;
    }
}

typedef struct _stack_info_t {
    vtype_kind_t vtype;
    stack_info_kind_t kind;
    union {
        int u_reg;
        mp_int_t u_imm;
    } data;
} stack_info_t;

struct _emit_t {
    mp_obj_t *error_slot;
    int pass;

    bool do_viper_types;

    vtype_kind_t return_vtype;

    mp_uint_t local_vtype_alloc;
    vtype_kind_t *local_vtype;

    mp_uint_t stack_info_alloc;
    stack_info_t *stack_info;
    vtype_kind_t saved_stack_vtype;

    int prelude_offset;
    int const_table_offset;
    int n_state;
    int stack_start;
    int stack_size;

    bool last_emit_was_return_value;

    scope_t *scope;

    ASM_T *as;
};

STATIC const uint8_t reg_arg_table[REG_ARG_NUM] = {REG_ARG_1, REG_ARG_2, REG_ARG_3, REG_ARG_4};
STATIC const uint8_t reg_local_table[REG_LOCAL_NUM] = {REG_LOCAL_1, REG_LOCAL_2, REG_LOCAL_3};

emit_t *EXPORT_FUN(new)(mp_obj_t *error_slot, mp_uint_t max_num_labels) {
    emit_t *emit = m_new0(emit_t, 1);
    emit->error_slot = error_slot;
    emit->stack_info_alloc = 8;
    emit->stack_info = m_new(stack_info_t, emit->stack_info_alloc);
    emit->as = m_new0(ASM_T, 1);
    mp_asm_base_init(&emit->as->base, max_num_labels);
    return emit;
}

void EXPORT_FUN(free)(emit_t *emit) {
    mp_asm_base_deinit(&emit->as->base, false);
    m_del_obj(ASM_T, emit->as);
    m_del(vtype_kind_t, emit->local_vtype, emit->local_vtype_alloc);
    m_del(stack_info_t, emit->stack_info, emit->stack_info_alloc);
    m_del_obj(emit_t, emit);
}

STATIC void emit_native_set_native_type(emit_t *emit, mp_uint_t op, mp_uint_t arg1, qstr arg2) {
    switch (op) {
        case MP_EMIT_NATIVE_TYPE_ENABLE:
            emit->do_viper_types = arg1;
            break;

        default: {
            vtype_kind_t type;
            switch (arg2) {
                case MP_QSTR_object: type = VTYPE_PYOBJ; break;
                case MP_QSTR_bool: type = VTYPE_BOOL; break;
                case MP_QSTR_int: type = VTYPE_INT; break;
                case MP_QSTR_uint: type = VTYPE_UINT; break;
                case MP_QSTR_ptr: type = VTYPE_PTR; break;
                case MP_QSTR_ptr8: type = VTYPE_PTR8; break;
                case MP_QSTR_ptr16: type = VTYPE_PTR16; break;
                case MP_QSTR_ptr32: type = VTYPE_PTR32; break;
                default: EMIT_NATIVE_VIPER_TYPE_ERROR(emit, "unknown type '%q'", arg2); return;
            }
            if (op == MP_EMIT_NATIVE_TYPE_RETURN) {
                emit->return_vtype = type;
            } else {
                assert(arg1 < emit->local_vtype_alloc);
                emit->local_vtype[arg1] = type;
            }
            break;
        }
    }
}

STATIC void emit_pre_pop_reg(emit_t *emit, vtype_kind_t *vtype, int reg_dest);
STATIC void emit_post_push_reg(emit_t *emit, vtype_kind_t vtype, int reg);
STATIC void emit_native_load_fast(emit_t *emit, qstr qst, mp_uint_t local_num);
STATIC void emit_native_store_fast(emit_t *emit, qstr qst, mp_uint_t local_num);

#define STATE_START (sizeof(mp_code_state_t) / sizeof(mp_uint_t))

STATIC void emit_native_start_pass(emit_t *emit, pass_kind_t pass, scope_t *scope) {
    DEBUG_printf("start_pass(pass=%u, scope=%p)\n", pass, scope);

    emit->pass = pass;
    emit->stack_start = 0;
    emit->stack_size = 0;
    emit->last_emit_was_return_value = false;
    emit->scope = scope;

    // allocate memory for keeping track of the types of locals
    if (emit->local_vtype_alloc < scope->num_locals) {
        emit->local_vtype = m_renew(vtype_kind_t, emit->local_vtype, emit->local_vtype_alloc, scope->num_locals);
        emit->local_vtype_alloc = scope->num_locals;
    }

    // set default type for return
    emit->return_vtype = VTYPE_PYOBJ;

    // set default type for arguments
    mp_uint_t num_args = emit->scope->num_pos_args + emit->scope->num_kwonly_args;
    if (scope->scope_flags & MP_SCOPE_FLAG_VARARGS) {
        num_args += 1;
    }
    if (scope->scope_flags & MP_SCOPE_FLAG_VARKEYWORDS) {
        num_args += 1;
    }
    for (mp_uint_t i = 0; i < num_args; i++) {
        emit->local_vtype[i] = VTYPE_PYOBJ;
    }

    // local variables begin unbound, and have unknown type
    for (mp_uint_t i = num_args; i < emit->local_vtype_alloc; i++) {
        emit->local_vtype[i] = VTYPE_UNBOUND;
    }

    // values on stack begin unbound
    for (mp_uint_t i = 0; i < emit->stack_info_alloc; i++) {
        emit->stack_info[i].kind = STACK_VALUE;
        emit->stack_info[i].vtype = VTYPE_UNBOUND;
    }

    mp_asm_base_start_pass(&emit->as->base, pass == MP_PASS_EMIT ? MP_ASM_PASS_EMIT : MP_ASM_PASS_COMPUTE);

    // generate code for entry to function

    if (emit->do_viper_types) {

        // right now we have a restriction of maximum of 4 arguments
        if (scope->num_pos_args > REG_ARG_NUM) {
            EMIT_NATIVE_VIPER_TYPE_ERROR(emit, "Viper functions don't currently support more than 4 arguments");
            return;
        }

        // entry to function
        int num_locals = 0;
        if (pass > MP_PASS_SCOPE) {
            num_locals = scope->num_locals - REG_LOCAL_NUM;
            if (num_locals < 0) {
                num_locals = 0;
            }
            emit->stack_start = num_locals;
            num_locals += scope->stack_size;
        }
        ASM_ENTRY(emit->as, num_locals);

        // TODO don't load r7 if we don't need it
        #if N_THUMB
        asm_thumb_mov_reg_i32(emit->as, ASM_THUMB_REG_R7, (mp_uint_t)mp_fun_table);
        #elif N_ARM
        asm_arm_mov_reg_i32(emit->as, ASM_ARM_REG_R7, (mp_uint_t)mp_fun_table);
        #endif

        #if N_X86
        for (int i = 0; i < scope->num_pos_args; i++) {
            if (i < REG_LOCAL_NUM) {
                asm_x86_mov_arg_to_r32(emit->as, i, reg_local_table[i]);
            } else {
                asm_x86_mov_arg_to_r32(emit->as, i, REG_TEMP0);
                asm_x86_mov_r32_to_local(emit->as, REG_TEMP0, i - REG_LOCAL_NUM);
            }
        }
        #else
        for (int i = 0; i < scope->num_pos_args; i++) {
            if (i < REG_LOCAL_NUM) {
                ASM_MOV_REG_REG(emit->as, reg_local_table[i], reg_arg_table[i]);
            } else {
                assert(i < REG_ARG_NUM); // should be true; max args is checked above
                ASM_MOV_LOCAL_REG(emit->as, i - REG_LOCAL_NUM, reg_arg_table[i]);
            }
        }
        #endif

    } else {
        // work out size of state (locals plus stack)
        emit->n_state = scope->num_locals + scope->stack_size;

        // the locals and stack start after the code_state structure
        emit->stack_start = STATE_START;

        // allocate space on C-stack for code_state structure, which includes state
        ASM_ENTRY(emit->as, STATE_START + emit->n_state);

        // TODO don't load r7 if we don't need it
        #if N_THUMB
        asm_thumb_mov_reg_i32(emit->as, ASM_THUMB_REG_R7, (mp_uint_t)mp_fun_table);
        #elif N_ARM
        asm_arm_mov_reg_i32(emit->as, ASM_ARM_REG_R7, (mp_uint_t)mp_fun_table);
        #endif

        // prepare incoming arguments for call to mp_setup_code_state

        #if N_X86
        asm_x86_mov_arg_to_r32(emit->as, 0, REG_ARG_1);
        asm_x86_mov_arg_to_r32(emit->as, 1, REG_ARG_2);
        asm_x86_mov_arg_to_r32(emit->as, 2, REG_ARG_3);
        asm_x86_mov_arg_to_r32(emit->as, 3, REG_ARG_4);
        #endif

        // set code_state.fun_bc
        ASM_MOV_LOCAL_REG(emit->as, offsetof(mp_code_state_t, fun_bc) / sizeof(uintptr_t), REG_ARG_1);

        // set code_state.ip (offset from start of this function to prelude info)
        // XXX this encoding may change size
        ASM_MOV_LOCAL_IMM_VIA(emit->as, offsetof(mp_code_state_t, ip) / sizeof(uintptr_t), emit->prelude_offset, REG_ARG_1);

        // put address of code_state into first arg
        ASM_MOV_REG_LOCAL_ADDR(emit->as, REG_ARG_1, 0);

        // call mp_setup_code_state to prepare code_state structure
        #if N_THUMB
        asm_thumb_bl_ind(emit->as, mp_fun_table[MP_F_SETUP_CODE_STATE], MP_F_SETUP_CODE_STATE, ASM_THUMB_REG_R4);
        #elif N_ARM
        asm_arm_bl_ind(emit->as, mp_fun_table[MP_F_SETUP_CODE_STATE], MP_F_SETUP_CODE_STATE, ASM_ARM_REG_R4);
        #else
        ASM_CALL_IND(emit->as, mp_fun_table[MP_F_SETUP_CODE_STATE], MP_F_SETUP_CODE_STATE);
        #endif

        // cache some locals in registers
        for (int i = 0; i < REG_LOCAL_NUM && i < scope->num_locals; ++i) {
            ASM_MOV_REG_LOCAL(emit->as, reg_local_table[i], STATE_START + emit->n_state - 1 - i);
        }

        // set the type of closed over variables
        for (mp_uint_t i = 0; i < scope->id_info_len; i++) {
            id_info_t *id = &scope->id_info[i];
            if (id->kind == ID_INFO_KIND_CELL) {
                emit->local_vtype[id->local_num] = VTYPE_PYOBJ;
            }
        }
    }

}

STATIC void emit_native_end_pass(emit_t *emit) {
    if (!emit->last_emit_was_return_value) {
        ASM_EXIT(emit->as);
    }

    if (!emit->do_viper_types) {
        emit->prelude_offset = mp_asm_base_get_code_pos(&emit->as->base);
        mp_asm_base_data(&emit->as->base, 1, 0x80 | ((emit->n_state >> 7) & 0x7f));
        mp_asm_base_data(&emit->as->base, 1, emit->n_state & 0x7f);
        mp_asm_base_data(&emit->as->base, 1, 0); // n_exc_stack
        mp_asm_base_data(&emit->as->base, 1, emit->scope->scope_flags);
        mp_asm_base_data(&emit->as->base, 1, emit->scope->num_pos_args);
        mp_asm_base_data(&emit->as->base, 1, emit->scope->num_kwonly_args);
        mp_asm_base_data(&emit->as->base, 1, emit->scope->num_def_pos_args);

        // write code info
        #if MICROPY_PERSISTENT_CODE
        mp_asm_base_data(&emit->as->base, 1, 5);
        mp_asm_base_data(&emit->as->base, 1, emit->scope->simple_name);
        mp_asm_base_data(&emit->as->base, 1, emit->scope->simple_name >> 8);
        mp_asm_base_data(&emit->as->base, 1, emit->scope->source_file);
        mp_asm_base_data(&emit->as->base, 1, emit->scope->source_file >> 8);
        #else
        mp_asm_base_data(&emit->as->base, 1, 1);
        #endif

        // bytecode prelude: initialise closed over variables
        for (int i = 0; i < emit->scope->id_info_len; i++) {
            id_info_t *id = &emit->scope->id_info[i];
            if (id->kind == ID_INFO_KIND_CELL) {
                assert(id->local_num < 255);
                mp_asm_base_data(&emit->as->base, 1, id->local_num); // write the local which should be converted to a cell
            }
        }
        mp_asm_base_data(&emit->as->base, 1, 255); // end of list sentinel

        mp_asm_base_align(&emit->as->base, ASM_WORD_SIZE);
        emit->const_table_offset = mp_asm_base_get_code_pos(&emit->as->base);

        // write argument names as qstr objects
        // see comment in corresponding part of emitbc.c about the logic here
        for (int i = 0; i < emit->scope->num_pos_args + emit->scope->num_kwonly_args; i++) {
            qstr qst = MP_QSTR__star_;
            for (int j = 0; j < emit->scope->id_info_len; ++j) {
                id_info_t *id = &emit->scope->id_info[j];
                if ((id->flags & ID_FLAG_IS_PARAM) && id->local_num == i) {
                    qst = id->qst;
                    break;
                }
            }
            mp_asm_base_data(&emit->as->base, ASM_WORD_SIZE, (mp_uint_t)MP_OBJ_NEW_QSTR(qst));
        }

    }

    ASM_END_PASS(emit->as);

    // check stack is back to zero size
    assert(emit->stack_size == 0);

    if (emit->pass == MP_PASS_EMIT) {
        void *f = mp_asm_base_get_code(&emit->as->base);
        mp_uint_t f_len = mp_asm_base_get_code_size(&emit->as->base);

        // compute type signature
        // note that the lower 4 bits of a vtype are tho correct MP_NATIVE_TYPE_xxx
        mp_uint_t type_sig = emit->return_vtype & 0xf;
        for (mp_uint_t i = 0; i < emit->scope->num_pos_args; i++) {
            type_sig |= (emit->local_vtype[i] & 0xf) << (i * 4 + 4);
        }

        mp_emit_glue_assign_native(emit->scope->raw_code,
            emit->do_viper_types ? MP_CODE_NATIVE_VIPER : MP_CODE_NATIVE_PY,
            f, f_len, (mp_uint_t*)((byte*)f + emit->const_table_offset),
            emit->scope->num_pos_args, emit->scope->scope_flags, type_sig);
    }
}

STATIC bool emit_native_last_emit_was_return_value(emit_t *emit) {
    return emit->last_emit_was_return_value;
}

STATIC void ensure_extra_stack(emit_t *emit, size_t delta) {
    if (emit->stack_size + delta > emit->stack_info_alloc) {
        size_t new_alloc = (emit->stack_size + delta + 8) & ~3;
        emit->stack_info = m_renew(stack_info_t, emit->stack_info, emit->stack_info_alloc, new_alloc);
        emit->stack_info_alloc = new_alloc;
    }
}

STATIC void adjust_stack(emit_t *emit, mp_int_t stack_size_delta) {
    assert((mp_int_t)emit->stack_size + stack_size_delta >= 0);
    assert((mp_int_t)emit->stack_size + stack_size_delta <= (mp_int_t)emit->stack_info_alloc);
    emit->stack_size += stack_size_delta;
    if (emit->pass > MP_PASS_SCOPE && emit->stack_size > emit->scope->stack_size) {
        emit->scope->stack_size = emit->stack_size;
    }
#ifdef DEBUG_PRINT
    DEBUG_printf("  adjust_stack; stack_size=%d+%d; stack now:", emit->stack_size - stack_size_delta, stack_size_delta);
    for (int i = 0; i < emit->stack_size; i++) {
        stack_info_t *si = &emit->stack_info[i];
        DEBUG_printf(" (v=%d k=%d %d)", si->vtype, si->kind, si->data.u_reg);
    }
    DEBUG_printf("\n");
#endif
}

STATIC void emit_native_adjust_stack_size(emit_t *emit, mp_int_t delta) {
    DEBUG_printf("adjust_stack_size(" INT_FMT ")\n", delta);
    if (delta > 0) {
        ensure_extra_stack(emit, delta);
    }
    // If we are adjusting the stack in a positive direction (pushing) then we
    // need to fill in values for the stack kind and vtype of the newly-pushed
    // entries.  These should be set to "value" (ie not reg or imm) because we
    // should only need to adjust the stack due to a jump to this part in the
    // code (and hence we have settled the stack before the jump).
    for (mp_int_t i = 0; i < delta; i++) {
        stack_info_t *si = &emit->stack_info[emit->stack_size + i];
        si->kind = STACK_VALUE;
        // TODO we don't know the vtype to use here.  At the moment this is a
        // hack to get the case of multi comparison working.
        if (delta == 1) {
            si->vtype = emit->saved_stack_vtype;
        } else {
            si->vtype = VTYPE_PYOBJ;
        }
    }
    adjust_stack(emit, delta);
}

STATIC void emit_native_set_source_line(emit_t *emit, mp_uint_t source_line) {
    (void)emit;
    (void)source_line;
}

// this must be called at start of emit functions
STATIC void emit_native_pre(emit_t *emit) {
    emit->last_emit_was_return_value = false;
}

// depth==0 is top, depth==1 is before top, etc
STATIC stack_info_t *peek_stack(emit_t *emit, mp_uint_t depth) {
    return &emit->stack_info[emit->stack_size - 1 - depth];
}

// depth==0 is top, depth==1 is before top, etc
STATIC vtype_kind_t peek_vtype(emit_t *emit, mp_uint_t depth) {
    return peek_stack(emit, depth)->vtype;
}

// pos=1 is TOS, pos=2 is next, etc
// use pos=0 for no skipping
STATIC void need_reg_single(emit_t *emit, int reg_needed, int skip_stack_pos) {
    skip_stack_pos = emit->stack_size - skip_stack_pos;
    for (int i = 0; i < emit->stack_size; i++) {
        if (i != skip_stack_pos) {
            stack_info_t *si = &emit->stack_info[i];
            if (si->kind == STACK_REG && si->data.u_reg == reg_needed) {
                si->kind = STACK_VALUE;
                ASM_MOV_LOCAL_REG(emit->as, emit->stack_start + i, si->data.u_reg);
            }
        }
    }
}

STATIC void need_reg_all(emit_t *emit) {
    for (int i = 0; i < emit->stack_size; i++) {
        stack_info_t *si = &emit->stack_info[i];
        if (si->kind == STACK_REG) {
            si->kind = STACK_VALUE;
            ASM_MOV_LOCAL_REG(emit->as, emit->stack_start + i, si->data.u_reg);
        }
    }
}

STATIC void need_stack_settled(emit_t *emit) {
    DEBUG_printf("  need_stack_settled; stack_size=%d\n", emit->stack_size);
    for (int i = 0; i < emit->stack_size; i++) {
        stack_info_t *si = &emit->stack_info[i];
        if (si->kind == STACK_REG) {
            DEBUG_printf("    reg(%u) to local(%u)\n", si->data.u_reg, emit->stack_start + i);
            si->kind = STACK_VALUE;
            ASM_MOV_LOCAL_REG(emit->as, emit->stack_start + i, si->data.u_reg);
        }
    }
    for (int i = 0; i < emit->stack_size; i++) {
        stack_info_t *si = &emit->stack_info[i];
        if (si->kind == STACK_IMM) {
            DEBUG_printf("    imm(" INT_FMT ") to local(%u)\n", si->data.u_imm, emit->stack_start + i);
            si->kind = STACK_VALUE;
            ASM_MOV_LOCAL_IMM_VIA(emit->as, emit->stack_start + i, si->data.u_imm, REG_TEMP0);
        }
    }
}

// pos=1 is TOS, pos=2 is next, etc
STATIC void emit_access_stack(emit_t *emit, int pos, vtype_kind_t *vtype, int reg_dest) {
    need_reg_single(emit, reg_dest, pos);
    stack_info_t *si = &emit->stack_info[emit->stack_size - pos];
    *vtype = si->vtype;
    switch (si->kind) {
        case STACK_VALUE:
            ASM_MOV_REG_LOCAL(emit->as, reg_dest, emit->stack_start + emit->stack_size - pos);
            break;

        case STACK_REG:
            if (si->data.u_reg != reg_dest) {
                ASM_MOV_REG_REG(emit->as, reg_dest, si->data.u_reg);
            }
            break;

        case STACK_IMM:
            ASM_MOV_REG_IMM(emit->as, reg_dest, si->data.u_imm);
            break;
    }
}

// does an efficient X=pop(); discard(); push(X)
// needs a (non-temp) register in case the poped element was stored in the stack
STATIC void emit_fold_stack_top(emit_t *emit, int reg_dest) {
    stack_info_t *si = &emit->stack_info[emit->stack_size - 2];
    si[0] = si[1];
    if (si->kind == STACK_VALUE) {
        // if folded element was on the stack we need to put it in a register
        ASM_MOV_REG_LOCAL(emit->as, reg_dest, emit->stack_start + emit->stack_size - 1);
        si->kind = STACK_REG;
        si->data.u_reg = reg_dest;
    }
    adjust_stack(emit, -1);
}

// If stacked value is in a register and the register is not r1 or r2, then
// *reg_dest is set to that register.  Otherwise the value is put in *reg_dest.
STATIC void emit_pre_pop_reg_flexible(emit_t *emit, vtype_kind_t *vtype, int *reg_dest, int not_r1, int not_r2) {
    emit->last_emit_was_return_value = false;
    stack_info_t *si = peek_stack(emit, 0);
    if (si->kind == STACK_REG && si->data.u_reg != not_r1 && si->data.u_reg != not_r2) {
        *vtype = si->vtype;
        *reg_dest = si->data.u_reg;
        need_reg_single(emit, *reg_dest, 1);
    } else {
        emit_access_stack(emit, 1, vtype, *reg_dest);
    }
    adjust_stack(emit, -1);
}

STATIC void emit_pre_pop_discard(emit_t *emit) {
    emit->last_emit_was_return_value = false;
    adjust_stack(emit, -1);
}

STATIC void emit_pre_pop_reg(emit_t *emit, vtype_kind_t *vtype, int reg_dest) {
    emit->last_emit_was_return_value = false;
    emit_access_stack(emit, 1, vtype, reg_dest);
    adjust_stack(emit, -1);
}

STATIC void emit_pre_pop_reg_reg(emit_t *emit, vtype_kind_t *vtypea, int rega, vtype_kind_t *vtypeb, int regb) {
    emit_pre_pop_reg(emit, vtypea, rega);
    emit_pre_pop_reg(emit, vtypeb, regb);
}

STATIC void emit_pre_pop_reg_reg_reg(emit_t *emit, vtype_kind_t *vtypea, int rega, vtype_kind_t *vtypeb, int regb, vtype_kind_t *vtypec, int regc) {
    emit_pre_pop_reg(emit, vtypea, rega);
    emit_pre_pop_reg(emit, vtypeb, regb);
    emit_pre_pop_reg(emit, vtypec, regc);
}

STATIC void emit_post(emit_t *emit) {
    (void)emit;
}

STATIC void emit_post_top_set_vtype(emit_t *emit, vtype_kind_t new_vtype) {
    stack_info_t *si = &emit->stack_info[emit->stack_size - 1];
    si->vtype = new_vtype;
}

STATIC void emit_post_push_reg(emit_t *emit, vtype_kind_t vtype, int reg) {
    ensure_extra_stack(emit, 1);
    stack_info_t *si = &emit->stack_info[emit->stack_size];
    si->vtype = vtype;
    si->kind = STACK_REG;
    si->data.u_reg = reg;
    adjust_stack(emit, 1);
}

STATIC void emit_post_push_imm(emit_t *emit, vtype_kind_t vtype, mp_int_t imm) {
    ensure_extra_stack(emit, 1);
    stack_info_t *si = &emit->stack_info[emit->stack_size];
    si->vtype = vtype;
    si->kind = STACK_IMM;
    si->data.u_imm = imm;
    adjust_stack(emit, 1);
}

STATIC void emit_post_push_reg_reg(emit_t *emit, vtype_kind_t vtypea, int rega, vtype_kind_t vtypeb, int regb) {
    emit_post_push_reg(emit, vtypea, rega);
    emit_post_push_reg(emit, vtypeb, regb);
}

STATIC void emit_post_push_reg_reg_reg(emit_t *emit, vtype_kind_t vtypea, int rega, vtype_kind_t vtypeb, int regb, vtype_kind_t vtypec, int regc) {
    emit_post_push_reg(emit, vtypea, rega);
    emit_post_push_reg(emit, vtypeb, regb);
    emit_post_push_reg(emit, vtypec, regc);
}

STATIC void emit_post_push_reg_reg_reg_reg(emit_t *emit, vtype_kind_t vtypea, int rega, vtype_kind_t vtypeb, int regb, vtype_kind_t vtypec, int regc, vtype_kind_t vtyped, int regd) {
    emit_post_push_reg(emit, vtypea, rega);
    emit_post_push_reg(emit, vtypeb, regb);
    emit_post_push_reg(emit, vtypec, regc);
    emit_post_push_reg(emit, vtyped, regd);
}

STATIC void emit_call(emit_t *emit, mp_fun_kind_t fun_kind) {
    need_reg_all(emit);
    ASM_CALL_IND(emit->as, mp_fun_table[fun_kind], fun_kind);
}

STATIC void emit_call_with_imm_arg(emit_t *emit, mp_fun_kind_t fun_kind, mp_int_t arg_val, int arg_reg) {
    need_reg_all(emit);
    ASM_MOV_REG_IMM(emit->as, arg_reg, arg_val);
    ASM_CALL_IND(emit->as, mp_fun_table[fun_kind], fun_kind);
}

// the first arg is stored in the code aligned on a mp_uint_t boundary
STATIC void emit_call_with_imm_arg_aligned(emit_t *emit, mp_fun_kind_t fun_kind, mp_int_t arg_val, int arg_reg) {
    need_reg_all(emit);
    ASM_MOV_REG_ALIGNED_IMM(emit->as, arg_reg, arg_val);
    ASM_CALL_IND(emit->as, mp_fun_table[fun_kind], fun_kind);
}

STATIC void emit_call_with_2_imm_args(emit_t *emit, mp_fun_kind_t fun_kind, mp_int_t arg_val1, int arg_reg1, mp_int_t arg_val2, int arg_reg2) {
    need_reg_all(emit);
    ASM_MOV_REG_IMM(emit->as, arg_reg1, arg_val1);
    ASM_MOV_REG_IMM(emit->as, arg_reg2, arg_val2);
    ASM_CALL_IND(emit->as, mp_fun_table[fun_kind], fun_kind);
}

// the first arg is stored in the code aligned on a mp_uint_t boundary
STATIC void emit_call_with_3_imm_args_and_first_aligned(emit_t *emit, mp_fun_kind_t fun_kind, mp_int_t arg_val1, int arg_reg1, mp_int_t arg_val2, int arg_reg2, mp_int_t arg_val3, int arg_reg3) {
    need_reg_all(emit);
    ASM_MOV_REG_ALIGNED_IMM(emit->as, arg_reg1, arg_val1);
    ASM_MOV_REG_IMM(emit->as, arg_reg2, arg_val2);
    ASM_MOV_REG_IMM(emit->as, arg_reg3, arg_val3);
    ASM_CALL_IND(emit->as, mp_fun_table[fun_kind], fun_kind);
}

// vtype of all n_pop objects is VTYPE_PYOBJ
// Will convert any items that are not VTYPE_PYOBJ to this type and put them back on the stack.
// If any conversions of non-immediate values are needed, then it uses REG_ARG_1, REG_ARG_2 and REG_RET.
// Otherwise, it does not use any temporary registers (but may use reg_dest before loading it with stack pointer).
STATIC void emit_get_stack_pointer_to_reg_for_pop(emit_t *emit, mp_uint_t reg_dest, mp_uint_t n_pop) {
    need_reg_all(emit);

    // First, store any immediate values to their respective place on the stack.
    for (mp_uint_t i = 0; i < n_pop; i++) {
        stack_info_t *si = &emit->stack_info[emit->stack_size - 1 - i];
        // must push any imm's to stack
        // must convert them to VTYPE_PYOBJ for viper code
        if (si->kind == STACK_IMM) {
            si->kind = STACK_VALUE;
            switch (si->vtype) {
                case VTYPE_PYOBJ:
                    ASM_MOV_LOCAL_IMM_VIA(emit->as, emit->stack_start + emit->stack_size - 1 - i, si->data.u_imm, reg_dest);
                    break;
                case VTYPE_BOOL:
                    if (si->data.u_imm == 0) {
                        ASM_MOV_LOCAL_IMM_VIA(emit->as, emit->stack_start + emit->stack_size - 1 - i, (mp_uint_t)mp_const_false, reg_dest);
                    } else {
                        ASM_MOV_LOCAL_IMM_VIA(emit->as, emit->stack_start + emit->stack_size - 1 - i, (mp_uint_t)mp_const_true, reg_dest);
                    }
                    si->vtype = VTYPE_PYOBJ;
                    break;
                case VTYPE_INT:
                case VTYPE_UINT:
                    ASM_MOV_LOCAL_IMM_VIA(emit->as, emit->stack_start + emit->stack_size - 1 - i, (uintptr_t)MP_OBJ_NEW_SMALL_INT(si->data.u_imm), reg_dest);
                    si->vtype = VTYPE_PYOBJ;
                    break;
                default:
                    // not handled
                    mp_raise_NotImplementedError("conversion to object");
            }
        }

        // verify that this value is on the stack
        assert(si->kind == STACK_VALUE);
    }

    // Second, convert any non-VTYPE_PYOBJ to that type.
    for (mp_uint_t i = 0; i < n_pop; i++) {
        stack_info_t *si = &emit->stack_info[emit->stack_size - 1 - i];
        if (si->vtype != VTYPE_PYOBJ) {
            mp_uint_t local_num = emit->stack_start + emit->stack_size - 1 - i;
            ASM_MOV_REG_LOCAL(emit->as, REG_ARG_1, local_num);
            emit_call_with_imm_arg(emit, MP_F_CONVERT_NATIVE_TO_OBJ, si->vtype, REG_ARG_2); // arg2 = type
            ASM_MOV_LOCAL_REG(emit->as, local_num, REG_RET);
            si->vtype = VTYPE_PYOBJ;
            DEBUG_printf("  convert_native_to_obj(local_num=" UINT_FMT ")\n", local_num);
        }
    }

    // Adujust the stack for a pop of n_pop items, and load the stack pointer into reg_dest.
    adjust_stack(emit, -n_pop);
    ASM_MOV_REG_LOCAL_ADDR(emit->as, reg_dest, emit->stack_start + emit->stack_size);
}

// vtype of all n_push objects is VTYPE_PYOBJ
STATIC void emit_get_stack_pointer_to_reg_for_push(emit_t *emit, mp_uint_t reg_dest, mp_uint_t n_push) {
    need_reg_all(emit);
    ensure_extra_stack(emit, n_push);
    for (mp_uint_t i = 0; i < n_push; i++) {
        emit->stack_info[emit->stack_size + i].kind = STACK_VALUE;
        emit->stack_info[emit->stack_size + i].vtype = VTYPE_PYOBJ;
    }
    ASM_MOV_REG_LOCAL_ADDR(emit->as, reg_dest, emit->stack_start + emit->stack_size);
    adjust_stack(emit, n_push);
}

STATIC void emit_native_label_assign(emit_t *emit, mp_uint_t l) {
    DEBUG_printf("label_assign(" UINT_FMT ")\n", l);
    emit_native_pre(emit);
    // need to commit stack because we can jump here from elsewhere
    need_stack_settled(emit);
    mp_asm_base_label_assign(&emit->as->base, l);
    emit_post(emit);
}

STATIC void emit_native_import_name(emit_t *emit, qstr qst) {
    DEBUG_printf("import_name %s\n", qstr_str(qst));

    // get arguments from stack: arg2 = fromlist, arg3 = level
    // if using viper types these arguments must be converted to proper objects
    if (emit->do_viper_types) {
        // fromlist should be None or a tuple
        stack_info_t *top = peek_stack(emit, 0);
        if (top->vtype == VTYPE_PTR_NONE) {
            emit_pre_pop_discard(emit);
            ASM_MOV_REG_IMM(emit->as, REG_ARG_2, (mp_uint_t)mp_const_none);
        } else {
            vtype_kind_t vtype_fromlist;
            emit_pre_pop_reg(emit, &vtype_fromlist, REG_ARG_2);
            assert(vtype_fromlist == VTYPE_PYOBJ);
        }

        // level argument should be an immediate integer
        top = peek_stack(emit, 0);
        assert(top->vtype == VTYPE_INT && top->kind == STACK_IMM);
        ASM_MOV_REG_IMM(emit->as, REG_ARG_3, (mp_uint_t)MP_OBJ_NEW_SMALL_INT(top->data.u_imm));
        emit_pre_pop_discard(emit);

    } else {
        vtype_kind_t vtype_fromlist;
        vtype_kind_t vtype_level;
        emit_pre_pop_reg_reg(emit, &vtype_fromlist, REG_ARG_2, &vtype_level, REG_ARG_3);
        assert(vtype_fromlist == VTYPE_PYOBJ);
        assert(vtype_level == VTYPE_PYOBJ);
    }

    emit_call_with_imm_arg(emit, MP_F_IMPORT_NAME, qst, REG_ARG_1); // arg1 = import name
    emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
}

STATIC void emit_native_import_from(emit_t *emit, qstr qst) {
    DEBUG_printf("import_from %s\n", qstr_str(qst));
    emit_native_pre(emit);
    vtype_kind_t vtype_module;
    emit_access_stack(emit, 1, &vtype_module, REG_ARG_1); // arg1 = module
    assert(vtype_module == VTYPE_PYOBJ);
    emit_call_with_imm_arg(emit, MP_F_IMPORT_FROM, qst, REG_ARG_2); // arg2 = import name
    emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
}

STATIC void emit_native_import_star(emit_t *emit) {
    DEBUG_printf("import_star\n");
    vtype_kind_t vtype_module;
    emit_pre_pop_reg(emit, &vtype_module, REG_ARG_1); // arg1 = module
    assert(vtype_module == VTYPE_PYOBJ);
    emit_call(emit, MP_F_IMPORT_ALL);
    emit_post(emit);
}

STATIC void emit_native_import(emit_t *emit, qstr qst, int kind) {
    if (kind == MP_EMIT_IMPORT_NAME) {
        emit_native_import_name(emit, qst);
    } else if (kind == MP_EMIT_IMPORT_FROM) {
        emit_native_import_from(emit, qst);
    } else {
        emit_native_import_star(emit);
    }
}

STATIC void emit_native_load_const_tok(emit_t *emit, mp_token_kind_t tok) {
    DEBUG_printf("load_const_tok(tok=%u)\n", tok);
    emit_native_pre(emit);
    vtype_kind_t vtype;
    mp_uint_t val;
    if (emit->do_viper_types) {
        switch (tok) {
            case MP_TOKEN_KW_NONE: vtype = VTYPE_PTR_NONE; val = 0; break;
            case MP_TOKEN_KW_FALSE: vtype = VTYPE_BOOL; val = 0; break;
            case MP_TOKEN_KW_TRUE: vtype = VTYPE_BOOL; val = 1; break;
            default:
                assert(tok == MP_TOKEN_ELLIPSIS);
                vtype = VTYPE_PYOBJ; val = (mp_uint_t)&mp_const_ellipsis_obj; break;
        }
    } else {
        vtype = VTYPE_PYOBJ;
        switch (tok) {
            case MP_TOKEN_KW_NONE: val = (mp_uint_t)mp_const_none; break;
            case MP_TOKEN_KW_FALSE: val = (mp_uint_t)mp_const_false; break;
            case MP_TOKEN_KW_TRUE: val = (mp_uint_t)mp_const_true; break;
            default:
                assert(tok == MP_TOKEN_ELLIPSIS);
                val = (mp_uint_t)&mp_const_ellipsis_obj; break;
        }
    }
    emit_post_push_imm(emit, vtype, val);
}

STATIC void emit_native_load_const_small_int(emit_t *emit, mp_int_t arg) {
    DEBUG_printf("load_const_small_int(int=" INT_FMT ")\n", arg);
    emit_native_pre(emit);
    if (emit->do_viper_types) {
        emit_post_push_imm(emit, VTYPE_INT, arg);
    } else {
        emit_post_push_imm(emit, VTYPE_PYOBJ, (mp_uint_t)MP_OBJ_NEW_SMALL_INT(arg));
    }
}

STATIC void emit_native_load_const_str(emit_t *emit, qstr qst) {
    emit_native_pre(emit);
    // TODO: Eventually we want to be able to work with raw pointers in viper to
    // do native array access.  For now we just load them as any other object.
    /*
    if (emit->do_viper_types) {
        // load a pointer to the asciiz string?
        emit_post_push_imm(emit, VTYPE_PTR, (mp_uint_t)qstr_str(qst));
    } else
    */
    {
        emit_post_push_imm(emit, VTYPE_PYOBJ, (mp_uint_t)MP_OBJ_NEW_QSTR(qst));
    }
}

STATIC void emit_native_load_const_obj(emit_t *emit, mp_obj_t obj) {
    emit_native_pre(emit);
    need_reg_single(emit, REG_RET, 0);
    ASM_MOV_REG_ALIGNED_IMM(emit->as, REG_RET, (mp_uint_t)obj);
    emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
}

STATIC void emit_native_load_null(emit_t *emit) {
    emit_native_pre(emit);
    emit_post_push_imm(emit, VTYPE_PYOBJ, 0);
}

STATIC void emit_native_load_fast(emit_t *emit, qstr qst, mp_uint_t local_num) {
    DEBUG_printf("load_fast(%s, " UINT_FMT ")\n", qstr_str(qst), local_num);
    vtype_kind_t vtype = emit->local_vtype[local_num];
    if (vtype == VTYPE_UNBOUND) {
        EMIT_NATIVE_VIPER_TYPE_ERROR(emit, "local '%q' used before type known", qst);
    }
    emit_native_pre(emit);
    if (local_num < REG_LOCAL_NUM) {
        emit_post_push_reg(emit, vtype, reg_local_table[local_num]);
    } else {
        need_reg_single(emit, REG_TEMP0, 0);
        if (emit->do_viper_types) {
            ASM_MOV_REG_LOCAL(emit->as, REG_TEMP0, local_num - REG_LOCAL_NUM);
        } else {
            ASM_MOV_REG_LOCAL(emit->as, REG_TEMP0, STATE_START + emit->n_state - 1 - local_num);
        }
        emit_post_push_reg(emit, vtype, REG_TEMP0);
    }
}

STATIC void emit_native_load_deref(emit_t *emit, qstr qst, mp_uint_t local_num) {
    DEBUG_printf("load_deref(%s, " UINT_FMT ")\n", qstr_str(qst), local_num);
    need_reg_single(emit, REG_RET, 0);
    emit_native_load_fast(emit, qst, local_num);
    vtype_kind_t vtype;
    int reg_base = REG_RET;
    emit_pre_pop_reg_flexible(emit, &vtype, &reg_base, -1, -1);
    ASM_LOAD_REG_REG_OFFSET(emit->as, REG_RET, reg_base, 1);
    // closed over vars are always Python objects
    emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
}

STATIC void emit_native_load_local(emit_t *emit, qstr qst, mp_uint_t local_num, int kind) {
    if (kind == MP_EMIT_IDOP_LOCAL_FAST) {
        emit_native_load_fast(emit, qst, local_num);
    } else {
        emit_native_load_deref(emit, qst, local_num);
    }
}

STATIC void emit_native_load_global(emit_t *emit, qstr qst, int kind) {
    MP_STATIC_ASSERT(MP_F_LOAD_NAME + MP_EMIT_IDOP_GLOBAL_NAME == MP_F_LOAD_NAME);
    MP_STATIC_ASSERT(MP_F_LOAD_NAME + MP_EMIT_IDOP_GLOBAL_GLOBAL == MP_F_LOAD_GLOBAL);
    emit_native_pre(emit);
    if (kind == MP_EMIT_IDOP_GLOBAL_NAME) {
        DEBUG_printf("load_name(%s)\n", qstr_str(qst));
    } else {
        DEBUG_printf("load_global(%s)\n", qstr_str(qst));
        if (emit->do_viper_types) {
            // check for builtin casting operators
            if (qst == MP_QSTR_int) {
                emit_post_push_imm(emit, VTYPE_BUILTIN_CAST, VTYPE_INT);
                return;
            } else if (qst == MP_QSTR_uint) {
                emit_post_push_imm(emit, VTYPE_BUILTIN_CAST, VTYPE_UINT);
                return;
            } else if (qst == MP_QSTR_ptr) {
                emit_post_push_imm(emit, VTYPE_BUILTIN_CAST, VTYPE_PTR);
                return;
            } else if (qst == MP_QSTR_ptr8) {
                emit_post_push_imm(emit, VTYPE_BUILTIN_CAST, VTYPE_PTR8);
                return;
            } else if (qst == MP_QSTR_ptr16) {
                emit_post_push_imm(emit, VTYPE_BUILTIN_CAST, VTYPE_PTR16);
                return;
            } else if (qst == MP_QSTR_ptr32) {
                emit_post_push_imm(emit, VTYPE_BUILTIN_CAST, VTYPE_PTR32);
                return;
            }
        }
    }
    emit_call_with_imm_arg(emit, MP_F_LOAD_NAME + kind, qst, REG_ARG_1);
    emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
}

STATIC void emit_native_load_attr(emit_t *emit, qstr qst) {
    // depends on type of subject:
    //  - integer, function, pointer to integers: error
    //  - pointer to structure: get member, quite easy
    //  - Python object: call mp_load_attr, and needs to be typed to convert result
    vtype_kind_t vtype_base;
    emit_pre_pop_reg(emit, &vtype_base, REG_ARG_1); // arg1 = base
    assert(vtype_base == VTYPE_PYOBJ);
    emit_call_with_imm_arg(emit, MP_F_LOAD_ATTR, qst, REG_ARG_2); // arg2 = attribute name
    emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
}

STATIC void emit_native_load_method(emit_t *emit, qstr qst, bool is_super) {
    if (is_super) {
        emit_get_stack_pointer_to_reg_for_pop(emit, REG_ARG_2, 3); // arg2 = dest ptr
        emit_get_stack_pointer_to_reg_for_push(emit, REG_ARG_2, 2); // arg2 = dest ptr
        emit_call_with_imm_arg(emit, MP_F_LOAD_SUPER_METHOD, qst, REG_ARG_1); // arg1 = method name
    } else {
        vtype_kind_t vtype_base;
        emit_pre_pop_reg(emit, &vtype_base, REG_ARG_1); // arg1 = base
        assert(vtype_base == VTYPE_PYOBJ);
        emit_get_stack_pointer_to_reg_for_push(emit, REG_ARG_3, 2); // arg3 = dest ptr
        emit_call_with_imm_arg(emit, MP_F_LOAD_METHOD, qst, REG_ARG_2); // arg2 = method name
    }
}

STATIC void emit_native_load_build_class(emit_t *emit) {
    emit_native_pre(emit);
    emit_call(emit, MP_F_LOAD_BUILD_CLASS);
    emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
}

STATIC void emit_native_load_subscr(emit_t *emit) {
    DEBUG_printf("load_subscr\n");
    // need to compile: base[index]

    // pop: index, base
    // optimise case where index is an immediate
    vtype_kind_t vtype_base = peek_vtype(emit, 1);

    if (vtype_base == VTYPE_PYOBJ) {
        // standard Python subscr
        // TODO factor this implicit cast code with other uses of it
        vtype_kind_t vtype_index = peek_vtype(emit, 0);
        if (vtype_index == VTYPE_PYOBJ) {
            emit_pre_pop_reg(emit, &vtype_index, REG_ARG_2);
        } else {
            emit_pre_pop_reg(emit, &vtype_index, REG_ARG_1);
            emit_call_with_imm_arg(emit, MP_F_CONVERT_NATIVE_TO_OBJ, vtype_index, REG_ARG_2); // arg2 = type
            ASM_MOV_REG_REG(emit->as, REG_ARG_2, REG_RET);
        }
        emit_pre_pop_reg(emit, &vtype_base, REG_ARG_1);
        emit_call_with_imm_arg(emit, MP_F_OBJ_SUBSCR, (mp_uint_t)MP_OBJ_SENTINEL, REG_ARG_3);
        emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
    } else {
        // viper load
        // TODO The different machine architectures have very different
        // capabilities and requirements for loads, so probably best to
        // write a completely separate load-optimiser for each one.
        stack_info_t *top = peek_stack(emit, 0);
        if (top->vtype == VTYPE_INT && top->kind == STACK_IMM) {
            // index is an immediate
            mp_int_t index_value = top->data.u_imm;
            emit_pre_pop_discard(emit); // discard index
            int reg_base = REG_ARG_1;
            int reg_index = REG_ARG_2;
            emit_pre_pop_reg_flexible(emit, &vtype_base, &reg_base, reg_index, reg_index);
            switch (vtype_base) {
                case VTYPE_PTR8: {
                    // pointer to 8-bit memory
                    // TODO optimise to use thumb ldrb r1, [r2, r3]
                    if (index_value != 0) {
                        // index is non-zero
                        #if N_THUMB
                        if (index_value > 0 && index_value < 32) {
                            asm_thumb_ldrb_rlo_rlo_i5(emit->as, REG_RET, reg_base, index_value);
                            break;
                        }
                        #endif
                        ASM_MOV_REG_IMM(emit->as, reg_index, index_value);
                        ASM_ADD_REG_REG(emit->as, reg_index, reg_base); // add index to base
                        reg_base = reg_index;
                    }
                    ASM_LOAD8_REG_REG(emit->as, REG_RET, reg_base); // load from (base+index)
                    break;
                }
                case VTYPE_PTR16: {
                    // pointer to 16-bit memory
                    if (index_value != 0) {
                        // index is a non-zero immediate
                        #if N_THUMB
                        if (index_value > 0 && index_value < 32) {
                            asm_thumb_ldrh_rlo_rlo_i5(emit->as, REG_RET, reg_base, index_value);
                            break;
                        }
                        #endif
                        ASM_MOV_REG_IMM(emit->as, reg_index, index_value << 1);
                        ASM_ADD_REG_REG(emit->as, reg_index, reg_base); // add 2*index to base
                        reg_base = reg_index;
                    }
                    ASM_LOAD16_REG_REG(emit->as, REG_RET, reg_base); // load from (base+2*index)
                    break;
                }
                case VTYPE_PTR32: {
                    // pointer to 32-bit memory
                    if (index_value != 0) {
                        // index is a non-zero immediate
                        #if N_THUMB
                        if (index_value > 0 && index_value < 32) {
                            asm_thumb_ldr_rlo_rlo_i5(emit->as, REG_RET, reg_base, index_value);
                            break;
                        }
                        #endif
                        ASM_MOV_REG_IMM(emit->as, reg_index, index_value << 2);
                        ASM_ADD_REG_REG(emit->as, reg_index, reg_base); // add 4*index to base
                        reg_base = reg_index;
                    }
                    ASM_LOAD32_REG_REG(emit->as, REG_RET, reg_base); // load from (base+4*index)
                    break;
                }
                default:
                    EMIT_NATIVE_VIPER_TYPE_ERROR(emit,
                        "can't load from '%q'", vtype_to_qstr(vtype_base));
            }
        } else {
            // index is not an immediate
            vtype_kind_t vtype_index;
            int reg_index = REG_ARG_2;
            emit_pre_pop_reg_flexible(emit, &vtype_index, &reg_index, REG_ARG_1, REG_ARG_1);
            emit_pre_pop_reg(emit, &vtype_base, REG_ARG_1);
            if (vtype_index != VTYPE_INT && vtype_index != VTYPE_UINT) {
                EMIT_NATIVE_VIPER_TYPE_ERROR(emit,
                    "can't load with '%q' index", vtype_to_qstr(vtype_index));
            }
            switch (vtype_base) {
                case VTYPE_PTR8: {
                    // pointer to 8-bit memory
                    // TODO optimise to use thumb ldrb r1, [r2, r3]
                    ASM_ADD_REG_REG(emit->as, REG_ARG_1, reg_index); // add index to base
                    ASM_LOAD8_REG_REG(emit->as, REG_RET, REG_ARG_1); // store value to (base+index)
                    break;
                }
                case VTYPE_PTR16: {
                    // pointer to 16-bit memory
                    ASM_ADD_REG_REG(emit->as, REG_ARG_1, reg_index); // add index to base
                    ASM_ADD_REG_REG(emit->as, REG_ARG_1, reg_index); // add index to base
                    ASM_LOAD16_REG_REG(emit->as, REG_RET, REG_ARG_1); // load from (base+2*index)
                    break;
                }
                case VTYPE_PTR32: {
                    // pointer to word-size memory
                    ASM_ADD_REG_REG(emit->as, REG_ARG_1, reg_index); // add index to base
                    ASM_ADD_REG_REG(emit->as, REG_ARG_1, reg_index); // add index to base
                    ASM_ADD_REG_REG(emit->as, REG_ARG_1, reg_index); // add index to base
                    ASM_ADD_REG_REG(emit->as, REG_ARG_1, reg_index); // add index to base
                    ASM_LOAD32_REG_REG(emit->as, REG_RET, REG_ARG_1); // load from (base+4*index)
                    break;
                }
                default:
                    EMIT_NATIVE_VIPER_TYPE_ERROR(emit,
                        "can't load from '%q'", vtype_to_qstr(vtype_base));
            }
        }
        emit_post_push_reg(emit, VTYPE_INT, REG_RET);
    }
}

STATIC void emit_native_store_fast(emit_t *emit, qstr qst, mp_uint_t local_num) {
    vtype_kind_t vtype;
    if (local_num < REG_LOCAL_NUM) {
        emit_pre_pop_reg(emit, &vtype, reg_local_table[local_num]);
    } else {
        emit_pre_pop_reg(emit, &vtype, REG_TEMP0);
        if (emit->do_viper_types) {
            ASM_MOV_LOCAL_REG(emit->as, local_num - REG_LOCAL_NUM, REG_TEMP0);
        } else {
            ASM_MOV_LOCAL_REG(emit->as, STATE_START + emit->n_state - 1 - local_num, REG_TEMP0);
        }
    }
    emit_post(emit);

    // check types
    if (emit->local_vtype[local_num] == VTYPE_UNBOUND) {
        // first time this local is assigned, so give it a type of the object stored in it
        emit->local_vtype[local_num] = vtype;
    } else if (emit->local_vtype[local_num] != vtype) {
        // type of local is not the same as object stored in it
        EMIT_NATIVE_VIPER_TYPE_ERROR(emit,
            "local '%q' has type '%q' but source is '%q'",
            qst, vtype_to_qstr(emit->local_vtype[local_num]), vtype_to_qstr(vtype));
    }
}

STATIC void emit_native_store_deref(emit_t *emit, qstr qst, mp_uint_t local_num) {
    DEBUG_printf("store_deref(%s, " UINT_FMT ")\n", qstr_str(qst), local_num);
    need_reg_single(emit, REG_TEMP0, 0);
    need_reg_single(emit, REG_TEMP1, 0);
    emit_native_load_fast(emit, qst, local_num);
    vtype_kind_t vtype;
    int reg_base = REG_TEMP0;
    emit_pre_pop_reg_flexible(emit, &vtype, &reg_base, -1, -1);
    int reg_src = REG_TEMP1;
    emit_pre_pop_reg_flexible(emit, &vtype, &reg_src, reg_base, reg_base);
    ASM_STORE_REG_REG_OFFSET(emit->as, reg_src, reg_base, 1);
    emit_post(emit);
}

STATIC void emit_native_store_local(emit_t *emit, qstr qst, mp_uint_t local_num, int kind) {
    if (kind == MP_EMIT_IDOP_LOCAL_FAST) {
        emit_native_store_fast(emit, qst, local_num);
    } else {
        emit_native_store_deref(emit, qst, local_num);
    }
}

STATIC void emit_native_store_global(emit_t *emit, qstr qst, int kind) {
    MP_STATIC_ASSERT(MP_F_STORE_NAME + MP_EMIT_IDOP_GLOBAL_NAME == MP_F_STORE_NAME);
    MP_STATIC_ASSERT(MP_F_STORE_NAME + MP_EMIT_IDOP_GLOBAL_GLOBAL == MP_F_STORE_GLOBAL);
    if (kind == MP_EMIT_IDOP_GLOBAL_NAME) {
        // mp_store_name, but needs conversion of object (maybe have mp_viper_store_name(obj, type))
        vtype_kind_t vtype;
        emit_pre_pop_reg(emit, &vtype, REG_ARG_2);
        assert(vtype == VTYPE_PYOBJ);
    } else {
        vtype_kind_t vtype = peek_vtype(emit, 0);
        if (vtype == VTYPE_PYOBJ) {
            emit_pre_pop_reg(emit, &vtype, REG_ARG_2);
        } else {
            emit_pre_pop_reg(emit, &vtype, REG_ARG_1);
            emit_call_with_imm_arg(emit, MP_F_CONVERT_NATIVE_TO_OBJ, vtype, REG_ARG_2); // arg2 = type
            ASM_MOV_REG_REG(emit->as, REG_ARG_2, REG_RET);
        }
    }
    emit_call_with_imm_arg(emit, MP_F_STORE_NAME + kind, qst, REG_ARG_1); // arg1 = name
    emit_post(emit);
}

STATIC void emit_native_store_attr(emit_t *emit, qstr qst) {
    vtype_kind_t vtype_base, vtype_val;
    emit_pre_pop_reg_reg(emit, &vtype_base, REG_ARG_1, &vtype_val, REG_ARG_3); // arg1 = base, arg3 = value
    assert(vtype_base == VTYPE_PYOBJ);
    assert(vtype_val == VTYPE_PYOBJ);
    emit_call_with_imm_arg(emit, MP_F_STORE_ATTR, qst, REG_ARG_2); // arg2 = attribute name
    emit_post(emit);
}

STATIC void emit_native_store_subscr(emit_t *emit) {
    DEBUG_printf("store_subscr\n");
    // need to compile: base[index] = value

    // pop: index, base, value
    // optimise case where index is an immediate
    vtype_kind_t vtype_base = peek_vtype(emit, 1);

    if (vtype_base == VTYPE_PYOBJ) {
        // standard Python subscr
        vtype_kind_t vtype_index = peek_vtype(emit, 0);
        vtype_kind_t vtype_value = peek_vtype(emit, 2);
        if (vtype_index != VTYPE_PYOBJ || vtype_value != VTYPE_PYOBJ) {
            // need to implicitly convert non-objects to objects
            // TODO do this properly
            emit_get_stack_pointer_to_reg_for_pop(emit, REG_ARG_1, 3);
            adjust_stack(emit, 3);
        }
        emit_pre_pop_reg_reg_reg(emit, &vtype_index, REG_ARG_2, &vtype_base, REG_ARG_1, &vtype_value, REG_ARG_3);
        emit_call(emit, MP_F_OBJ_SUBSCR);
    } else {
        // viper store
        // TODO The different machine architectures have very different
        // capabilities and requirements for stores, so probably best to
        // write a completely separate store-optimiser for each one.
        stack_info_t *top = peek_stack(emit, 0);
        if (top->vtype == VTYPE_INT && top->kind == STACK_IMM) {
            // index is an immediate
            mp_int_t index_value = top->data.u_imm;
            emit_pre_pop_discard(emit); // discard index
            vtype_kind_t vtype_value;
            int reg_base = REG_ARG_1;
            int reg_index = REG_ARG_2;
            int reg_value = REG_ARG_3;
            emit_pre_pop_reg_flexible(emit, &vtype_base, &reg_base, reg_index, reg_value);
            #if N_X86
            // special case: x86 needs byte stores to be from lower 4 regs (REG_ARG_3 is EDX)
            emit_pre_pop_reg(emit, &vtype_value, reg_value);
            #else
            emit_pre_pop_reg_flexible(emit, &vtype_value, &reg_value, reg_base, reg_index);
            #endif
            if (vtype_value != VTYPE_BOOL && vtype_value != VTYPE_INT && vtype_value != VTYPE_UINT) {
                EMIT_NATIVE_VIPER_TYPE_ERROR(emit,
                    "can't store '%q'", vtype_to_qstr(vtype_value));
            }
            switch (vtype_base) {
                case VTYPE_PTR8: {
                    // pointer to 8-bit memory
                    // TODO optimise to use thumb strb r1, [r2, r3]
                    if (index_value != 0) {
                        // index is non-zero
                        #if N_THUMB
                        if (index_value > 0 && index_value < 32) {
                            asm_thumb_strb_rlo_rlo_i5(emit->as, reg_value, reg_base, index_value);
                            break;
                        }
                        #endif
                        ASM_MOV_REG_IMM(emit->as, reg_index, index_value);
                        #if N_ARM
                        asm_arm_strb_reg_reg_reg(emit->as, reg_value, reg_base, reg_index);
                        return;
                        #endif
                        ASM_ADD_REG_REG(emit->as, reg_index, reg_base); // add index to base
                        reg_base = reg_index;
                    }
                    ASM_STORE8_REG_REG(emit->as, reg_value, reg_base); // store value to (base+index)
                    break;
                }
                case VTYPE_PTR16: {
                    // pointer to 16-bit memory
                    if (index_value != 0) {
                        // index is a non-zero immediate
                        #if N_THUMB
                        if (index_value > 0 && index_value < 32) {
                            asm_thumb_strh_rlo_rlo_i5(emit->as, reg_value, reg_base, index_value);
                            break;
                        }
                        #endif
                        ASM_MOV_REG_IMM(emit->as, reg_index, index_value << 1);
                        #if N_ARM
                        asm_arm_strh_reg_reg_reg(emit->as, reg_value, reg_base, reg_index);
                        return;
                        #endif
                        ASM_ADD_REG_REG(emit->as, reg_index, reg_base); // add 2*index to base
                        reg_base = reg_index;
                    }
                    ASM_STORE16_REG_REG(emit->as, reg_value, reg_base); // store value to (base+2*index)
                    break;
                }
                case VTYPE_PTR32: {
                    // pointer to 32-bit memory
                    if (index_value != 0) {
                        // index is a non-zero immediate
                        #if N_THUMB
                        if (index_value > 0 && index_value < 32) {
                            asm_thumb_str_rlo_rlo_i5(emit->as, reg_value, reg_base, index_value);
                            break;
                        }
                        #endif
                        ASM_MOV_REG_IMM(emit->as, reg_index, index_value << 2);
                        #if N_ARM
                        asm_arm_str_reg_reg_reg(emit->as, reg_value, reg_base, reg_index);
                        return;
                        #endif
                        ASM_ADD_REG_REG(emit->as, reg_index, reg_base); // add 4*index to base
                        reg_base = reg_index;
                    }
                    ASM_STORE32_REG_REG(emit->as, reg_value, reg_base); // store value to (base+4*index)
                    break;
                }
                default:
                    EMIT_NATIVE_VIPER_TYPE_ERROR(emit,
                        "can't store to '%q'", vtype_to_qstr(vtype_base));
            }
        } else {
            // index is not an immediate
            vtype_kind_t vtype_index, vtype_value;
            int reg_index = REG_ARG_2;
            int reg_value = REG_ARG_3;
            emit_pre_pop_reg_flexible(emit, &vtype_index, &reg_index, REG_ARG_1, reg_value);
            emit_pre_pop_reg(emit, &vtype_base, REG_ARG_1);
            if (vtype_index != VTYPE_INT && vtype_index != VTYPE_UINT) {
                EMIT_NATIVE_VIPER_TYPE_ERROR(emit,
                    "can't store with '%q' index", vtype_to_qstr(vtype_index));
            }
            #if N_X86
            // special case: x86 needs byte stores to be from lower 4 regs (REG_ARG_3 is EDX)
            emit_pre_pop_reg(emit, &vtype_value, reg_value);
            #else
            emit_pre_pop_reg_flexible(emit, &vtype_value, &reg_value, REG_ARG_1, reg_index);
            #endif
            if (vtype_value != VTYPE_BOOL && vtype_value != VTYPE_INT && vtype_value != VTYPE_UINT) {
                EMIT_NATIVE_VIPER_TYPE_ERROR(emit,
                    "can't store '%q'", vtype_to_qstr(vtype_value));
            }
            switch (vtype_base) {
                case VTYPE_PTR8: {
                    // pointer to 8-bit memory
                    // TODO optimise to use thumb strb r1, [r2, r3]
                    #if N_ARM
                    asm_arm_strb_reg_reg_reg(emit->as, reg_value, REG_ARG_1, reg_index);
                    break;
                    #endif
                    ASM_ADD_REG_REG(emit->as, REG_ARG_1, reg_index); // add index to base
                    ASM_STORE8_REG_REG(emit->as, reg_value, REG_ARG_1); // store value to (base+index)
                    break;
                }
                case VTYPE_PTR16: {
                    // pointer to 16-bit memory
                    #if N_ARM
                    asm_arm_strh_reg_reg_reg(emit->as, reg_value, REG_ARG_1, reg_index);
                    break;
                    #endif
                    ASM_ADD_REG_REG(emit->as, REG_ARG_1, reg_index); // add index to base
                    ASM_ADD_REG_REG(emit->as, REG_ARG_1, reg_index); // add index to base
                    ASM_STORE16_REG_REG(emit->as, reg_value, REG_ARG_1); // store value to (base+2*index)
                    break;
                }
                case VTYPE_PTR32: {
                    // pointer to 32-bit memory
                    #if N_ARM
                    asm_arm_str_reg_reg_reg(emit->as, reg_value, REG_ARG_1, reg_index);
                    break;
                    #endif
                    ASM_ADD_REG_REG(emit->as, REG_ARG_1, reg_index); // add index to base
                    ASM_ADD_REG_REG(emit->as, REG_ARG_1, reg_index); // add index to base
                    ASM_ADD_REG_REG(emit->as, REG_ARG_1, reg_index); // add index to base
                    ASM_ADD_REG_REG(emit->as, REG_ARG_1, reg_index); // add index to base
                    ASM_STORE32_REG_REG(emit->as, reg_value, REG_ARG_1); // store value to (base+4*index)
                    break;
                }
                default:
                    EMIT_NATIVE_VIPER_TYPE_ERROR(emit,
                        "can't store to '%q'", vtype_to_qstr(vtype_base));
            }
        }

    }
}

STATIC void emit_native_delete_local(emit_t *emit, qstr qst, mp_uint_t local_num, int kind) {
    if (kind == MP_EMIT_IDOP_LOCAL_FAST) {
        // TODO: This is not compliant implementation. We could use MP_OBJ_SENTINEL
        // to mark deleted vars but then every var would need to be checked on
        // each access. Very inefficient, so just set value to None to enable GC.
        emit_native_load_const_tok(emit, MP_TOKEN_KW_NONE);
        emit_native_store_fast(emit, qst, local_num);
    } else {
        // TODO implement me!
    }
}

STATIC void emit_native_delete_global(emit_t *emit, qstr qst, int kind) {
    MP_STATIC_ASSERT(MP_F_DELETE_NAME + MP_EMIT_IDOP_GLOBAL_NAME == MP_F_DELETE_NAME);
    MP_STATIC_ASSERT(MP_F_DELETE_NAME + MP_EMIT_IDOP_GLOBAL_GLOBAL == MP_F_DELETE_GLOBAL);
    emit_native_pre(emit);
    emit_call_with_imm_arg(emit, MP_F_DELETE_NAME + kind, qst, REG_ARG_1);
    emit_post(emit);
}

STATIC void emit_native_delete_attr(emit_t *emit, qstr qst) {
    vtype_kind_t vtype_base;
    emit_pre_pop_reg(emit, &vtype_base, REG_ARG_1); // arg1 = base
    assert(vtype_base == VTYPE_PYOBJ);
    emit_call_with_2_imm_args(emit, MP_F_STORE_ATTR, qst, REG_ARG_2, (mp_uint_t)MP_OBJ_NULL, REG_ARG_3); // arg2 = attribute name, arg3 = value (null for delete)
    emit_post(emit);
}

STATIC void emit_native_delete_subscr(emit_t *emit) {
    vtype_kind_t vtype_index, vtype_base;
    emit_pre_pop_reg_reg(emit, &vtype_index, REG_ARG_2, &vtype_base, REG_ARG_1); // index, base
    assert(vtype_index == VTYPE_PYOBJ);
    assert(vtype_base == VTYPE_PYOBJ);
    emit_call_with_imm_arg(emit, MP_F_OBJ_SUBSCR, (mp_uint_t)MP_OBJ_NULL, REG_ARG_3);
}

STATIC void emit_native_subscr(emit_t *emit, int kind) {
    if (kind == MP_EMIT_SUBSCR_LOAD) {
        emit_native_load_subscr(emit);
    } else if (kind == MP_EMIT_SUBSCR_STORE) {
        emit_native_store_subscr(emit);
    } else {
        emit_native_delete_subscr(emit);
    }
}

STATIC void emit_native_attr(emit_t *emit, qstr qst, int kind) {
    if (kind == MP_EMIT_ATTR_LOAD) {
        emit_native_load_attr(emit, qst);
    } else if (kind == MP_EMIT_ATTR_STORE) {
        emit_native_store_attr(emit, qst);
    } else {
        emit_native_delete_attr(emit, qst);
    }
}

STATIC void emit_native_dup_top(emit_t *emit) {
    DEBUG_printf("dup_top\n");
    vtype_kind_t vtype;
    int reg = REG_TEMP0;
    emit_pre_pop_reg_flexible(emit, &vtype, &reg, -1, -1);
    emit_post_push_reg_reg(emit, vtype, reg, vtype, reg);
}

STATIC void emit_native_dup_top_two(emit_t *emit) {
    vtype_kind_t vtype0, vtype1;
    emit_pre_pop_reg_reg(emit, &vtype0, REG_TEMP0, &vtype1, REG_TEMP1);
    emit_post_push_reg_reg_reg_reg(emit, vtype1, REG_TEMP1, vtype0, REG_TEMP0, vtype1, REG_TEMP1, vtype0, REG_TEMP0);
}

STATIC void emit_native_pop_top(emit_t *emit) {
    DEBUG_printf("pop_top\n");
    emit_pre_pop_discard(emit);
    emit_post(emit);
}

STATIC void emit_native_rot_two(emit_t *emit) {
    DEBUG_printf("rot_two\n");
    vtype_kind_t vtype0, vtype1;
    emit_pre_pop_reg_reg(emit, &vtype0, REG_TEMP0, &vtype1, REG_TEMP1);
    emit_post_push_reg_reg(emit, vtype0, REG_TEMP0, vtype1, REG_TEMP1);
}

STATIC void emit_native_rot_three(emit_t *emit) {
    DEBUG_printf("rot_three\n");
    vtype_kind_t vtype0, vtype1, vtype2;
    emit_pre_pop_reg_reg_reg(emit, &vtype0, REG_TEMP0, &vtype1, REG_TEMP1, &vtype2, REG_TEMP2);
    emit_post_push_reg_reg_reg(emit, vtype0, REG_TEMP0, vtype2, REG_TEMP2, vtype1, REG_TEMP1);
}

STATIC void emit_native_jump(emit_t *emit, mp_uint_t label) {
    DEBUG_printf("jump(label=" UINT_FMT ")\n", label);
    emit_native_pre(emit);
    // need to commit stack because we are jumping elsewhere
    need_stack_settled(emit);
    ASM_JUMP(emit->as, label);
    emit_post(emit);
}

STATIC void emit_native_jump_helper(emit_t *emit, bool cond, mp_uint_t label, bool pop) {
    vtype_kind_t vtype = peek_vtype(emit, 0);
    if (vtype == VTYPE_PYOBJ) {
        emit_pre_pop_reg(emit, &vtype, REG_ARG_1);
        if (!pop) {
            adjust_stack(emit, 1);
        }
        emit_call(emit, MP_F_OBJ_IS_TRUE);
    } else {
        emit_pre_pop_reg(emit, &vtype, REG_RET);
        if (!pop) {
            adjust_stack(emit, 1);
        }
        if (!(vtype == VTYPE_BOOL || vtype == VTYPE_INT || vtype == VTYPE_UINT)) {
            EMIT_NATIVE_VIPER_TYPE_ERROR(emit,
                "can't implicitly convert '%q' to 'bool'", vtype_to_qstr(vtype));
        }
    }
    // For non-pop need to save the vtype so that emit_native_adjust_stack_size
    // can use it.  This is a bit of a hack.
    if (!pop) {
        emit->saved_stack_vtype = vtype;
    }
    // need to commit stack because we may jump elsewhere
    need_stack_settled(emit);
    // Emit the jump
    if (cond) {
        ASM_JUMP_IF_REG_NONZERO(emit->as, REG_RET, label, vtype == VTYPE_PYOBJ);
    } else {
        ASM_JUMP_IF_REG_ZERO(emit->as, REG_RET, label, vtype == VTYPE_PYOBJ);
    }
    if (!pop) {
        adjust_stack(emit, -1);
    }
    emit_post(emit);
}

STATIC void emit_native_pop_jump_if(emit_t *emit, bool cond, mp_uint_t label) {
    DEBUG_printf("pop_jump_if(cond=%u, label=" UINT_FMT ")\n", cond, label);
    emit_native_jump_helper(emit, cond, label, true);
}

STATIC void emit_native_jump_if_or_pop(emit_t *emit, bool cond, mp_uint_t label) {
    DEBUG_printf("jump_if_or_pop(cond=%u, label=" UINT_FMT ")\n", cond, label);
    emit_native_jump_helper(emit, cond, label, false);
}

STATIC void emit_native_unwind_jump(emit_t *emit, mp_uint_t label, mp_uint_t except_depth) {
    (void)except_depth;
    emit_native_jump(emit, label & ~MP_EMIT_BREAK_FROM_FOR); // TODO properly
}

STATIC void emit_native_setup_with(emit_t *emit, mp_uint_t label) {
    // the context manager is on the top of the stack
    // stack: (..., ctx_mgr)

    // get __exit__ method
    vtype_kind_t vtype;
    emit_access_stack(emit, 1, &vtype, REG_ARG_1); // arg1 = ctx_mgr
    assert(vtype == VTYPE_PYOBJ);
    emit_get_stack_pointer_to_reg_for_push(emit, REG_ARG_3, 2); // arg3 = dest ptr
    emit_call_with_imm_arg(emit, MP_F_LOAD_METHOD, MP_QSTR___exit__, REG_ARG_2);
    // stack: (..., ctx_mgr, __exit__, self)

    emit_pre_pop_reg(emit, &vtype, REG_ARG_3); // self
    emit_pre_pop_reg(emit, &vtype, REG_ARG_2); // __exit__
    emit_pre_pop_reg(emit, &vtype, REG_ARG_1); // ctx_mgr
    emit_post_push_reg(emit, vtype, REG_ARG_2); // __exit__
    emit_post_push_reg(emit, vtype, REG_ARG_3); // self
    // stack: (..., __exit__, self)
    // REG_ARG_1=ctx_mgr

    // get __enter__ method
    emit_get_stack_pointer_to_reg_for_push(emit, REG_ARG_3, 2); // arg3 = dest ptr
    emit_call_with_imm_arg(emit, MP_F_LOAD_METHOD, MP_QSTR___enter__, REG_ARG_2); // arg2 = method name
    // stack: (..., __exit__, self, __enter__, self)

    // call __enter__ method
    emit_get_stack_pointer_to_reg_for_pop(emit, REG_ARG_3, 2); // pointer to items, including meth and self
    emit_call_with_2_imm_args(emit, MP_F_CALL_METHOD_N_KW, 0, REG_ARG_1, 0, REG_ARG_2);
    emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET); // push return value of __enter__
    // stack: (..., __exit__, self, as_value)

    // need to commit stack because we may jump elsewhere
    need_stack_settled(emit);
    emit_get_stack_pointer_to_reg_for_push(emit, REG_ARG_1, sizeof(nlr_buf_t) / sizeof(mp_uint_t)); // arg1 = pointer to nlr buf
    emit_call(emit, MP_F_NLR_PUSH);
    ASM_JUMP_IF_REG_NONZERO(emit->as, REG_RET, label, true);

    emit_access_stack(emit, sizeof(nlr_buf_t) / sizeof(mp_uint_t) + 1, &vtype, REG_RET); // access return value of __enter__
    emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET); // push return value of __enter__
    // stack: (..., __exit__, self, as_value, nlr_buf, as_value)
}

STATIC void emit_native_setup_block(emit_t *emit, mp_uint_t label, int kind) {
    if (kind == MP_EMIT_SETUP_BLOCK_WITH) {
        emit_native_setup_with(emit, label);
    } else {
        // Set up except and finally
        emit_native_pre(emit);
        // need to commit stack because we may jump elsewhere
        need_stack_settled(emit);
        emit_get_stack_pointer_to_reg_for_push(emit, REG_ARG_1, sizeof(nlr_buf_t) / sizeof(mp_uint_t)); // arg1 = pointer to nlr buf
        emit_call(emit, MP_F_NLR_PUSH);
        ASM_JUMP_IF_REG_NONZERO(emit->as, REG_RET, label, true);
        emit_post(emit);
    }
}

STATIC void emit_native_with_cleanup(emit_t *emit, mp_uint_t label) {
    // note: label+1 is available as an auxiliary label

    // stack: (..., __exit__, self, as_value, nlr_buf)
    emit_native_pre(emit);
    emit_call(emit, MP_F_NLR_POP);
    adjust_stack(emit, -(mp_int_t)(sizeof(nlr_buf_t) / sizeof(mp_uint_t)) - 1);
    // stack: (..., __exit__, self)

    // call __exit__
    emit_post_push_imm(emit, VTYPE_PYOBJ, (mp_uint_t)mp_const_none);
    emit_post_push_imm(emit, VTYPE_PYOBJ, (mp_uint_t)mp_const_none);
    emit_post_push_imm(emit, VTYPE_PYOBJ, (mp_uint_t)mp_const_none);
    emit_get_stack_pointer_to_reg_for_pop(emit, REG_ARG_3, 5);
    emit_call_with_2_imm_args(emit, MP_F_CALL_METHOD_N_KW, 3, REG_ARG_1, 0, REG_ARG_2);

    // jump to after with cleanup nlr_catch block
    adjust_stack(emit, 1); // dummy nlr_buf.prev
    emit_native_load_const_tok(emit, MP_TOKEN_KW_NONE); // nlr_buf.ret_val = no exception
    emit_native_jump(emit, label + 1);

    // nlr_catch
    emit_native_label_assign(emit, label);

    // adjust stack counter for: __exit__, self, as_value
    adjust_stack(emit, 3);
    // stack: (..., __exit__, self, as_value, nlr_buf.prev, nlr_buf.ret_val)

    vtype_kind_t vtype;
    emit_pre_pop_reg(emit, &vtype, REG_ARG_1); // get the thrown value (exc)
    adjust_stack(emit, -2); // discard nlr_buf.prev and as_value
    // stack: (..., __exit__, self)
    // REG_ARG_1=exc

    emit_pre_pop_reg(emit, &vtype, REG_ARG_2); // self
    emit_pre_pop_reg(emit, &vtype, REG_ARG_3); // __exit__
    adjust_stack(emit, 1); // dummy nlr_buf.prev
    emit_post_push_reg(emit, vtype, REG_ARG_1); // push exc to save it for later
    emit_post_push_reg(emit, vtype, REG_ARG_3); // __exit__
    emit_post_push_reg(emit, vtype, REG_ARG_2); // self
    // stack: (..., exc, __exit__, self)
    // REG_ARG_1=exc

    ASM_LOAD_REG_REG_OFFSET(emit->as, REG_ARG_2, REG_ARG_1, 0); // get type(exc)
    emit_post_push_reg(emit, VTYPE_PYOBJ, REG_ARG_2); // push type(exc)
    emit_post_push_reg(emit, VTYPE_PYOBJ, REG_ARG_1); // push exc value
    emit_post_push_imm(emit, VTYPE_PYOBJ, (mp_uint_t)mp_const_none); // traceback info
    // stack: (..., exc, __exit__, self, type(exc), exc, traceback)

    // call __exit__ method
    emit_get_stack_pointer_to_reg_for_pop(emit, REG_ARG_3, 5);
    emit_call_with_2_imm_args(emit, MP_F_CALL_METHOD_N_KW, 3, REG_ARG_1, 0, REG_ARG_2);
    // stack: (..., exc)

    // if REG_RET is true then we need to replace top-of-stack with None (swallow exception)
    if (REG_ARG_1 != REG_RET) {
        ASM_MOV_REG_REG(emit->as, REG_ARG_1, REG_RET);
    }
    emit_call(emit, MP_F_OBJ_IS_TRUE);
    ASM_JUMP_IF_REG_ZERO(emit->as, REG_RET, label + 1, true);

    // replace exc with None
    emit_pre_pop_discard(emit);
    emit_post_push_imm(emit, VTYPE_PYOBJ, (mp_uint_t)mp_const_none);

    // end of with cleanup nlr_catch block
    emit_native_label_assign(emit, label + 1);
}

STATIC void emit_native_end_finally(emit_t *emit) {
    // logic:
    //   exc = pop_stack
    //   if exc == None: pass
    //   else: raise exc
    // the check if exc is None is done in the MP_F_NATIVE_RAISE stub
    vtype_kind_t vtype;
    emit_pre_pop_reg(emit, &vtype, REG_ARG_1); // get nlr_buf.ret_val
    emit_pre_pop_discard(emit); // discard nlr_buf.prev
    emit_call(emit, MP_F_NATIVE_RAISE);
    emit_post(emit);
}

STATIC void emit_native_get_iter(emit_t *emit, bool use_stack) {
    // perhaps the difficult one, as we want to rewrite for loops using native code
    // in cases where we iterate over a Python object, can we use normal runtime calls?

    vtype_kind_t vtype;
    emit_pre_pop_reg(emit, &vtype, REG_ARG_1);
    assert(vtype == VTYPE_PYOBJ);
    if (use_stack) {
        emit_get_stack_pointer_to_reg_for_push(emit, REG_ARG_2, MP_OBJ_ITER_BUF_NSLOTS);
        emit_call(emit, MP_F_NATIVE_GETITER);
    } else {
        // mp_getiter will allocate the iter_buf on the heap
        ASM_MOV_REG_IMM(emit->as, REG_ARG_2, 0);
        emit_call(emit, MP_F_NATIVE_GETITER);
        emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
    }
}

STATIC void emit_native_for_iter(emit_t *emit, mp_uint_t label) {
    emit_native_pre(emit);
    emit_get_stack_pointer_to_reg_for_pop(emit, REG_ARG_1, MP_OBJ_ITER_BUF_NSLOTS);
    adjust_stack(emit, MP_OBJ_ITER_BUF_NSLOTS);
    emit_call(emit, MP_F_NATIVE_ITERNEXT);
    #ifdef NDEBUG
    MP_STATIC_ASSERT(MP_OBJ_STOP_ITERATION == 0);
    ASM_JUMP_IF_REG_ZERO(emit->as, REG_RET, label, false);
    #else
    ASM_MOV_REG_IMM(emit->as, REG_TEMP1, (mp_uint_t)MP_OBJ_STOP_ITERATION);
    ASM_JUMP_IF_REG_EQ(emit->as, REG_RET, REG_TEMP1, label);
    #endif
    emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
}

STATIC void emit_native_for_iter_end(emit_t *emit) {
    // adjust stack counter (we get here from for_iter ending, which popped the value for us)
    emit_native_pre(emit);
    adjust_stack(emit, -MP_OBJ_ITER_BUF_NSLOTS);
    emit_post(emit);
}

STATIC void emit_native_pop_block(emit_t *emit) {
    emit_native_pre(emit);
    emit_call(emit, MP_F_NLR_POP);
    adjust_stack(emit, -(mp_int_t)(sizeof(nlr_buf_t) / sizeof(mp_uint_t)) + 1);
    emit_post(emit);
}

STATIC void emit_native_pop_except(emit_t *emit) {
    (void)emit;
}

STATIC void emit_native_unary_op(emit_t *emit, mp_unary_op_t op) {
    vtype_kind_t vtype;
    emit_pre_pop_reg(emit, &vtype, REG_ARG_2);
    if (vtype == VTYPE_PYOBJ) {
        emit_call_with_imm_arg(emit, MP_F_UNARY_OP, op, REG_ARG_1);
        emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
    } else {
        adjust_stack(emit, 1);
        EMIT_NATIVE_VIPER_TYPE_ERROR(emit,
            "unary op %q not implemented", mp_unary_op_method_name[op]);
    }
}

STATIC void emit_native_binary_op(emit_t *emit, mp_binary_op_t op) {
    DEBUG_printf("binary_op(" UINT_FMT ")\n", op);
    vtype_kind_t vtype_lhs = peek_vtype(emit, 1);
    vtype_kind_t vtype_rhs = peek_vtype(emit, 0);
    if (vtype_lhs == VTYPE_INT && vtype_rhs == VTYPE_INT) {
        // for integers, inplace and normal ops are equivalent, so use just normal ops
        if (MP_BINARY_OP_INPLACE_OR <= op && op <= MP_BINARY_OP_INPLACE_POWER) {
            op += MP_BINARY_OP_OR - MP_BINARY_OP_INPLACE_OR;
        }

        #if N_X64 || N_X86
        // special cases for x86 and shifting
        if (op == MP_BINARY_OP_LSHIFT || op == MP_BINARY_OP_RSHIFT) {
            #if N_X64
            emit_pre_pop_reg_reg(emit, &vtype_rhs, ASM_X64_REG_RCX, &vtype_lhs, REG_RET);
            #else
            emit_pre_pop_reg_reg(emit, &vtype_rhs, ASM_X86_REG_ECX, &vtype_lhs, REG_RET);
            #endif
            if (op == MP_BINARY_OP_LSHIFT) {
                ASM_LSL_REG(emit->as, REG_RET);
            } else {
                ASM_ASR_REG(emit->as, REG_RET);
            }
            emit_post_push_reg(emit, VTYPE_INT, REG_RET);
            return;
        }
        #endif

        // special cases for floor-divide and module because we dispatch to helper functions
        if (op == MP_BINARY_OP_FLOOR_DIVIDE || op == MP_BINARY_OP_MODULO) {
            emit_pre_pop_reg_reg(emit, &vtype_rhs, REG_ARG_2, &vtype_lhs, REG_ARG_1);
            if (op == MP_BINARY_OP_FLOOR_DIVIDE) {
                emit_call(emit, MP_F_SMALL_INT_FLOOR_DIVIDE);
            } else {
                emit_call(emit, MP_F_SMALL_INT_MODULO);
            }
            emit_post_push_reg(emit, VTYPE_INT, REG_RET);
            return;
        }

        int reg_rhs = REG_ARG_3;
        emit_pre_pop_reg_flexible(emit, &vtype_rhs, &reg_rhs, REG_RET, REG_ARG_2);
        emit_pre_pop_reg(emit, &vtype_lhs, REG_ARG_2);
        if (0) {
            // dummy
        #if !(N_X64 || N_X86)
        } else if (op == MP_BINARY_OP_LSHIFT) {
            ASM_LSL_REG_REG(emit->as, REG_ARG_2, reg_rhs);
            emit_post_push_reg(emit, VTYPE_INT, REG_ARG_2);
        } else if (op == MP_BINARY_OP_RSHIFT) {
            ASM_ASR_REG_REG(emit->as, REG_ARG_2, reg_rhs);
            emit_post_push_reg(emit, VTYPE_INT, REG_ARG_2);
        #endif
        } else if (op == MP_BINARY_OP_OR) {
            ASM_OR_REG_REG(emit->as, REG_ARG_2, reg_rhs);
            emit_post_push_reg(emit, VTYPE_INT, REG_ARG_2);
        } else if (op == MP_BINARY_OP_XOR) {
            ASM_XOR_REG_REG(emit->as, REG_ARG_2, reg_rhs);
            emit_post_push_reg(emit, VTYPE_INT, REG_ARG_2);
        } else if (op == MP_BINARY_OP_AND) {
            ASM_AND_REG_REG(emit->as, REG_ARG_2, reg_rhs);
            emit_post_push_reg(emit, VTYPE_INT, REG_ARG_2);
        } else if (op == MP_BINARY_OP_ADD) {
            ASM_ADD_REG_REG(emit->as, REG_ARG_2, reg_rhs);
            emit_post_push_reg(emit, VTYPE_INT, REG_ARG_2);
        } else if (op == MP_BINARY_OP_SUBTRACT) {
            ASM_SUB_REG_REG(emit->as, REG_ARG_2, reg_rhs);
            emit_post_push_reg(emit, VTYPE_INT, REG_ARG_2);
        } else if (op == MP_BINARY_OP_MULTIPLY) {
            ASM_MUL_REG_REG(emit->as, REG_ARG_2, reg_rhs);
            emit_post_push_reg(emit, VTYPE_INT, REG_ARG_2);
        } else if (MP_BINARY_OP_LESS <= op && op <= MP_BINARY_OP_NOT_EQUAL) {
            // comparison ops are (in enum order):
            //  MP_BINARY_OP_LESS
            //  MP_BINARY_OP_MORE
            //  MP_BINARY_OP_EQUAL
            //  MP_BINARY_OP_LESS_EQUAL
            //  MP_BINARY_OP_MORE_EQUAL
            //  MP_BINARY_OP_NOT_EQUAL
            need_reg_single(emit, REG_RET, 0);
            #if N_X64
            asm_x64_xor_r64_r64(emit->as, REG_RET, REG_RET);
            asm_x64_cmp_r64_with_r64(emit->as, reg_rhs, REG_ARG_2);
            static byte ops[6] = {
                ASM_X64_CC_JL,
                ASM_X64_CC_JG,
                ASM_X64_CC_JE,
                ASM_X64_CC_JLE,
                ASM_X64_CC_JGE,
                ASM_X64_CC_JNE,
            };
            asm_x64_setcc_r8(emit->as, ops[op - MP_BINARY_OP_LESS], REG_RET);
            #elif N_X86
            asm_x86_xor_r32_r32(emit->as, REG_RET, REG_RET);
            asm_x86_cmp_r32_with_r32(emit->as, reg_rhs, REG_ARG_2);
            static byte ops[6] = {
                ASM_X86_CC_JL,
                ASM_X86_CC_JG,
                ASM_X86_CC_JE,
                ASM_X86_CC_JLE,
                ASM_X86_CC_JGE,
                ASM_X86_CC_JNE,
            };
            asm_x86_setcc_r8(emit->as, ops[op - MP_BINARY_OP_LESS], REG_RET);
            #elif N_THUMB
            asm_thumb_cmp_rlo_rlo(emit->as, REG_ARG_2, reg_rhs);
            static uint16_t ops[6] = {
                ASM_THUMB_OP_ITE_GE,
                ASM_THUMB_OP_ITE_GT,
                ASM_THUMB_OP_ITE_EQ,
                ASM_THUMB_OP_ITE_GT,
                ASM_THUMB_OP_ITE_GE,
                ASM_THUMB_OP_ITE_EQ,
            };
            static byte ret[6] = { 0, 1, 1, 0, 1, 0, };
            asm_thumb_op16(emit->as, ops[op - MP_BINARY_OP_LESS]);
            asm_thumb_mov_rlo_i8(emit->as, REG_RET, ret[op - MP_BINARY_OP_LESS]);
            asm_thumb_mov_rlo_i8(emit->as, REG_RET, ret[op - MP_BINARY_OP_LESS] ^ 1);
            #elif N_ARM
            asm_arm_cmp_reg_reg(emit->as, REG_ARG_2, reg_rhs);
            static uint ccs[6] = {
                ASM_ARM_CC_LT,
                ASM_ARM_CC_GT,
                ASM_ARM_CC_EQ,
                ASM_ARM_CC_LE,
                ASM_ARM_CC_GE,
                ASM_ARM_CC_NE,
            };
            asm_arm_setcc_reg(emit->as, REG_RET, ccs[op - MP_BINARY_OP_LESS]);
            #elif N_XTENSA
            static uint8_t ccs[6] = {
                ASM_XTENSA_CC_LT,
                0x80 | ASM_XTENSA_CC_LT, // for GT we'll swap args
                ASM_XTENSA_CC_EQ,
                0x80 | ASM_XTENSA_CC_GE, // for LE we'll swap args
                ASM_XTENSA_CC_GE,
                ASM_XTENSA_CC_NE,
            };
            uint8_t cc = ccs[op - MP_BINARY_OP_LESS];
            if ((cc & 0x80) == 0) {
                asm_xtensa_setcc_reg_reg_reg(emit->as, cc, REG_RET, REG_ARG_2, reg_rhs);
            } else {
                asm_xtensa_setcc_reg_reg_reg(emit->as, cc & ~0x80, REG_RET, reg_rhs, REG_ARG_2);
            }
            #else
                #error not implemented
            #endif
            emit_post_push_reg(emit, VTYPE_BOOL, REG_RET);
        } else {
            // TODO other ops not yet implemented
            adjust_stack(emit, 1);
            EMIT_NATIVE_VIPER_TYPE_ERROR(emit,
                "binary op %q not implemented", mp_binary_op_method_name[op]);
        }
    } else if (vtype_lhs == VTYPE_PYOBJ && vtype_rhs == VTYPE_PYOBJ) {
        emit_pre_pop_reg_reg(emit, &vtype_rhs, REG_ARG_3, &vtype_lhs, REG_ARG_2);
        bool invert = false;
        if (op == MP_BINARY_OP_NOT_IN) {
            invert = true;
            op = MP_BINARY_OP_IN;
        } else if (op == MP_BINARY_OP_IS_NOT) {
            invert = true;
            op = MP_BINARY_OP_IS;
        }
        emit_call_with_imm_arg(emit, MP_F_BINARY_OP, op, REG_ARG_1);
        if (invert) {
            ASM_MOV_REG_REG(emit->as, REG_ARG_2, REG_RET);
            emit_call_with_imm_arg(emit, MP_F_UNARY_OP, MP_UNARY_OP_NOT, REG_ARG_1);
        }
        emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
    } else {
        adjust_stack(emit, -1);
        EMIT_NATIVE_VIPER_TYPE_ERROR(emit,
            "can't do binary op between '%q' and '%q'",
            vtype_to_qstr(vtype_lhs), vtype_to_qstr(vtype_rhs));
    }
}

#if MICROPY_PY_BUILTINS_SLICE
STATIC void emit_native_build_slice(emit_t *emit, mp_uint_t n_args);
#endif

STATIC void emit_native_build(emit_t *emit, mp_uint_t n_args, int kind) {
    // for viper: call runtime, with types of args
    //   if wrapped in byte_array, or something, allocates memory and fills it
    MP_STATIC_ASSERT(MP_F_BUILD_TUPLE + MP_EMIT_BUILD_TUPLE == MP_F_BUILD_TUPLE);
    MP_STATIC_ASSERT(MP_F_BUILD_TUPLE + MP_EMIT_BUILD_LIST == MP_F_BUILD_LIST);
    MP_STATIC_ASSERT(MP_F_BUILD_TUPLE + MP_EMIT_BUILD_MAP == MP_F_BUILD_MAP);
    MP_STATIC_ASSERT(MP_F_BUILD_TUPLE + MP_EMIT_BUILD_SET == MP_F_BUILD_SET);
    #if MICROPY_PY_BUILTINS_SLICE
    if (kind == MP_EMIT_BUILD_SLICE) {
        emit_native_build_slice(emit, n_args);
        return;
    }
    #endif
    emit_native_pre(emit);
    if (kind == MP_EMIT_BUILD_TUPLE || kind == MP_EMIT_BUILD_LIST || kind == MP_EMIT_BUILD_SET) {
        emit_get_stack_pointer_to_reg_for_pop(emit, REG_ARG_2, n_args); // pointer to items
    }
    emit_call_with_imm_arg(emit, MP_F_BUILD_TUPLE + kind, n_args, REG_ARG_1);
    emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET); // new tuple/list/map/set
}

STATIC void emit_native_store_map(emit_t *emit) {
    vtype_kind_t vtype_key, vtype_value, vtype_map;
    emit_pre_pop_reg_reg_reg(emit, &vtype_key, REG_ARG_2, &vtype_value, REG_ARG_3, &vtype_map, REG_ARG_1); // key, value, map
    assert(vtype_key == VTYPE_PYOBJ);
    assert(vtype_value == VTYPE_PYOBJ);
    assert(vtype_map == VTYPE_PYOBJ);
    emit_call(emit, MP_F_STORE_MAP);
    emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET); // map
}

#if MICROPY_PY_BUILTINS_SLICE
STATIC void emit_native_build_slice(emit_t *emit, mp_uint_t n_args) {
    DEBUG_printf("build_slice %d\n", n_args);
    if (n_args == 2) {
        vtype_kind_t vtype_start, vtype_stop;
        emit_pre_pop_reg_reg(emit, &vtype_stop, REG_ARG_2, &vtype_start, REG_ARG_1); // arg1 = start, arg2 = stop
        assert(vtype_start == VTYPE_PYOBJ);
        assert(vtype_stop == VTYPE_PYOBJ);
        emit_call_with_imm_arg(emit, MP_F_NEW_SLICE, (mp_uint_t)mp_const_none, REG_ARG_3); // arg3 = step
        emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
    } else {
        assert(n_args == 3);
        vtype_kind_t vtype_start, vtype_stop, vtype_step;
        emit_pre_pop_reg_reg_reg(emit, &vtype_step, REG_ARG_3, &vtype_stop, REG_ARG_2, &vtype_start, REG_ARG_1); // arg1 = start, arg2 = stop, arg3 = step
        assert(vtype_start == VTYPE_PYOBJ);
        assert(vtype_stop == VTYPE_PYOBJ);
        assert(vtype_step == VTYPE_PYOBJ);
        emit_call(emit, MP_F_NEW_SLICE);
        emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
    }
}
#endif

STATIC void emit_native_store_comp(emit_t *emit, scope_kind_t kind, mp_uint_t collection_index) {
    mp_fun_kind_t f;
    if (kind == SCOPE_LIST_COMP) {
        vtype_kind_t vtype_item;
        emit_pre_pop_reg(emit, &vtype_item, REG_ARG_2);
        assert(vtype_item == VTYPE_PYOBJ);
        f = MP_F_LIST_APPEND;
    #if MICROPY_PY_BUILTINS_SET
    } else if (kind == SCOPE_SET_COMP) {
        vtype_kind_t vtype_item;
        emit_pre_pop_reg(emit, &vtype_item, REG_ARG_2);
        assert(vtype_item == VTYPE_PYOBJ);
        f = MP_F_STORE_SET;
    #endif
    } else {
        // SCOPE_DICT_COMP
        vtype_kind_t vtype_key, vtype_value;
        emit_pre_pop_reg_reg(emit, &vtype_key, REG_ARG_2, &vtype_value, REG_ARG_3);
        assert(vtype_key == VTYPE_PYOBJ);
        assert(vtype_value == VTYPE_PYOBJ);
        f = MP_F_STORE_MAP;
    }
    vtype_kind_t vtype_collection;
    emit_access_stack(emit, collection_index, &vtype_collection, REG_ARG_1);
    assert(vtype_collection == VTYPE_PYOBJ);
    emit_call(emit, f);
    emit_post(emit);
}

STATIC void emit_native_unpack_sequence(emit_t *emit, mp_uint_t n_args) {
    DEBUG_printf("unpack_sequence %d\n", n_args);
    vtype_kind_t vtype_base;
    emit_pre_pop_reg(emit, &vtype_base, REG_ARG_1); // arg1 = seq
    assert(vtype_base == VTYPE_PYOBJ);
    emit_get_stack_pointer_to_reg_for_push(emit, REG_ARG_3, n_args); // arg3 = dest ptr
    emit_call_with_imm_arg(emit, MP_F_UNPACK_SEQUENCE, n_args, REG_ARG_2); // arg2 = n_args
}

STATIC void emit_native_unpack_ex(emit_t *emit, mp_uint_t n_left, mp_uint_t n_right) {
    DEBUG_printf("unpack_ex %d %d\n", n_left, n_right);
    vtype_kind_t vtype_base;
    emit_pre_pop_reg(emit, &vtype_base, REG_ARG_1); // arg1 = seq
    assert(vtype_base == VTYPE_PYOBJ);
    emit_get_stack_pointer_to_reg_for_push(emit, REG_ARG_3, n_left + n_right + 1); // arg3 = dest ptr
    emit_call_with_imm_arg(emit, MP_F_UNPACK_EX, n_left | (n_right << 8), REG_ARG_2); // arg2 = n_left + n_right
}

STATIC void emit_native_make_function(emit_t *emit, scope_t *scope, mp_uint_t n_pos_defaults, mp_uint_t n_kw_defaults) {
    // call runtime, with type info for args, or don't support dict/default params, or only support Python objects for them
    emit_native_pre(emit);
    if (n_pos_defaults == 0 && n_kw_defaults == 0) {
        emit_call_with_3_imm_args_and_first_aligned(emit, MP_F_MAKE_FUNCTION_FROM_RAW_CODE, (mp_uint_t)scope->raw_code, REG_ARG_1, (mp_uint_t)MP_OBJ_NULL, REG_ARG_2, (mp_uint_t)MP_OBJ_NULL, REG_ARG_3);
    } else {
        vtype_kind_t vtype_def_tuple, vtype_def_dict;
        emit_pre_pop_reg_reg(emit, &vtype_def_dict, REG_ARG_3, &vtype_def_tuple, REG_ARG_2);
        assert(vtype_def_tuple == VTYPE_PYOBJ);
        assert(vtype_def_dict == VTYPE_PYOBJ);
        emit_call_with_imm_arg_aligned(emit, MP_F_MAKE_FUNCTION_FROM_RAW_CODE, (mp_uint_t)scope->raw_code, REG_ARG_1);
    }
    emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
}

STATIC void emit_native_make_closure(emit_t *emit, scope_t *scope, mp_uint_t n_closed_over, mp_uint_t n_pos_defaults, mp_uint_t n_kw_defaults) {
    emit_native_pre(emit);
    if (n_pos_defaults == 0 && n_kw_defaults == 0) {
        emit_get_stack_pointer_to_reg_for_pop(emit, REG_ARG_3, n_closed_over);
        ASM_MOV_REG_IMM(emit->as, REG_ARG_2, n_closed_over);
    } else {
        emit_get_stack_pointer_to_reg_for_pop(emit, REG_ARG_3, n_closed_over + 2);
        ASM_MOV_REG_IMM(emit->as, REG_ARG_2, 0x100 | n_closed_over);
    }
    ASM_MOV_REG_ALIGNED_IMM(emit->as, REG_ARG_1, (mp_uint_t)scope->raw_code);
    ASM_CALL_IND(emit->as, mp_fun_table[MP_F_MAKE_CLOSURE_FROM_RAW_CODE], MP_F_MAKE_CLOSURE_FROM_RAW_CODE);
    emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
}

STATIC void emit_native_call_function(emit_t *emit, mp_uint_t n_positional, mp_uint_t n_keyword, mp_uint_t star_flags) {
    DEBUG_printf("call_function(n_pos=" UINT_FMT ", n_kw=" UINT_FMT ", star_flags=" UINT_FMT ")\n", n_positional, n_keyword, star_flags);

    // TODO: in viper mode, call special runtime routine with type info for args,
    // and wanted type info for return, to remove need for boxing/unboxing

    emit_native_pre(emit);
    vtype_kind_t vtype_fun = peek_vtype(emit, n_positional + 2 * n_keyword);
    if (vtype_fun == VTYPE_BUILTIN_CAST) {
        // casting operator
        assert(n_positional == 1 && n_keyword == 0);
        assert(!star_flags);
        DEBUG_printf("  cast to %d\n", vtype_fun);
        vtype_kind_t vtype_cast = peek_stack(emit, 1)->data.u_imm;
        switch (peek_vtype(emit, 0)) {
            case VTYPE_PYOBJ: {
                vtype_kind_t vtype;
                emit_pre_pop_reg(emit, &vtype, REG_ARG_1);
                emit_pre_pop_discard(emit);
                emit_call_with_imm_arg(emit, MP_F_CONVERT_OBJ_TO_NATIVE, vtype_cast, REG_ARG_2); // arg2 = type
                emit_post_push_reg(emit, vtype_cast, REG_RET);
                break;
            }
            case VTYPE_BOOL:
            case VTYPE_INT:
            case VTYPE_UINT:
            case VTYPE_PTR:
            case VTYPE_PTR8:
            case VTYPE_PTR16:
            case VTYPE_PTR32:
            case VTYPE_PTR_NONE:
                emit_fold_stack_top(emit, REG_ARG_1);
                emit_post_top_set_vtype(emit, vtype_cast);
                break;
            default:
                // this can happen when casting a cast: int(int)
                mp_raise_NotImplementedError("casting");
        }
    } else {
        assert(vtype_fun == VTYPE_PYOBJ);
        if (star_flags) {
            emit_get_stack_pointer_to_reg_for_pop(emit, REG_ARG_3, n_positional + 2 * n_keyword + 3); // pointer to args
            emit_call_with_2_imm_args(emit, MP_F_CALL_METHOD_N_KW_VAR, 0, REG_ARG_1, n_positional | (n_keyword << 8), REG_ARG_2);
            emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
        } else {
            if (n_positional != 0 || n_keyword != 0) {
                emit_get_stack_pointer_to_reg_for_pop(emit, REG_ARG_3, n_positional + 2 * n_keyword); // pointer to args
            }
            emit_pre_pop_reg(emit, &vtype_fun, REG_ARG_1); // the function
            emit_call_with_imm_arg(emit, MP_F_NATIVE_CALL_FUNCTION_N_KW, n_positional | (n_keyword << 8), REG_ARG_2);
            emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
        }
    }
}

STATIC void emit_native_call_method(emit_t *emit, mp_uint_t n_positional, mp_uint_t n_keyword, mp_uint_t star_flags) {
    if (star_flags) {
        emit_get_stack_pointer_to_reg_for_pop(emit, REG_ARG_3, n_positional + 2 * n_keyword + 4); // pointer to args
        emit_call_with_2_imm_args(emit, MP_F_CALL_METHOD_N_KW_VAR, 1, REG_ARG_1, n_positional | (n_keyword << 8), REG_ARG_2);
        emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
    } else {
        emit_native_pre(emit);
        emit_get_stack_pointer_to_reg_for_pop(emit, REG_ARG_3, 2 + n_positional + 2 * n_keyword); // pointer to items, including meth and self
        emit_call_with_2_imm_args(emit, MP_F_CALL_METHOD_N_KW, n_positional, REG_ARG_1, n_keyword, REG_ARG_2);
        emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
    }
}

STATIC void emit_native_return_value(emit_t *emit) {
    DEBUG_printf("return_value\n");
    if (emit->do_viper_types) {
        if (peek_vtype(emit, 0) == VTYPE_PTR_NONE) {
            emit_pre_pop_discard(emit);
            if (emit->return_vtype == VTYPE_PYOBJ) {
                ASM_MOV_REG_IMM(emit->as, REG_RET, (mp_uint_t)mp_const_none);
            } else {
                ASM_MOV_REG_IMM(emit->as, REG_RET, 0);
            }
        } else {
            vtype_kind_t vtype;
            emit_pre_pop_reg(emit, &vtype, REG_RET);
            if (vtype != emit->return_vtype) {
                EMIT_NATIVE_VIPER_TYPE_ERROR(emit,
                    "return expected '%q' but got '%q'",
                    vtype_to_qstr(emit->return_vtype), vtype_to_qstr(vtype));
            }
        }
    } else {
        vtype_kind_t vtype;
        emit_pre_pop_reg(emit, &vtype, REG_RET);
        assert(vtype == VTYPE_PYOBJ);
    }
    emit->last_emit_was_return_value = true;
    ASM_EXIT(emit->as);
}

STATIC void emit_native_raise_varargs(emit_t *emit, mp_uint_t n_args) {
    assert(n_args == 1);
    vtype_kind_t vtype_exc;
    emit_pre_pop_reg(emit, &vtype_exc, REG_ARG_1); // arg1 = object to raise
    if (vtype_exc != VTYPE_PYOBJ) {
        EMIT_NATIVE_VIPER_TYPE_ERROR(emit, "must raise an object");
    }
    // TODO probably make this 1 call to the runtime (which could even call convert, native_raise(obj, type))
    emit_call(emit, MP_F_NATIVE_RAISE);
}

STATIC void emit_native_yield(emit_t *emit, int kind) {
    // not supported (for now)
    (void)emit;
    (void)kind;
    mp_raise_NotImplementedError("native yield");
}

STATIC void emit_native_start_except_handler(emit_t *emit) {
    // This instruction follows a pop_block call, so the stack counter is up by one when really
    // it should be up by a whole nlr_buf_t.  We then want to pop the nlr_buf_t here, but save
    // the first 2 elements, so we can get the thrown value.
    adjust_stack(emit, 1);
}

STATIC void emit_native_end_except_handler(emit_t *emit) {
    (void)emit;
}

const emit_method_table_t EXPORT_FUN(method_table) = {
    emit_native_set_native_type,
    emit_native_start_pass,
    emit_native_end_pass,
    emit_native_last_emit_was_return_value,
    emit_native_adjust_stack_size,
    emit_native_set_source_line,

    {
        emit_native_load_local,
        emit_native_load_global,
    },
    {
        emit_native_store_local,
        emit_native_store_global,
    },
    {
        emit_native_delete_local,
        emit_native_delete_global,
    },

    emit_native_label_assign,
    emit_native_import,
    emit_native_load_const_tok,
    emit_native_load_const_small_int,
    emit_native_load_const_str,
    emit_native_load_const_obj,
    emit_native_load_null,
    emit_native_load_method,
    emit_native_load_build_class,
    emit_native_subscr,
    emit_native_attr,
    emit_native_dup_top,
    emit_native_dup_top_two,
    emit_native_pop_top,
    emit_native_rot_two,
    emit_native_rot_three,
    emit_native_jump,
    emit_native_pop_jump_if,
    emit_native_jump_if_or_pop,
    emit_native_unwind_jump,
    emit_native_setup_block,
    emit_native_with_cleanup,
    emit_native_end_finally,
    emit_native_get_iter,
    emit_native_for_iter,
    emit_native_for_iter_end,
    emit_native_pop_block,
    emit_native_pop_except,
    emit_native_unary_op,
    emit_native_binary_op,
    emit_native_build,
    emit_native_store_map,
    emit_native_store_comp,
    emit_native_unpack_sequence,
    emit_native_unpack_ex,
    emit_native_make_function,
    emit_native_make_closure,
    emit_native_call_function,
    emit_native_call_method,
    emit_native_return_value,
    emit_native_raise_varargs,
    emit_native_yield,

    emit_native_start_except_handler,
    emit_native_end_except_handler,
};

#endif
