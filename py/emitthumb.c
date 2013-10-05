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
#include "asmthumb.h"

#ifdef EMIT_DO_THUMB

#define REG_LOCAL_1 (REG_R4)
#define REG_LOCAL_2 (REG_R5)
#define REG_LOCAL_3 (REG_R6)
#define REG_TEMP    (REG_R7)
#define REG_LOCAL_NUM (3)

typedef enum {
    NEED_TO_PUSH_NOTHING,
    NEED_TO_PUSH_REG,
    NEED_TO_PUSH_I32,
} need_to_push_t;

struct _emitter_t {
    int pass;
    int stack_start;
    int stack_size;
    bool last_emit_was_return_value;
    need_to_push_t need_to_push;
    int last_reg;
    int32_t last_i32;

    scope_t *scope;

    asm_thumb_t *as;
    bool do_native_types;
};

emitter_t *emit_new() {
    emitter_t *emit = m_new(emitter_t, 1);
    emit->as = asm_thumb_new();
    emit->do_native_types = true;
    return emit;
}

void emit_start_pass(emitter_t *emit, pass_kind_t pass, scope_t *scope) {
    emit->pass = pass;
    emit->stack_start = 0;
    emit->stack_size = 0;
    emit->last_emit_was_return_value = false;
    emit->need_to_push = NEED_TO_PUSH_NOTHING;
    emit->scope = scope;

    asm_thumb_start_pass(emit->as, pass);

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
    asm_thumb_entry(emit->as, num_locals);

    // initialise locals from parameters
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
}

void emit_end_pass(emitter_t *emit) {
    if (!emit->last_emit_was_return_value) {
        asm_thumb_exit(emit->as);
    }
    asm_thumb_end_pass(emit->as);

    // check stack is back to zero size
    if (emit->stack_size != 0) {
        printf("ERROR: stack size not back to zero; got %d\n", emit->stack_size);
    }

    if (emit->pass == PASS_3) {
        py_fun_t f = asm_thumb_get_code(emit->as);
        rt_assign_native_code(emit->scope->unique_code_id, f, asm_thumb_get_code_size(emit->as), emit->scope->num_params);
    }
}

bool emit_last_emit_was_return_value(emitter_t *emit) {
    return emit->last_emit_was_return_value;
}

int emit_get_stack_size(emitter_t *emit) {
    return emit->stack_size;
}

void emit_set_stack_size(emitter_t *emit, int size) {
    emit->stack_size = size;
}

static void adjust_stack(emitter_t *emit, int stack_size_delta) {
    emit->stack_size += stack_size_delta;
    assert(emit->stack_size >= 0);
    if (emit->pass > PASS_1 && emit->stack_size > emit->scope->stack_size) {
        emit->scope->stack_size = emit->stack_size;
    }
}

static void stack_settle(emitter_t *emit) {
    switch (emit->need_to_push) {
        case NEED_TO_PUSH_NOTHING:
            break;

        case NEED_TO_PUSH_REG:
            asm_thumb_mov_local_reg(emit->as, emit->stack_start + emit->stack_size, emit->last_reg);
            adjust_stack(emit, 1);
            break;

        case NEED_TO_PUSH_I32:
            asm_thumb_mov_reg_i32_optimised(emit->as, REG_R0, emit->last_i32);
            asm_thumb_mov_local_reg(emit->as, emit->stack_start + emit->stack_size, REG_R0);
            adjust_stack(emit, 1);
            break;
    }
    emit->need_to_push = NEED_TO_PUSH_NOTHING;
}

static void emit_pre_raw(emitter_t *emit, int stack_size_delta) {
    adjust_stack(emit, stack_size_delta);
    emit->last_emit_was_return_value = false;
}

static void emit_pre(emitter_t *emit) {
    stack_settle(emit);
    emit_pre_raw(emit, 0);
}

static void emit_pre_pop_reg(emitter_t *emit, int reg_dest) {
    switch (emit->need_to_push) {
        case NEED_TO_PUSH_NOTHING:
            asm_thumb_mov_reg_local(emit->as, reg_dest, emit->stack_start + emit->stack_size - 1);
            emit_pre_raw(emit, -1);
            break;

        case NEED_TO_PUSH_REG:
            emit_pre_raw(emit, 0);
            if (emit->last_reg != reg_dest) {
                asm_thumb_mov_reg_reg(emit->as, reg_dest, emit->last_reg);
            }
            break;

        case NEED_TO_PUSH_I32:
            emit_pre_raw(emit, 0);
            asm_thumb_mov_reg_i32_optimised(emit->as, reg_dest, emit->last_i32);
            break;
    }
    emit->need_to_push = NEED_TO_PUSH_NOTHING;
}

static void emit_pre_pop_reg_reg(emitter_t *emit, int rega, int regb) {
    emit_pre_pop_reg(emit, rega);
    asm_thumb_mov_reg_local(emit->as, regb, emit->stack_start + emit->stack_size - 1);
    adjust_stack(emit, -1);
}

static void emit_pre_pop_reg_reg_reg(emitter_t *emit, int rega, int regb, int regc) {
    emit_pre_pop_reg(emit, rega);
    asm_thumb_mov_reg_local(emit->as, regb, emit->stack_start + emit->stack_size - 1);
    asm_thumb_mov_reg_local(emit->as, regc, emit->stack_start + emit->stack_size - 2);
    adjust_stack(emit, -2);
}

static void emit_post(emitter_t *emit) {
}

static void emit_post_push_reg(emitter_t *emit, int reg) {
    emit->need_to_push = NEED_TO_PUSH_REG;
    emit->last_reg = reg;
}

static void emit_post_push_i32(emitter_t *emit, int32_t i32) {
    emit->need_to_push = NEED_TO_PUSH_I32;
    emit->last_i32 = i32;
}

static void emit_post_push_reg_reg(emitter_t *emit, int rega, int regb) {
    asm_thumb_mov_local_reg(emit->as, emit->stack_start + emit->stack_size, rega);
    emit->need_to_push = NEED_TO_PUSH_REG;
    emit->last_reg = regb;
    adjust_stack(emit, 1);
}

static void emit_post_push_reg_reg_reg(emitter_t *emit, int rega, int regb, int regc) {
    asm_thumb_mov_local_reg(emit->as, emit->stack_start + emit->stack_size, rega);
    asm_thumb_mov_local_reg(emit->as, emit->stack_start + emit->stack_size + 1, regb);
    asm_thumb_mov_local_reg(emit->as, emit->stack_start + emit->stack_size + 2, regc);
    adjust_stack(emit, 3);
}

static void emit_post_push_reg_reg_reg_reg(emitter_t *emit, int rega, int regb, int regc, int regd) {
    asm_thumb_mov_local_reg(emit->as, emit->stack_start + emit->stack_size, rega);
    asm_thumb_mov_local_reg(emit->as, emit->stack_start + emit->stack_size + 1, regb);
    asm_thumb_mov_local_reg(emit->as, emit->stack_start + emit->stack_size + 2, regc);
    asm_thumb_mov_local_reg(emit->as, emit->stack_start + emit->stack_size + 3, regd);
    adjust_stack(emit, 4);
}

static void emit_get_stack_pointer_to_reg_for_pop(emitter_t *emit, int reg_dest, int n_pop) {
    asm_thumb_mov_reg_local_addr(emit->as, reg_dest, emit->stack_start + emit->stack_size - 1);
    adjust_stack(emit, -n_pop);
}

static void emit_get_stack_pointer_to_reg_for_push(emitter_t *emit, int reg_dest, int n_push) {
    asm_thumb_mov_reg_local_addr(emit->as, reg_dest, emit->stack_start + emit->stack_size + n_push - 1);
    adjust_stack(emit, n_push);
}

static void emit_call(emitter_t *emit, rt_fun_kind_t fun_kind) {
    asm_thumb_bl_ind(emit->as, rt_fun_table[fun_kind], fun_kind, REG_R3);
}

static void emit_call_with_i32_arg(emitter_t *emit, rt_fun_kind_t fun_kind, int32_t arg_val, int arg_reg) {
    asm_thumb_mov_reg_i32_optimised(emit->as, arg_reg, arg_val);
    asm_thumb_bl_ind(emit->as, rt_fun_table[fun_kind], fun_kind, REG_R3);
}

int emit_label_new(emitter_t *emit) {
    return asm_thumb_label_new(emit->as);
}

void emit_label_assign(emitter_t *emit, int l) {
    asm_thumb_label_assign(emit->as, l);
}

void emit_import_name(emitter_t *emit, qstr qstr) {
    assert(0);
}
void emit_import_from(emitter_t *emit, qstr qstr) {
    assert(0);
}
void emit_import_star(emitter_t *emit) {
    assert(0);
}

void emit_load_const_tok(emitter_t *emit, py_token_kind_t tok) {
    emit_pre(emit);
    py_obj_t o;
    switch (tok) {
        case PY_TOKEN_KW_NONE: o = py_const_none; break;
        case PY_TOKEN_KW_FALSE: o = py_const_false; break;
        case PY_TOKEN_KW_TRUE: o = py_const_true; break;
        default: assert(0); o = 0; // shouldn't happen
    }
    emit_post_push_i32(emit, (machine_uint_t)o);
}

void emit_load_const_small_int(emitter_t *emit, int arg) {
    emit_pre(emit);
    if (emit->do_native_types) {
        emit_post_push_i32(emit, arg);
    } else {
        emit_post_push_i32(emit, (arg << 1) | 1);
    }
}

void emit_load_const_int(emitter_t *emit, qstr qstr) {
    assert(0);
}
void emit_load_const_dec(emitter_t *emit, qstr qstr) {
    assert(0);
}
void emit_load_const_id(emitter_t *emit, qstr qstr) {
    assert(0);
}

void emit_load_const_str(emitter_t *emit, qstr qstr, bool bytes) {
    emit_pre(emit);
    emit_call_with_i32_arg(emit, RT_F_LOAD_CONST_STR, qstr, REG_ARG_1);
    emit_post_push_reg(emit, REG_RET);
}

void emit_load_const_verbatim_start(emitter_t *emit) {
    assert(0);
}
void emit_load_const_verbatim_int(emitter_t *emit, int val) {
    assert(0);
}
void emit_load_const_verbatim_str(emitter_t *emit, const char *str) {
    assert(0);
}
void emit_load_const_verbatim_strn(emitter_t *emit, const char *str, int len) {
    assert(0);
}
void emit_load_const_verbatim_quoted_str(emitter_t *emit, qstr qstr, bool bytes) {
    assert(0);
}
void emit_load_const_verbatim_end(emitter_t *emit) {
    assert(0);
}

void emit_load_fast(emitter_t *emit, qstr qstr, int local_num) {
    emit_pre(emit);
    if (local_num == 0) {
        emit_post_push_reg(emit, REG_LOCAL_1);
    } else if (local_num == 1) {
        emit_post_push_reg(emit, REG_LOCAL_2);
    } else if (local_num == 2) {
        emit_post_push_reg(emit, REG_LOCAL_3);
    } else {
        asm_thumb_mov_reg_local(emit->as, REG_R0, local_num - 1);
        emit_post_push_reg(emit, REG_R0);
    }
}

void emit_load_name(emitter_t *emit, qstr qstr) {
    emit_pre(emit);
    emit_call_with_i32_arg(emit, RT_F_LOAD_NAME, qstr, REG_ARG_1);
    emit_post_push_reg(emit, REG_RET);
}

void emit_load_global(emitter_t *emit, qstr qstr) {
    emit_pre(emit);
    emit_call_with_i32_arg(emit, RT_F_LOAD_GLOBAL, qstr, REG_ARG_1);
    emit_post_push_reg(emit, REG_RET);
}

void emit_load_deref(emitter_t *emit, qstr qstr) {
    assert(0);
}
void emit_load_closure(emitter_t *emit, qstr qstr) {
    assert(0);
}

void emit_load_attr(emitter_t *emit, qstr qstr) {
    emit_pre_pop_reg(emit, REG_ARG_1); // arg1 = base
    emit_call_with_i32_arg(emit, RT_F_LOAD_ATTR, qstr, REG_ARG_2); // arg2 = attribute name
    emit_post_push_reg(emit, REG_RET);
}

void emit_load_method(emitter_t *emit, qstr qstr) {
    emit_pre_pop_reg(emit, REG_ARG_1); // arg1 = base
    emit_get_stack_pointer_to_reg_for_push(emit, REG_ARG_3, 2); // arg3 = dest ptr
    emit_call_with_i32_arg(emit, RT_F_LOAD_METHOD, qstr, REG_ARG_2); // arg2 = method name
}

void emit_load_build_class(emitter_t *emit) {
   assert(0);
} // basically load __build_class__ from builtins

void emit_store_fast(emitter_t *emit, qstr qstr, int local_num) {
    if (local_num == 0) {
        emit_pre_pop_reg(emit, REG_LOCAL_1);
    } else if (local_num == 1) {
        emit_pre_pop_reg(emit, REG_LOCAL_2);
    } else if (local_num == 2) {
        emit_pre_pop_reg(emit, REG_LOCAL_3);
    } else {
        emit_pre_pop_reg(emit, REG_R0);
        asm_thumb_mov_local_reg(emit->as, local_num - 1, REG_R0);
    }
    emit_post(emit);
}

void emit_store_name(emitter_t *emit, qstr qstr) {
    emit_pre_pop_reg(emit, REG_ARG_2);
    emit_call_with_i32_arg(emit, RT_F_STORE_NAME, qstr, REG_ARG_1); // arg1 = name
    emit_post(emit);
}

void emit_store_global(emitter_t *emit, qstr qstr) {
    assert(0);
}

void emit_store_deref(emitter_t *emit, qstr qstr) {
    assert(0);
}
void emit_store_attr(emitter_t *emit, qstr qstr) {
    assert(0);
}
void emit_store_locals(emitter_t *emit) {
    assert(0);
}

void emit_store_subscr(emitter_t *emit) {
    emit_pre_pop_reg_reg_reg(emit, REG_ARG_2, REG_ARG_1, REG_ARG_3); // index, base, value to store
    emit_call(emit, RT_F_STORE_SUBSCR);
}

void emit_delete_fast(emitter_t *emit, qstr qstr, int local_num) {
    assert(0);
}
void emit_delete_name(emitter_t *emit, qstr qstr) {
    assert(0);
}
void emit_delete_global(emitter_t *emit, qstr qstr) {
    assert(0);
}
void emit_delete_deref(emitter_t *emit, qstr qstr) {
    assert(0);
}
void emit_delete_attr(emitter_t *emit, qstr qstr) {
    assert(0);
}
void emit_delete_subscr(emitter_t *emit) {
    assert(0);
}

void emit_dup_top(emitter_t *emit) {
    emit_pre_pop_reg(emit, REG_R0);
    emit_post_push_reg_reg(emit, REG_R0, REG_R0);
}

void emit_dup_top_two(emitter_t *emit) {
    emit_pre_pop_reg_reg(emit, REG_R0, REG_R1);
    emit_post_push_reg_reg_reg_reg(emit, REG_R1, REG_R0, REG_R1, REG_R0);
}

void emit_pop_top(emitter_t *emit) {
    emit_pre_pop_reg(emit, REG_R0);
    emit_post(emit);
}

void emit_rot_two(emitter_t *emit) {
    assert(0);
}

void emit_rot_three(emitter_t *emit) {
    emit_pre_pop_reg_reg_reg(emit, REG_R0, REG_R1, REG_R2);
    emit_post_push_reg_reg_reg(emit, REG_R0, REG_R2, REG_R1);
}

void emit_jump(emitter_t *emit, int label) {
    emit_pre(emit);
    asm_thumb_b_label(emit->as, label);
    emit_post(emit);
}

void emit_pop_jump_if_false(emitter_t *emit, int label) {
    if (emit->do_native_types) {
        emit_pre_pop_reg(emit, REG_RET);
        asm_thumb_cmp_reg_bz_label(emit->as, REG_RET, label);
        emit_post(emit);
    } else {
        emit_pre_pop_reg(emit, REG_ARG_1);
        emit_call(emit, RT_F_IS_TRUE);
        asm_thumb_cmp_reg_bz_label(emit->as, REG_RET, label);
        emit_post(emit);
    }
}

void emit_pop_jump_if_true(emitter_t *emit, int label) {
    assert(0);
}
void emit_jump_if_true_or_pop(emitter_t *emit, int label) {
    assert(0);
}
void emit_jump_if_false_or_pop(emitter_t *emit, int label) {
    assert(0);
}

void emit_setup_loop(emitter_t *emit, int label) {
    emit_pre(emit);
    emit_post(emit);
}

void emit_break_loop(emitter_t *emit, int label) {
    assert(0);
}
void emit_continue_loop(emitter_t *emit, int label) {
    assert(0);
}
void emit_setup_with(emitter_t *emit, int label) {
    assert(0);
}
void emit_with_cleanup(emitter_t *emit) {
    assert(0);
}
void emit_setup_except(emitter_t *emit, int label) {
    assert(0);
}
void emit_setup_finally(emitter_t *emit, int label) {
    assert(0);
}
void emit_end_finally(emitter_t *emit) {
    assert(0);
}
void emit_get_iter(emitter_t *emit) {
    assert(0);
} // tos = getiter(tos)
void emit_for_iter(emitter_t *emit, int label) {
    assert(0);
}
void emit_for_iter_end(emitter_t *emit) {
    assert(0);
}
void emit_pop_except(emitter_t *emit) {
    assert(0);
}

void emit_unary_op(emitter_t *emit, rt_unary_op_t op) {
    emit_pre_pop_reg(emit, REG_ARG_2);
    emit_call_with_i32_arg(emit, RT_F_UNARY_OP, op, REG_ARG_1);
    emit_post_push_reg(emit, REG_RET);
}

void emit_build_tuple(emitter_t *emit, int n_args) {
    assert(0);
}

void emit_build_list(emitter_t *emit, int n_args) {
    emit_pre(emit);
    emit_get_stack_pointer_to_reg_for_pop(emit, REG_ARG_2, n_args); // pointer to items in reverse order
    emit_call_with_i32_arg(emit, RT_F_BUILD_LIST, n_args, REG_ARG_1);
    emit_post_push_reg(emit, REG_RET); // new list
}

void emit_list_append(emitter_t *emit, int list_index) {
    assert(0);
}

void emit_build_map(emitter_t *emit, int n_args) {
    emit_pre(emit);
    emit_call_with_i32_arg(emit, RT_F_BUILD_MAP, n_args, REG_ARG_1);
    emit_post_push_reg(emit, REG_RET); // new map
}

void emit_store_map(emitter_t *emit) {
    emit_pre_pop_reg_reg_reg(emit, REG_ARG_2, REG_ARG_3, REG_ARG_1); // key, value, map
    emit_call(emit, RT_F_STORE_MAP);
    emit_post_push_reg(emit, REG_RET); // map
}

void emit_map_add(emitter_t *emit, int map_index) {
    assert(0);
}

void emit_build_set(emitter_t *emit, int n_args) {
    emit_pre(emit);
    emit_get_stack_pointer_to_reg_for_pop(emit, REG_ARG_2, n_args); // pointer to items in reverse order
    emit_call_with_i32_arg(emit, RT_F_BUILD_SET, n_args, REG_ARG_1);
    emit_post_push_reg(emit, REG_RET); // new set
}

void emit_set_add(emitter_t *emit, int set_index) {
    assert(0);
}
void emit_build_slice(emitter_t *emit, int n_args) {
    assert(0);
}
void emit_unpack_sequence(emitter_t *emit, int n_args) {
    assert(0);
}
void emit_unpack_ex(emitter_t *emit, int n_left, int n_right) {
    assert(0);
}

void emit_make_function(emitter_t *emit, scope_t *scope, int n_dict_params, int n_default_params) {
    assert(n_default_params == 0 && n_dict_params == 0);
    emit_pre(emit);
    emit_call_with_i32_arg(emit, RT_F_MAKE_FUNCTION_FROM_ID, scope->unique_code_id, REG_ARG_1);
    emit_post_push_reg(emit, REG_RET);
}

void emit_make_closure(emitter_t *emit, scope_t *scope, int n_dict_params, int n_default_params) {
    assert(0);
}

void emit_call_function(emitter_t *emit, int n_positional, int n_keyword, bool have_star_arg, bool have_dbl_star_arg) {
    assert(n_keyword == 0 && !have_star_arg && !have_dbl_star_arg);
    if (n_positional == 0) {
        emit_pre_pop_reg(emit, REG_ARG_1); // the function
        emit_call(emit, RT_F_CALL_FUNCTION_0);
    } else if (n_positional == 1) {
        emit_pre_pop_reg_reg(emit, REG_ARG_2, REG_ARG_1); // the single argument, the function
        emit_call(emit, RT_F_CALL_FUNCTION_1);
    } else if (n_positional == 2) {
        emit_pre_pop_reg_reg_reg(emit, REG_ARG_3, REG_ARG_2, REG_ARG_1); // the second argument, the first argument, the function
        emit_call(emit, RT_F_CALL_FUNCTION_2);
    } else {
        assert(0);
    }
    emit_post_push_reg(emit, REG_RET);
}

void emit_call_method(emitter_t *emit, int n_positional, int n_keyword, bool have_star_arg, bool have_dbl_star_arg) {
    assert(n_keyword == 0 && !have_star_arg && !have_dbl_star_arg);
    if (n_positional == 0) {
        emit_pre_pop_reg_reg(emit, REG_ARG_2, REG_ARG_1); // the self object (or NULL), the method
        emit_call(emit, RT_F_CALL_METHOD_1);
    } else if (n_positional == 1) {
        emit_pre_pop_reg_reg_reg(emit, REG_ARG_3, REG_ARG_2, REG_ARG_1); // the first argument, the self object (or NULL), the method
        emit_call(emit, RT_F_CALL_METHOD_2);
    } else {
        assert(0);
    }
    emit_post_push_reg(emit, REG_RET);
}

void emit_pop_block(emitter_t *emit) {
    emit_pre(emit);
    emit_post(emit);
}

void emit_binary_op(emitter_t *emit, rt_binary_op_t op) {
    if (emit->do_native_types) {
        emit_pre_pop_reg_reg(emit, REG_ARG_2, REG_ARG_1);
        asm_thumb_add_reg_reg_reg(emit->as, REG_RET, REG_ARG_1, REG_ARG_2);
        emit_post_push_reg(emit, REG_RET);
    } else {
        emit_pre_pop_reg_reg(emit, REG_ARG_3, REG_ARG_2);
        emit_call_with_i32_arg(emit, RT_F_BINARY_OP, op, REG_ARG_1);
        emit_post_push_reg(emit, REG_RET);
    }
}

void emit_compare_op(emitter_t *emit, rt_compare_op_t op) {
    if (emit->do_native_types) {
        emit_pre_pop_reg_reg(emit, REG_ARG_2, REG_ARG_1);
        asm_thumb_cmp_reg_reg(emit->as, REG_ARG_1, REG_ARG_2);
        asm_thumb_ite_ge(emit->as);
        asm_thumb_mov_reg_i8(emit->as, REG_RET, 0); // if r0 >= r1
        asm_thumb_mov_reg_i8(emit->as, REG_RET, 1); // if r0 < r1
        emit_post_push_reg(emit, REG_RET);
    } else {
        emit_pre_pop_reg_reg(emit, REG_ARG_3, REG_ARG_2);
        emit_call_with_i32_arg(emit, RT_F_COMPARE_OP, op, REG_ARG_1);
        emit_post_push_reg(emit, REG_RET);
    }
}

void emit_return_value(emitter_t *emit) {
    emit_pre_pop_reg(emit, REG_RET);
    emit->last_emit_was_return_value = true;
    //asm_thumb_call_ind(emit->as, 0, REG_R0); to seg fault for debugging with gdb
    asm_thumb_exit(emit->as);
}

void emit_raise_varargs(emitter_t *emit, int n_args) {
    assert(0);
}
void emit_yield_value(emitter_t *emit) {
    assert(0);
}
void emit_yield_from(emitter_t *emit) {
    assert(0);
}

#endif // EMIT_DO_THUMB
