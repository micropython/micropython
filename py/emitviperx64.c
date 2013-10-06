#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "misc.h"
#include "lexer.h"
#include "machine.h"
#include "parse.h"
#include "scope.h"
#include "runtime.h"
#include "emit.h"
#include "asmx64.h"

#ifdef EMIT_ENABLE_X64

#define REG_LOCAL_1 (REG_RBX)
#define REG_LOCAL_NUM (1)

typedef enum {
    NEED_TO_PUSH_NOTHING,
    NEED_TO_PUSH_R64,
    NEED_TO_PUSH_I64,
} need_to_push_t;

typedef enum {
    VTYPE_UNBOUND,
    VTYPE_PYOBJ,
    VTYPE_BOOL,
    VTYPE_INT,
    VTYPE_PTR,
    VTYPE_PTR_NONE,
    VTYPE_BUILTIN_V_INT,
} vtype_kind_t;

struct _emit_t {
    int pass;

    vtype_kind_t *all_vtype;
    vtype_kind_t *local_vtype;
    vtype_kind_t *stack_vtype;
    int stack_start;
    int stack_size;

    bool last_emit_was_return_value;

    need_to_push_t need_to_push;
    vtype_kind_t last_vtype;
    int last_r64;
    int64_t last_i64;

    scope_t *scope;

    asm_x64_t *as;
};

emit_t *emit_viper_x64_new(uint max_num_labels) {
    emit_t *emit = m_new(emit_t, 1);
    emit->all_vtype = NULL;
    emit->as = asm_x64_new(max_num_labels);
    return emit;
}

static void emit_viper_x64_set_native_types(emit_t *emit, bool do_native_types) {
    //emit->do_native_types = do_native_types;
}

static void emit_viper_x64_start_pass(emit_t *emit, pass_kind_t pass, scope_t *scope) {
    emit->pass = pass;
    emit->stack_start = 0;
    emit->stack_size = 0;
    emit->last_emit_was_return_value = false;
    emit->need_to_push = NEED_TO_PUSH_NOTHING;
    emit->scope = scope;

    if (emit->all_vtype == NULL) {
        emit->all_vtype = m_new(vtype_kind_t, scope->num_locals + scope->stack_size + 100); // XXX don't know stack size on entry
        emit->local_vtype = emit->all_vtype;
        emit->stack_vtype = emit->all_vtype + scope->num_locals;
        memset(emit->all_vtype, VTYPE_UNBOUND, sizeof(vtype_kind_t));
    }

    asm_x64_start_pass(emit->as, pass);

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
    asm_x64_entry(emit->as, num_locals);

    // initialise locals from parameters
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
}

static void emit_viper_x64_end_pass(emit_t *emit) {
    if (!emit->last_emit_was_return_value) {
        asm_x64_exit(emit->as);
    }
    asm_x64_end_pass(emit->as);

    // check stack is back to zero size
    if (emit->stack_size != 0) {
        printf("ERROR: stack size not back to zero; got %d\n", emit->stack_size);
    }

    if (emit->pass == PASS_3) {
        py_fun_t f = asm_x64_get_code(emit->as);
        rt_assign_native_code(emit->scope->unique_code_id, f, asm_x64_get_code_size(emit->as), emit->scope->num_params);
    }
}

static bool emit_viper_x64_last_emit_was_return_value(emit_t *emit) {
    return emit->last_emit_was_return_value;
}

static int emit_viper_x64_get_stack_size(emit_t *emit) {
    return emit->stack_size;
}

static void emit_viper_x64_set_stack_size(emit_t *emit, int size) {
    emit->stack_size = size;
}

static void adjust_stack(emit_t *emit, int stack_size_delta) {
    emit->stack_size += stack_size_delta;
    assert(emit->stack_size >= 0);
    if (emit->pass > PASS_1 && emit->stack_size > emit->scope->stack_size) {
        emit->scope->stack_size = emit->stack_size;
    }
}

static void stack_settle(emit_t *emit) {
    switch (emit->need_to_push) {
        case NEED_TO_PUSH_NOTHING:
            break;

        case NEED_TO_PUSH_R64:
            emit->stack_vtype[emit->stack_size] = emit->last_vtype;
            asm_x64_mov_r64_to_local(emit->as, emit->last_r64, emit->stack_start + emit->stack_size);
            adjust_stack(emit, 1);
            break;

        case NEED_TO_PUSH_I64:
            emit->stack_vtype[emit->stack_size] = emit->last_vtype;
            asm_x64_mov_i64_to_r64_optimised(emit->as, emit->last_i64, REG_RAX);
            asm_x64_mov_r64_to_local(emit->as, REG_RAX, emit->stack_start + emit->stack_size);
            adjust_stack(emit, 1);
            break;
    }
    emit->need_to_push = NEED_TO_PUSH_NOTHING;
}

static void emit_pre_raw(emit_t *emit, int stack_size_delta) {
    adjust_stack(emit, stack_size_delta);
    emit->last_emit_was_return_value = false;
}

static void emit_pre(emit_t *emit) {
    stack_settle(emit);
    emit_pre_raw(emit, 0);
}

static vtype_kind_t peek_vtype(emit_t *emit) {
    switch (emit->need_to_push) {
        case NEED_TO_PUSH_NOTHING:
            return emit->stack_vtype[emit->stack_size - 1];

        case NEED_TO_PUSH_R64:
        case NEED_TO_PUSH_I64:
            return emit->last_vtype;

        default:
            assert(0);
            return VTYPE_UNBOUND;
    }
}

static void emit_pre_pop_r64(emit_t *emit, vtype_kind_t *vtype, int r64) {
    switch (emit->need_to_push) {
        case NEED_TO_PUSH_NOTHING:
            *vtype = emit->stack_vtype[emit->stack_size - 1];
            asm_x64_mov_local_to_r64(emit->as, emit->stack_start + emit->stack_size - 1, r64);
            emit_pre_raw(emit, -1);
            break;

        case NEED_TO_PUSH_R64:
            emit_pre_raw(emit, 0);
            *vtype = emit->last_vtype;
            if (emit->last_r64 != r64) {
                asm_x64_mov_r64_to_r64(emit->as, emit->last_r64, r64);
            }
            break;

        case NEED_TO_PUSH_I64:
            emit_pre_raw(emit, 0);
            *vtype = emit->last_vtype;
            asm_x64_mov_i64_to_r64_optimised(emit->as, emit->last_i64, r64);
            break;
    }
    emit->need_to_push = NEED_TO_PUSH_NOTHING;
}

static void emit_pre_pop_r64_r64(emit_t *emit, vtype_kind_t *vtypea, int r64a, vtype_kind_t *vtypeb, int r64b) {
    emit_pre_pop_r64(emit, vtypea, r64a);
    *vtypeb = emit->stack_vtype[emit->stack_size - 1];
    asm_x64_mov_local_to_r64(emit->as, emit->stack_start + emit->stack_size - 1, r64b);
    adjust_stack(emit, -1);
}

static void emit_pre_pop_r64_r64_r64(emit_t *emit, vtype_kind_t *vtypea, int r64a, vtype_kind_t *vtypeb, int r64b, vtype_kind_t *vtypec, int r64c) {
    emit_pre_pop_r64(emit, vtypea, r64a);
    *vtypeb = emit->stack_vtype[emit->stack_size - 1];
    asm_x64_mov_local_to_r64(emit->as, emit->stack_start + emit->stack_size - 1, r64b);
    *vtypec = emit->stack_vtype[emit->stack_size - 2];
    asm_x64_mov_local_to_r64(emit->as, emit->stack_start + emit->stack_size - 2, r64c);
    adjust_stack(emit, -2);
}

static void emit_post(emit_t *emit) {
}

static void emit_post_push_r64(emit_t *emit, vtype_kind_t vtype, int r64) {
    emit->need_to_push = NEED_TO_PUSH_R64;
    emit->last_vtype = vtype;
    emit->last_r64 = r64;
}

static void emit_post_push_i64(emit_t *emit, vtype_kind_t vtype, int64_t i64) {
    emit->need_to_push = NEED_TO_PUSH_I64;
    emit->last_vtype = vtype;
    emit->last_i64 = i64;
}

static void emit_post_push_r64_r64(emit_t *emit, vtype_kind_t vtypea, int r64a, vtype_kind_t vtypeb, int r64b) {
    emit->stack_vtype[emit->stack_size] = vtypea;
    asm_x64_mov_r64_to_local(emit->as, r64a, emit->stack_start + emit->stack_size);
    emit->need_to_push = NEED_TO_PUSH_R64;
    emit->last_vtype = vtypeb;
    emit->last_r64 = r64b;
    adjust_stack(emit, 1);
}

static void emit_post_push_r64_r64_r64(emit_t *emit, vtype_kind_t vtypea, int r64a, vtype_kind_t vtypeb, int r64b, vtype_kind_t vtypec, int r64c) {
    emit->stack_vtype[emit->stack_size] = vtypea;
    asm_x64_mov_r64_to_local(emit->as, r64a, emit->stack_start + emit->stack_size);
    emit->stack_vtype[emit->stack_size + 1] = vtypeb;
    asm_x64_mov_r64_to_local(emit->as, r64b, emit->stack_start + emit->stack_size + 1);
    emit->stack_vtype[emit->stack_size + 2] = vtypec;
    asm_x64_mov_r64_to_local(emit->as, r64c, emit->stack_start + emit->stack_size + 2);
    adjust_stack(emit, 3);
}

static void emit_post_push_r64_r64_r64_r64(emit_t *emit, vtype_kind_t vtypea, int r64a, vtype_kind_t vtypeb, int r64b, vtype_kind_t vtypec, int r64c, vtype_kind_t vtyped, int r64d) {
    emit->stack_vtype[emit->stack_size] = vtypea;
    asm_x64_mov_r64_to_local(emit->as, r64a, emit->stack_start + emit->stack_size);
    emit->stack_vtype[emit->stack_size + 1] = vtypeb;
    asm_x64_mov_r64_to_local(emit->as, r64b, emit->stack_start + emit->stack_size + 1);
    emit->stack_vtype[emit->stack_size + 2] = vtypec;
    asm_x64_mov_r64_to_local(emit->as, r64c, emit->stack_start + emit->stack_size + 2);
    emit->stack_vtype[emit->stack_size + 3] = vtyped;
    asm_x64_mov_r64_to_local(emit->as, r64d, emit->stack_start + emit->stack_size + 3);
    adjust_stack(emit, 4);
}

// vtype of all n_pop objects is VTYPE_PYOBJ
static void emit_get_stack_pointer_to_r64_for_pop(emit_t *emit, int r64, int n_pop) {
    asm_x64_mov_local_addr_to_r64(emit->as, emit->stack_start + emit->stack_size - 1, r64);
    adjust_stack(emit, -n_pop);
}

// vtype of all n_push objects is VTYPE_PYOBJ
static void emit_get_stack_pointer_to_r64_for_push(emit_t *emit, int r64, int n_push) {
    for (int i = 0; i < n_push; i++) {
        emit->stack_vtype[emit->stack_size + i] = VTYPE_PYOBJ;
    }
    asm_x64_mov_local_addr_to_r64(emit->as, emit->stack_start + emit->stack_size + n_push - 1, r64);
    adjust_stack(emit, n_push);
}

static void emit_call(emit_t *emit, void *fun) {
    asm_x64_call_ind(emit->as, fun, REG_RAX);
}

static void emit_call_with_i64_arg(emit_t *emit, void *fun, int64_t arg_val, int arg_r64) {
    asm_x64_mov_i64_to_r64_optimised(emit->as, arg_val, arg_r64);
    asm_x64_call_ind(emit->as, fun, REG_RAX);
}

static void emit_viper_x64_load_id(emit_t *emit, qstr qstr) {
    // check for built-ins
    if (strcmp(qstr_str(qstr), "v_int") == 0) {
        emit_pre(emit);
        //emit_post_push_blank(emit, VTYPE_BUILTIN_V_INT);

    // not a built-in, so do usual thing
    } else {
        emit_common_load_id(emit, &emit_viper_x64_method_table, emit->scope, qstr);
    }
}

static void emit_viper_x64_store_id(emit_t *emit, qstr qstr) {
    // TODO check for built-ins and disallow
    emit_common_store_id(emit, &emit_viper_x64_method_table, emit->scope, qstr);
}

static void emit_viper_x64_delete_id(emit_t *emit, qstr qstr) {
    // TODO check for built-ins and disallow
    emit_common_delete_id(emit, &emit_viper_x64_method_table, emit->scope, qstr);
}

static void emit_viper_x64_label_assign(emit_t *emit, int l) {
    asm_x64_label_assign(emit->as, l);
}

static void emit_viper_x64_import_name(emit_t *emit, qstr qstr) {
    // not supported
    assert(0);
}

static void emit_viper_x64_import_from(emit_t *emit, qstr qstr) {
    // not supported
    assert(0);
}

static void emit_viper_x64_import_star(emit_t *emit) {
    // not supported
    assert(0);
}

static void emit_viper_x64_load_const_tok(emit_t *emit, py_token_kind_t tok) {
    emit_pre(emit);
    int vtype;
    machine_uint_t val;
    switch (tok) {
        case PY_TOKEN_KW_NONE: vtype = VTYPE_PTR_NONE; val = 0; break;
        case PY_TOKEN_KW_FALSE: vtype = VTYPE_BOOL; val = 0; break;
        case PY_TOKEN_KW_TRUE: vtype = VTYPE_BOOL; val = 1; break;
        default: assert(0); vtype = 0; val = 0; // shouldn't happen
    }
    emit_post_push_i64(emit, vtype, val);
}

static void emit_viper_x64_load_const_small_int(emit_t *emit, int arg) {
    emit_pre(emit);
    emit_post_push_i64(emit, VTYPE_INT, arg);
}

static void emit_viper_x64_load_const_int(emit_t *emit, qstr qstr) {
    // not implemented
    assert(0);
}

static void emit_viper_x64_load_const_dec(emit_t *emit, qstr qstr) {
    // not supported
    assert(0);
}

static void emit_viper_x64_load_const_id(emit_t *emit, qstr qstr) {
    // not supported?
    assert(0);
}

static void emit_viper_x64_load_const_str(emit_t *emit, qstr qstr, bool bytes) {
    // not implemented properly
    assert(0);
    emit_pre(emit);
    emit_post_push_i64(emit, VTYPE_PTR, (machine_uint_t)qstr_str(qstr));
}

static void emit_viper_x64_load_const_verbatim_start(emit_t *emit) {
    // not supported/needed
    assert(0);
}

static void emit_viper_x64_load_const_verbatim_int(emit_t *emit, int val) {
    // not supported/needed
    assert(0);
}

static void emit_viper_x64_load_const_verbatim_str(emit_t *emit, const char *str) {
    // not supported/needed
    assert(0);
}

static void emit_viper_x64_load_const_verbatim_strn(emit_t *emit, const char *str, int len) {
    // not supported/needed
    assert(0);
}

static void emit_viper_x64_load_const_verbatim_quoted_str(emit_t *emit, qstr qstr, bool bytes) {
    // not supported/needed
    assert(0);
}

static void emit_viper_x64_load_const_verbatim_end(emit_t *emit) {
    // not supported/needed
    assert(0);
}

static void emit_viper_x64_load_fast(emit_t *emit, qstr qstr, int local_num) {
    if (emit->local_vtype[local_num] == VTYPE_UNBOUND) {
        printf("ViperTypeError: local %s used before type known\n", qstr_str(qstr));
    }
    if (local_num == 0) {
        emit_pre(emit);
        emit_post_push_r64(emit, emit->local_vtype[local_num], REG_LOCAL_1);
    } else {
        emit_pre(emit);
        asm_x64_mov_local_to_r64(emit->as, local_num - 1, REG_RAX);
        emit_post_push_r64(emit, emit->local_vtype[local_num], REG_RAX);
    }
}

static void emit_viper_x64_load_name(emit_t *emit, qstr qstr) {
    emit_pre(emit);
    emit_call_with_i64_arg(emit, rt_load_name, qstr, REG_ARG_1);
    emit_post_push_r64(emit, VTYPE_PYOBJ, REG_RET);
}

static void emit_viper_x64_load_global(emit_t *emit, qstr qstr) {
    emit_pre(emit);
    emit_call_with_i64_arg(emit, rt_load_global, qstr, REG_ARG_1);
    emit_post_push_r64(emit, VTYPE_PYOBJ, REG_RET);
}

static void emit_viper_x64_load_deref(emit_t *emit, qstr qstr) {
    // not implemented
    assert(0);
}

static void emit_viper_x64_load_closure(emit_t *emit, qstr qstr) {
    // not implemented
    assert(0);
}

static void emit_viper_x64_load_attr(emit_t *emit, qstr qstr) {
    vtype_kind_t vtype_base;
    emit_pre_pop_r64(emit, &vtype_base, REG_ARG_1); // arg1 = base
    assert(vtype_base == VTYPE_PYOBJ);
    emit_call_with_i64_arg(emit, rt_load_attr, qstr, REG_ARG_2); // arg2 = attribute name
    emit_post_push_r64(emit, VTYPE_PYOBJ, REG_RET);
}

static void emit_viper_x64_load_method(emit_t *emit, qstr qstr) {
    vtype_kind_t vtype_base;
    emit_pre_pop_r64(emit, &vtype_base, REG_ARG_1); // arg1 = base
    assert(vtype_base == VTYPE_PYOBJ);
    emit_get_stack_pointer_to_r64_for_push(emit, REG_ARG_3, 2); // arg3 = dest ptr
    emit_call_with_i64_arg(emit, rt_load_method, qstr, REG_ARG_2); // arg2 = method name
}

static void emit_viper_x64_load_build_class(emit_t *emit) {
    // not supported
   assert(0);
}

static void emit_viper_x64_store_fast(emit_t *emit, qstr qstr, int local_num) {
    vtype_kind_t vtype;
    if (local_num == 0) {
        emit_pre_pop_r64(emit, &vtype, REG_LOCAL_1);
        emit_post(emit);
    } else {
        emit_pre_pop_r64(emit, &vtype, REG_RAX);
        asm_x64_mov_r64_to_local(emit->as, REG_RAX, local_num - 1);
        emit_post(emit);
    }

    // check types
    if (emit->local_vtype[local_num] == VTYPE_UNBOUND) {
        // first time this local is assigned, so give it a type of the object stored in it
        emit->local_vtype[local_num] = vtype;
    } else if (emit->local_vtype[local_num] != vtype) {
        // type of local is not the same as object stored in it
        printf("ViperTypeError: type mismatch, local %s has type %d but source object has type %d\n", qstr_str(qstr), emit->local_vtype[local_num], vtype);
    }
}

static void emit_viper_x64_store_name(emit_t *emit, qstr qstr) {
    vtype_kind_t vtype;
    emit_pre_pop_r64(emit, &vtype, REG_ARG_2);
    assert(vtype == VTYPE_PYOBJ);
    emit_call_with_i64_arg(emit, rt_store_name, qstr, REG_ARG_1); // arg1 = name
    emit_post(emit);
}

static void emit_viper_x64_store_global(emit_t *emit, qstr qstr) {
    // not implemented
    assert(0);
}

static void emit_viper_x64_store_deref(emit_t *emit, qstr qstr) {
    // not implemented
    assert(0);
}

static void emit_viper_x64_store_attr(emit_t *emit, qstr qstr) {
    // not implemented
    assert(0);
}

static void emit_viper_x64_store_locals(emit_t *emit) {
    // not supported
    assert(0);
}

static void emit_viper_x64_store_subscr(emit_t *emit) {
    vtype_kind_t vtype_index, vtype_base, vtype_value;
    emit_pre_pop_r64_r64_r64(emit, &vtype_index, REG_ARG_2, &vtype_base, REG_ARG_1, &vtype_value, REG_ARG_3); // index, base, value to store
    assert(vtype_index == VTYPE_PYOBJ);
    assert(vtype_base == VTYPE_PYOBJ);
    assert(vtype_value == VTYPE_PYOBJ);
    emit_call(emit, rt_store_subscr);
}

static void emit_viper_x64_delete_fast(emit_t *emit, qstr qstr, int local_num) {
    // not implemented
    assert(0);
}

static void emit_viper_x64_delete_name(emit_t *emit, qstr qstr) {
    // not implemented
    assert(0);
}

static void emit_viper_x64_delete_global(emit_t *emit, qstr qstr) {
    // not implemented
    assert(0);
}

static void emit_viper_x64_delete_deref(emit_t *emit, qstr qstr) {
    // not supported
    assert(0);
}

static void emit_viper_x64_delete_attr(emit_t *emit, qstr qstr) {
    // not supported
    assert(0);
}

static void emit_viper_x64_delete_subscr(emit_t *emit) {
    // not supported
    assert(0);
}

static void emit_viper_x64_dup_top(emit_t *emit) {
    vtype_kind_t vtype;
    emit_pre_pop_r64(emit, &vtype, REG_RAX);
    emit_post_push_r64_r64(emit, vtype, REG_RAX, vtype, REG_RAX);
}

static void emit_viper_x64_dup_top_two(emit_t *emit) {
    vtype_kind_t vtype1, vtype2;
    emit_pre_pop_r64_r64(emit, &vtype1, REG_RAX, &vtype2, REG_RDI);
    emit_post_push_r64_r64_r64_r64(emit, vtype2, REG_RDI, vtype1, REG_RAX, vtype2, REG_RDI, vtype1, REG_RAX);
}

static void emit_viper_x64_pop_top(emit_t *emit) {
    vtype_kind_t vtype;
    emit_pre_pop_r64(emit, &vtype, REG_RAX);
    emit_post(emit);
}

static void emit_viper_x64_rot_two(emit_t *emit) {
    assert(0);
}

static void emit_viper_x64_rot_three(emit_t *emit) {
    vtype_kind_t vtype_rax, vtype_rdi, vtype_rsi;
    emit_pre_pop_r64_r64_r64(emit, &vtype_rax, REG_RAX, &vtype_rdi, REG_RDI, &vtype_rsi, REG_RSI);
    emit_post_push_r64_r64_r64(emit, vtype_rax, REG_RAX, vtype_rsi, REG_RSI, vtype_rdi, REG_RDI);
}

static void emit_viper_x64_jump(emit_t *emit, int label) {
    emit_pre(emit);
    asm_x64_jmp_label(emit->as, label);
    emit_post(emit);
}

static void emit_viper_x64_pop_jump_if_false(emit_t *emit, int label) {
    vtype_kind_t vtype = peek_vtype(emit);
    if (vtype == VTYPE_BOOL) {
        emit_pre_pop_r64(emit, &vtype, REG_RAX);
        asm_x64_test_r8_with_r8(emit->as, REG_RAX, REG_RAX);
        asm_x64_jcc_label(emit->as, JCC_JZ, label);
        emit_post(emit);
    } else if (vtype == VTYPE_PYOBJ) {
        emit_pre_pop_r64(emit, &vtype, REG_ARG_1);
        emit_call(emit, rt_is_true);
        asm_x64_test_r8_with_r8(emit->as, REG_RET, REG_RET);
        asm_x64_jcc_label(emit->as, JCC_JZ, label);
        emit_post(emit);
    } else {
        printf("ViperTypeError: expecting a bool or pyobj, got %d\n", vtype);
        assert(0);
    }
}

static void emit_viper_x64_pop_jump_if_true(emit_t *emit, int label) {
    assert(0);
}
static void emit_viper_x64_jump_if_true_or_pop(emit_t *emit, int label) {
    assert(0);
}
static void emit_viper_x64_jump_if_false_or_pop(emit_t *emit, int label) {
    assert(0);
}

static void emit_viper_x64_setup_loop(emit_t *emit, int label) {
    emit_pre(emit);
    emit_post(emit);
}

static void emit_viper_x64_break_loop(emit_t *emit, int label) {
    assert(0);
}
static void emit_viper_x64_continue_loop(emit_t *emit, int label) {
    assert(0);
}
static void emit_viper_x64_setup_with(emit_t *emit, int label) {
    assert(0);
}
static void emit_viper_x64_with_cleanup(emit_t *emit) {
    assert(0);
}
static void emit_viper_x64_setup_except(emit_t *emit, int label) {
    assert(0);
}
static void emit_viper_x64_setup_finally(emit_t *emit, int label) {
    assert(0);
}
static void emit_viper_x64_end_finally(emit_t *emit) {
    assert(0);
}
static void emit_viper_x64_get_iter(emit_t *emit) {
    assert(0);
} // tos = getiter(tos)
static void emit_viper_x64_for_iter(emit_t *emit, int label) {
    assert(0);
}
static void emit_viper_x64_for_iter_end(emit_t *emit) {
    assert(0);
}
static void emit_viper_x64_pop_except(emit_t *emit) {
    assert(0);
}

static void emit_viper_x64_unary_op(emit_t *emit, rt_unary_op_t op) {
    vtype_kind_t vtype;
    emit_pre_pop_r64(emit, &vtype, REG_ARG_2);
    assert(vtype == VTYPE_PYOBJ);
    emit_call_with_i64_arg(emit, rt_unary_op, op, REG_ARG_1);
    emit_post_push_r64(emit, VTYPE_PYOBJ, REG_RET);
}

static void emit_viper_x64_build_tuple(emit_t *emit, int n_args) {
    assert(0);
}

static void emit_viper_x64_build_list(emit_t *emit, int n_args) {
    emit_pre(emit);
    emit_get_stack_pointer_to_r64_for_pop(emit, REG_ARG_2, n_args); // pointer to items in reverse order
    emit_call_with_i64_arg(emit, rt_build_list, n_args, REG_ARG_1);
    emit_post_push_r64(emit, VTYPE_PYOBJ, REG_RET); // new list
}

static void emit_viper_x64_list_append(emit_t *emit, int list_index) {
    assert(0);
}

static void emit_viper_x64_build_map(emit_t *emit, int n_args) {
    emit_pre(emit);
    emit_call_with_i64_arg(emit, rt_build_map, n_args, REG_ARG_1);
    emit_post_push_r64(emit, VTYPE_PYOBJ, REG_RET); // new map
}

static void emit_viper_x64_store_map(emit_t *emit) {
    vtype_kind_t vtype_key, vtype_value, vtype_map;
    emit_pre_pop_r64_r64_r64(emit, &vtype_key, REG_ARG_2, &vtype_value, REG_ARG_3, &vtype_map, REG_ARG_1); // key, value, map
    assert(vtype_key == VTYPE_PYOBJ);
    assert(vtype_value == VTYPE_PYOBJ);
    assert(vtype_map == VTYPE_PYOBJ);
    emit_call(emit, rt_store_map);
    emit_post_push_r64(emit, VTYPE_PYOBJ, REG_RET); // map
}

static void emit_viper_x64_map_add(emit_t *emit, int map_index) {
    assert(0);
}

static void emit_viper_x64_build_set(emit_t *emit, int n_args) {
    emit_pre(emit);
    emit_get_stack_pointer_to_r64_for_pop(emit, REG_ARG_2, n_args); // pointer to items in reverse order
    emit_call_with_i64_arg(emit, rt_build_set, n_args, REG_ARG_1);
    emit_post_push_r64(emit, VTYPE_PYOBJ, REG_RET); // new set
}

static void emit_viper_x64_set_add(emit_t *emit, int set_index) {
    assert(0);
}
static void emit_viper_x64_build_slice(emit_t *emit, int n_args) {
    assert(0);
}
static void emit_viper_x64_unpack_sequence(emit_t *emit, int n_args) {
    assert(0);
}
static void emit_viper_x64_unpack_ex(emit_t *emit, int n_left, int n_right) {
    assert(0);
}

static void emit_viper_x64_make_function(emit_t *emit, scope_t *scope, int n_dict_params, int n_default_params) {
    assert(n_default_params == 0 && n_dict_params == 0);
    emit_pre(emit);
    emit_call_with_i64_arg(emit, rt_make_function_from_id, scope->unique_code_id, REG_ARG_1);
    emit_post_push_r64(emit, VTYPE_PYOBJ, REG_RET);
}

static void emit_viper_x64_make_closure(emit_t *emit, scope_t *scope, int n_dict_params, int n_default_params) {
    assert(0);
}

static void emit_viper_x64_call_function(emit_t *emit, int n_positional, int n_keyword, bool have_star_arg, bool have_dbl_star_arg) {
    assert(n_keyword == 0 && !have_star_arg && !have_dbl_star_arg);
    if (n_positional == 0) {
        vtype_kind_t vtype_fun;
        emit_pre_pop_r64(emit, &vtype_fun, REG_ARG_1); // the function
        assert(vtype_fun == VTYPE_PYOBJ);
        emit_call(emit, rt_call_function_0);
    } else if (n_positional == 1) {
        vtype_kind_t vtype_fun, vtype_arg1;
        emit_pre_pop_r64_r64(emit, &vtype_arg1, REG_ARG_2, &vtype_fun, REG_ARG_1); // the single argument, the function
        assert(vtype_fun == VTYPE_PYOBJ);
        assert(vtype_arg1 == VTYPE_PYOBJ);
        emit_call(emit, rt_call_function_1);
    } else if (n_positional == 2) {
        vtype_kind_t vtype_fun, vtype_arg1, vtype_arg2;
        emit_pre_pop_r64_r64_r64(emit, &vtype_arg2, REG_ARG_3, &vtype_arg1, REG_ARG_2, &vtype_fun, REG_ARG_1); // the second argument, the first argument, the function
        assert(vtype_fun == VTYPE_PYOBJ);
        assert(vtype_arg1 == VTYPE_PYOBJ);
        assert(vtype_arg2 == VTYPE_PYOBJ);
        emit_call(emit, rt_call_function_2);
    } else {
        assert(0);
    }
    emit_post_push_r64(emit, VTYPE_PYOBJ, REG_RET);
}

static void emit_viper_x64_call_method(emit_t *emit, int n_positional, int n_keyword, bool have_star_arg, bool have_dbl_star_arg) {
    assert(n_keyword == 0 && !have_star_arg && !have_dbl_star_arg);
    if (n_positional == 0) {
        vtype_kind_t vtype_meth, vtype_self;
        emit_pre_pop_r64_r64(emit, &vtype_self, REG_ARG_2, &vtype_meth, REG_ARG_1); // the self object (or NULL), the method
        assert(vtype_meth == VTYPE_PYOBJ);
        assert(vtype_self == VTYPE_PYOBJ);
        emit_call(emit, rt_call_method_1);
    } else if (n_positional == 1) {
        vtype_kind_t vtype_meth, vtype_self, vtype_arg1;
        emit_pre_pop_r64_r64_r64(emit, &vtype_arg1, REG_ARG_3, &vtype_self, REG_ARG_2, &vtype_meth, REG_ARG_1); // the first argument, the self object (or NULL), the method
        assert(vtype_meth == VTYPE_PYOBJ);
        assert(vtype_self == VTYPE_PYOBJ);
        assert(vtype_arg1 == VTYPE_PYOBJ);
        emit_call(emit, rt_call_method_2);
    } else {
        assert(0);
    }
    emit_post_push_r64(emit, VTYPE_PYOBJ, REG_RET);
}

static void emit_viper_x64_pop_block(emit_t *emit) {
    emit_pre(emit);
    emit_post(emit);
}

static void emit_viper_x64_binary_op(emit_t *emit, rt_binary_op_t op) {
    vtype_kind_t vtype_lhs, vtype_rhs;
    emit_pre_pop_r64_r64(emit, &vtype_rhs, REG_ARG_3, &vtype_lhs, REG_ARG_2);
    if (vtype_lhs == VTYPE_INT && vtype_rhs == VTYPE_INT) {
        assert(op == RT_BINARY_OP_ADD);
        asm_x64_add_r64_to_r64(emit->as, REG_ARG_3, REG_ARG_2);
        emit_post_push_r64(emit, VTYPE_INT, REG_ARG_2);
    } else if (vtype_lhs == VTYPE_PYOBJ && vtype_rhs == VTYPE_PYOBJ) {
        emit_call_with_i64_arg(emit, rt_binary_op, op, REG_ARG_1);
        emit_post_push_r64(emit, VTYPE_PYOBJ, REG_RET);
    } else {
        printf("ViperTypeError: can't do binary op between types %d and %d\n", vtype_lhs, vtype_rhs);
        assert(0);
    }
}

static void emit_viper_x64_compare_op(emit_t *emit, rt_compare_op_t op) {
    vtype_kind_t vtype_lhs, vtype_rhs;
    emit_pre_pop_r64_r64(emit, &vtype_rhs, REG_ARG_3, &vtype_lhs, REG_ARG_2);
    if (vtype_lhs == VTYPE_INT && vtype_rhs == VTYPE_INT) {
        assert(op == RT_COMPARE_OP_LESS);
        asm_x64_xor_r64_to_r64(emit->as, REG_RET, REG_RET);
        asm_x64_cmp_r64_with_r64(emit->as, REG_ARG_3, REG_ARG_2);
        asm_x64_setcc_r8(emit->as, JCC_JL, REG_RET);
        emit_post_push_r64(emit, VTYPE_BOOL, REG_RET);
    } else if (vtype_lhs == VTYPE_PYOBJ && vtype_rhs == VTYPE_PYOBJ) {
        emit_call_with_i64_arg(emit, rt_compare_op, op, REG_ARG_1);
        emit_post_push_r64(emit, VTYPE_PYOBJ, REG_RET);
    } else {
        printf("ViperTypeError: can't do comparison between types %d and %d\n", vtype_lhs, vtype_rhs);
        assert(0);
    }
}

static void emit_viper_x64_return_value(emit_t *emit) {
    vtype_kind_t vtype;
    emit_pre_pop_r64(emit, &vtype, REG_RAX);
    assert(vtype == VTYPE_PTR_NONE);
    emit->last_emit_was_return_value = true;
    //asm_x64_call_ind(emit->as, 0, REG_RAX); to seg fault for debugging with gdb
    asm_x64_exit(emit->as);
}

static void emit_viper_x64_raise_varargs(emit_t *emit, int n_args) {
    assert(0);
}
static void emit_viper_x64_yield_value(emit_t *emit) {
    assert(0);
}
static void emit_viper_x64_yield_from(emit_t *emit) {
    assert(0);
}

const emit_method_table_t emit_viper_x64_method_table = {
    emit_viper_x64_set_native_types,
    emit_viper_x64_start_pass,
    emit_viper_x64_end_pass,
    emit_viper_x64_last_emit_was_return_value,
    emit_viper_x64_get_stack_size,
    emit_viper_x64_set_stack_size,

    emit_viper_x64_load_id,
    emit_viper_x64_store_id,
    emit_viper_x64_delete_id,

    emit_viper_x64_label_assign,
    emit_viper_x64_import_name,
    emit_viper_x64_import_from,
    emit_viper_x64_import_star,
    emit_viper_x64_load_const_tok,
    emit_viper_x64_load_const_small_int,
    emit_viper_x64_load_const_int,
    emit_viper_x64_load_const_dec,
    emit_viper_x64_load_const_id,
    emit_viper_x64_load_const_str,
    emit_viper_x64_load_const_verbatim_start,
    emit_viper_x64_load_const_verbatim_int,
    emit_viper_x64_load_const_verbatim_str,
    emit_viper_x64_load_const_verbatim_strn,
    emit_viper_x64_load_const_verbatim_quoted_str,
    emit_viper_x64_load_const_verbatim_end,
    emit_viper_x64_load_fast,
    emit_viper_x64_load_name,
    emit_viper_x64_load_global,
    emit_viper_x64_load_deref,
    emit_viper_x64_load_closure,
    emit_viper_x64_load_attr,
    emit_viper_x64_load_method,
    emit_viper_x64_load_build_class,
    emit_viper_x64_store_fast,
    emit_viper_x64_store_name,
    emit_viper_x64_store_global,
    emit_viper_x64_store_deref,
    emit_viper_x64_store_attr,
    emit_viper_x64_store_locals,
    emit_viper_x64_store_subscr,
    emit_viper_x64_delete_fast,
    emit_viper_x64_delete_name,
    emit_viper_x64_delete_global,
    emit_viper_x64_delete_deref,
    emit_viper_x64_delete_attr,
    emit_viper_x64_delete_subscr,
    emit_viper_x64_dup_top,
    emit_viper_x64_dup_top_two,
    emit_viper_x64_pop_top,
    emit_viper_x64_rot_two,
    emit_viper_x64_rot_three,
    emit_viper_x64_jump,
    emit_viper_x64_pop_jump_if_true,
    emit_viper_x64_pop_jump_if_false,
    emit_viper_x64_jump_if_true_or_pop,
    emit_viper_x64_jump_if_false_or_pop,
    emit_viper_x64_setup_loop,
    emit_viper_x64_break_loop,
    emit_viper_x64_continue_loop,
    emit_viper_x64_setup_with,
    emit_viper_x64_with_cleanup,
    emit_viper_x64_setup_except,
    emit_viper_x64_setup_finally,
    emit_viper_x64_end_finally,
    emit_viper_x64_get_iter,
    emit_viper_x64_for_iter,
    emit_viper_x64_for_iter_end,
    emit_viper_x64_pop_block,
    emit_viper_x64_pop_except,
    emit_viper_x64_unary_op,
    emit_viper_x64_binary_op,
    emit_viper_x64_compare_op,
    emit_viper_x64_build_tuple,
    emit_viper_x64_build_list,
    emit_viper_x64_list_append,
    emit_viper_x64_build_map,
    emit_viper_x64_store_map,
    emit_viper_x64_map_add,
    emit_viper_x64_build_set,
    emit_viper_x64_set_add,
    emit_viper_x64_build_slice,
    emit_viper_x64_unpack_sequence,
    emit_viper_x64_unpack_ex,
    emit_viper_x64_make_function,
    emit_viper_x64_make_closure,
    emit_viper_x64_call_function,
    emit_viper_x64_call_method,
    emit_viper_x64_return_value,
    emit_viper_x64_raise_varargs,
    emit_viper_x64_yield_value,
    emit_viper_x64_yield_from,
};

#endif // EMIT_ENABLE_X64
