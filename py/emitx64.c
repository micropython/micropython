/* This code is equivalent to emitx64.c but pre-allocates stack
 * space and uses mov instead of push/pop instructions to access
 * the temporary stack.  It runs in similar time, but uses 3*n
 * more bytes, where n is number of push/pop instructions.
 *
 * This code is preferred because it keeps the stack aligned on a
 * 16 byte boundary.
 *
 * Improvements:
 *  Doesn't call stub functions, does all the work inline.
 *  Has optimisations for loading i64s to stack.
 */

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

#ifdef EMIT_DO_X64

#define REG_LOCAL_1 (REG_RBX)
#define REG_LOCAL_NUM (1)

typedef enum {
    NEED_TO_PUSH_NOTHING,
    NEED_TO_PUSH_R64,
    NEED_TO_PUSH_I64,
} need_to_push_t;

struct _emitter_t {
    int pass;
    int stack_start;
    int stack_size;
    bool last_emit_was_return_value;
    need_to_push_t need_to_push;
    int last_r64;
    int64_t last_i64;

    scope_t *scope;

    asm_x64_t *as;
    bool do_native_types;
};

emitter_t *emit_new() {
    emitter_t *emit = m_new(emitter_t, 1);
    emit->as = asm_x64_new();
    emit->do_native_types = false;
    return emit;
}

void emit_set_native_types(emitter_t *emit, bool do_native_types) {
    emit->do_native_types = do_native_types;
}

void emit_start_pass(emitter_t *emit, pass_kind_t pass, scope_t *scope) {
    emit->pass = pass;
    emit->stack_start = 0;
    emit->stack_size = 0;
    emit->last_emit_was_return_value = false;
    emit->need_to_push = NEED_TO_PUSH_NOTHING;
    emit->scope = scope;
    if (pass == PASS_1) {
        scope->unique_code_id = rt_get_new_unique_code_id();
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

void emit_end_pass(emitter_t *emit) {
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

        case NEED_TO_PUSH_R64:
            asm_x64_mov_r64_to_local(emit->as, emit->last_r64, emit->stack_start + emit->stack_size);
            adjust_stack(emit, 1);
            break;

        case NEED_TO_PUSH_I64:
            asm_x64_mov_i64_to_r64_optimised(emit->as, emit->last_i64, REG_RAX);
            asm_x64_mov_r64_to_local(emit->as, REG_RAX, emit->stack_start + emit->stack_size);
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

static void emit_pre_pop_r64(emitter_t *emit, int r64) {
    switch (emit->need_to_push) {
        case NEED_TO_PUSH_NOTHING:
            asm_x64_mov_local_to_r64(emit->as, emit->stack_start + emit->stack_size - 1, r64);
            emit_pre_raw(emit, -1);
            break;

        case NEED_TO_PUSH_R64:
            emit_pre_raw(emit, 0);
            if (emit->last_r64 != r64) {
                asm_x64_mov_r64_to_r64(emit->as, emit->last_r64, r64);
            }
            break;

        case NEED_TO_PUSH_I64:
            emit_pre_raw(emit, 0);
            asm_x64_mov_i64_to_r64_optimised(emit->as, emit->last_i64, r64);
            break;
    }
    emit->need_to_push = NEED_TO_PUSH_NOTHING;
}

static void emit_pre_pop_r64_r64(emitter_t *emit, int r64a, int r64b) {
    emit_pre_pop_r64(emit, r64a);
    asm_x64_mov_local_to_r64(emit->as, emit->stack_start + emit->stack_size - 1, r64b);
    adjust_stack(emit, -1);
}

static void emit_pre_pop_r64_r64_r64(emitter_t *emit, int r64a, int r64b, int r64c) {
    emit_pre_pop_r64(emit, r64a);
    asm_x64_mov_local_to_r64(emit->as, emit->stack_start + emit->stack_size - 1, r64b);
    asm_x64_mov_local_to_r64(emit->as, emit->stack_start + emit->stack_size - 2, r64c);
    adjust_stack(emit, -2);
}

static void emit_post(emitter_t *emit) {
}

static void emit_post_push_r64(emitter_t *emit, int r64) {
    emit->need_to_push = NEED_TO_PUSH_R64;
    emit->last_r64 = r64;
}

static void emit_post_push_i64(emitter_t *emit, int64_t i64) {
    emit->need_to_push = NEED_TO_PUSH_I64;
    emit->last_i64 = i64;
}

static void emit_post_push_r64_r64(emitter_t *emit, int r64a, int r64b) {
    asm_x64_mov_r64_to_local(emit->as, r64a, emit->stack_start + emit->stack_size);
    emit->need_to_push = NEED_TO_PUSH_R64;
    emit->last_r64 = r64b;
    adjust_stack(emit, 1);
}

static void emit_post_push_r64_r64_r64(emitter_t *emit, int r64a, int r64b, int r64c) {
    asm_x64_mov_r64_to_local(emit->as, r64a, emit->stack_start + emit->stack_size);
    asm_x64_mov_r64_to_local(emit->as, r64b, emit->stack_start + emit->stack_size + 1);
    asm_x64_mov_r64_to_local(emit->as, r64c, emit->stack_start + emit->stack_size + 2);
    adjust_stack(emit, 3);
}

static void emit_post_push_r64_r64_r64_r64(emitter_t *emit, int r64a, int r64b, int r64c, int r64d) {
    asm_x64_mov_r64_to_local(emit->as, r64a, emit->stack_start + emit->stack_size);
    asm_x64_mov_r64_to_local(emit->as, r64b, emit->stack_start + emit->stack_size + 1);
    asm_x64_mov_r64_to_local(emit->as, r64c, emit->stack_start + emit->stack_size + 2);
    asm_x64_mov_r64_to_local(emit->as, r64d, emit->stack_start + emit->stack_size + 3);
    adjust_stack(emit, 4);
}

static void emit_get_stack_pointer_to_r64_for_pop(emitter_t *emit, int r64, int n_pop) {
    asm_x64_mov_local_addr_to_r64(emit->as, emit->stack_start + emit->stack_size - 1, r64);
    adjust_stack(emit, -n_pop);
}

static void emit_get_stack_pointer_to_r64_for_push(emitter_t *emit, int r64, int n_push) {
    asm_x64_mov_local_addr_to_r64(emit->as, emit->stack_start + emit->stack_size + n_push - 1, r64);
    adjust_stack(emit, n_push);
}

static void emit_call(emitter_t *emit, void *fun) {
    asm_x64_call_ind(emit->as, fun, REG_RAX);
}

static void emit_call_with_i64_arg(emitter_t *emit, void *fun, int64_t arg_val, int arg_r64) {
    asm_x64_mov_i64_to_r64_optimised(emit->as, arg_val, arg_r64);
    asm_x64_call_ind(emit->as, fun, REG_RAX);
}

int emit_label_new(emitter_t *emit) {
    return asm_x64_label_new(emit->as);
}

void emit_label_assign(emitter_t *emit, int l) {
    asm_x64_label_assign(emit->as, l);
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
        default: assert(0); // shouldn't happen
    }
    emit_post_push_i64(emit, (uint64_t)o);
}

void emit_load_const_small_int(emitter_t *emit, int arg) {
    emit_pre(emit);
    if (emit->do_native_types) {
        emit_post_push_i64(emit, arg);
    } else {
        emit_post_push_i64(emit, (arg << 1) | 1);
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
    emit_call_with_i64_arg(emit, rt_load_const_str, qstr, REG_ARG_1);
    emit_post_push_r64(emit, REG_RET);
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
    if (local_num == 0) {
        emit_pre(emit);
        emit_post_push_r64(emit, REG_LOCAL_1);
    } else {
        emit_pre(emit);
        asm_x64_mov_local_to_r64(emit->as, local_num - 1, REG_RAX);
        emit_post_push_r64(emit, REG_RAX);
    }
}

void emit_load_name(emitter_t *emit, qstr qstr) {
    emit_pre(emit);
    emit_call_with_i64_arg(emit, rt_load_name, qstr, REG_ARG_1);
    emit_post_push_r64(emit, REG_RET);
}

void emit_load_global(emitter_t *emit, qstr qstr) {
    emit_pre(emit);
    emit_call_with_i64_arg(emit, rt_load_global, qstr, REG_ARG_1);
    emit_post_push_r64(emit, REG_RET);
}

void emit_load_deref(emitter_t *emit, qstr qstr) {
    assert(0);
}
void emit_load_closure(emitter_t *emit, qstr qstr) {
    assert(0);
}

void emit_load_attr(emitter_t *emit, qstr qstr) {
    emit_pre_pop_r64(emit, REG_ARG_1); // arg1 = base
    emit_call_with_i64_arg(emit, rt_load_attr, qstr, REG_ARG_2); // arg2 = attribute name
    emit_post_push_r64(emit, REG_RET);
}

void emit_load_method(emitter_t *emit, qstr qstr) {
    emit_pre_pop_r64(emit, REG_ARG_1); // arg1 = base
    emit_get_stack_pointer_to_r64_for_push(emit, REG_ARG_3, 2); // arg3 = dest ptr
    emit_call_with_i64_arg(emit, rt_load_method, qstr, REG_ARG_2); // arg2 = method name
}

void emit_load_build_class(emitter_t *emit) {
   assert(0);
} // basically load __build_class__ from builtins

void emit_store_fast(emitter_t *emit, qstr qstr, int local_num) {
    if (local_num == 0) {
        emit_pre_pop_r64(emit, REG_LOCAL_1);
        emit_post(emit);
    } else {
        emit_pre_pop_r64(emit, REG_RAX);
        asm_x64_mov_r64_to_local(emit->as, REG_RAX, local_num - 1);
        emit_post(emit);
    }
}

void emit_store_name(emitter_t *emit, qstr qstr) {
    emit_pre_pop_r64(emit, REG_ARG_2);
    emit_call_with_i64_arg(emit, rt_store_name, qstr, REG_ARG_1); // arg1 = name
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
    emit_pre_pop_r64_r64_r64(emit, REG_ARG_2, REG_ARG_1, REG_ARG_3); // index, base, value to store
    emit_call(emit, rt_store_subscr);
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
    emit_pre_pop_r64(emit, REG_RAX);
    emit_post_push_r64_r64(emit, REG_RAX, REG_RAX);
}

void emit_dup_top_two(emitter_t *emit) {
    emit_pre_pop_r64_r64(emit, REG_RAX, REG_RDI);
    emit_post_push_r64_r64_r64_r64(emit, REG_RDI, REG_RAX, REG_RDI, REG_RAX);
}

void emit_pop_top(emitter_t *emit) {
    emit_pre_pop_r64(emit, REG_RAX);
    emit_post(emit);
}

void emit_rot_two(emitter_t *emit) {
    assert(0);
}

void emit_rot_three(emitter_t *emit) {
    emit_pre_pop_r64_r64_r64(emit, REG_RAX, REG_RDI, REG_RSI);
    emit_post_push_r64_r64_r64(emit, REG_RAX, REG_RSI, REG_RDI);
}

void emit_jump(emitter_t *emit, int label) {
    emit_pre(emit);
    asm_x64_jmp_label(emit->as, label);
    emit_post(emit);
}

void emit_pop_jump_if_false(emitter_t *emit, int label) {
    if (emit->do_native_types) {
        emit_pre_pop_r64(emit, REG_RET);
        asm_x64_test_r8_with_r8(emit->as, REG_RET, REG_RET);
        asm_x64_jcc_label(emit->as, JCC_JZ, label);
        emit_post(emit);
    } else {
        emit_pre_pop_r64(emit, REG_ARG_1);
        emit_call(emit, rt_is_true);
        asm_x64_test_r8_with_r8(emit->as, REG_RET, REG_RET);
        asm_x64_jcc_label(emit->as, JCC_JZ, label);
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
    emit_pre_pop_r64(emit, REG_ARG_2);
    emit_call_with_i64_arg(emit, rt_unary_op, op, REG_ARG_1);
    emit_post_push_r64(emit, REG_RET);
}

void emit_build_tuple(emitter_t *emit, int n_args) {
    assert(0);
}

void emit_build_list(emitter_t *emit, int n_args) {
    emit_pre(emit);
    emit_get_stack_pointer_to_r64_for_pop(emit, REG_ARG_2, n_args); // pointer to items in reverse order
    emit_call_with_i64_arg(emit, rt_build_list, n_args, REG_ARG_1);
    emit_post_push_r64(emit, REG_RET); // new list
}

void emit_list_append(emitter_t *emit, int list_index) {
    assert(0);
}

void emit_build_map(emitter_t *emit, int n_args) {
    emit_pre(emit);
    emit_call_with_i64_arg(emit, rt_build_map, n_args, REG_ARG_1);
    emit_post_push_r64(emit, REG_RET); // new map
}

void emit_store_map(emitter_t *emit) {
    emit_pre_pop_r64_r64_r64(emit, REG_ARG_2, REG_ARG_3, REG_ARG_1); // key, value, map
    emit_call(emit, rt_store_map);
    emit_post_push_r64(emit, REG_RET); // map
}

void emit_map_add(emitter_t *emit, int map_index) {
    assert(0);
}

void emit_build_set(emitter_t *emit, int n_args) {
    emit_pre(emit);
    emit_get_stack_pointer_to_r64_for_pop(emit, REG_ARG_2, n_args); // pointer to items in reverse order
    emit_call_with_i64_arg(emit, rt_build_set, n_args, REG_ARG_1);
    emit_post_push_r64(emit, REG_RET); // new set
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
    emit_call_with_i64_arg(emit, rt_make_function_from_id, scope->unique_code_id, REG_ARG_1);
    emit_post_push_r64(emit, REG_RET);
}

void emit_make_closure(emitter_t *emit, scope_t *scope, int n_dict_params, int n_default_params) {
    assert(0);
}

void emit_call_function(emitter_t *emit, int n_positional, int n_keyword, bool have_star_arg, bool have_dbl_star_arg) {
    assert(n_keyword == 0 && !have_star_arg && !have_dbl_star_arg);
    if (n_positional == 0) {
        emit_pre_pop_r64(emit, REG_ARG_1); // the function
        emit_call(emit, rt_call_function_0);
    } else if (n_positional == 1) {
        emit_pre_pop_r64_r64(emit, REG_ARG_2, REG_ARG_1); // the single argument, the function
        emit_call(emit, rt_call_function_1);
    } else if (n_positional == 2) {
        emit_pre_pop_r64_r64_r64(emit, REG_ARG_3, REG_ARG_2, REG_ARG_1); // the second argument, the first argument, the function
        emit_call(emit, rt_call_function_2);
    } else {
        assert(0);
    }
    emit_post_push_r64(emit, REG_RET);
}

void emit_call_method(emitter_t *emit, int n_positional, int n_keyword, bool have_star_arg, bool have_dbl_star_arg) {
    assert(n_keyword == 0 && !have_star_arg && !have_dbl_star_arg);
    if (n_positional == 0) {
        emit_pre_pop_r64_r64(emit, REG_ARG_2, REG_ARG_1); // the self object (or NULL), the method
        emit_call(emit, rt_call_method_1);
    } else if (n_positional == 1) {
        emit_pre_pop_r64_r64_r64(emit, REG_ARG_3, REG_ARG_2, REG_ARG_1); // the first argument, the self object (or NULL), the method
        emit_call(emit, rt_call_method_2);
    } else {
        assert(0);
    }
    emit_post_push_r64(emit, REG_RET);
}

void emit_pop_block(emitter_t *emit) {
    emit_pre(emit);
    emit_post(emit);
}

void emit_binary_op(emitter_t *emit, rt_binary_op_t op) {
    if (emit->do_native_types) {
        assert(op == RT_BINARY_OP_ADD);
        emit_pre_pop_r64_r64(emit, REG_ARG_2, REG_RET);
        asm_x64_add_r64_to_r64(emit->as, REG_ARG_2, REG_RET);
        emit_post_push_r64(emit, REG_RET);
    } else {
        emit_pre_pop_r64_r64(emit, REG_ARG_3, REG_ARG_2);
        emit_call_with_i64_arg(emit, rt_binary_op, op, REG_ARG_1);
        emit_post_push_r64(emit, REG_RET);
    }
}

void emit_compare_op(emitter_t *emit, rt_compare_op_t op) {
    if (emit->do_native_types) {
        assert(op == RT_COMPARE_OP_LESS);
        emit_pre_pop_r64_r64(emit, REG_ARG_3, REG_ARG_2);
        asm_x64_xor_r64_to_r64(emit->as, REG_RET, REG_RET);
        asm_x64_cmp_r64_with_r64(emit->as, REG_ARG_3, REG_ARG_2);
        asm_x64_setcc_r8(emit->as, JCC_JL, REG_RET);
        emit_post_push_r64(emit, REG_RET);
    } else {
        emit_pre_pop_r64_r64(emit, REG_ARG_3, REG_ARG_2);
        emit_call_with_i64_arg(emit, rt_compare_op, op, REG_ARG_1);
        emit_post_push_r64(emit, REG_RET);
    }
}

void emit_return_value(emitter_t *emit) {
    emit_pre_pop_r64(emit, REG_RAX);
    emit->last_emit_was_return_value = true;
    //asm_x64_call_ind(emit->as, 0, REG_RAX); to seg fault for debugging with gdb
    asm_x64_exit(emit->as);
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

#endif // EMIT_DO_X64
