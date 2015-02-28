/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "py/emit.h"

// wrapper around everything in this file
#if MICROPY_EMIT_CPYTHON

struct _emit_t {
    int pass;
    int bytecode_offset;
    int stack_size;
    bool last_emit_was_return_value;

    scope_t *scope;

    mp_uint_t max_num_labels;
    mp_uint_t *label_offsets;
};

emit_t *emit_cpython_new(mp_uint_t max_num_labels) {
    emit_t *emit = m_new(emit_t, 1);
    emit->max_num_labels = max_num_labels;
    emit->label_offsets = m_new(mp_uint_t, max_num_labels);
    return emit;
}

STATIC void emit_cpy_set_native_type(emit_t *emit, mp_uint_t op, mp_uint_t arg1, qstr arg2) {
}

STATIC void emit_cpy_start_pass(emit_t *emit, pass_kind_t pass, scope_t *scope) {
    emit->pass = pass;
    emit->bytecode_offset = 0;
    emit->stack_size = 0;
    emit->last_emit_was_return_value = false;
    emit->scope = scope;
    if (pass < MP_PASS_EMIT) {
        memset(emit->label_offsets, -1, emit->max_num_labels * sizeof(mp_uint_t));
    }
}

STATIC void emit_cpy_end_pass(emit_t *emit) {
    // check stack is back to zero size
    if (emit->stack_size != 0) {
        printf("ERROR: stack size not back to zero; got %d\n", emit->stack_size);
    }
}

STATIC bool emit_cpy_last_emit_was_return_value(emit_t *emit) {
    return emit->last_emit_was_return_value;
}

STATIC void emit_cpy_adjust_stack_size(emit_t *emit, mp_int_t delta) {
    emit->stack_size += delta;
}

STATIC void emit_cpy_set_source_line(emit_t *emit, mp_uint_t source_line) {
}

STATIC void emit_cpy_load_id(emit_t *emit, qstr qst) {
    emit_common_load_id(emit, &emit_cpython_method_table, emit->scope, qst);
}

STATIC void emit_cpy_store_id(emit_t *emit, qstr qst) {
    emit_common_store_id(emit, &emit_cpython_method_table, emit->scope, qst);
}

STATIC void emit_cpy_delete_id(emit_t *emit, qstr qst) {
    emit_common_delete_id(emit, &emit_cpython_method_table, emit->scope, qst);
}

// TODO: module-polymorphic function (read: name clash if made global)
static void emit_pre(emit_t *emit, int stack_size_delta, int bytecode_size) {
    emit->stack_size += stack_size_delta;
    if (emit->stack_size > emit->scope->stack_size) {
        emit->scope->stack_size = emit->stack_size;
    }
    emit->last_emit_was_return_value = false;
    if (emit->pass == MP_PASS_EMIT && bytecode_size > 0) {
        if (emit->bytecode_offset >= 1000) {
            printf("%d ", emit->bytecode_offset);
        } else {
            printf("% 4d ", emit->bytecode_offset);
        }
    }
    emit->bytecode_offset += bytecode_size;
}

STATIC void emit_cpy_label_assign(emit_t *emit, mp_uint_t l) {
    emit_pre(emit, 0, 0);
    assert(l < emit->max_num_labels);
    if (emit->pass < MP_PASS_EMIT) {
        // assign label offset
        assert(emit->label_offsets[l] == -1);
        emit->label_offsets[l] = emit->bytecode_offset;
    } else {
        // ensure label offset has not changed from MP_PASS_CODE_SIZE to MP_PASS_EMIT
        assert(emit->label_offsets[l] == emit->bytecode_offset);
        //printf("l%d: (at %d)\n", l, emit->bytecode_offset);
    }
}

STATIC void emit_cpy_import_name(emit_t *emit, qstr qst) {
    emit_pre(emit, -1, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("IMPORT_NAME %s\n", qstr_str(qst));
    }
}

STATIC void emit_cpy_import_from(emit_t *emit, qstr qst) {
    emit_pre(emit, 1, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("IMPORT_FROM %s\n", qstr_str(qst));
    }
}

STATIC void emit_cpy_import_star(emit_t *emit) {
    emit_pre(emit, -1, 1);
    if (emit->pass == MP_PASS_EMIT) {
        printf("IMPORT_STAR\n");
    }
}

STATIC void emit_cpy_load_const_tok(emit_t *emit, mp_token_kind_t tok) {
    emit_pre(emit, 1, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("LOAD_CONST ");
        switch (tok) {
            case MP_TOKEN_KW_FALSE: printf("False"); break;
            case MP_TOKEN_KW_NONE: printf("None"); break;
            case MP_TOKEN_KW_TRUE: printf("True"); break;
            default: printf("?=%d\n", tok); return; assert(0);
        }
        printf("\n");
    }
}

STATIC void emit_cpy_load_const_small_int(emit_t *emit, mp_int_t arg) {
    emit_pre(emit, 1, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("LOAD_CONST " INT_FMT "\n", arg);
    }
}

STATIC void print_quoted_str(qstr qst, bool bytes) {
    const char *str = qstr_str(qst);
    int len = strlen(str);
    bool has_single_quote = false;
    bool has_double_quote = false;
    for (int i = 0; i < len; i++) {
        if (str[i] == '\'') {
            has_single_quote = true;
        } else if (str[i] == '"') {
            has_double_quote = true;
        }
    }
    if (bytes) {
        printf("b");
    }
    int quote_char = '\'';
    if (has_single_quote && !has_double_quote) {
        quote_char = '"';
    }
    printf("%c", quote_char);
    for (const char *s = str, *top = str + len; s < top; s++) {
        if (*s == quote_char) {
            printf("\\%c", quote_char);
        } else if (*s == '\\') {
            printf("\\\\");
        } else if (32 <= *s && *s <= 126) {
            printf("%c", *s);
        } else if (*s == '\n') {
            printf("\\n");
        // TODO add more escape codes here
        } else {
            printf("\\x%02x", (*s) & 0xff);
        }
    }
    printf("%c", quote_char);
}

STATIC void emit_cpy_load_const_str(emit_t *emit, qstr qst, bool bytes) {
    emit_pre(emit, 1, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("LOAD_CONST ");
        print_quoted_str(qst, bytes);
        printf("\n");
    }
}

STATIC void emit_cpy_load_const_obj(emit_t *emit, void *obj) {
    emit_pre(emit, 1, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("LOAD_CONST ");
        mp_obj_print(obj, PRINT_REPR);
        printf("\n");
    }
}

STATIC void emit_cpy_load_null(emit_t *emit) {
    // unused for cpy
    assert(0);
}

STATIC void emit_cpy_load_fast(emit_t *emit, qstr qst, mp_uint_t local_num) {
    emit_pre(emit, 1, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("LOAD_FAST " UINT_FMT " %s\n", local_num, qstr_str(qst));
    }
}

STATIC void emit_cpy_load_deref(emit_t *emit, qstr qst, mp_uint_t local_num) {
    emit_pre(emit, 1, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("LOAD_DEREF " UINT_FMT " %s\n", local_num, qstr_str(qst));
    }
}

STATIC void emit_cpy_load_name(emit_t *emit, qstr qst) {
    emit_pre(emit, 1, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("LOAD_NAME %s\n", qstr_str(qst));
    }
}

STATIC void emit_cpy_load_global(emit_t *emit, qstr qst) {
    emit_pre(emit, 1, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("LOAD_GLOBAL %s\n", qstr_str(qst));
    }
}

STATIC void emit_cpy_load_attr(emit_t *emit, qstr qst) {
    emit_pre(emit, 0, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("LOAD_ATTR %s\n", qstr_str(qst));
    }
}

STATIC void emit_cpy_load_method(emit_t *emit, qstr qst) {
    emit_cpy_load_attr(emit, qst);
}

STATIC void emit_cpy_load_build_class(emit_t *emit) {
    emit_pre(emit, 1, 1);
    if (emit->pass == MP_PASS_EMIT) {
        printf("LOAD_BUILD_CLASS\n");
    }
}

STATIC void emit_cpy_load_subscr(emit_t *emit) {
    emit_pre(emit, -1, 1);
    if (emit->pass == MP_PASS_EMIT) {
        printf("BINARY_SUBSCR\n");
    }
}

STATIC void emit_cpy_store_fast(emit_t *emit, qstr qst, mp_uint_t local_num) {
    emit_pre(emit, -1, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("STORE_FAST " UINT_FMT " %s\n", local_num, qstr_str(qst));
    }
}

STATIC void emit_cpy_store_deref(emit_t *emit, qstr qst, mp_uint_t local_num) {
    emit_pre(emit, -1, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("STORE_DEREF " UINT_FMT " %s\n", local_num, qstr_str(qst));
    }
}

STATIC void emit_cpy_store_name(emit_t *emit, qstr qst) {
    emit_pre(emit, -1, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("STORE_NAME %s\n", qstr_str(qst));
    }
}

STATIC void emit_cpy_store_global(emit_t *emit, qstr qst) {
    emit_pre(emit, -1, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("STORE_GLOBAL %s\n", qstr_str(qst));
    }
}

STATIC void emit_cpy_store_attr(emit_t *emit, qstr qst) {
    emit_pre(emit, -2, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("STORE_ATTR %s\n", qstr_str(qst));
    }
}

STATIC void emit_cpy_store_subscr(emit_t *emit) {
    emit_pre(emit, -3, 1);
    if (emit->pass == MP_PASS_EMIT) {
        printf("STORE_SUBSCR\n");
    }
}

STATIC void emit_cpy_delete_fast(emit_t *emit, qstr qst, mp_uint_t local_num) {
    emit_pre(emit, 0, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("DELETE_FAST " UINT_FMT " %s\n", local_num, qstr_str(qst));
    }
}

STATIC void emit_cpy_delete_deref(emit_t *emit, qstr qst, mp_uint_t local_num) {
    emit_pre(emit, 0, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("DELETE_DEREF " UINT_FMT " %s\n", local_num, qstr_str(qst));
    }
}

STATIC void emit_cpy_delete_name(emit_t *emit, qstr qst) {
    emit_pre(emit, 0, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("DELETE_NAME %s\n", qstr_str(qst));
    }
}

STATIC void emit_cpy_delete_global(emit_t *emit, qstr qst) {
    emit_pre(emit, 0, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("DELETE_GLOBAL %s\n", qstr_str(qst));
    }
}

STATIC void emit_cpy_delete_attr(emit_t *emit, qstr qst) {
    emit_pre(emit, -1, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("DELETE_ATTR %s\n", qstr_str(qst));
    }
}

STATIC void emit_cpy_delete_subscr(emit_t *emit) {
    emit_pre(emit, -2, 1);
    if (emit->pass == MP_PASS_EMIT) {
        printf("DELETE_SUBSCR\n");
    }
}

STATIC void emit_cpy_dup_top(emit_t *emit) {
    emit_pre(emit, 1, 1);
    if (emit->pass == MP_PASS_EMIT) {
        printf("DUP_TOP\n");
    }
}

STATIC void emit_cpy_dup_top_two(emit_t *emit) {
    emit_pre(emit, 2, 1);
    if (emit->pass == MP_PASS_EMIT) {
        printf("DUP_TOP_TWO\n");
    }
}

STATIC void emit_cpy_pop_top(emit_t *emit) {
    emit_pre(emit, -1, 1);
    if (emit->pass == MP_PASS_EMIT) {
        printf("POP_TOP\n");
    }
}

STATIC void emit_cpy_rot_two(emit_t *emit) {
    emit_pre(emit, 0, 1);
    if (emit->pass == MP_PASS_EMIT) {
        printf("ROT_TWO\n");
    }
}

STATIC void emit_cpy_rot_three(emit_t *emit) {
    emit_pre(emit, 0, 1);
    if (emit->pass == MP_PASS_EMIT) {
        printf("ROT_THREE\n");
    }
}

STATIC void emit_cpy_jump(emit_t *emit, mp_uint_t label) {
    emit_pre(emit, 0, 3);
    if (emit->pass == MP_PASS_EMIT) {
        int dest = emit->label_offsets[label];
        if (dest < emit->bytecode_offset) {
            printf("JUMP_ABSOLUTE " UINT_FMT "\n", emit->label_offsets[label]);
        } else {
            printf("JUMP_FORWARD " UINT_FMT "\n", emit->label_offsets[label]);
        }
    }
}

STATIC void emit_cpy_pop_jump_if(emit_t *emit, bool cond, mp_uint_t label) {
    emit_pre(emit, -1, 3);
    if (emit->pass == MP_PASS_EMIT) {
        if (cond) {
            printf("POP_JUMP_IF_TRUE " UINT_FMT "\n", emit->label_offsets[label]);
        } else {
            printf("POP_JUMP_IF_FALSE " UINT_FMT "\n", emit->label_offsets[label]);
        }
    }
}

STATIC void emit_cpy_jump_if_or_pop(emit_t *emit, bool cond, mp_uint_t label) {
    emit_pre(emit, -1, 3);
    if (emit->pass == MP_PASS_EMIT) {
        if (cond) {
            printf("JUMP_IF_TRUE_OR_POP " UINT_FMT "\n", emit->label_offsets[label]);
        } else {
            printf("JUMP_IF_FALSE_OR_POP " UINT_FMT "\n", emit->label_offsets[label]);
        }
    }
}

STATIC void emit_cpy_break_loop(emit_t *emit, mp_uint_t label, mp_uint_t except_depth) {
    emit_pre(emit, 0, 1);
    if (emit->pass == MP_PASS_EMIT) {
        printf("BREAK_LOOP\n");
    }
}

STATIC void emit_cpy_continue_loop(emit_t *emit, mp_uint_t label, mp_uint_t except_depth) {
    if (except_depth == 0) {
        emit_cpy_jump(emit, label);
    } else {
        emit_pre(emit, 0, 3);
        if (emit->pass == MP_PASS_EMIT) {
            printf("CONTINUE_LOOP " UINT_FMT "\n", emit->label_offsets[label]);
        }
    }
}

STATIC void emit_cpy_setup_with(emit_t *emit, mp_uint_t label) {
    emit_pre(emit, 7, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("SETUP_WITH " UINT_FMT "\n", emit->label_offsets[label]);
    }
}

STATIC void emit_cpy_with_cleanup(emit_t *emit) {
    emit_pre(emit, -7, 1);
    if (emit->pass == MP_PASS_EMIT) {
        printf("WITH_CLEANUP\n");
    }
}

STATIC void emit_cpy_setup_except(emit_t *emit, mp_uint_t label) {
    emit_pre(emit, 0, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("SETUP_EXCEPT " UINT_FMT "\n", emit->label_offsets[label]);
    }
}

STATIC void emit_cpy_setup_finally(emit_t *emit, mp_uint_t label) {
    emit_pre(emit, 0, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("SETUP_FINALLY " UINT_FMT "\n", emit->label_offsets[label]);
    }
}

STATIC void emit_cpy_end_finally(emit_t *emit) {
    emit_pre(emit, -1, 1);
    if (emit->pass == MP_PASS_EMIT) {
        printf("END_FINALLY\n");
    }
}

STATIC void emit_cpy_get_iter(emit_t *emit) {
    emit_pre(emit, 0, 1);
    if (emit->pass == MP_PASS_EMIT) {
        printf("GET_ITER\n");
    }
}

STATIC void emit_cpy_for_iter(emit_t *emit, mp_uint_t label) {
    emit_pre(emit, 1, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("FOR_ITER " UINT_FMT "\n", emit->label_offsets[label]);
    }
}

STATIC void emit_cpy_for_iter_end(emit_t *emit) {
    emit_pre(emit, -1, 0);
}

STATIC void emit_cpy_pop_block(emit_t *emit) {
    emit_pre(emit, 0, 1);
    if (emit->pass == MP_PASS_EMIT) {
        printf("POP_BLOCK\n");
    }
}

STATIC void emit_cpy_pop_except(emit_t *emit) {
    emit_pre(emit, 0, 1);
    if (emit->pass == MP_PASS_EMIT) {
        printf("POP_EXCEPT\n");
    }
}

STATIC void emit_cpy_unary_op(emit_t *emit, mp_unary_op_t op) {
    emit_pre(emit, 0, 1);
    if (emit->pass == MP_PASS_EMIT) {
        switch (op) {
            case MP_UNARY_OP_POSITIVE: printf("UNARY_POSITIVE\n"); break;
            case MP_UNARY_OP_NEGATIVE: printf("UNARY_NEGATIVE\n"); break;
            case MP_UNARY_OP_INVERT: printf("UNARY_INVERT\n"); break;
            case MP_UNARY_OP_NOT: printf("UNARY_NOT\n"); break;
            default: assert(0);
        }
    }
}

STATIC void emit_cpy_binary_op(emit_t *emit, mp_binary_op_t op) {
    if (op <= MP_BINARY_OP_INPLACE_POWER) {
        // CPython uses a byte code for each binary op
        emit_pre(emit, -1, 1);
    } else {
        // CPython uses a byte code plus an argument for compare ops
        emit_pre(emit, -1, 3);
    }
    if (emit->pass == MP_PASS_EMIT) {
        switch (op) {
            case MP_BINARY_OP_OR: printf("BINARY_OR\n"); break;
            case MP_BINARY_OP_XOR: printf("BINARY_XOR\n"); break;
            case MP_BINARY_OP_AND: printf("BINARY_AND\n"); break;
            case MP_BINARY_OP_LSHIFT: printf("BINARY_LSHIFT\n"); break;
            case MP_BINARY_OP_RSHIFT: printf("BINARY_RSHIFT\n"); break;
            case MP_BINARY_OP_ADD: printf("BINARY_ADD\n"); break;
            case MP_BINARY_OP_SUBTRACT: printf("BINARY_SUBTRACT\n"); break;
            case MP_BINARY_OP_MULTIPLY: printf("BINARY_MULTIPLY\n"); break;
            case MP_BINARY_OP_FLOOR_DIVIDE: printf("BINARY_FLOOR_DIVIDE\n"); break;
            case MP_BINARY_OP_TRUE_DIVIDE: printf("BINARY_TRUE_DIVIDE\n"); break;
            case MP_BINARY_OP_MODULO: printf("BINARY_MODULO\n"); break;
            case MP_BINARY_OP_POWER: printf("BINARY_POWER\n"); break;
            case MP_BINARY_OP_INPLACE_OR: printf("INPLACE_OR\n"); break;
            case MP_BINARY_OP_INPLACE_XOR: printf("INPLACE_XOR\n"); break;
            case MP_BINARY_OP_INPLACE_AND: printf("INPLACE_AND\n"); break;
            case MP_BINARY_OP_INPLACE_LSHIFT: printf("INPLACE_LSHIFT\n"); break;
            case MP_BINARY_OP_INPLACE_RSHIFT: printf("INPLACE_RSHIFT\n"); break;
            case MP_BINARY_OP_INPLACE_ADD: printf("INPLACE_ADD\n"); break;
            case MP_BINARY_OP_INPLACE_SUBTRACT: printf("INPLACE_SUBTRACT\n"); break;
            case MP_BINARY_OP_INPLACE_MULTIPLY: printf("INPLACE_MULTIPLY\n"); break;
            case MP_BINARY_OP_INPLACE_FLOOR_DIVIDE: printf("INPLACE_FLOOR_DIVIDE\n"); break;
            case MP_BINARY_OP_INPLACE_TRUE_DIVIDE: printf("INPLACE_TRUE_DIVIDE\n"); break;
            case MP_BINARY_OP_INPLACE_MODULO: printf("INPLACE_MODULO\n"); break;
            case MP_BINARY_OP_INPLACE_POWER: printf("INPLACE_POWER\n"); break;
            case MP_BINARY_OP_LESS: printf("COMPARE_OP <\n"); break;
            case MP_BINARY_OP_MORE: printf("COMPARE_OP >\n"); break;
            case MP_BINARY_OP_EQUAL: printf("COMPARE_OP ==\n"); break;
            case MP_BINARY_OP_LESS_EQUAL: printf("COMPARE_OP <=\n"); break;
            case MP_BINARY_OP_MORE_EQUAL: printf("COMPARE_OP >=\n"); break;
            case MP_BINARY_OP_NOT_EQUAL: printf("COMPARE_OP !=\n"); break;
            case MP_BINARY_OP_IN: printf("COMPARE_OP in\n"); break;
            case MP_BINARY_OP_IS: printf("COMPARE_OP is\n"); break;
            case MP_BINARY_OP_EXCEPTION_MATCH: printf("COMPARE_OP exception match\n"); break;
            case MP_BINARY_OP_NOT_IN: printf("COMPARE_OP not in\n"); break;
            case MP_BINARY_OP_IS_NOT: printf("COMPARE_OP is not\n"); break;
            default: assert(0);
        }
    }
}

STATIC void emit_cpy_build_tuple(emit_t *emit, mp_uint_t n_args) {
    emit_pre(emit, 1 - n_args, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("BUILD_TUPLE " UINT_FMT "\n", n_args);
    }
}

STATIC void emit_cpy_build_list(emit_t *emit, mp_uint_t n_args) {
    emit_pre(emit, 1 - n_args, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("BUILD_LIST " UINT_FMT "\n", n_args);
    }
}

STATIC void emit_cpy_list_append(emit_t *emit, mp_uint_t list_index) {
    emit_pre(emit, -1, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("LIST_APPEND " UINT_FMT "\n", list_index);
    }
}

STATIC void emit_cpy_build_map(emit_t *emit, mp_uint_t n_args) {
    emit_pre(emit, 1, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("BUILD_MAP " UINT_FMT "\n", n_args);
    }
}

STATIC void emit_cpy_store_map(emit_t *emit) {
    emit_pre(emit, -2, 1);
    if (emit->pass == MP_PASS_EMIT) {
        printf("STORE_MAP\n");
    }
}

STATIC void emit_cpy_map_add(emit_t *emit, mp_uint_t map_index) {
    emit_pre(emit, -2, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("MAP_ADD " UINT_FMT "\n", map_index);
    }
}

STATIC void emit_cpy_build_set(emit_t *emit, mp_uint_t n_args) {
    emit_pre(emit, 1 - n_args, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("BUILD_SET " UINT_FMT "\n", n_args);
    }
}

STATIC void emit_cpy_set_add(emit_t *emit, mp_uint_t set_index) {
    emit_pre(emit, -1, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("SET_ADD " UINT_FMT "\n", set_index);
    }
}

STATIC void emit_cpy_build_slice(emit_t *emit, mp_uint_t n_args) {
    emit_pre(emit, 1 - n_args, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("BUILD_SLICE " UINT_FMT "\n", n_args);
    }
}

STATIC void emit_cpy_unpack_sequence(emit_t *emit, mp_uint_t n_args) {
    emit_pre(emit, -1 + n_args, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("UNPACK_SEQUENCE " UINT_FMT "\n", n_args);
    }
}

STATIC void emit_cpy_unpack_ex(emit_t *emit, mp_uint_t n_left, mp_uint_t n_right) {
    emit_pre(emit, -1 + n_left + n_right + 1, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("UNPACK_EX " UINT_FMT "\n", n_left | (n_right << 8));
    }
}

STATIC void emit_cpy_call_function(emit_t *emit, mp_uint_t n_positional, mp_uint_t n_keyword, mp_uint_t star_flags) {
    mp_int_t s = 0;
    if (star_flags & MP_EMIT_STAR_FLAG_SINGLE) {
        s += 1;
    }
    if (star_flags & MP_EMIT_STAR_FLAG_DOUBLE) {
        s += 1;
    }
    emit_pre(emit, -(mp_int_t)n_positional - 2 * (mp_int_t)n_keyword - s, 3);
    if (emit->pass == MP_PASS_EMIT) {
        if (star_flags & MP_EMIT_STAR_FLAG_SINGLE) {
            if (star_flags & MP_EMIT_STAR_FLAG_DOUBLE) {
                printf("CALL_FUNCTION_VAR_KW");
            } else {
                printf("CALL_FUNCTION_VAR");
            }
        } else {
            if (star_flags & MP_EMIT_STAR_FLAG_DOUBLE) {
                printf("CALL_FUNCTION_KW");
            } else {
                printf("CALL_FUNCTION");
            }
        }
        printf(" " UINT_FMT ", " UINT_FMT "\n", n_positional, n_keyword);
    }
}

STATIC void emit_cpy_call_method(emit_t *emit, mp_uint_t n_positional, mp_uint_t n_keyword, mp_uint_t star_flags) {
    emit_cpy_call_function(emit, n_positional, n_keyword, star_flags);
}

STATIC void emit_cpy_return_value(emit_t *emit) {
    emit_pre(emit, -1, 1);
    emit->last_emit_was_return_value = true;
    if (emit->pass == MP_PASS_EMIT) {
        printf("RETURN_VALUE\n");
    }
}

STATIC void emit_cpy_raise_varargs(emit_t *emit, mp_uint_t n_args) {
    emit_pre(emit, -n_args, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("RAISE_VARARGS " UINT_FMT "\n", n_args);
    }
}

STATIC void load_cpy_const_code_and_name(emit_t *emit, qstr qst) {
    emit_pre(emit, 1, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("LOAD_CONST code %s\n", qstr_str(qst));
    }
    // load qualified name
    emit_pre(emit, 1, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("LOAD_CONST '");
        // code just to work out the qualname (or whatever it is)
        {
            int depth = 0;
            for (scope_t *s = emit->scope; s->parent != NULL; s = s->parent) {
                depth += 1;
            }
            for (int wanted_depth = depth; wanted_depth >= 0; wanted_depth--) {
                scope_t *s = emit->scope;
                for (int i = 0; i < wanted_depth; i++) {
                    s = s->parent;
                }
                if (s->kind == SCOPE_FUNCTION) {
                    printf("%s.<locals>.", qstr_str(s->simple_name));
                } else if (s->kind == SCOPE_CLASS) {
                    printf("%s.", qstr_str(s->simple_name));
                }
            }
        }
        printf("%s'\n", qstr_str(qst));
    }
}

STATIC void emit_cpy_make_function(emit_t *emit, scope_t *scope, mp_uint_t n_pos_defaults, mp_uint_t n_kw_defaults) {
    load_cpy_const_code_and_name(emit, scope->simple_name);
    emit_pre(emit, -1 - n_pos_defaults - 2 * n_kw_defaults, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("MAKE_FUNCTION " UINT_FMT "\n", (n_kw_defaults << 8) | n_pos_defaults);
    }
}

STATIC void emit_cpy_make_closure(emit_t *emit, scope_t *scope, mp_uint_t n_closed_over, mp_uint_t n_pos_defaults, mp_uint_t n_kw_defaults) {
    emit_cpy_build_tuple(emit, n_closed_over);
    load_cpy_const_code_and_name(emit, scope->simple_name);
    emit_pre(emit, -2 - n_pos_defaults - 2 * n_kw_defaults, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("MAKE_CLOSURE " UINT_FMT "\n", (n_kw_defaults << 8) | n_pos_defaults);
    }
}

STATIC void emit_cpy_yield_value(emit_t *emit) {
    emit_pre(emit, 0, 1);
    emit->scope->scope_flags |= MP_SCOPE_FLAG_GENERATOR;
    if (emit->pass == MP_PASS_EMIT) {
        printf("YIELD_VALUE\n");
    }
}

STATIC void emit_cpy_yield_from(emit_t *emit) {
    emit_pre(emit, -1, 1);
    emit->scope->scope_flags |= MP_SCOPE_FLAG_GENERATOR;
    if (emit->pass == MP_PASS_EMIT) {
        printf("YIELD_FROM\n");
    }
}

STATIC void emit_cpy_start_except_handler(emit_t *emit) {
    emit_cpy_adjust_stack_size(emit, 3); // stack adjust for the 3 exception items
}

STATIC void emit_cpy_end_except_handler(emit_t *emit) {
    emit_cpy_adjust_stack_size(emit, -5); // stack adjust
}

STATIC void emit_cpy_load_const_verbatim_strn(emit_t *emit, const char *str, mp_uint_t len) {
    emit_pre(emit, 1, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("LOAD_CONST %.*s\n", (int)len, str);
    }
}

STATIC void emit_cpy_load_closure(emit_t *emit, qstr qst, mp_uint_t local_num) {
    emit_pre(emit, 1, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("LOAD_CLOSURE " UINT_FMT " %s\n", local_num, qstr_str(qst));
    }
}

STATIC void emit_cpy_setup_loop(emit_t *emit, mp_uint_t label) {
    emit_pre(emit, 0, 3);
    if (emit->pass == MP_PASS_EMIT) {
        printf("SETUP_LOOP " UINT_FMT "\n", emit->label_offsets[label]);
    }
}

const emit_method_table_t emit_cpython_method_table = {
    emit_cpy_set_native_type,
    emit_cpy_start_pass,
    emit_cpy_end_pass,
    emit_cpy_last_emit_was_return_value,
    emit_cpy_adjust_stack_size,
    emit_cpy_set_source_line,

    emit_cpy_load_id,
    emit_cpy_store_id,
    emit_cpy_delete_id,

    emit_cpy_label_assign,
    emit_cpy_import_name,
    emit_cpy_import_from,
    emit_cpy_import_star,
    emit_cpy_load_const_tok,
    emit_cpy_load_const_small_int,
    emit_cpy_load_const_str,
    emit_cpy_load_const_obj,
    emit_cpy_load_null,
    emit_cpy_load_fast,
    emit_cpy_load_deref,
    emit_cpy_load_name,
    emit_cpy_load_global,
    emit_cpy_load_attr,
    emit_cpy_load_method,
    emit_cpy_load_build_class,
    emit_cpy_load_subscr,
    emit_cpy_store_fast,
    emit_cpy_store_deref,
    emit_cpy_store_name,
    emit_cpy_store_global,
    emit_cpy_store_attr,
    emit_cpy_store_subscr,
    emit_cpy_delete_fast,
    emit_cpy_delete_deref,
    emit_cpy_delete_name,
    emit_cpy_delete_global,
    emit_cpy_delete_attr,
    emit_cpy_delete_subscr,
    emit_cpy_dup_top,
    emit_cpy_dup_top_two,
    emit_cpy_pop_top,
    emit_cpy_rot_two,
    emit_cpy_rot_three,
    emit_cpy_jump,
    emit_cpy_pop_jump_if,
    emit_cpy_jump_if_or_pop,
    emit_cpy_break_loop,
    emit_cpy_continue_loop,
    emit_cpy_setup_with,
    emit_cpy_with_cleanup,
    emit_cpy_setup_except,
    emit_cpy_setup_finally,
    emit_cpy_end_finally,
    emit_cpy_get_iter,
    emit_cpy_for_iter,
    emit_cpy_for_iter_end,
    emit_cpy_pop_block,
    emit_cpy_pop_except,
    emit_cpy_unary_op,
    emit_cpy_binary_op,
    emit_cpy_build_tuple,
    emit_cpy_build_list,
    emit_cpy_list_append,
    emit_cpy_build_map,
    emit_cpy_store_map,
    emit_cpy_map_add,
    emit_cpy_build_set,
    emit_cpy_set_add,
    emit_cpy_build_slice,
    emit_cpy_unpack_sequence,
    emit_cpy_unpack_ex,
    emit_cpy_make_function,
    emit_cpy_make_closure,
    emit_cpy_call_function,
    emit_cpy_call_method,
    emit_cpy_return_value,
    emit_cpy_raise_varargs,
    emit_cpy_yield_value,
    emit_cpy_yield_from,

    emit_cpy_start_except_handler,
    emit_cpy_end_except_handler,

    // emitcpy specific functions
    emit_cpy_load_const_verbatim_strn,
    emit_cpy_load_closure,
    emit_cpy_setup_loop,
};

#endif // MICROPY_EMIT_CPYTHON
