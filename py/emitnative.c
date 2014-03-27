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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "lexer.h"
#include "parse.h"
#include "scope.h"
#include "runtime0.h"
#include "emit.h"
#include "emitglue.h"
#include "obj.h"
#include "runtime.h"

// wrapper around everything in this file
#if (MICROPY_EMIT_X64 && N_X64) || (MICROPY_EMIT_THUMB && N_THUMB)

#if N_X64

// x64 specific stuff

#include "asmx64.h"

#define REG_LOCAL_1 (REG_RBX)
#define REG_LOCAL_NUM (1)

#define EXPORT_FUN(name) emit_native_x64_##name

#define REG_TEMP0 (REG_RAX)
#define REG_TEMP1 (REG_RDI)
#define REG_TEMP2 (REG_RSI)
#define ASM_MOV_REG_TO_LOCAL(reg, local_num) asm_x64_mov_r64_to_local(emit->as, (reg), (local_num))
#define ASM_MOV_IMM_TO_REG(imm, reg) asm_x64_mov_i64_to_r64_optimised(emit->as, (imm), (reg))
#define ASM_MOV_IMM_TO_LOCAL_USING(imm, local_num, reg_temp) do { asm_x64_mov_i64_to_r64_optimised(emit->as, (imm), (reg_temp)); asm_x64_mov_r64_to_local(emit->as, (reg_temp), (local_num)); } while (false)
#define ASM_MOV_LOCAL_TO_REG(local_num, reg) asm_x64_mov_local_to_r64(emit->as, (local_num), (reg))
#define ASM_MOV_REG_TO_REG(reg_src, reg_dest) asm_x64_mov_r64_to_r64(emit->as, (reg_src), (reg_dest))
#define ASM_MOV_LOCAL_ADDR_TO_REG(local_num, reg) asm_x64_mov_local_addr_to_r64(emit->as, (local_num), (reg))

#elif N_THUMB

// thumb specific stuff

#include "asmthumb.h"

#define REG_LOCAL_1 (REG_R4)
#define REG_LOCAL_2 (REG_R5)
#define REG_LOCAL_3 (REG_R6)
#define REG_LOCAL_NUM (3)

#define EXPORT_FUN(name) emit_native_thumb_##name

#define REG_TEMP0 (REG_R0)
#define REG_TEMP1 (REG_R1)
#define REG_TEMP2 (REG_R2)
#define ASM_MOV_REG_TO_LOCAL(reg, local_num) asm_thumb_mov_local_reg(emit->as, (local_num), (reg))
#define ASM_MOV_IMM_TO_REG(imm, reg) asm_thumb_mov_reg_i32_optimised(emit->as, (reg), (imm))
#define ASM_MOV_IMM_TO_LOCAL_USING(imm, local_num, reg_temp) do { asm_thumb_mov_reg_i32_optimised(emit->as, (reg_temp), (imm)); asm_thumb_mov_local_reg(emit->as, (local_num), (reg_temp)); } while (false)
#define ASM_MOV_LOCAL_TO_REG(local_num, reg) asm_thumb_mov_reg_local(emit->as, (reg), (local_num))
#define ASM_MOV_REG_TO_REG(reg_src, reg_dest) asm_thumb_mov_reg_reg(emit->as, (reg_dest), (reg_src))
#define ASM_MOV_LOCAL_ADDR_TO_REG(local_num, reg) asm_thumb_mov_reg_local_addr(emit->as, (reg), (local_num))

#endif

typedef enum {
    STACK_VALUE,
    STACK_REG,
    STACK_IMM,
} stack_info_kind_t;

typedef enum {
    VTYPE_UNBOUND,
    VTYPE_PYOBJ,
    VTYPE_BOOL,
    VTYPE_INT,
    VTYPE_PTR,
    VTYPE_PTR_NONE,
    VTYPE_BUILTIN_V_INT,
} vtype_kind_t;

typedef struct _stack_info_t {
    vtype_kind_t vtype;
    stack_info_kind_t kind;
    union {
        int u_reg;
        machine_int_t u_imm;
    };
} stack_info_t;

struct _emit_t {
    int pass;

    bool do_viper_types;

    int local_vtype_alloc;
    vtype_kind_t *local_vtype;

    int stack_info_alloc;
    stack_info_t *stack_info;

    int stack_start;
    int stack_size;

    bool last_emit_was_return_value;

    scope_t *scope;

#if N_X64
    asm_x64_t *as;
#elif N_THUMB
    asm_thumb_t *as;
#endif
};

emit_t *EXPORT_FUN(new)(uint max_num_labels) {
    emit_t *emit = m_new(emit_t, 1);
    emit->do_viper_types = false;
    emit->local_vtype = NULL;
    emit->stack_info = NULL;
#if N_X64
    emit->as = asm_x64_new(max_num_labels);
#elif N_THUMB
    emit->as = asm_thumb_new(max_num_labels);
#endif
    return emit;
}

void EXPORT_FUN(free)(emit_t *emit) {
#if N_X64
    asm_x64_free(emit->as, false);
#elif N_THUMB
    asm_thumb_free(emit->as, false);
#endif
    m_del_obj(emit_t, emit);
}

STATIC void emit_native_set_viper_types(emit_t *emit, bool do_viper_types) {
    emit->do_viper_types = do_viper_types;
}

STATIC void emit_native_start_pass(emit_t *emit, pass_kind_t pass, scope_t *scope) {
    emit->pass = pass;
    emit->stack_start = 0;
    emit->stack_size = 0;
    emit->last_emit_was_return_value = false;
    emit->scope = scope;

    if (emit->local_vtype == NULL) {
        emit->local_vtype_alloc = scope->num_locals + 20; // XXX should be maximum over all scopes
        emit->local_vtype = m_new(vtype_kind_t, emit->local_vtype_alloc);
    }
    if (emit->stack_info == NULL) {
        emit->stack_info_alloc = scope->stack_size + 50; // XXX don't know stack size on entry, should be maximum over all scopes
        emit->stack_info = m_new(stack_info_t, emit->stack_info_alloc);
    }

    if (emit->do_viper_types) {
        // TODO set types of arguments based on type signature
        for (int i = 0; i < emit->local_vtype_alloc; i++) {
            emit->local_vtype[i] = VTYPE_UNBOUND;
        }
        for (int i = 0; i < emit->stack_info_alloc; i++) {
            emit->stack_info[i].kind = STACK_VALUE;
            emit->stack_info[i].vtype = VTYPE_UNBOUND;
        }
    } else {
        for (int i = 0; i < emit->local_vtype_alloc; i++) {
            emit->local_vtype[i] = VTYPE_PYOBJ;
        }
        for (int i = 0; i < emit->stack_info_alloc; i++) {
            emit->stack_info[i].kind = STACK_VALUE;
            emit->stack_info[i].vtype = VTYPE_PYOBJ;
        }
    }

#if N_X64
    asm_x64_start_pass(emit->as, pass);
#elif N_THUMB
    asm_thumb_start_pass(emit->as, pass);
#endif

    // entry to function
    int num_locals = 0;
    if (pass > PASS_1) {
        num_locals = scope->num_locals - REG_LOCAL_NUM;
        if (num_locals < 0) {
            num_locals = 0;
        }
        emit->stack_start = num_locals;
        num_locals += scope->stack_size;
    }
    if (pass == PASS_2) {
        // XXX big hack to make sure we have some locals in PASS_2
        // this is so that on PASS_2 the code emitted in x64 has the right size
        num_locals += 2;
    }
#if N_X64
    asm_x64_entry(emit->as, num_locals);
#elif N_THUMB
    asm_thumb_entry(emit->as, num_locals);
#endif

    // initialise locals from parameters
#if N_X64
    for (int i = 0; i < scope->num_params; i++) {
        if (i == 0) {
            asm_x64_mov_r64_to_r64(emit->as, REG_ARG_1, REG_LOCAL_1);
        } else if (i == 1) {
            asm_x64_mov_r64_to_local(emit->as, REG_ARG_2, i - 1);
        } else if (i == 2) {
            asm_x64_mov_r64_to_local(emit->as, REG_ARG_3, i - 1);
        } else {
            // TODO not implemented
            assert(0);
        }
    }
#elif N_THUMB
    for (int i = 0; i < scope->num_params; i++) {
        if (i == 0) {
            asm_thumb_mov_reg_reg(emit->as, REG_LOCAL_1, REG_ARG_1);
        } else if (i == 1) {
            asm_thumb_mov_reg_reg(emit->as, REG_LOCAL_2, REG_ARG_2);
        } else if (i == 2) {
            asm_thumb_mov_reg_reg(emit->as, REG_LOCAL_3, REG_ARG_3);
        } else if (i == 3) {
            asm_thumb_mov_local_reg(emit->as, i - REG_LOCAL_NUM, REG_ARG_4);
        } else {
            // TODO not implemented
            assert(0);
        }
    }

    asm_thumb_mov_reg_i32(emit->as, REG_R7, (machine_uint_t)rt_fun_table);
#endif
}

STATIC void emit_native_end_pass(emit_t *emit) {
#if N_X64
    if (!emit->last_emit_was_return_value) {
        asm_x64_exit(emit->as);
    }
    asm_x64_end_pass(emit->as);
#elif N_THUMB
    if (!emit->last_emit_was_return_value) {
        asm_thumb_exit(emit->as);
    }
    asm_thumb_end_pass(emit->as);
#endif

    // check stack is back to zero size
    if (emit->stack_size != 0) {
        printf("ERROR: stack size not back to zero; got %d\n", emit->stack_size);
    }

    if (emit->pass == PASS_3) {
#if N_X64
        void *f = asm_x64_get_code(emit->as);
        mp_emit_glue_assign_native_code(emit->scope->unique_code_id, f, asm_x64_get_code_size(emit->as), emit->scope->num_params);
#elif N_THUMB
        void *f = asm_thumb_get_code(emit->as);
        mp_emit_glue_assign_native_code(emit->scope->unique_code_id, f, asm_thumb_get_code_size(emit->as), emit->scope->num_params);
#endif
    }
}

STATIC bool emit_native_last_emit_was_return_value(emit_t *emit) {
    return emit->last_emit_was_return_value;
}

STATIC int emit_native_get_stack_size(emit_t *emit) {
    return emit->stack_size;
}

STATIC void emit_native_set_stack_size(emit_t *emit, int size) {
    emit->stack_size = size;
}

STATIC void emit_native_set_source_line(emit_t *emit, int source_line) {
}

STATIC void adjust_stack(emit_t *emit, int stack_size_delta) {
    emit->stack_size += stack_size_delta;
    assert(emit->stack_size >= 0);
    if (emit->pass > PASS_1 && emit->stack_size > emit->scope->stack_size) {
        emit->scope->stack_size = emit->stack_size;
    }
}

/*
STATIC void emit_pre_raw(emit_t *emit, int stack_size_delta) {
    adjust_stack(emit, stack_size_delta);
    emit->last_emit_was_return_value = false;
}
*/

// this must be called at start of emit functions
STATIC void emit_native_pre(emit_t *emit) {
    emit->last_emit_was_return_value = false;
    // settle the stack
    /*
    if (regs_needed != 0) {
        for (int i = 0; i < emit->stack_size; i++) {
            switch (emit->stack_info[i].kind) {
                case STACK_VALUE:
                    break;

                case STACK_REG:
                    // TODO only push reg if in regs_needed
                    emit->stack_info[i].kind = STACK_VALUE;
                    ASM_MOV_REG_TO_LOCAL(emit->stack_info[i].u_reg, emit->stack_start + i);
                    break;

                case STACK_IMM:
                    // don't think we ever need to push imms for settling
                    //ASM_MOV_IMM_TO_LOCAL(emit->last_imm, emit->stack_start + i);
                    break;
            }
        }
    }
    */
}

STATIC vtype_kind_t peek_vtype(emit_t *emit) {
    return emit->stack_info[emit->stack_size - 1].vtype;
}

// pos=1 is TOS, pos=2 is next, etc
// use pos=0 for no skipping
STATIC void need_reg_single(emit_t *emit, int reg_needed, int skip_stack_pos) {
    skip_stack_pos = emit->stack_size - skip_stack_pos;
    for (int i = 0; i < emit->stack_size; i++) {
        if (i != skip_stack_pos) {
            stack_info_t *si = &emit->stack_info[i];
            if (si->kind == STACK_REG && si->u_reg == reg_needed) {
                si->kind = STACK_VALUE;
                ASM_MOV_REG_TO_LOCAL(si->u_reg, emit->stack_start + i);
            }
        }
    }
}

STATIC void need_reg_all(emit_t *emit) {
    for (int i = 0; i < emit->stack_size; i++) {
        stack_info_t *si = &emit->stack_info[i];
        if (si->kind == STACK_REG) {
            si->kind = STACK_VALUE;
            ASM_MOV_REG_TO_LOCAL(si->u_reg, emit->stack_start + i);
        }
    }
}

STATIC void need_stack_settled(emit_t *emit) {
    for (int i = 0; i < emit->stack_size; i++) {
        stack_info_t *si = &emit->stack_info[i];
        if (si->kind == STACK_REG) {
            si->kind = STACK_VALUE;
            ASM_MOV_REG_TO_LOCAL(si->u_reg, emit->stack_start + i);
        }
    }
    for (int i = 0; i < emit->stack_size; i++) {
        stack_info_t *si = &emit->stack_info[i];
        if (si->kind == STACK_IMM) {
            ASM_MOV_IMM_TO_LOCAL_USING(si->u_imm, emit->stack_start + i, REG_TEMP0);
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
            ASM_MOV_LOCAL_TO_REG(emit->stack_start + emit->stack_size - pos, reg_dest);
            break;

        case STACK_REG:
            if (si->u_reg != reg_dest) {
                ASM_MOV_REG_TO_REG(si->u_reg, reg_dest);
            }
            break;

        case STACK_IMM:
            ASM_MOV_IMM_TO_REG(si->u_imm, reg_dest);
            break;
    }
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
}

STATIC void emit_post_push_reg(emit_t *emit, vtype_kind_t vtype, int reg) {
    stack_info_t *si = &emit->stack_info[emit->stack_size];
    si->vtype = vtype;
    si->kind = STACK_REG;
    si->u_reg = reg;
    adjust_stack(emit, 1);
}

STATIC void emit_post_push_imm(emit_t *emit, vtype_kind_t vtype, machine_int_t imm) {
    stack_info_t *si = &emit->stack_info[emit->stack_size];
    si->vtype = vtype;
    si->kind = STACK_IMM;
    si->u_imm = imm;
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

// vtype of all n_pop objects is VTYPE_PYOBJ
// does not use any temporary registers (but may use reg_dest before loading it with stack pointer)
// TODO this needs some thinking for viper code
STATIC void emit_get_stack_pointer_to_reg_for_pop(emit_t *emit, int reg_dest, int n_pop) {
    need_reg_all(emit);
    for (int i = 0; i < n_pop; i++) {
        stack_info_t *si = &emit->stack_info[emit->stack_size - 1 - i];
        // must push any imm's to stack
        // must convert them to VTYPE_PYOBJ for viper code
        if (si->kind == STACK_IMM) {
            si->kind = STACK_VALUE;
            switch (si->vtype) {
                case VTYPE_PYOBJ:
                    ASM_MOV_IMM_TO_LOCAL_USING(si->u_imm, emit->stack_start + emit->stack_size - 1 - i, reg_dest);
                    break;
                case VTYPE_BOOL:
                    si->vtype = VTYPE_PYOBJ;
                    if (si->u_imm == 0) {
                        ASM_MOV_IMM_TO_LOCAL_USING((machine_uint_t)mp_const_false, emit->stack_start + emit->stack_size - 1 - i, reg_dest);
                    } else {
                        ASM_MOV_IMM_TO_LOCAL_USING((machine_uint_t)mp_const_true, emit->stack_start + emit->stack_size - 1 - i, reg_dest);
                    }
                    break;
                case VTYPE_INT:
                    si->vtype = VTYPE_PYOBJ;
                    ASM_MOV_IMM_TO_LOCAL_USING((si->u_imm << 1) | 1, emit->stack_start + emit->stack_size - 1 - i, reg_dest);
                    break;
                default:
                    // not handled
                    assert(0);
            }
        }
        assert(si->kind == STACK_VALUE);
        assert(si->vtype == VTYPE_PYOBJ);
    }
    adjust_stack(emit, -n_pop);
    ASM_MOV_LOCAL_ADDR_TO_REG(emit->stack_start + emit->stack_size, reg_dest);
}

// vtype of all n_push objects is VTYPE_PYOBJ
STATIC void emit_get_stack_pointer_to_reg_for_push(emit_t *emit, int reg_dest, int n_push) {
    need_reg_all(emit);
    for (int i = 0; i < n_push; i++) {
        emit->stack_info[emit->stack_size + i].kind = STACK_VALUE;
        emit->stack_info[emit->stack_size + i].vtype = VTYPE_PYOBJ;
    }
    ASM_MOV_LOCAL_ADDR_TO_REG(emit->stack_start + emit->stack_size, reg_dest);
    adjust_stack(emit, n_push);
}

STATIC void emit_call(emit_t *emit, rt_fun_kind_t fun_kind, void *fun) {
    need_reg_all(emit);
#if N_X64
    asm_x64_call_ind(emit->as, fun, REG_RAX);
#elif N_THUMB
    asm_thumb_bl_ind(emit->as, rt_fun_table[fun_kind], fun_kind, REG_R3);
#endif
}

STATIC void emit_call_with_imm_arg(emit_t *emit, rt_fun_kind_t fun_kind, void *fun, machine_int_t arg_val, int arg_reg) {
    need_reg_all(emit);
    ASM_MOV_IMM_TO_REG(arg_val, arg_reg);
#if N_X64
    asm_x64_call_ind(emit->as, fun, REG_RAX);
#elif N_THUMB
    asm_thumb_bl_ind(emit->as, rt_fun_table[fun_kind], fun_kind, REG_R3);
#endif
}

STATIC void emit_call_with_2_imm_args(emit_t *emit, rt_fun_kind_t fun_kind, void *fun, machine_int_t arg_val1, int arg_reg1, machine_int_t arg_val2, int arg_reg2) {
    need_reg_all(emit);
    ASM_MOV_IMM_TO_REG(arg_val1, arg_reg1);
    ASM_MOV_IMM_TO_REG(arg_val2, arg_reg2);
#if N_X64
    asm_x64_call_ind(emit->as, fun, REG_RAX);
#elif N_THUMB
    asm_thumb_bl_ind(emit->as, rt_fun_table[fun_kind], fun_kind, REG_R3);
#endif
}

STATIC void emit_native_load_id(emit_t *emit, qstr qstr) {
    // check for built-ins
    if (strcmp(qstr_str(qstr), "v_int") == 0) {
        assert(0);
        emit_native_pre(emit);
        //emit_post_push_blank(emit, VTYPE_BUILTIN_V_INT);

    // not a built-in, so do usual thing
    } else {
        emit_common_load_id(emit, &EXPORT_FUN(method_table), emit->scope, qstr);
    }
}

STATIC void emit_native_store_id(emit_t *emit, qstr qstr) {
    // TODO check for built-ins and disallow
    emit_common_store_id(emit, &EXPORT_FUN(method_table), emit->scope, qstr);
}

STATIC void emit_native_delete_id(emit_t *emit, qstr qstr) {
    // TODO check for built-ins and disallow
    emit_common_delete_id(emit, &EXPORT_FUN(method_table), emit->scope, qstr);
}

STATIC void emit_native_label_assign(emit_t *emit, int l) {
    emit_native_pre(emit);
    // need to commit stack because we can jump here from elsewhere
    need_stack_settled(emit);
#if N_X64
    asm_x64_label_assign(emit->as, l);
#elif N_THUMB
    asm_thumb_label_assign(emit->as, l);
#endif
    emit_post(emit);
}

STATIC void emit_native_import_name(emit_t *emit, qstr qstr) {
    // not implemented
    assert(0);
}

STATIC void emit_native_import_from(emit_t *emit, qstr qstr) {
    // not implemented
    assert(0);
}

STATIC void emit_native_import_star(emit_t *emit) {
    // not implemented
    assert(0);
}

STATIC void emit_native_load_const_tok(emit_t *emit, mp_token_kind_t tok) {
    emit_native_pre(emit);
    int vtype;
    machine_uint_t val;
    if (emit->do_viper_types) {
        switch (tok) {
            case MP_TOKEN_KW_NONE: vtype = VTYPE_PTR_NONE; val = 0; break;
            case MP_TOKEN_KW_FALSE: vtype = VTYPE_BOOL; val = 0; break;
            case MP_TOKEN_KW_TRUE: vtype = VTYPE_BOOL; val = 1; break;
            default: assert(0); vtype = 0; val = 0; // shouldn't happen
        }
    } else {
        vtype = VTYPE_PYOBJ;
        switch (tok) {
            case MP_TOKEN_KW_NONE: val = (machine_uint_t)mp_const_none; break;
            case MP_TOKEN_KW_FALSE: val = (machine_uint_t)mp_const_false; break;
            case MP_TOKEN_KW_TRUE: val = (machine_uint_t)mp_const_true; break;
            default: assert(0); vtype = 0; val = 0; // shouldn't happen
        }
    }
    emit_post_push_imm(emit, vtype, val);
}

STATIC void emit_native_load_const_small_int(emit_t *emit, machine_int_t arg) {
    emit_native_pre(emit);
    if (emit->do_viper_types) {
        emit_post_push_imm(emit, VTYPE_INT, arg);
    } else {
        emit_post_push_imm(emit, VTYPE_PYOBJ, (arg << 1) | 1);
    }
}

STATIC void emit_native_load_const_int(emit_t *emit, qstr qstr) {
    // not implemented
    // load integer, check fits in 32 bits
    assert(0);
}

STATIC void emit_native_load_const_dec(emit_t *emit, qstr qstr) {
    // for viper, a float/complex is just a Python object
    emit_native_pre(emit);
    emit_call_with_imm_arg(emit, RT_F_LOAD_CONST_DEC, rt_load_const_dec, qstr, REG_ARG_1);
    emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
}

STATIC void emit_native_load_const_id(emit_t *emit, qstr qstr) {
    emit_native_pre(emit);
    if (emit->do_viper_types) {
        assert(0);
    } else {
        emit_call_with_imm_arg(emit, RT_F_LOAD_CONST_STR, rt_load_const_str, qstr, REG_ARG_1); // TODO
        emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
    }
}

STATIC void emit_native_load_const_str(emit_t *emit, qstr qstr, bool bytes) {
    emit_native_pre(emit);
    if (emit->do_viper_types) {
        // not implemented properly
        // load a pointer to the asciiz string?
        assert(0);
        emit_post_push_imm(emit, VTYPE_PTR, (machine_uint_t)qstr_str(qstr));
    } else {
        emit_call_with_imm_arg(emit, RT_F_LOAD_CONST_STR, rt_load_const_str, qstr, REG_ARG_1);
        emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
    }
}

STATIC void emit_native_load_const_verbatim_str(emit_t *emit, const char *str) {
    // not supported/needed for viper
    assert(0);
}

STATIC void emit_native_load_fast(emit_t *emit, qstr qstr, int local_num) {
    vtype_kind_t vtype = emit->local_vtype[local_num];
    if (vtype == VTYPE_UNBOUND) {
        printf("ViperTypeError: local %s used before type known\n", qstr_str(qstr));
    }
    emit_native_pre(emit);
#if N_X64
    if (local_num == 0) {
        emit_post_push_reg(emit, vtype, REG_LOCAL_1);
    } else {
        need_reg_single(emit, REG_RAX, 0);
        asm_x64_mov_local_to_r64(emit->as, local_num - 1, REG_RAX);
        emit_post_push_reg(emit, vtype, REG_RAX);
    }
#elif N_THUMB
    if (local_num == 0) {
        emit_post_push_reg(emit, vtype, REG_LOCAL_1);
    } else if (local_num == 1) {
        emit_post_push_reg(emit, vtype, REG_LOCAL_2);
    } else if (local_num == 2) {
        emit_post_push_reg(emit, vtype, REG_LOCAL_3);
    } else {
        need_reg_single(emit, REG_R0, 0);
        asm_thumb_mov_reg_local(emit->as, REG_R0, local_num - 1);
        emit_post_push_reg(emit, vtype, REG_R0);
    }
#endif
}

STATIC void emit_native_load_deref(emit_t *emit, qstr qstr, int local_num) {
    // not implemented
    // in principle could support this quite easily (ldr r0, [r0, #0]) and then get closed over variables!
    assert(0);
}

STATIC void emit_native_load_closure(emit_t *emit, qstr qstr, int local_num) {
    // not implemented
    assert(0);
}

STATIC void emit_native_load_name(emit_t *emit, qstr qstr) {
    emit_native_pre(emit);
    emit_call_with_imm_arg(emit, RT_F_LOAD_NAME, rt_load_name, qstr, REG_ARG_1);
    emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
}

STATIC void emit_native_load_global(emit_t *emit, qstr qstr) {
    emit_native_pre(emit);
    emit_call_with_imm_arg(emit, RT_F_LOAD_GLOBAL, rt_load_global, qstr, REG_ARG_1);
    emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
}

STATIC void emit_native_load_attr(emit_t *emit, qstr qstr) {
    // depends on type of subject:
    //  - integer, function, pointer to integers: error
    //  - pointer to structure: get member, quite easy
    //  - Python object: call rt_load_attr, and needs to be typed to convert result
    vtype_kind_t vtype_base;
    emit_pre_pop_reg(emit, &vtype_base, REG_ARG_1); // arg1 = base
    assert(vtype_base == VTYPE_PYOBJ);
    emit_call_with_imm_arg(emit, RT_F_LOAD_ATTR, rt_load_attr, qstr, REG_ARG_2); // arg2 = attribute name
    emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
}

STATIC void emit_native_load_method(emit_t *emit, qstr qstr) {
    vtype_kind_t vtype_base;
    emit_pre_pop_reg(emit, &vtype_base, REG_ARG_1); // arg1 = base
    assert(vtype_base == VTYPE_PYOBJ);
    emit_get_stack_pointer_to_reg_for_push(emit, REG_ARG_3, 2); // arg3 = dest ptr
    emit_call_with_imm_arg(emit, RT_F_LOAD_METHOD, rt_load_method, qstr, REG_ARG_2); // arg2 = method name
}

STATIC void emit_native_load_build_class(emit_t *emit) {
    emit_native_pre(emit);
    emit_call(emit, RT_F_LOAD_BUILD_CLASS, rt_load_build_class);
    emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
}

STATIC void emit_native_store_fast(emit_t *emit, qstr qstr, int local_num) {
    vtype_kind_t vtype;
#if N_X64
    if (local_num == 0) {
        emit_pre_pop_reg(emit, &vtype, REG_LOCAL_1);
    } else {
        emit_pre_pop_reg(emit, &vtype, REG_RAX);
        asm_x64_mov_r64_to_local(emit->as, REG_RAX, local_num - 1);
    }
#elif N_THUMB
    if (local_num == 0) {
        emit_pre_pop_reg(emit, &vtype, REG_LOCAL_1);
    } else if (local_num == 1) {
        emit_pre_pop_reg(emit, &vtype, REG_LOCAL_2);
    } else if (local_num == 2) {
        emit_pre_pop_reg(emit, &vtype, REG_LOCAL_3);
    } else {
        emit_pre_pop_reg(emit, &vtype, REG_R0);
        asm_thumb_mov_local_reg(emit->as, local_num - 1, REG_R0);
    }
#endif

    emit_post(emit);

    // check types
    if (emit->local_vtype[local_num] == VTYPE_UNBOUND) {
        // first time this local is assigned, so give it a type of the object stored in it
        emit->local_vtype[local_num] = vtype;
    } else if (emit->local_vtype[local_num] != vtype) {
        // type of local is not the same as object stored in it
        printf("ViperTypeError: type mismatch, local %s has type %d but source object has type %d\n", qstr_str(qstr), emit->local_vtype[local_num], vtype);
    }
}

STATIC void emit_native_store_deref(emit_t *emit, qstr qstr, int local_num) {
    // not implemented
    assert(0);
}

STATIC void emit_native_store_name(emit_t *emit, qstr qstr) {
    // rt_store_name, but needs conversion of object (maybe have rt_viper_store_name(obj, type))
    vtype_kind_t vtype;
    emit_pre_pop_reg(emit, &vtype, REG_ARG_2);
    assert(vtype == VTYPE_PYOBJ);
    emit_call_with_imm_arg(emit, RT_F_STORE_NAME, rt_store_name, qstr, REG_ARG_1); // arg1 = name
    emit_post(emit);
}

STATIC void emit_native_store_global(emit_t *emit, qstr qstr) {
    // not implemented
    assert(0);
}

STATIC void emit_native_store_attr(emit_t *emit, qstr qstr) {
    vtype_kind_t vtype_base, vtype_val;
    emit_pre_pop_reg_reg(emit, &vtype_base, REG_ARG_1, &vtype_val, REG_ARG_3); // arg1 = base, arg3 = value
    assert(vtype_base == VTYPE_PYOBJ);
    assert(vtype_val == VTYPE_PYOBJ);
    emit_call_with_imm_arg(emit, RT_F_STORE_ATTR, rt_store_attr, qstr, REG_ARG_2); // arg2 = attribute name
    emit_post(emit);
}

STATIC void emit_native_store_subscr(emit_t *emit) {
    // depends on type of subject:
    //  - integer, function, pointer to structure: error
    //  - pointer to integers: store as per array
    //  - Python object: call runtime with converted object or type info
    vtype_kind_t vtype_index, vtype_base, vtype_value;
    emit_pre_pop_reg_reg_reg(emit, &vtype_index, REG_ARG_2, &vtype_base, REG_ARG_1, &vtype_value, REG_ARG_3); // index, base, value to store
    assert(vtype_index == VTYPE_PYOBJ);
    assert(vtype_base == VTYPE_PYOBJ);
    assert(vtype_value == VTYPE_PYOBJ);
    emit_call(emit, RT_F_STORE_SUBSCR, rt_store_subscr);
}

STATIC void emit_native_store_locals(emit_t *emit) {
    // not needed
    vtype_kind_t vtype;
    emit_pre_pop_reg(emit, &vtype, REG_TEMP0);
    emit_post(emit);
}

STATIC void emit_native_delete_fast(emit_t *emit, qstr qstr, int local_num) {
    // not implemented
    // could support for Python types, just set to None (so GC can reclaim it)
    assert(0);
}

STATIC void emit_native_delete_deref(emit_t *emit, qstr qstr, int local_num) {
    // not supported
    assert(0);
}

STATIC void emit_native_delete_name(emit_t *emit, qstr qstr) {
    // not implemented
    // use rt_delete_name
    assert(0);
}

STATIC void emit_native_delete_global(emit_t *emit, qstr qstr) {
    // not implemented
    // use rt_delete_global
    assert(0);
}

STATIC void emit_native_delete_attr(emit_t *emit, qstr qstr) {
    // not supported
    assert(0);
}

STATIC void emit_native_delete_subscr(emit_t *emit) {
    // not supported
    assert(0);
}

STATIC void emit_native_dup_top(emit_t *emit) {
    vtype_kind_t vtype;
    emit_pre_pop_reg(emit, &vtype, REG_TEMP0);
    emit_post_push_reg_reg(emit, vtype, REG_TEMP0, vtype, REG_TEMP0);
}

STATIC void emit_native_dup_top_two(emit_t *emit) {
    vtype_kind_t vtype0, vtype1;
    emit_pre_pop_reg_reg(emit, &vtype0, REG_TEMP0, &vtype1, REG_TEMP1);
    emit_post_push_reg_reg_reg_reg(emit, vtype1, REG_TEMP1, vtype0, REG_TEMP0, vtype1, REG_TEMP1, vtype0, REG_TEMP0);
}

STATIC void emit_native_pop_top(emit_t *emit) {
    vtype_kind_t vtype;
    emit_pre_pop_reg(emit, &vtype, REG_TEMP0);
    emit_post(emit);
}

STATIC void emit_native_rot_two(emit_t *emit) {
    vtype_kind_t vtype0, vtype1;
    emit_pre_pop_reg_reg(emit, &vtype0, REG_TEMP0, &vtype1, REG_TEMP1);
    emit_post_push_reg_reg(emit, vtype0, REG_TEMP0, vtype1, REG_TEMP1);
}

STATIC void emit_native_rot_three(emit_t *emit) {
    vtype_kind_t vtype0, vtype1, vtype2;
    emit_pre_pop_reg_reg_reg(emit, &vtype0, REG_TEMP0, &vtype1, REG_TEMP1, &vtype2, REG_TEMP2);
    emit_post_push_reg_reg_reg(emit, vtype0, REG_TEMP0, vtype2, REG_TEMP2, vtype1, REG_TEMP1);
}

STATIC void emit_native_jump(emit_t *emit, int label) {
    emit_native_pre(emit);
#if N_X64
    asm_x64_jmp_label(emit->as, label);
#elif N_THUMB
    asm_thumb_b_label(emit->as, label);
#endif
    emit_post(emit);
}

STATIC void emit_native_pop_jump_pre_helper(emit_t *emit, int label) {
    vtype_kind_t vtype = peek_vtype(emit);
    if (vtype == VTYPE_BOOL) {
        emit_pre_pop_reg(emit, &vtype, REG_RET);
    } else if (vtype == VTYPE_PYOBJ) {
        emit_pre_pop_reg(emit, &vtype, REG_ARG_1);
        emit_call(emit, RT_F_IS_TRUE, rt_is_true);
    } else {
        printf("ViperTypeError: expecting a bool or pyobj, got %d\n", vtype);
        assert(0);
    }
}

STATIC void emit_native_pop_jump_if_false(emit_t *emit, int label) {
    emit_native_pop_jump_pre_helper(emit, label);
#if N_X64
    asm_x64_test_r8_with_r8(emit->as, REG_RET, REG_RET);
    asm_x64_jcc_label(emit->as, JCC_JZ, label);
#elif N_THUMB
    asm_thumb_cmp_rlo_i8(emit->as, REG_RET, 0);
    asm_thumb_bcc_label(emit->as, THUMB_CC_EQ, label);
#endif
    emit_post(emit);
}

STATIC void emit_native_pop_jump_if_true(emit_t *emit, int label) {
    emit_native_pop_jump_pre_helper(emit, label);
#if N_X64
    asm_x64_test_r8_with_r8(emit->as, REG_RET, REG_RET);
    asm_x64_jcc_label(emit->as, JCC_JNZ, label);
#elif N_THUMB
    asm_thumb_cmp_rlo_i8(emit->as, REG_RET, 0);
    asm_thumb_bcc_label(emit->as, THUMB_CC_NE, label);
#endif
    emit_post(emit);
}

STATIC void emit_native_jump_if_true_or_pop(emit_t *emit, int label) {
    assert(0);
}
STATIC void emit_native_jump_if_false_or_pop(emit_t *emit, int label) {
    assert(0);
}

STATIC void emit_native_setup_loop(emit_t *emit, int label) {
    emit_native_pre(emit);
    emit_post(emit);
}

STATIC void emit_native_break_loop(emit_t *emit, int label, int except_depth) {
    emit_native_jump(emit, label); // TODO properly
}
STATIC void emit_native_continue_loop(emit_t *emit, int label, int except_depth) {
    assert(0);
}
STATIC void emit_native_setup_with(emit_t *emit, int label) {
    // not supported, or could be with runtime call
    assert(0);
}
STATIC void emit_native_with_cleanup(emit_t *emit) {
    assert(0);
}
STATIC void emit_native_setup_except(emit_t *emit, int label) {
    assert(0);
}
STATIC void emit_native_setup_finally(emit_t *emit, int label) {
    assert(0);
}
STATIC void emit_native_end_finally(emit_t *emit) {
    assert(0);
}

STATIC void emit_native_get_iter(emit_t *emit) {
    // perhaps the difficult one, as we want to rewrite for loops using native code
    // in cases where we iterate over a Python object, can we use normal runtime calls?

    vtype_kind_t vtype;
    emit_pre_pop_reg(emit, &vtype, REG_ARG_1);
    assert(vtype == VTYPE_PYOBJ);
    emit_call(emit, RT_F_GETITER, rt_getiter);
    emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
}

STATIC void emit_native_for_iter(emit_t *emit, int label) {
    emit_native_pre(emit);
    vtype_kind_t vtype;
    emit_access_stack(emit, 1, &vtype, REG_ARG_1);
    assert(vtype == VTYPE_PYOBJ);
    emit_call(emit, RT_F_ITERNEXT, rt_iternext);
    ASM_MOV_IMM_TO_REG((machine_uint_t)MP_OBJ_NULL, REG_TEMP1);
#if N_X64
    asm_x64_cmp_r64_with_r64(emit->as, REG_RET, REG_TEMP1);
    asm_x64_jcc_label(emit->as, JCC_JE, label);
#elif N_THUMB
    asm_thumb_cmp_reg_reg(emit->as, REG_RET, REG_TEMP1);
    asm_thumb_bcc_label(emit->as, THUMB_CC_EQ, label);
#endif
    emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
}

STATIC void emit_native_for_iter_end(emit_t *emit) {
    // adjust stack counter (we get here from for_iter ending, which popped the value for us)
    emit_native_pre(emit);
    adjust_stack(emit, -1);
    emit_post(emit);
}

STATIC void emit_native_pop_block(emit_t *emit) {
    emit_native_pre(emit);
    emit_post(emit);
}

STATIC void emit_native_pop_except(emit_t *emit) {
    assert(0);
}

STATIC void emit_native_unary_op(emit_t *emit, rt_unary_op_t op) {
    vtype_kind_t vtype;
    emit_pre_pop_reg(emit, &vtype, REG_ARG_2);
    assert(vtype == VTYPE_PYOBJ);
    emit_call_with_imm_arg(emit, RT_F_UNARY_OP, rt_unary_op, op, REG_ARG_1);
    emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
}

STATIC void emit_native_binary_op(emit_t *emit, rt_binary_op_t op) {
    vtype_kind_t vtype_lhs, vtype_rhs;
    emit_pre_pop_reg_reg(emit, &vtype_rhs, REG_ARG_3, &vtype_lhs, REG_ARG_2);
    if (vtype_lhs == VTYPE_INT && vtype_rhs == VTYPE_INT) {
        if (op == RT_BINARY_OP_ADD || op == RT_BINARY_OP_INPLACE_ADD) {
#if N_X64
            asm_x64_add_r64_to_r64(emit->as, REG_ARG_3, REG_ARG_2);
#elif N_THUMB
            asm_thumb_add_reg_reg_reg(emit->as, REG_ARG_2, REG_ARG_2, REG_ARG_3);
#endif
            emit_post_push_reg(emit, VTYPE_INT, REG_ARG_2);
        } else if (op == RT_BINARY_OP_LESS) {
#if N_X64
            asm_x64_xor_r64_to_r64(emit->as, REG_RET, REG_RET);
            asm_x64_cmp_r64_with_r64(emit->as, REG_ARG_3, REG_ARG_2);
            asm_x64_setcc_r8(emit->as, JCC_JL, REG_RET);
#elif N_THUMB
            asm_thumb_cmp_reg_reg(emit->as, REG_ARG_2, REG_ARG_3);
            asm_thumb_ite_ge(emit->as);
            asm_thumb_movs_rlo_i8(emit->as, REG_RET, 0); // if r0 >= r1
            asm_thumb_movs_rlo_i8(emit->as, REG_RET, 1); // if r0 < r1
#endif
            emit_post_push_reg(emit, VTYPE_BOOL, REG_RET);
        } else {
            // TODO other ops not yet implemented
            assert(0);
        }
    } else if (vtype_lhs == VTYPE_PYOBJ && vtype_rhs == VTYPE_PYOBJ) {
        emit_call_with_imm_arg(emit, RT_F_BINARY_OP, rt_binary_op, op, REG_ARG_1);
        emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
    } else {
        printf("ViperTypeError: can't do binary op between types %d and %d\n", vtype_lhs, vtype_rhs);
        assert(0);
    }
}

STATIC void emit_native_build_tuple(emit_t *emit, int n_args) {
    // for viper: call runtime, with types of args
    //   if wrapped in byte_array, or something, allocates memory and fills it
    emit_native_pre(emit);
    emit_get_stack_pointer_to_reg_for_pop(emit, REG_ARG_2, n_args); // pointer to items
    emit_call_with_imm_arg(emit, RT_F_BUILD_TUPLE, rt_build_tuple, n_args, REG_ARG_1);
    emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET); // new tuple
}

STATIC void emit_native_build_list(emit_t *emit, int n_args) {
    emit_native_pre(emit);
    emit_get_stack_pointer_to_reg_for_pop(emit, REG_ARG_2, n_args); // pointer to items
    emit_call_with_imm_arg(emit, RT_F_BUILD_LIST, rt_build_list, n_args, REG_ARG_1);
    emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET); // new list
}

STATIC void emit_native_list_append(emit_t *emit, int list_index) {
    // only used in list comprehension
    vtype_kind_t vtype_list, vtype_item;
    emit_pre_pop_reg(emit, &vtype_item, REG_ARG_2);
    emit_access_stack(emit, list_index, &vtype_list, REG_ARG_1);
    assert(vtype_list == VTYPE_PYOBJ);
    assert(vtype_item == VTYPE_PYOBJ);
    emit_call(emit, RT_F_LIST_APPEND, rt_list_append);
    emit_post(emit);
}

STATIC void emit_native_build_map(emit_t *emit, int n_args) {
    emit_native_pre(emit);
    emit_call_with_imm_arg(emit, RT_F_BUILD_MAP, rt_build_map, n_args, REG_ARG_1);
    emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET); // new map
}

STATIC void emit_native_store_map(emit_t *emit) {
    vtype_kind_t vtype_key, vtype_value, vtype_map;
    emit_pre_pop_reg_reg_reg(emit, &vtype_key, REG_ARG_2, &vtype_value, REG_ARG_3, &vtype_map, REG_ARG_1); // key, value, map
    assert(vtype_key == VTYPE_PYOBJ);
    assert(vtype_value == VTYPE_PYOBJ);
    assert(vtype_map == VTYPE_PYOBJ);
    emit_call(emit, RT_F_STORE_MAP, rt_store_map);
    emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET); // map
}

STATIC void emit_native_map_add(emit_t *emit, int map_index) {
    // only used in list comprehension
    vtype_kind_t vtype_map, vtype_key, vtype_value;
    emit_pre_pop_reg_reg(emit, &vtype_key, REG_ARG_2, &vtype_value, REG_ARG_3);
    emit_access_stack(emit, map_index, &vtype_map, REG_ARG_1);
    assert(vtype_map == VTYPE_PYOBJ);
    assert(vtype_key == VTYPE_PYOBJ);
    assert(vtype_value == VTYPE_PYOBJ);
    emit_call(emit, RT_F_STORE_MAP, rt_store_map);
    emit_post(emit);
}

STATIC void emit_native_build_set(emit_t *emit, int n_args) {
    emit_native_pre(emit);
    emit_get_stack_pointer_to_reg_for_pop(emit, REG_ARG_2, n_args); // pointer to items
    emit_call_with_imm_arg(emit, RT_F_BUILD_SET, rt_build_set, n_args, REG_ARG_1);
    emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET); // new set
}

STATIC void emit_native_set_add(emit_t *emit, int set_index) {
    // only used in set comprehension
    vtype_kind_t vtype_set, vtype_item;
    emit_pre_pop_reg(emit, &vtype_item, REG_ARG_2);
    emit_access_stack(emit, set_index, &vtype_set, REG_ARG_1);
    assert(vtype_set == VTYPE_PYOBJ);
    assert(vtype_item == VTYPE_PYOBJ);
    emit_call(emit, RT_F_STORE_SET, rt_store_set);
    emit_post(emit);
}

STATIC void emit_native_build_slice(emit_t *emit, int n_args) {
    assert(0);
}
STATIC void emit_native_unpack_sequence(emit_t *emit, int n_args) {
    // call runtime, needs type decl
    assert(0);
}
STATIC void emit_native_unpack_ex(emit_t *emit, int n_left, int n_right) {
    assert(0);
}

STATIC void emit_native_make_function(emit_t *emit, scope_t *scope, int n_dict_params, int n_default_params) {
    // call runtime, with type info for args, or don't support dict/default params, or only support Python objects for them
    assert(n_default_params == 0 && n_dict_params == 0);
    emit_native_pre(emit);
    emit_call_with_imm_arg(emit, RT_F_MAKE_FUNCTION_FROM_ID, rt_make_function_from_id, scope->unique_code_id, REG_ARG_1);
    emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
}

STATIC void emit_native_make_closure(emit_t *emit, scope_t *scope, int n_dict_params, int n_default_params) {
    assert(0);
}

STATIC void emit_native_call_function(emit_t *emit, int n_positional, int n_keyword, bool have_star_arg, bool have_dbl_star_arg) {
    // call special viper runtime routine with type info for args, and wanted type info for return
    assert(n_keyword == 0 && !have_star_arg && !have_dbl_star_arg);

    /* we no longer have these _n specific call_function's
     * they anyway push args into an array
     * and they would take too much room in the native dispatch table
    if (n_positional == 0) {
        vtype_kind_t vtype_fun;
        emit_pre_pop_reg(emit, &vtype_fun, REG_ARG_1); // the function
        assert(vtype_fun == VTYPE_PYOBJ);
        emit_call(emit, RT_F_CALL_FUNCTION_0, rt_call_function_0);
    } else if (n_positional == 1) {
        vtype_kind_t vtype_fun, vtype_arg1;
        emit_pre_pop_reg_reg(emit, &vtype_arg1, REG_ARG_2, &vtype_fun, REG_ARG_1); // the single argument, the function
        assert(vtype_fun == VTYPE_PYOBJ);
        assert(vtype_arg1 == VTYPE_PYOBJ);
        emit_call(emit, RT_F_CALL_FUNCTION_1, rt_call_function_1);
    } else if (n_positional == 2) {
        vtype_kind_t vtype_fun, vtype_arg1, vtype_arg2;
        emit_pre_pop_reg_reg_reg(emit, &vtype_arg2, REG_ARG_3, &vtype_arg1, REG_ARG_2, &vtype_fun, REG_ARG_1); // the second argument, the first argument, the function
        assert(vtype_fun == VTYPE_PYOBJ);
        assert(vtype_arg1 == VTYPE_PYOBJ);
        assert(vtype_arg2 == VTYPE_PYOBJ);
        emit_call(emit, RT_F_CALL_FUNCTION_2, rt_call_function_2);
    } else {
    */

    emit_native_pre(emit);
    if (n_positional != 0) {
        emit_get_stack_pointer_to_reg_for_pop(emit, REG_ARG_3, n_positional); // pointer to args
    }
    vtype_kind_t vtype_fun;
    emit_pre_pop_reg(emit, &vtype_fun, REG_ARG_1); // the function
    assert(vtype_fun == VTYPE_PYOBJ);
    emit_call_with_imm_arg(emit, RT_F_CALL_FUNCTION_N_KW_FOR_NATIVE, rt_call_function_n_kw_for_native, n_positional, REG_ARG_2);
    emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
}

STATIC void emit_native_call_method(emit_t *emit, int n_positional, int n_keyword, bool have_star_arg, bool have_dbl_star_arg) {
    assert(n_keyword == 0 && !have_star_arg && !have_dbl_star_arg);

    /*
    if (n_positional == 0) {
        vtype_kind_t vtype_meth, vtype_self;
        emit_pre_pop_reg_reg(emit, &vtype_self, REG_ARG_2, &vtype_meth, REG_ARG_1); // the self object (or NULL), the method
        assert(vtype_meth == VTYPE_PYOBJ);
        assert(vtype_self == VTYPE_PYOBJ);
        emit_call(emit, RT_F_CALL_METHOD_1, rt_call_method_1);
    } else if (n_positional == 1) {
        vtype_kind_t vtype_meth, vtype_self, vtype_arg1;
        emit_pre_pop_reg_reg_reg(emit, &vtype_arg1, REG_ARG_3, &vtype_self, REG_ARG_2, &vtype_meth, REG_ARG_1); // the first argument, the self object (or NULL), the method
        assert(vtype_meth == VTYPE_PYOBJ);
        assert(vtype_self == VTYPE_PYOBJ);
        assert(vtype_arg1 == VTYPE_PYOBJ);
        emit_call(emit, RT_F_CALL_METHOD_2, rt_call_method_2);
    } else {
    */

    emit_native_pre(emit);
    emit_get_stack_pointer_to_reg_for_pop(emit, REG_ARG_3, n_positional + 2); // pointer to items, including meth and self
    emit_call_with_2_imm_args(emit, RT_F_CALL_METHOD_N_KW, rt_call_method_n_kw, n_positional, REG_ARG_1, n_keyword, REG_ARG_2);
    emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
}

STATIC void emit_native_return_value(emit_t *emit) {
    // easy.  since we don't know who we return to, just return the raw value.
    // runtime needs then to know our type signature, but I think that's possible.
    vtype_kind_t vtype;
    emit_pre_pop_reg(emit, &vtype, REG_RET);
    if (emit->do_viper_types) {
        assert(vtype == VTYPE_PTR_NONE);
    } else {
        assert(vtype == VTYPE_PYOBJ);
    }
    emit->last_emit_was_return_value = true;
#if N_X64
    //asm_x64_call_ind(emit->as, 0, REG_RAX); to seg fault for debugging with gdb
    asm_x64_exit(emit->as);
#elif N_THUMB
    //asm_thumb_call_ind(emit->as, 0, REG_R0); to seg fault for debugging with gdb
    asm_thumb_exit(emit->as);
#endif
}

STATIC void emit_native_raise_varargs(emit_t *emit, int n_args) {
    // call runtime
    assert(0);
}
STATIC void emit_native_yield_value(emit_t *emit) {
    // not supported (for now)
    assert(0);
}
STATIC void emit_native_yield_from(emit_t *emit) {
    // not supported (for now)
    assert(0);
}

const emit_method_table_t EXPORT_FUN(method_table) = {
    emit_native_set_viper_types,
    emit_native_start_pass,
    emit_native_end_pass,
    emit_native_last_emit_was_return_value,
    emit_native_get_stack_size,
    emit_native_set_stack_size,
    emit_native_set_source_line,

    emit_native_load_id,
    emit_native_store_id,
    emit_native_delete_id,

    emit_native_label_assign,
    emit_native_import_name,
    emit_native_import_from,
    emit_native_import_star,
    emit_native_load_const_tok,
    emit_native_load_const_small_int,
    emit_native_load_const_int,
    emit_native_load_const_dec,
    emit_native_load_const_id,
    emit_native_load_const_str,
    emit_native_load_const_verbatim_str,
    emit_native_load_fast,
    emit_native_load_deref,
    emit_native_load_closure,
    emit_native_load_name,
    emit_native_load_global,
    emit_native_load_attr,
    emit_native_load_method,
    emit_native_load_build_class,
    emit_native_store_fast,
    emit_native_store_deref,
    emit_native_store_name,
    emit_native_store_global,
    emit_native_store_attr,
    emit_native_store_subscr,
    emit_native_store_locals,
    emit_native_delete_fast,
    emit_native_delete_deref,
    emit_native_delete_name,
    emit_native_delete_global,
    emit_native_delete_attr,
    emit_native_delete_subscr,
    emit_native_dup_top,
    emit_native_dup_top_two,
    emit_native_pop_top,
    emit_native_rot_two,
    emit_native_rot_three,
    emit_native_jump,
    emit_native_pop_jump_if_true,
    emit_native_pop_jump_if_false,
    emit_native_jump_if_true_or_pop,
    emit_native_jump_if_false_or_pop,
    emit_native_setup_loop,
    emit_native_break_loop,
    emit_native_continue_loop,
    emit_native_setup_with,
    emit_native_with_cleanup,
    emit_native_setup_except,
    emit_native_setup_finally,
    emit_native_end_finally,
    emit_native_get_iter,
    emit_native_for_iter,
    emit_native_for_iter_end,
    emit_native_pop_block,
    emit_native_pop_except,
    emit_native_unary_op,
    emit_native_binary_op,
    emit_native_build_tuple,
    emit_native_build_list,
    emit_native_list_append,
    emit_native_build_map,
    emit_native_store_map,
    emit_native_map_add,
    emit_native_build_set,
    emit_native_set_add,
    emit_native_build_slice,
    emit_native_unpack_sequence,
    emit_native_unpack_ex,
    emit_native_make_function,
    emit_native_make_closure,
    emit_native_call_function,
    emit_native_call_method,
    emit_native_return_value,
    emit_native_raise_varargs,
    emit_native_yield_value,
    emit_native_yield_from,
};

#endif // (MICROPY_EMIT_X64 && N_X64) || (MICROPY_EMIT_THUMB && N_THUMB)
