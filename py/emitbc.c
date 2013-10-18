#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "misc.h"
#include "mpyconfig.h"
#include "lexer.h"
#include "parse.h"
#include "compile.h"
#include "scope.h"
#include "runtime.h"
#include "emit.h"
#include "bc.h"

struct _emit_t {
    pass_kind_t pass;
    int stack_size;
    bool last_emit_was_return_value;

    scope_t *scope;

    uint max_num_labels;
    uint *label_offsets;

    uint code_offset;
    uint code_size;
    byte *code_base;
    byte dummy_data[8];
};

emit_t *emit_bc_new(uint max_num_labels) {
    emit_t *emit = m_new(emit_t, 1);
    emit->max_num_labels = max_num_labels;
    emit->label_offsets = m_new(uint, emit->max_num_labels);
    emit->code_offset = 0;
    emit->code_size = 0;
    emit->code_base = NULL;
    return emit;
}

uint emit_bc_get_code_size(emit_t* emit) {
    return emit->code_size;
}

void* emit_bc_get_code(emit_t* emit) {
    return emit->code_base;
}

static void emit_bc_set_native_types(emit_t *emit, bool do_native_types) {
}

static void emit_bc_start_pass(emit_t *emit, pass_kind_t pass, scope_t *scope) {
    emit->pass = pass;
    emit->stack_size = 0;
    emit->last_emit_was_return_value = false;
    emit->scope = scope;
    if (pass == PASS_2) {
        memset(emit->label_offsets, -1, emit->max_num_labels * sizeof(uint));
    }
    emit->code_offset = 0;
}

static void emit_bc_end_pass(emit_t *emit) {
    // check stack is back to zero size
    if (emit->stack_size != 0) {
        printf("ERROR: stack size not back to zero; got %d\n", emit->stack_size);
    }

    if (emit->pass == PASS_2) {
        // calculate size of code in bytes
        emit->code_size = emit->code_offset;
        emit->code_base = m_new(byte, emit->code_size);

    } else if (emit->pass == PASS_3) {
        rt_assign_byte_code(emit->scope->unique_code_id, emit->code_base, emit->code_size, emit->scope->num_params, emit->scope->num_locals, emit->scope->stack_size, (emit->scope->flags & SCOPE_FLAG_GENERATOR) != 0);
    }
}

// all functions must go through this one to emit bytes
static byte* emit_get_cur_to_write_bytes(emit_t* emit, int num_bytes_to_write) {
    //printf("emit %d\n", num_bytes_to_write);
    if (emit->pass < PASS_3) {
        emit->code_offset += num_bytes_to_write;
        return emit->dummy_data;
    } else {
        assert(emit->code_offset + num_bytes_to_write <= emit->code_size);
        byte *c = emit->code_base + emit->code_offset;
        emit->code_offset += num_bytes_to_write;
        return c;
    }
}

static void emit_write_byte_1(emit_t* emit, byte b1) {
    byte* c = emit_get_cur_to_write_bytes(emit, 1);
    c[0] = b1;
}

static void emit_write_byte_1_byte(emit_t* emit, byte b1, uint b2) {
    assert((b2 & (~0xff)) == 0);
    byte* c = emit_get_cur_to_write_bytes(emit, 2);
    c[0] = b1;
    c[1] = b2;
}

static void emit_write_byte_1_int(emit_t* emit, byte b1, int num) {
    assert((num & (~0x7fff)) == 0 || (num & (~0x7fff)) == (~0x7fff));
    byte* c = emit_get_cur_to_write_bytes(emit, 3);
    c[0] = b1;
    c[1] = num;
    c[2] = num >> 8;
}

static void emit_write_byte_1_uint(emit_t* emit, byte b1, uint num) {
    if (num <= 127) { // fits in 0x7f
        // fit argument in single byte
        byte* c = emit_get_cur_to_write_bytes(emit, 2);
        c[0] = b1;
        c[1] = num;
    } else if (num <= 16383) { // fits in 0x3fff
        // fit argument in two bytes
        byte* c = emit_get_cur_to_write_bytes(emit, 3);
        c[0] = b1;
        c[1] = (num >> 8) | 0x80;
        c[2] = num;
    } else {
        // larger numbers not implemented/supported
        assert(0);
    }
}

static void emit_write_byte_1_qstr(emit_t* emit, byte b1, qstr qstr) {
    emit_write_byte_1_uint(emit, b1, qstr);
}

static void emit_write_byte_1_label(emit_t* emit, byte b1, int label) {
    uint code_offset;
    if (emit->pass < PASS_3) {
        code_offset = 0;
    } else {
        code_offset = emit->label_offsets[label];
    }
    emit_write_byte_1_uint(emit, b1, code_offset);
}

bool emit_bc_last_emit_was_return_value(emit_t *emit) {
    return emit->last_emit_was_return_value;
}

int emit_bc_get_stack_size(emit_t *emit) {
    return emit->stack_size;
}

static void emit_bc_set_stack_size(emit_t *emit, int size) {
    emit->stack_size = size;
}

static void emit_bc_load_id(emit_t *emit, qstr qstr) {
    emit_common_load_id(emit, &emit_bc_method_table, emit->scope, qstr);
}

static void emit_bc_store_id(emit_t *emit, qstr qstr) {
    emit_common_store_id(emit, &emit_bc_method_table, emit->scope, qstr);
}

static void emit_bc_delete_id(emit_t *emit, qstr qstr) {
    emit_common_delete_id(emit, &emit_bc_method_table, emit->scope, qstr);
}

static void emit_pre(emit_t *emit, int stack_size_delta) {
    emit->stack_size += stack_size_delta;
    if (emit->stack_size > emit->scope->stack_size) {
        emit->scope->stack_size = emit->stack_size;
    }
    emit->last_emit_was_return_value = false;
}

static void emit_bc_label_assign(emit_t *emit, int l) {
    emit_pre(emit, 0);
    assert(l < emit->max_num_labels);
    if (emit->pass == PASS_2) {
        // assign label offset
        assert(emit->label_offsets[l] == -1);
        emit->label_offsets[l] = emit->code_offset;
    } else if (emit->pass == PASS_3) {
        // ensure label offset has not changed from PASS_2 to PASS_3
        assert(emit->label_offsets[l] == emit->code_offset);
        //printf("l%d: (at %d)\n", l, emit->code_offset);
    }
}

static void emit_bc_import_name(emit_t *emit, qstr qstr) {
    emit_pre(emit, -1);
    emit_write_byte_1_qstr(emit, PYBC_IMPORT_NAME, qstr);
}

static void emit_bc_import_from(emit_t *emit, qstr qstr) {
    emit_pre(emit, 1);
    emit_write_byte_1_qstr(emit, PYBC_IMPORT_FROM, qstr);
}

static void emit_bc_import_star(emit_t *emit) {
    emit_pre(emit, -1);
    emit_write_byte_1(emit, PYBC_IMPORT_STAR);
}

static void emit_bc_load_const_tok(emit_t *emit, py_token_kind_t tok) {
    emit_pre(emit, 1);
    switch (tok) {
        case PY_TOKEN_KW_FALSE: emit_write_byte_1(emit, PYBC_LOAD_CONST_FALSE); break;
        case PY_TOKEN_KW_NONE: emit_write_byte_1(emit, PYBC_LOAD_CONST_NONE); break;
        case PY_TOKEN_KW_TRUE: emit_write_byte_1(emit, PYBC_LOAD_CONST_TRUE); break;
        default: assert(0);
    }
}

static void emit_bc_load_const_small_int(emit_t *emit, int arg) {
    emit_pre(emit, 1);
    emit_write_byte_1_int(emit, PYBC_LOAD_CONST_SMALL_INT, arg);
}

static void emit_bc_load_const_int(emit_t *emit, qstr qstr) {
    emit_pre(emit, 1);
    emit_write_byte_1_qstr(emit, PYBC_LOAD_CONST_INT, qstr);
}

static void emit_bc_load_const_dec(emit_t *emit, qstr qstr) {
    emit_pre(emit, 1);
    emit_write_byte_1_qstr(emit, PYBC_LOAD_CONST_DEC, qstr);
}

static void emit_bc_load_const_id(emit_t *emit, qstr qstr) {
    emit_pre(emit, 1);
    emit_write_byte_1_qstr(emit, PYBC_LOAD_CONST_ID, qstr);
}

static void emit_bc_load_const_str(emit_t *emit, qstr qstr, bool bytes) {
    emit_pre(emit, 1);
    if (bytes) {
        emit_write_byte_1_qstr(emit, PYBC_LOAD_CONST_BYTES, qstr);
    } else {
        emit_write_byte_1_qstr(emit, PYBC_LOAD_CONST_STRING, qstr);
    }
}

static void emit_bc_load_const_verbatim_start(emit_t *emit) {
    emit_pre(emit, 1);
    assert(0);
}

static void emit_bc_load_const_verbatim_int(emit_t *emit, int val) {
    assert(0);
}

static void emit_bc_load_const_verbatim_str(emit_t *emit, const char *str) {
    assert(0);
}

static void emit_bc_load_const_verbatim_strn(emit_t *emit, const char *str, int len) {
    assert(0);
}

static void emit_bc_load_const_verbatim_quoted_str(emit_t *emit, qstr qstr, bool bytes) {
    assert(0);
}

static void emit_bc_load_const_verbatim_end(emit_t *emit) {
    assert(0);
}

static void emit_bc_load_fast(emit_t *emit, qstr qstr, int local_num) {
    assert(local_num >= 0);
    emit_pre(emit, 1);
    switch (local_num) {
        case 0: emit_write_byte_1(emit, PYBC_LOAD_FAST_0); break;
        case 1: emit_write_byte_1(emit, PYBC_LOAD_FAST_1); break;
        case 2: emit_write_byte_1(emit, PYBC_LOAD_FAST_2); break;
        default: emit_write_byte_1_uint(emit, PYBC_LOAD_FAST_N, local_num); break;
    }
}

static void emit_bc_load_name(emit_t *emit, qstr qstr) {
    emit_pre(emit, 1);
    emit_write_byte_1_qstr(emit, PYBC_LOAD_NAME, qstr);
}

static void emit_bc_load_global(emit_t *emit, qstr qstr) {
    emit_pre(emit, 1);
    emit_write_byte_1_qstr(emit, PYBC_LOAD_GLOBAL, qstr);
}

static void emit_bc_load_deref(emit_t *emit, qstr qstr) {
    emit_pre(emit, 1);
    assert(0);
}

static void emit_bc_load_closure(emit_t *emit, qstr qstr) {
    emit_pre(emit, 1);
    assert(0);
}

static void emit_bc_load_attr(emit_t *emit, qstr qstr) {
    emit_pre(emit, 0);
    emit_write_byte_1_qstr(emit, PYBC_LOAD_ATTR, qstr);
}

static void emit_bc_load_method(emit_t *emit, qstr qstr) {
    emit_pre(emit, 0);
    emit_write_byte_1_qstr(emit, PYBC_LOAD_METHOD, qstr);
}

static void emit_bc_load_build_class(emit_t *emit) {
    emit_pre(emit, 1);
    emit_write_byte_1(emit, PYBC_LOAD_BUILD_CLASS);
}

static void emit_bc_store_fast(emit_t *emit, qstr qstr, int local_num) {
    assert(local_num >= 0);
    emit_pre(emit, -1);
    switch (local_num) {
        case 0: emit_write_byte_1(emit, PYBC_STORE_FAST_0); break;
        case 1: emit_write_byte_1(emit, PYBC_STORE_FAST_1); break;
        case 2: emit_write_byte_1(emit, PYBC_STORE_FAST_2); break;
        default: emit_write_byte_1_uint(emit, PYBC_STORE_FAST_N, local_num); break;
    }
}

static void emit_bc_store_name(emit_t *emit, qstr qstr) {
    emit_pre(emit, -1);
    emit_write_byte_1_qstr(emit, PYBC_STORE_NAME, qstr);
}

static void emit_bc_store_global(emit_t *emit, qstr qstr) {
    emit_pre(emit, -1);
    emit_write_byte_1_qstr(emit, PYBC_STORE_GLOBAL, qstr);
}

static void emit_bc_store_deref(emit_t *emit, qstr qstr) {
    emit_pre(emit, -1);
    assert(0);
}

static void emit_bc_store_attr(emit_t *emit, qstr qstr) {
    emit_pre(emit, -2);
    emit_write_byte_1_qstr(emit, PYBC_STORE_ATTR, qstr);
}

static void emit_bc_store_subscr(emit_t *emit) {
    emit_pre(emit, -3);
    emit_write_byte_1(emit, PYBC_STORE_SUBSCR);
}

static void emit_bc_store_locals(emit_t *emit) {
    // not needed
    emit_pre(emit, -1);
    emit_write_byte_1(emit, PYBC_POP_TOP);
}

static void emit_bc_delete_fast(emit_t *emit, qstr qstr, int local_num) {
    assert(local_num >= 0);
    emit_pre(emit, 0);
    emit_write_byte_1_uint(emit, PYBC_DELETE_FAST_N, local_num);
}

static void emit_bc_delete_name(emit_t *emit, qstr qstr) {
    emit_pre(emit, 0);
    emit_write_byte_1_qstr(emit, PYBC_DELETE_NAME, qstr);
}

static void emit_bc_delete_global(emit_t *emit, qstr qstr) {
    emit_pre(emit, 0);
    emit_write_byte_1_qstr(emit, PYBC_DELETE_GLOBAL, qstr);
}

static void emit_bc_delete_deref(emit_t *emit, qstr qstr) {
    emit_pre(emit, 0);
    emit_write_byte_1_qstr(emit, PYBC_DELETE_DEREF, qstr);
}

static void emit_bc_delete_attr(emit_t *emit, qstr qstr) {
    emit_pre(emit, -1);
    emit_write_byte_1_qstr(emit, PYBC_DELETE_ATTR, qstr);
}

static void emit_bc_delete_subscr(emit_t *emit) {
    emit_pre(emit, -2);
    emit_write_byte_1(emit, PYBC_DELETE_SUBSCR);
}

static void emit_bc_dup_top(emit_t *emit) {
    emit_pre(emit, 1);
    emit_write_byte_1(emit, PYBC_DUP_TOP);
}

static void emit_bc_dup_top_two(emit_t *emit) {
    emit_pre(emit, 2);
    emit_write_byte_1(emit, PYBC_DUP_TOP_TWO);
}

static void emit_bc_pop_top(emit_t *emit) {
    emit_pre(emit, -1);
    emit_write_byte_1(emit, PYBC_POP_TOP);
}

static void emit_bc_rot_two(emit_t *emit) {
    emit_pre(emit, 0);
    emit_write_byte_1(emit, PYBC_ROT_TWO);
}

static void emit_bc_rot_three(emit_t *emit) {
    emit_pre(emit, 0);
    emit_write_byte_1(emit, PYBC_ROT_THREE);
}

static void emit_bc_jump(emit_t *emit, int label) {
    emit_pre(emit, 0);
    emit_write_byte_1_label(emit, PYBC_JUMP, label);
}

static void emit_bc_pop_jump_if_true(emit_t *emit, int label) {
    emit_pre(emit, -1);
    emit_write_byte_1_label(emit, PYBC_POP_JUMP_IF_TRUE, label);
}

static void emit_bc_pop_jump_if_false(emit_t *emit, int label) {
    emit_pre(emit, -1);
    emit_write_byte_1_label(emit, PYBC_POP_JUMP_IF_FALSE, label);
}

static void emit_bc_jump_if_true_or_pop(emit_t *emit, int label) {
    emit_pre(emit, -1);
    emit_write_byte_1_label(emit, PYBC_JUMP_IF_TRUE_OR_POP, label);
}

static void emit_bc_jump_if_false_or_pop(emit_t *emit, int label) {
    emit_pre(emit, -1);
    emit_write_byte_1_label(emit, PYBC_JUMP_IF_FALSE_OR_POP, label);
}

static void emit_bc_setup_loop(emit_t *emit, int label) {
    emit_pre(emit, 0);
    emit_write_byte_1_label(emit, PYBC_SETUP_LOOP, label);
}

static void emit_bc_break_loop(emit_t *emit, int label) {
    emit_pre(emit, 0);
    emit_write_byte_1_label(emit, PYBC_BREAK_LOOP, label);
}

static void emit_bc_continue_loop(emit_t *emit, int label) {
    emit_pre(emit, 0);
    emit_write_byte_1_label(emit, PYBC_CONTINUE_LOOP, label);
}

static void emit_bc_setup_with(emit_t *emit, int label) {
    emit_pre(emit, 7);
    emit_write_byte_1_label(emit, PYBC_SETUP_WITH, label);
}

static void emit_bc_with_cleanup(emit_t *emit) {
    emit_pre(emit, -7);
    emit_write_byte_1(emit, PYBC_WITH_CLEANUP);
}

static void emit_bc_setup_except(emit_t *emit, int label) {
    emit_pre(emit, 6);
    emit_write_byte_1_label(emit, PYBC_SETUP_EXCEPT, label);
}

static void emit_bc_setup_finally(emit_t *emit, int label) {
    emit_pre(emit, 6);
    emit_write_byte_1_label(emit, PYBC_SETUP_FINALLY, label);
}

static void emit_bc_end_finally(emit_t *emit) {
    emit_pre(emit, -1);
    emit_write_byte_1(emit, PYBC_END_FINALLY);
}

static void emit_bc_get_iter(emit_t *emit) {
    emit_pre(emit, 0);
    emit_write_byte_1(emit, PYBC_GET_ITER);
}

static void emit_bc_for_iter(emit_t *emit, int label) {
    emit_pre(emit, 1);
    emit_write_byte_1_label(emit, PYBC_FOR_ITER, label);
}

static void emit_bc_for_iter_end(emit_t *emit) {
    emit_pre(emit, -1);
}

static void emit_bc_pop_block(emit_t *emit) {
    emit_pre(emit, 0);
    emit_write_byte_1(emit, PYBC_POP_BLOCK);
}

static void emit_bc_pop_except(emit_t *emit) {
    emit_pre(emit, 0);
    emit_write_byte_1(emit, PYBC_POP_EXCEPT);
}

static void emit_bc_unary_op(emit_t *emit, rt_unary_op_t op) {
    emit_pre(emit, 0);
    emit_write_byte_1_byte(emit, PYBC_UNARY_OP, op);
}

static void emit_bc_binary_op(emit_t *emit, rt_binary_op_t op) {
    emit_pre(emit, -1);
    emit_write_byte_1_byte(emit, PYBC_BINARY_OP, op);
}

static void emit_bc_compare_op(emit_t *emit, rt_compare_op_t op) {
    emit_pre(emit, -1);
    emit_write_byte_1_byte(emit, PYBC_COMPARE_OP, op);
}

static void emit_bc_build_tuple(emit_t *emit, int n_args) {
    assert(n_args >= 0);
    emit_pre(emit, 1 - n_args);
    emit_write_byte_1_uint(emit, PYBC_BUILD_TUPLE, n_args);
}

static void emit_bc_build_list(emit_t *emit, int n_args) {
    assert(n_args >= 0);
    emit_pre(emit, 1 - n_args);
    emit_write_byte_1_uint(emit, PYBC_BUILD_LIST, n_args);
}

static void emit_bc_list_append(emit_t *emit, int list_stack_index) {
    assert(list_stack_index >= 0);
    emit_pre(emit, -1);
    emit_write_byte_1_uint(emit, PYBC_LIST_APPEND, list_stack_index);
}

static void emit_bc_build_map(emit_t *emit, int n_args) {
    assert(n_args >= 0);
    emit_pre(emit, 1);
    emit_write_byte_1_uint(emit, PYBC_BUILD_MAP, n_args);
}

static void emit_bc_store_map(emit_t *emit) {
    emit_pre(emit, -2);
    emit_write_byte_1(emit, PYBC_STORE_MAP);
}

static void emit_bc_map_add(emit_t *emit, int map_stack_index) {
    assert(map_stack_index >= 0);
    emit_pre(emit, -2);
    emit_write_byte_1_uint(emit, PYBC_MAP_ADD, map_stack_index);
}

static void emit_bc_build_set(emit_t *emit, int n_args) {
    assert(n_args >= 0);
    emit_pre(emit, 1 - n_args);
    emit_write_byte_1_uint(emit, PYBC_BUILD_SET, n_args);
}

static void emit_bc_set_add(emit_t *emit, int set_stack_index) {
    assert(set_stack_index >= 0);
    emit_pre(emit, -1);
    emit_write_byte_1_uint(emit, PYBC_SET_ADD, set_stack_index);
}

static void emit_bc_build_slice(emit_t *emit, int n_args) {
    assert(n_args >= 0);
    emit_pre(emit, 1 - n_args);
    emit_write_byte_1_uint(emit, PYBC_BUILD_SLICE, n_args);
}

static void emit_bc_unpack_sequence(emit_t *emit, int n_args) {
    assert(n_args >= 0);
    emit_pre(emit, -1 + n_args);
    emit_write_byte_1_uint(emit, PYBC_UNPACK_SEQUENCE, n_args);
}

static void emit_bc_unpack_ex(emit_t *emit, int n_left, int n_right) {
    assert(n_left >=0 && n_right >= 0);
    emit_pre(emit, -1 + n_left + n_right + 1);
    emit_write_byte_1_uint(emit, PYBC_UNPACK_EX, n_left | (n_right << 8));
}

static void emit_bc_make_function(emit_t *emit, scope_t *scope, int n_dict_params, int n_default_params) {
    assert(n_default_params == 0 && n_dict_params == 0);
    emit_pre(emit, 1);
    emit_write_byte_1_uint(emit, PYBC_MAKE_FUNCTION, scope->unique_code_id);
}

static void emit_bc_make_closure(emit_t *emit, scope_t *scope, int n_dict_params, int n_default_params) {
    assert(0);
    emit_pre(emit, -2 - n_default_params - 2 * n_dict_params);
    if (emit->pass == PASS_3) {
        printf("MAKE_CLOSURE %d\n", (n_dict_params << 8) | n_default_params);
    }
}

static void emit_bc_call_function(emit_t *emit, int n_positional, int n_keyword, bool have_star_arg, bool have_dbl_star_arg) {
    int s = 0;
    if (have_star_arg) {
        s += 1;
    }
    if (have_dbl_star_arg) {
        s += 1;
    }
    emit_pre(emit, -n_positional - 2 * n_keyword - s);
    int op;
    if (have_star_arg) {
        if (have_dbl_star_arg) {
            op = PYBC_CALL_FUNCTION_VAR_KW;
        } else {
            op = PYBC_CALL_FUNCTION_VAR;
        }
    } else {
        if (have_dbl_star_arg) {
            op = PYBC_CALL_FUNCTION_KW;
        } else {
            op = PYBC_CALL_FUNCTION;
        }
    }
    emit_write_byte_1_uint(emit, op, (n_keyword << 8) | n_positional); // TODO make it 2 separate uints
}

static void emit_bc_call_method(emit_t *emit, int n_positional, int n_keyword, bool have_star_arg, bool have_dbl_star_arg) {
    int s = 0;
    if (have_star_arg) {
        s += 1;
    }
    if (have_dbl_star_arg) {
        s += 1;
    }
    emit_pre(emit, -n_positional - 2 * n_keyword - s);
    int op;
    if (have_star_arg) {
        if (have_dbl_star_arg) {
            op = PYBC_CALL_METHOD_VAR_KW;
        } else {
            op = PYBC_CALL_METHOD_VAR;
        }
    } else {
        if (have_dbl_star_arg) {
            op = PYBC_CALL_METHOD_KW;
        } else {
            op = PYBC_CALL_METHOD;
        }
    }
    emit_write_byte_1_uint(emit, op, (n_keyword << 8) | n_positional); // TODO make it 2 separate uints
}

static void emit_bc_return_value(emit_t *emit) {
    emit_pre(emit, -1);
    emit->last_emit_was_return_value = true;
    emit_write_byte_1(emit, PYBC_RETURN_VALUE);
}

static void emit_bc_raise_varargs(emit_t *emit, int n_args) {
    assert(n_args >= 0);
    emit_pre(emit, -n_args);
    emit_write_byte_1_uint(emit, PYBC_RAISE_VARARGS, n_args);
}

static void emit_bc_yield_value(emit_t *emit) {
    emit_pre(emit, 0);
    if (emit->pass == PASS_2) {
        emit->scope->flags |= SCOPE_FLAG_GENERATOR;
    }
    emit_write_byte_1(emit, PYBC_YIELD_VALUE);
}

static void emit_bc_yield_from(emit_t *emit) {
    emit_pre(emit, -1);
    if (emit->pass == PASS_2) {
        emit->scope->flags |= SCOPE_FLAG_GENERATOR;
    }
    emit_write_byte_1(emit, PYBC_YIELD_FROM);
}

const emit_method_table_t emit_bc_method_table = {
    emit_bc_set_native_types,
    emit_bc_start_pass,
    emit_bc_end_pass,
    emit_bc_last_emit_was_return_value,
    emit_bc_get_stack_size,
    emit_bc_set_stack_size,

    emit_bc_load_id,
    emit_bc_store_id,
    emit_bc_delete_id,

    emit_bc_label_assign,
    emit_bc_import_name,
    emit_bc_import_from,
    emit_bc_import_star,
    emit_bc_load_const_tok,
    emit_bc_load_const_small_int,
    emit_bc_load_const_int,
    emit_bc_load_const_dec,
    emit_bc_load_const_id,
    emit_bc_load_const_str,
    emit_bc_load_const_verbatim_start,
    emit_bc_load_const_verbatim_int,
    emit_bc_load_const_verbatim_str,
    emit_bc_load_const_verbatim_strn,
    emit_bc_load_const_verbatim_quoted_str,
    emit_bc_load_const_verbatim_end,
    emit_bc_load_fast,
    emit_bc_load_name,
    emit_bc_load_global,
    emit_bc_load_deref,
    emit_bc_load_closure,
    emit_bc_load_attr,
    emit_bc_load_method,
    emit_bc_load_build_class,
    emit_bc_store_fast,
    emit_bc_store_name,
    emit_bc_store_global,
    emit_bc_store_deref,
    emit_bc_store_attr,
    emit_bc_store_subscr,
    emit_bc_store_locals,
    emit_bc_delete_fast,
    emit_bc_delete_name,
    emit_bc_delete_global,
    emit_bc_delete_deref,
    emit_bc_delete_attr,
    emit_bc_delete_subscr,
    emit_bc_dup_top,
    emit_bc_dup_top_two,
    emit_bc_pop_top,
    emit_bc_rot_two,
    emit_bc_rot_three,
    emit_bc_jump,
    emit_bc_pop_jump_if_true,
    emit_bc_pop_jump_if_false,
    emit_bc_jump_if_true_or_pop,
    emit_bc_jump_if_false_or_pop,
    emit_bc_setup_loop,
    emit_bc_break_loop,
    emit_bc_continue_loop,
    emit_bc_setup_with,
    emit_bc_with_cleanup,
    emit_bc_setup_except,
    emit_bc_setup_finally,
    emit_bc_end_finally,
    emit_bc_get_iter,
    emit_bc_for_iter,
    emit_bc_for_iter_end,
    emit_bc_pop_block,
    emit_bc_pop_except,
    emit_bc_unary_op,
    emit_bc_binary_op,
    emit_bc_compare_op,
    emit_bc_build_tuple,
    emit_bc_build_list,
    emit_bc_list_append,
    emit_bc_build_map,
    emit_bc_store_map,
    emit_bc_map_add,
    emit_bc_build_set,
    emit_bc_set_add,
    emit_bc_build_slice,
    emit_bc_unpack_sequence,
    emit_bc_unpack_ex,
    emit_bc_make_function,
    emit_bc_make_closure,
    emit_bc_call_function,
    emit_bc_call_method,
    emit_bc_return_value,
    emit_bc_raise_varargs,
    emit_bc_yield_value,
    emit_bc_yield_from,
};
