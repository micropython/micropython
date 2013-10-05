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

#ifdef EMIT_ENABLE_X64

#define REG_LOCAL_1 (REG_RBX)
#define REG_LOCAL_NUM (1)

typedef enum {
    NEED_TO_PUSH_NOTHING,
    NEED_TO_PUSH_R64,
    NEED_TO_PUSH_I64,
} need_to_push_t;

struct _emit_t {
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

static void emit_x64_set_native_types(emit_t *emit, bool do_native_types) {
    emit->do_native_types = do_native_types;
}

static void emit_x64_start_pass(emit_t *emit, pass_kind_t pass, scope_t *scope) {
    emit->pass = pass;
    emit->stack_start = 0;
    emit->stack_size = 0;
    emit->last_emit_was_return_value = false;
    emit->need_to_push = NEED_TO_PUSH_NOTHING;
    emit->scope = scope;

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

static void emit_x64_end_pass(emit_t *emit) {
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

static bool emit_x64_last_emit_was_return_value(emit_t *emit) {
    return emit->last_emit_was_return_value;
}

static int emit_x64_get_stack_size(emit_t *emit) {
    return emit->stack_size;
}

static void emit_x64_set_stack_size(emit_t *emit, int size) {
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

static void emit_pre_raw(emit_t *emit, int stack_size_delta) {
    adjust_stack(emit, stack_size_delta);
    emit->last_emit_was_return_value = false;
}

static void emit_pre(emit_t *emit) {
    stack_settle(emit);
    emit_pre_raw(emit, 0);
}

static void emit_pre_pop_r64(emit_t *emit, int r64) {
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

static void emit_pre_pop_r64_r64(emit_t *emit, int r64a, int r64b) {
    emit_pre_pop_r64(emit, r64a);
    asm_x64_mov_local_to_r64(emit->as, emit->stack_start + emit->stack_size - 1, r64b);
    adjust_stack(emit, -1);
}

static void emit_pre_pop_r64_r64_r64(emit_t *emit, int r64a, int r64b, int r64c) {
    emit_pre_pop_r64(emit, r64a);
    asm_x64_mov_local_to_r64(emit->as, emit->stack_start + emit->stack_size - 1, r64b);
    asm_x64_mov_local_to_r64(emit->as, emit->stack_start + emit->stack_size - 2, r64c);
    adjust_stack(emit, -2);
}

static void emit_post(emit_t *emit) {
}

static void emit_post_push_r64(emit_t *emit, int r64) {
    emit->need_to_push = NEED_TO_PUSH_R64;
    emit->last_r64 = r64;
}

static void emit_post_push_i64(emit_t *emit, int64_t i64) {
    emit->need_to_push = NEED_TO_PUSH_I64;
    emit->last_i64 = i64;
}

static void emit_post_push_r64_r64(emit_t *emit, int r64a, int r64b) {
    asm_x64_mov_r64_to_local(emit->as, r64a, emit->stack_start + emit->stack_size);
    emit->need_to_push = NEED_TO_PUSH_R64;
    emit->last_r64 = r64b;
    adjust_stack(emit, 1);
}

static void emit_post_push_r64_r64_r64(emit_t *emit, int r64a, int r64b, int r64c) {
    asm_x64_mov_r64_to_local(emit->as, r64a, emit->stack_start + emit->stack_size);
    asm_x64_mov_r64_to_local(emit->as, r64b, emit->stack_start + emit->stack_size + 1);
    asm_x64_mov_r64_to_local(emit->as, r64c, emit->stack_start + emit->stack_size + 2);
    adjust_stack(emit, 3);
}

static void emit_post_push_r64_r64_r64_r64(emit_t *emit, int r64a, int r64b, int r64c, int r64d) {
    asm_x64_mov_r64_to_local(emit->as, r64a, emit->stack_start + emit->stack_size);
    asm_x64_mov_r64_to_local(emit->as, r64b, emit->stack_start + emit->stack_size + 1);
    asm_x64_mov_r64_to_local(emit->as, r64c, emit->stack_start + emit->stack_size + 2);
    asm_x64_mov_r64_to_local(emit->as, r64d, emit->stack_start + emit->stack_size + 3);
    adjust_stack(emit, 4);
}

static void emit_get_stack_pointer_to_r64_for_pop(emit_t *emit, int r64, int n_pop) {
    asm_x64_mov_local_addr_to_r64(emit->as, emit->stack_start + emit->stack_size - 1, r64);
    adjust_stack(emit, -n_pop);
}

static void emit_get_stack_pointer_to_r64_for_push(emit_t *emit, int r64, int n_push) {
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

static void emit_x64_label_assign(emit_t *emit, int l) {
    asm_x64_label_assign(emit->as, l);
}

static void emit_x64_import_name(emit_t *emit, qstr qstr) {
    assert(0);
}
static void emit_x64_import_from(emit_t *emit, qstr qstr) {
    assert(0);
}
static void emit_x64_import_star(emit_t *emit) {
    assert(0);
}

static void emit_x64_load_const_tok(emit_t *emit, py_token_kind_t tok) {
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

static void emit_x64_load_const_small_int(emit_t *emit, int arg) {
    emit_pre(emit);
    if (emit->do_native_types) {
        emit_post_push_i64(emit, arg);
    } else {
        emit_post_push_i64(emit, (arg << 1) | 1);
    }
}

static void emit_x64_load_const_int(emit_t *emit, qstr qstr) {
    assert(0);
}
static void emit_x64_load_const_dec(emit_t *emit, qstr qstr) {
    assert(0);
}
static void emit_x64_load_const_id(emit_t *emit, qstr qstr) {
    assert(0);
}

static void emit_x64_load_const_str(emit_t *emit, qstr qstr, bool bytes) {
    emit_pre(emit);
    emit_call_with_i64_arg(emit, rt_load_const_str, qstr, REG_ARG_1);
    emit_post_push_r64(emit, REG_RET);
}

static void emit_x64_load_const_verbatim_start(emit_t *emit) {
    assert(0);
}
static void emit_x64_load_const_verbatim_int(emit_t *emit, int val) {
    assert(0);
}
static void emit_x64_load_const_verbatim_str(emit_t *emit, const char *str) {
    assert(0);
}
static void emit_x64_load_const_verbatim_strn(emit_t *emit, const char *str, int len) {
    assert(0);
}
static void emit_x64_load_const_verbatim_quoted_str(emit_t *emit, qstr qstr, bool bytes) {
    assert(0);
}
static void emit_x64_load_const_verbatim_end(emit_t *emit) {
    assert(0);
}

static void emit_x64_load_fast(emit_t *emit, qstr qstr, int local_num) {
    if (local_num == 0) {
        emit_pre(emit);
        emit_post_push_r64(emit, REG_LOCAL_1);
    } else {
        emit_pre(emit);
        asm_x64_mov_local_to_r64(emit->as, local_num - 1, REG_RAX);
        emit_post_push_r64(emit, REG_RAX);
    }
}

static void emit_x64_load_name(emit_t *emit, qstr qstr) {
    emit_pre(emit);
    emit_call_with_i64_arg(emit, rt_load_name, qstr, REG_ARG_1);
    emit_post_push_r64(emit, REG_RET);
}

static void emit_x64_load_global(emit_t *emit, qstr qstr) {
    emit_pre(emit);
    emit_call_with_i64_arg(emit, rt_load_global, qstr, REG_ARG_1);
    emit_post_push_r64(emit, REG_RET);
}

static void emit_x64_load_deref(emit_t *emit, qstr qstr) {
    assert(0);
}
static void emit_x64_load_closure(emit_t *emit, qstr qstr) {
    assert(0);
}

static void emit_x64_load_attr(emit_t *emit, qstr qstr) {
    emit_pre_pop_r64(emit, REG_ARG_1); // arg1 = base
    emit_call_with_i64_arg(emit, rt_load_attr, qstr, REG_ARG_2); // arg2 = attribute name
    emit_post_push_r64(emit, REG_RET);
}

static void emit_x64_load_method(emit_t *emit, qstr qstr) {
    emit_pre_pop_r64(emit, REG_ARG_1); // arg1 = base
    emit_get_stack_pointer_to_r64_for_push(emit, REG_ARG_3, 2); // arg3 = dest ptr
    emit_call_with_i64_arg(emit, rt_load_method, qstr, REG_ARG_2); // arg2 = method name
}

static void emit_x64_load_build_class(emit_t *emit) {
   assert(0);
} // basically load __build_class__ from builtins

static void emit_x64_store_fast(emit_t *emit, qstr qstr, int local_num) {
    if (local_num == 0) {
        emit_pre_pop_r64(emit, REG_LOCAL_1);
        emit_post(emit);
    } else {
        emit_pre_pop_r64(emit, REG_RAX);
        asm_x64_mov_r64_to_local(emit->as, REG_RAX, local_num - 1);
        emit_post(emit);
    }
}

static void emit_x64_store_name(emit_t *emit, qstr qstr) {
    emit_pre_pop_r64(emit, REG_ARG_2);
    emit_call_with_i64_arg(emit, rt_store_name, qstr, REG_ARG_1); // arg1 = name
    emit_post(emit);
}

static void emit_x64_store_global(emit_t *emit, qstr qstr) {
    assert(0);
}

static void emit_x64_store_deref(emit_t *emit, qstr qstr) {
    assert(0);
}
static void emit_x64_store_attr(emit_t *emit, qstr qstr) {
    assert(0);
}
static void emit_x64_store_locals(emit_t *emit) {
    assert(0);
}

static void emit_x64_store_subscr(emit_t *emit) {
    emit_pre_pop_r64_r64_r64(emit, REG_ARG_2, REG_ARG_1, REG_ARG_3); // index, base, value to store
    emit_call(emit, rt_store_subscr);
}

static void emit_x64_delete_fast(emit_t *emit, qstr qstr, int local_num) {
    assert(0);
}
static void emit_x64_delete_name(emit_t *emit, qstr qstr) {
    assert(0);
}
static void emit_x64_delete_global(emit_t *emit, qstr qstr) {
    assert(0);
}
static void emit_x64_delete_deref(emit_t *emit, qstr qstr) {
    assert(0);
}
static void emit_x64_delete_attr(emit_t *emit, qstr qstr) {
    assert(0);
}
static void emit_x64_delete_subscr(emit_t *emit) {
    assert(0);
}

static void emit_x64_dup_top(emit_t *emit) {
    emit_pre_pop_r64(emit, REG_RAX);
    emit_post_push_r64_r64(emit, REG_RAX, REG_RAX);
}

static void emit_x64_dup_top_two(emit_t *emit) {
    emit_pre_pop_r64_r64(emit, REG_RAX, REG_RDI);
    emit_post_push_r64_r64_r64_r64(emit, REG_RDI, REG_RAX, REG_RDI, REG_RAX);
}

static void emit_x64_pop_top(emit_t *emit) {
    emit_pre_pop_r64(emit, REG_RAX);
    emit_post(emit);
}

static void emit_x64_rot_two(emit_t *emit) {
    assert(0);
}

static void emit_x64_rot_three(emit_t *emit) {
    emit_pre_pop_r64_r64_r64(emit, REG_RAX, REG_RDI, REG_RSI);
    emit_post_push_r64_r64_r64(emit, REG_RAX, REG_RSI, REG_RDI);
}

static void emit_x64_jump(emit_t *emit, int label) {
    emit_pre(emit);
    asm_x64_jmp_label(emit->as, label);
    emit_post(emit);
}

static void emit_x64_pop_jump_if_false(emit_t *emit, int label) {
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

static void emit_x64_pop_jump_if_true(emit_t *emit, int label) {
    assert(0);
}
static void emit_x64_jump_if_true_or_pop(emit_t *emit, int label) {
    assert(0);
}
static void emit_x64_jump_if_false_or_pop(emit_t *emit, int label) {
    assert(0);
}

static void emit_x64_setup_loop(emit_t *emit, int label) {
    emit_pre(emit);
    emit_post(emit);
}

static void emit_x64_break_loop(emit_t *emit, int label) {
    assert(0);
}
static void emit_x64_continue_loop(emit_t *emit, int label) {
    assert(0);
}
static void emit_x64_setup_with(emit_t *emit, int label) {
    assert(0);
}
static void emit_x64_with_cleanup(emit_t *emit) {
    assert(0);
}
static void emit_x64_setup_except(emit_t *emit, int label) {
    assert(0);
}
static void emit_x64_setup_finally(emit_t *emit, int label) {
    assert(0);
}
static void emit_x64_end_finally(emit_t *emit) {
    assert(0);
}
static void emit_x64_get_iter(emit_t *emit) {
    assert(0);
} // tos = getiter(tos)
static void emit_x64_for_iter(emit_t *emit, int label) {
    assert(0);
}
static void emit_x64_for_iter_end(emit_t *emit) {
    assert(0);
}
static void emit_x64_pop_except(emit_t *emit) {
    assert(0);
}

static void emit_x64_unary_op(emit_t *emit, rt_unary_op_t op) {
    emit_pre_pop_r64(emit, REG_ARG_2);
    emit_call_with_i64_arg(emit, rt_unary_op, op, REG_ARG_1);
    emit_post_push_r64(emit, REG_RET);
}

static void emit_x64_build_tuple(emit_t *emit, int n_args) {
    assert(0);
}

static void emit_x64_build_list(emit_t *emit, int n_args) {
    emit_pre(emit);
    emit_get_stack_pointer_to_r64_for_pop(emit, REG_ARG_2, n_args); // pointer to items in reverse order
    emit_call_with_i64_arg(emit, rt_build_list, n_args, REG_ARG_1);
    emit_post_push_r64(emit, REG_RET); // new list
}

static void emit_x64_list_append(emit_t *emit, int list_index) {
    assert(0);
}

static void emit_x64_build_map(emit_t *emit, int n_args) {
    emit_pre(emit);
    emit_call_with_i64_arg(emit, rt_build_map, n_args, REG_ARG_1);
    emit_post_push_r64(emit, REG_RET); // new map
}

static void emit_x64_store_map(emit_t *emit) {
    emit_pre_pop_r64_r64_r64(emit, REG_ARG_2, REG_ARG_3, REG_ARG_1); // key, value, map
    emit_call(emit, rt_store_map);
    emit_post_push_r64(emit, REG_RET); // map
}

static void emit_x64_map_add(emit_t *emit, int map_index) {
    assert(0);
}

static void emit_x64_build_set(emit_t *emit, int n_args) {
    emit_pre(emit);
    emit_get_stack_pointer_to_r64_for_pop(emit, REG_ARG_2, n_args); // pointer to items in reverse order
    emit_call_with_i64_arg(emit, rt_build_set, n_args, REG_ARG_1);
    emit_post_push_r64(emit, REG_RET); // new set
}

static void emit_x64_set_add(emit_t *emit, int set_index) {
    assert(0);
}
static void emit_x64_build_slice(emit_t *emit, int n_args) {
    assert(0);
}
static void emit_x64_unpack_sequence(emit_t *emit, int n_args) {
    assert(0);
}
static void emit_x64_unpack_ex(emit_t *emit, int n_left, int n_right) {
    assert(0);
}

static void emit_x64_make_function(emit_t *emit, scope_t *scope, int n_dict_params, int n_default_params) {
    assert(n_default_params == 0 && n_dict_params == 0);
    emit_pre(emit);
    emit_call_with_i64_arg(emit, rt_make_function_from_id, scope->unique_code_id, REG_ARG_1);
    emit_post_push_r64(emit, REG_RET);
}

static void emit_x64_make_closure(emit_t *emit, scope_t *scope, int n_dict_params, int n_default_params) {
    assert(0);
}

static void emit_x64_call_function(emit_t *emit, int n_positional, int n_keyword, bool have_star_arg, bool have_dbl_star_arg) {
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

static void emit_x64_call_method(emit_t *emit, int n_positional, int n_keyword, bool have_star_arg, bool have_dbl_star_arg) {
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

static void emit_x64_pop_block(emit_t *emit) {
    emit_pre(emit);
    emit_post(emit);
}

static void emit_x64_binary_op(emit_t *emit, rt_binary_op_t op) {
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

static void emit_x64_compare_op(emit_t *emit, rt_compare_op_t op) {
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

static void emit_x64_return_value(emit_t *emit) {
    emit_pre_pop_r64(emit, REG_RAX);
    emit->last_emit_was_return_value = true;
    //asm_x64_call_ind(emit->as, 0, REG_RAX); to seg fault for debugging with gdb
    asm_x64_exit(emit->as);
}

static void emit_x64_raise_varargs(emit_t *emit, int n_args) {
    assert(0);
}
static void emit_x64_yield_value(emit_t *emit) {
    assert(0);
}
static void emit_x64_yield_from(emit_t *emit) {
    assert(0);
}

static const emit_method_table_t emit_x64_method_table = {
    emit_x64_set_native_types,
    emit_x64_start_pass,
    emit_x64_end_pass,
    emit_x64_last_emit_was_return_value,
    emit_x64_get_stack_size,
    emit_x64_set_stack_size,

    emit_x64_label_assign,
    emit_x64_import_name,
    emit_x64_import_from,
    emit_x64_import_star,
    emit_x64_load_const_tok,
    emit_x64_load_const_small_int,
    emit_x64_load_const_int,
    emit_x64_load_const_dec,
    emit_x64_load_const_id,
    emit_x64_load_const_str,
    emit_x64_load_const_verbatim_start,
    emit_x64_load_const_verbatim_int,
    emit_x64_load_const_verbatim_str,
    emit_x64_load_const_verbatim_strn,
    emit_x64_load_const_verbatim_quoted_str,
    emit_x64_load_const_verbatim_end,
    emit_x64_load_fast,
    emit_x64_load_name,
    emit_x64_load_global,
    emit_x64_load_deref,
    emit_x64_load_closure,
    emit_x64_load_attr,
    emit_x64_load_method,
    emit_x64_load_build_class,
    emit_x64_store_fast,
    emit_x64_store_name,
    emit_x64_store_global,
    emit_x64_store_deref,
    emit_x64_store_attr,
    emit_x64_store_locals,
    emit_x64_store_subscr,
    emit_x64_delete_fast,
    emit_x64_delete_name,
    emit_x64_delete_global,
    emit_x64_delete_deref,
    emit_x64_delete_attr,
    emit_x64_delete_subscr,
    emit_x64_dup_top,
    emit_x64_dup_top_two,
    emit_x64_pop_top,
    emit_x64_rot_two,
    emit_x64_rot_three,
    emit_x64_jump,
    emit_x64_pop_jump_if_true,
    emit_x64_pop_jump_if_false,
    emit_x64_jump_if_true_or_pop,
    emit_x64_jump_if_false_or_pop,
    emit_x64_setup_loop,
    emit_x64_break_loop,
    emit_x64_continue_loop,
    emit_x64_setup_with,
    emit_x64_with_cleanup,
    emit_x64_setup_except,
    emit_x64_setup_finally,
    emit_x64_end_finally,
    emit_x64_get_iter,
    emit_x64_for_iter,
    emit_x64_for_iter_end,
    emit_x64_pop_block,
    emit_x64_pop_except,
    emit_x64_unary_op,
    emit_x64_binary_op,
    emit_x64_compare_op,
    emit_x64_build_tuple,
    emit_x64_build_list,
    emit_x64_list_append,
    emit_x64_build_map,
    emit_x64_store_map,
    emit_x64_map_add,
    emit_x64_build_set,
    emit_x64_set_add,
    emit_x64_build_slice,
    emit_x64_unpack_sequence,
    emit_x64_unpack_ex,
    emit_x64_make_function,
    emit_x64_make_closure,
    emit_x64_call_function,
    emit_x64_call_method,
    emit_x64_return_value,
    emit_x64_raise_varargs,
    emit_x64_yield_value,
    emit_x64_yield_from,
};

void emit_x64_new(emit_t **emit_out, const emit_method_table_t **emit_method_table_out, uint max_num_labels) {
    emit_t *emit = m_new(emit_t, 1);
    emit->as = asm_x64_new(max_num_labels);
    emit->do_native_types = false;

    *emit_out = emit;
    *emit_method_table_out = &emit_x64_method_table;
}

#endif // EMIT_ENABLE_X64
