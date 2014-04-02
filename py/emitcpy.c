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

// wrapper around everything in this file
#if MICROPY_EMIT_CPYTHON

struct _emit_t {
    int pass;
    int byte_code_offset;
    int stack_size;
    bool last_emit_was_return_value;

    scope_t *scope;

    uint max_num_labels;
    int *label_offsets;
};

emit_t *emit_cpython_new(uint max_num_labels) {
    emit_t *emit = m_new(emit_t, 1);
    emit->max_num_labels = max_num_labels;
    emit->label_offsets = m_new(int, max_num_labels);
    return emit;
}

STATIC void emit_cpy_set_native_types(emit_t *emit, bool do_native_types) {
}

STATIC void emit_cpy_start_pass(emit_t *emit, pass_kind_t pass, scope_t *scope) {
    emit->pass = pass;
    emit->byte_code_offset = 0;
    emit->stack_size = 0;
    emit->last_emit_was_return_value = false;
    emit->scope = scope;
    if (pass == PASS_2) {
        memset(emit->label_offsets, -1, emit->max_num_labels * sizeof(int));
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

STATIC int emit_cpy_get_stack_size(emit_t *emit) {
    return emit->stack_size;
}

STATIC void emit_cpy_set_stack_size(emit_t *emit, int size) {
    emit->stack_size = size;
}

STATIC void emit_cpy_set_source_line(emit_t *emit, int source_line) {
}

STATIC void emit_cpy_load_id(emit_t *emit, qstr qstr) {
    emit_common_load_id(emit, &emit_cpython_method_table, emit->scope, qstr);
}

STATIC void emit_cpy_store_id(emit_t *emit, qstr qstr) {
    emit_common_store_id(emit, &emit_cpython_method_table, emit->scope, qstr);
}

STATIC void emit_cpy_delete_id(emit_t *emit, qstr qstr) {
    emit_common_delete_id(emit, &emit_cpython_method_table, emit->scope, qstr);
}

// TODO: module-polymorphic function (read: name clash if made global)
static void emit_pre(emit_t *emit, int stack_size_delta, int byte_code_size) {
    emit->stack_size += stack_size_delta;
    if (emit->stack_size > emit->scope->stack_size) {
        emit->scope->stack_size = emit->stack_size;
    }
    emit->last_emit_was_return_value = false;
    if (emit->pass == PASS_3 && byte_code_size > 0) {
        if (emit->byte_code_offset >= 1000) {
            printf("%d ", emit->byte_code_offset);
        } else {
            printf("% 4d ", emit->byte_code_offset);
        }
    }
    emit->byte_code_offset += byte_code_size;
}

STATIC void emit_cpy_label_assign(emit_t *emit, int l) {
    emit_pre(emit, 0, 0);
    assert(l < emit->max_num_labels);
    if (emit->pass == PASS_2) {
        // assign label offset
        assert(emit->label_offsets[l] == -1);
        emit->label_offsets[l] = emit->byte_code_offset;
    } else if (emit->pass == PASS_3) {
        // ensure label offset has not changed from PASS_2 to PASS_3
        assert(emit->label_offsets[l] == emit->byte_code_offset);
        //printf("l%d: (at %d)\n", l, emit->byte_code_offset);
    }
}

STATIC void emit_cpy_import_name(emit_t *emit, qstr qstr) {
    emit_pre(emit, -1, 3);
    if (emit->pass == PASS_3) {
        printf("IMPORT_NAME %s\n", qstr_str(qstr));
    }
}

STATIC void emit_cpy_import_from(emit_t *emit, qstr qstr) {
    emit_pre(emit, 1, 3);
    if (emit->pass == PASS_3) {
        printf("IMPORT_FROM %s\n", qstr_str(qstr));
    }
}

STATIC void emit_cpy_import_star(emit_t *emit) {
    emit_pre(emit, -1, 1);
    if (emit->pass == PASS_3) {
        printf("IMPORT_STAR\n");
    }
}

STATIC void emit_cpy_load_const_tok(emit_t *emit, mp_token_kind_t tok) {
    emit_pre(emit, 1, 3);
    if (emit->pass == PASS_3) {
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

STATIC void emit_cpy_load_const_small_int(emit_t *emit, machine_int_t arg) {
    emit_pre(emit, 1, 3);
    if (emit->pass == PASS_3) {
        printf("LOAD_CONST " INT_FMT "\n", arg);
    }
}

STATIC void emit_cpy_load_const_int(emit_t *emit, qstr qstr) {
    emit_pre(emit, 1, 3);
    if (emit->pass == PASS_3) {
        printf("LOAD_CONST %s\n", qstr_str(qstr));
    }
}

STATIC void emit_cpy_load_const_dec(emit_t *emit, qstr qstr) {
    emit_pre(emit, 1, 3);
    if (emit->pass == PASS_3) {
        printf("LOAD_CONST %s\n", qstr_str(qstr));
    }
}

STATIC void emit_cpy_load_const_id(emit_t *emit, qstr qstr) {
    emit_pre(emit, 1, 3);
    if (emit->pass == PASS_3) {
        printf("LOAD_CONST '%s'\n", qstr_str(qstr));
    }
}

STATIC void print_quoted_str(qstr qstr, bool bytes) {
    const char *str = qstr_str(qstr);
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

STATIC void emit_cpy_load_const_str(emit_t *emit, qstr qstr, bool bytes) {
    emit_pre(emit, 1, 3);
    if (emit->pass == PASS_3) {
        printf("LOAD_CONST ");
        print_quoted_str(qstr, bytes);
        printf("\n");
    }
}

STATIC void emit_cpy_load_const_verbatim_str(emit_t *emit, const char *str) {
    emit_pre(emit, 1, 3);
    if (emit->pass == PASS_3) {
        printf("LOAD_CONST %s\n", str);
    }
}

STATIC void emit_cpy_load_fast(emit_t *emit, qstr qstr, int local_num) {
    emit_pre(emit, 1, 3);
    if (emit->pass == PASS_3) {
        printf("LOAD_FAST %d %s\n", local_num, qstr_str(qstr));
    }
}

STATIC void emit_cpy_load_deref(emit_t *emit, qstr qstr, int local_num) {
    emit_pre(emit, 1, 3);
    if (emit->pass == PASS_3) {
        printf("LOAD_DEREF %d %s\n", local_num, qstr_str(qstr));
    }
}

STATIC void emit_cpy_load_closure(emit_t *emit, qstr qstr, int local_num) {
    emit_pre(emit, 1, 3);
    if (emit->pass == PASS_3) {
        printf("LOAD_CLOSURE %d %s\n", local_num, qstr_str(qstr));
    }
}

STATIC void emit_cpy_load_name(emit_t *emit, qstr qstr) {
    emit_pre(emit, 1, 3);
    if (emit->pass == PASS_3) {
        printf("LOAD_NAME %s\n", qstr_str(qstr));
    }
}

STATIC void emit_cpy_load_global(emit_t *emit, qstr qstr) {
    emit_pre(emit, 1, 3);
    if (emit->pass == PASS_3) {
        printf("LOAD_GLOBAL %s\n", qstr_str(qstr));
    }
}

STATIC void emit_cpy_load_attr(emit_t *emit, qstr qstr) {
    emit_pre(emit, 0, 3);
    if (emit->pass == PASS_3) {
        printf("LOAD_ATTR %s\n", qstr_str(qstr));
    }
}

STATIC void emit_cpy_load_method(emit_t *emit, qstr qstr) {
    emit_cpy_load_attr(emit, qstr);
}

STATIC void emit_cpy_load_build_class(emit_t *emit) {
    emit_pre(emit, 1, 1);
    if (emit->pass == PASS_3) {
        printf("LOAD_BUILD_CLASS\n");
    }
}

STATIC void emit_cpy_store_fast(emit_t *emit, qstr qstr, int local_num) {
    emit_pre(emit, -1, 3);
    if (emit->pass == PASS_3) {
        printf("STORE_FAST %d %s\n", local_num, qstr_str(qstr));
    }
}

STATIC void emit_cpy_store_deref(emit_t *emit, qstr qstr, int local_num) {
    emit_pre(emit, -1, 3);
    if (emit->pass == PASS_3) {
        printf("STORE_DEREF %d %s\n", local_num, qstr_str(qstr));
    }
}

STATIC void emit_cpy_store_name(emit_t *emit, qstr qstr) {
    emit_pre(emit, -1, 3);
    if (emit->pass == PASS_3) {
        printf("STORE_NAME %s\n", qstr_str(qstr));
    }
}

STATIC void emit_cpy_store_global(emit_t *emit, qstr qstr) {
    emit_pre(emit, -1, 3);
    if (emit->pass == PASS_3) {
        printf("STORE_GLOBAL %s\n", qstr_str(qstr));
    }
}

STATIC void emit_cpy_store_attr(emit_t *emit, qstr qstr) {
    emit_pre(emit, -2, 3);
    if (emit->pass == PASS_3) {
        printf("STORE_ATTR %s\n", qstr_str(qstr));
    }
}

STATIC void emit_cpy_store_subscr(emit_t *emit) {
    emit_pre(emit, -3, 1);
    if (emit->pass == PASS_3) {
        printf("STORE_SUBSCR\n");
    }
}

STATIC void emit_cpy_delete_fast(emit_t *emit, qstr qstr, int local_num) {
    emit_pre(emit, 0, 3);
    if (emit->pass == PASS_3) {
        printf("DELETE_FAST %d %s\n", local_num, qstr_str(qstr));
    }
}

STATIC void emit_cpy_delete_deref(emit_t *emit, qstr qstr, int local_num) {
    emit_pre(emit, 0, 3);
    if (emit->pass == PASS_3) {
        printf("DELETE_DEREF %d %s\n", local_num, qstr_str(qstr));
    }
}

STATIC void emit_cpy_delete_name(emit_t *emit, qstr qstr) {
    emit_pre(emit, 0, 3);
    if (emit->pass == PASS_3) {
        printf("DELETE_NAME %s\n", qstr_str(qstr));
    }
}

STATIC void emit_cpy_delete_global(emit_t *emit, qstr qstr) {
    emit_pre(emit, 0, 3);
    if (emit->pass == PASS_3) {
        printf("DELETE_GLOBAL %s\n", qstr_str(qstr));
    }
}

STATIC void emit_cpy_delete_attr(emit_t *emit, qstr qstr) {
    emit_pre(emit, -1, 3);
    if (emit->pass == PASS_3) {
        printf("DELETE_ATTR %s\n", qstr_str(qstr));
    }
}

STATIC void emit_cpy_delete_subscr(emit_t *emit) {
    emit_pre(emit, -2, 1);
    if (emit->pass == PASS_3) {
        printf("DELETE_SUBSCR\n");
    }
}

STATIC void emit_cpy_dup_top(emit_t *emit) {
    emit_pre(emit, 1, 1);
    if (emit->pass == PASS_3) {
        printf("DUP_TOP\n");
    }
}

STATIC void emit_cpy_dup_top_two(emit_t *emit) {
    emit_pre(emit, 2, 1);
    if (emit->pass == PASS_3) {
        printf("DUP_TOP_TWO\n");
    }
}

STATIC void emit_cpy_pop_top(emit_t *emit) {
    emit_pre(emit, -1, 1);
    if (emit->pass == PASS_3) {
        printf("POP_TOP\n");
    }
}

STATIC void emit_cpy_rot_two(emit_t *emit) {
    emit_pre(emit, 0, 1);
    if (emit->pass == PASS_3) {
        printf("ROT_TWO\n");
    }
}

STATIC void emit_cpy_rot_three(emit_t *emit) {
    emit_pre(emit, 0, 1);
    if (emit->pass == PASS_3) {
        printf("ROT_THREE\n");
    }
}

STATIC void emit_cpy_jump(emit_t *emit, int label) {
    emit_pre(emit, 0, 3);
    if (emit->pass == PASS_3) {
        int dest = emit->label_offsets[label];
        if (dest < emit->byte_code_offset) {
            printf("JUMP_ABSOLUTE %d\n", emit->label_offsets[label]);
        } else {
            printf("JUMP_FORWARD %d\n", emit->label_offsets[label]);
        }
    }
}

STATIC void emit_cpy_pop_jump_if_true(emit_t *emit, int label) {
    emit_pre(emit, -1, 3);
    if (emit->pass == PASS_3) {
        printf("POP_JUMP_IF_TRUE %d\n", emit->label_offsets[label]);
    }
}

STATIC void emit_cpy_pop_jump_if_false(emit_t *emit, int label) {
    emit_pre(emit, -1, 3);
    if (emit->pass == PASS_3) {
        printf("POP_JUMP_IF_FALSE %d\n", emit->label_offsets[label]);
    }
}

STATIC void emit_cpy_jump_if_true_or_pop(emit_t *emit, int label) {
    emit_pre(emit, -1, 3);
    if (emit->pass == PASS_3) {
        printf("JUMP_IF_TRUE_OR_POP %d\n", emit->label_offsets[label]);
    }
}

STATIC void emit_cpy_jump_if_false_or_pop(emit_t *emit, int label) {
    emit_pre(emit, -1, 3);
    if (emit->pass == PASS_3) {
        printf("JUMP_IF_FALSE_OR_POP %d\n", emit->label_offsets[label]);
    }
}

STATIC void emit_cpy_setup_loop(emit_t *emit, int label) {
    emit_pre(emit, 0, 3);
    if (emit->pass == PASS_3) {
        printf("SETUP_LOOP %d\n", emit->label_offsets[label]);
    }
}

STATIC void emit_cpy_break_loop(emit_t *emit, int label, int except_depth) {
    emit_pre(emit, 0, 1);
    if (emit->pass == PASS_3) {
        printf("BREAK_LOOP\n");
    }
}

STATIC void emit_cpy_continue_loop(emit_t *emit, int label, int except_depth) {
    if (except_depth == 0) {
        emit_cpy_jump(emit, label);
    } else {
        emit_pre(emit, 0, 3);
        if (emit->pass == PASS_3) {
            printf("CONTINUE_LOOP %d\n", emit->label_offsets[label]);
        }
    }
}

STATIC void emit_cpy_setup_with(emit_t *emit, int label) {
    emit_pre(emit, 7, 3);
    if (emit->pass == PASS_3) {
        printf("SETUP_WITH %d\n", emit->label_offsets[label]);
    }
}

STATIC void emit_cpy_with_cleanup(emit_t *emit) {
    emit_pre(emit, -7, 1);
    if (emit->pass == PASS_3) {
        printf("WITH_CLEANUP\n");
    }
}

STATIC void emit_cpy_setup_except(emit_t *emit, int label) {
    emit_pre(emit, 6, 3);
    if (emit->pass == PASS_3) {
        printf("SETUP_EXCEPT %d\n", emit->label_offsets[label]);
    }
}

STATIC void emit_cpy_setup_finally(emit_t *emit, int label) {
    emit_pre(emit, 6, 3);
    if (emit->pass == PASS_3) {
        printf("SETUP_FINALLY %d\n", emit->label_offsets[label]);
    }
}

STATIC void emit_cpy_end_finally(emit_t *emit) {
    emit_pre(emit, -1, 1);
    if (emit->pass == PASS_3) {
        printf("END_FINALLY\n");
    }
}

STATIC void emit_cpy_get_iter(emit_t *emit) {
    emit_pre(emit, 0, 1);
    if (emit->pass == PASS_3) {
        printf("GET_ITER\n");
    }
}

STATIC void emit_cpy_for_iter(emit_t *emit, int label) {
    emit_pre(emit, 1, 3);
    if (emit->pass == PASS_3) {
        printf("FOR_ITER %d\n", emit->label_offsets[label]);
    }
}

STATIC void emit_cpy_for_iter_end(emit_t *emit) {
    emit_pre(emit, -1, 0);
}

STATIC void emit_cpy_pop_block(emit_t *emit) {
    emit_pre(emit, 0, 1);
    if (emit->pass == PASS_3) {
        printf("POP_BLOCK\n");
    }
}

STATIC void emit_cpy_pop_except(emit_t *emit) {
    emit_pre(emit, 0, 1);
    if (emit->pass == PASS_3) {
        printf("POP_EXCEPT\n");
    }
}

STATIC void emit_cpy_unary_op(emit_t *emit, mp_unary_op_t op) {
    emit_pre(emit, 0, 1);
    if (emit->pass == PASS_3) {
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
    if (emit->pass == PASS_3) {
        switch (op) {
            case MP_BINARY_OP_SUBSCR: printf("BINARY_SUBSCR\n"); break;
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

STATIC void emit_cpy_build_tuple(emit_t *emit, int n_args) {
    emit_pre(emit, 1 - n_args, 3);
    if (emit->pass == PASS_3) {
        printf("BUILD_TUPLE %d\n", n_args);
    }
}

STATIC void emit_cpy_build_list(emit_t *emit, int n_args) {
    emit_pre(emit, 1 - n_args, 3);
    if (emit->pass == PASS_3) {
        printf("BUILD_LIST %d\n", n_args);
    }
}

STATIC void emit_cpy_list_append(emit_t *emit, int list_index) {
    emit_pre(emit, -1, 3);
    if (emit->pass == PASS_3) {
        printf("LIST_APPEND %d\n", list_index);
    }
}

STATIC void emit_cpy_build_map(emit_t *emit, int n_args) {
    emit_pre(emit, 1, 3);
    if (emit->pass == PASS_3) {
        printf("BUILD_MAP %d\n", n_args);
    }
}

STATIC void emit_cpy_store_map(emit_t *emit) {
    emit_pre(emit, -2, 1);
    if (emit->pass == PASS_3) {
        printf("STORE_MAP\n");
    }
}

STATIC void emit_cpy_map_add(emit_t *emit, int map_index) {
    emit_pre(emit, -2, 3);
    if (emit->pass == PASS_3) {
        printf("MAP_ADD %d\n", map_index);
    }
}

STATIC void emit_cpy_build_set(emit_t *emit, int n_args) {
    emit_pre(emit, 1 - n_args, 3);
    if (emit->pass == PASS_3) {
        printf("BUILD_SET %d\n", n_args);
    }
}

STATIC void emit_cpy_set_add(emit_t *emit, int set_index) {
    emit_pre(emit, -1, 3);
    if (emit->pass == PASS_3) {
        printf("SET_ADD %d\n", set_index);
    }
}

STATIC void emit_cpy_build_slice(emit_t *emit, int n_args) {
    emit_pre(emit, 1 - n_args, 3);
    if (emit->pass == PASS_3) {
        printf("BUILD_SLICE %d\n", n_args);
    }
}

STATIC void emit_cpy_unpack_sequence(emit_t *emit, int n_args) {
    emit_pre(emit, -1 + n_args, 3);
    if (emit->pass == PASS_3) {
        printf("UNPACK_SEQUENCE %d\n", n_args);
    }
}

STATIC void emit_cpy_unpack_ex(emit_t *emit, int n_left, int n_right) {
    emit_pre(emit, -1 + n_left + n_right + 1, 3);
    if (emit->pass == PASS_3) {
        printf("UNPACK_EX %d\n", n_left | (n_right << 8));
    }
}

STATIC void emit_cpy_call_function(emit_t *emit, int n_positional, int n_keyword, bool have_star_arg, bool have_dbl_star_arg) {
    int s = 0;
    if (have_star_arg) {
        s += 1;
    }
    if (have_dbl_star_arg) {
        s += 1;
    }
    emit_pre(emit, -n_positional - 2 * n_keyword - s, 3);
    if (emit->pass == PASS_3) {
        if (have_star_arg) {
            if (have_dbl_star_arg) {
                printf("CALL_FUNCTION_VAR_KW");
            } else {
                printf("CALL_FUNCTION_VAR");
            }
        } else {
            if (have_dbl_star_arg) {
                printf("CALL_FUNCTION_KW");
            } else {
                printf("CALL_FUNCTION");
            }
        }
        printf(" %d, %d\n", n_positional, n_keyword);
    }
}

STATIC void emit_cpy_call_method(emit_t *emit, int n_positional, int n_keyword, bool have_star_arg, bool have_dbl_star_arg) {
    emit_cpy_call_function(emit, n_positional, n_keyword, have_star_arg, have_dbl_star_arg);
}

STATIC void emit_cpy_return_value(emit_t *emit) {
    emit_pre(emit, -1, 1);
    emit->last_emit_was_return_value = true;
    if (emit->pass == PASS_3) {
        printf("RETURN_VALUE\n");
    }
}

STATIC void emit_cpy_raise_varargs(emit_t *emit, int n_args) {
    emit_pre(emit, -n_args, 3);
    if (emit->pass == PASS_3) {
        printf("RAISE_VARARGS %d\n", n_args);
    }
}

STATIC void load_cpy_const_code_and_name(emit_t *emit, qstr qstr) {
    emit_pre(emit, 1, 3);
    if (emit->pass == PASS_3) {
        printf("LOAD_CONST code %s\n", qstr_str(qstr));
    }
    // load qualified name
    emit_pre(emit, 1, 3);
    if (emit->pass == PASS_3) {
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
        printf("%s'\n", qstr_str(qstr));
    }
}

STATIC void emit_cpy_make_function(emit_t *emit, scope_t *scope, uint n_pos_defaults, uint n_kw_defaults) {
    load_cpy_const_code_and_name(emit, scope->simple_name);
    emit_pre(emit, -1 - n_pos_defaults - 2 * n_kw_defaults, 3);
    if (emit->pass == PASS_3) {
        printf("MAKE_FUNCTION %d\n", (n_kw_defaults << 8) | n_pos_defaults);
    }
}

STATIC void emit_cpy_make_closure(emit_t *emit, scope_t *scope, uint n_pos_defaults, uint n_kw_defaults) {
    load_cpy_const_code_and_name(emit, scope->simple_name);
    emit_pre(emit, -2 - n_pos_defaults - 2 * n_kw_defaults, 3);
    if (emit->pass == PASS_3) {
        printf("MAKE_CLOSURE %d\n", (n_kw_defaults << 8) | n_pos_defaults);
    }
}

STATIC void emit_cpy_yield_value(emit_t *emit) {
    emit_pre(emit, 0, 1);
    if (emit->pass == PASS_2) {
        emit->scope->scope_flags |= MP_SCOPE_FLAG_GENERATOR;
    }
    if (emit->pass == PASS_3) {
        printf("YIELD_VALUE\n");
    }
}

STATIC void emit_cpy_yield_from(emit_t *emit) {
    emit_pre(emit, -1, 1);
    if (emit->pass == PASS_2) {
        emit->scope->scope_flags |= MP_SCOPE_FLAG_GENERATOR;
    }
    if (emit->pass == PASS_3) {
        printf("YIELD_FROM\n");
    }
}

const emit_method_table_t emit_cpython_method_table = {
    emit_cpy_set_native_types,
    emit_cpy_start_pass,
    emit_cpy_end_pass,
    emit_cpy_last_emit_was_return_value,
    emit_cpy_get_stack_size,
    emit_cpy_set_stack_size,
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
    emit_cpy_load_const_int,
    emit_cpy_load_const_dec,
    emit_cpy_load_const_id,
    emit_cpy_load_const_str,
    emit_cpy_load_const_verbatim_str,
    emit_cpy_load_fast,
    emit_cpy_load_deref,
    emit_cpy_load_closure,
    emit_cpy_load_name,
    emit_cpy_load_global,
    emit_cpy_load_attr,
    emit_cpy_load_method,
    emit_cpy_load_build_class,
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
    emit_cpy_pop_jump_if_true,
    emit_cpy_pop_jump_if_false,
    emit_cpy_jump_if_true_or_pop,
    emit_cpy_jump_if_false_or_pop,
    emit_cpy_setup_loop,
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
};

#endif // MICROPY_EMIT_CPYTHON
