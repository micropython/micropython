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
#include "compile.h"
#include "scope.h"
#include "runtime.h"
#include "emit.h"

#ifdef EMIT_DO_CPY

struct _emitter_t {
    int pass;
    int next_label;
    int byte_code_offset;
    int stack_size;
    bool last_emit_was_return_value;

    scope_t *scope;

    int max_num_labels;
    int *label_offsets;
};

emitter_t *emit_new() {
    emitter_t *emit = m_new(emitter_t, 1);
    emit->max_num_labels = 0;
    emit->label_offsets = NULL;
    return emit;
}

void emit_start_pass(emitter_t *emit, pass_kind_t pass, scope_t *scope) {
    emit->pass = pass;
    emit->next_label = 1;
    emit->byte_code_offset = 0;
    emit->stack_size = 0;
    emit->last_emit_was_return_value = false;
    emit->scope = scope;
    if (pass > PASS_1) {
        if (emit->label_offsets == NULL) {
            emit->label_offsets = m_new(int, emit->max_num_labels);
        }
        if (pass == PASS_2) {
            memset(emit->label_offsets, -1, emit->max_num_labels * sizeof(int));
        }
    }
}

void emit_end_pass(emitter_t *emit) {
    // check stack is back to zero size
    if (emit->stack_size != 0) {
        printf("ERROR: stack size not back to zero; got %d\n", emit->stack_size);
    }

    // calculate number of labels need
    if (emit->pass == PASS_1) {
        if (emit->next_label > emit->max_num_labels) {
            emit->max_num_labels = emit->next_label;
        }
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

static void emit_pre(emitter_t *emit, int stack_size_delta, int byte_code_size) {
    emit->stack_size += stack_size_delta;
    if (emit->pass > PASS_1 && emit->stack_size > emit->scope->stack_size) {
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

int emit_label_new(emitter_t *emit) {
    return emit->next_label++;
}

void emit_label_assign(emitter_t *emit, int l) {
    emit_pre(emit, 0, 0);
    if (emit->pass > PASS_1) {
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
}

void emit_import_name(emitter_t *emit, qstr qstr) {
    emit_pre(emit, -1, 3);
    if (emit->pass == PASS_3) {
        printf("IMPORT_NAME %s\n", qstr_str(qstr));
    }
}

void emit_import_from(emitter_t *emit, qstr qstr) {
    emit_pre(emit, 1, 3);
    if (emit->pass == PASS_3) {
        printf("IMPORT_FROM %s\n", qstr_str(qstr));
    }
}

void emit_import_star(emitter_t *emit) {
    emit_pre(emit, -1, 1);
    if (emit->pass == PASS_3) {
        printf("IMPORT_STAR\n");
    }
}

void emit_load_const_tok(emitter_t *emit, py_token_kind_t tok) {
    emit_pre(emit, 1, 3);
    if (emit->pass == PASS_3) {
        printf("LOAD_CONST ");
        switch (tok) {
            case PY_TOKEN_KW_FALSE: printf("False"); break;
            case PY_TOKEN_KW_NONE: printf("None"); break;
            case PY_TOKEN_KW_TRUE: printf("True"); break;
            default: printf("?=%d\n", tok); return; assert(0);
        }
        printf("\n");
    }
}

void emit_load_const_small_int(emitter_t *emit, int arg) {
    emit_pre(emit, 1, 3);
    if (emit->pass == PASS_3) {
        printf("LOAD_CONST %d\n", arg);
    }
}

void emit_load_const_int(emitter_t *emit, qstr qstr) {
    emit_pre(emit, 1, 3);
    if (emit->pass == PASS_3) {
        printf("LOAD_CONST %s\n", qstr_str(qstr));
    }
}

void emit_load_const_dec(emitter_t *emit, qstr qstr) {
    emit_pre(emit, 1, 3);
    if (emit->pass == PASS_3) {
        printf("LOAD_CONST %s\n", qstr_str(qstr));
    }
}

void emit_load_const_id(emitter_t *emit, qstr qstr) {
    emit_pre(emit, 1, 3);
    if (emit->pass == PASS_3) {
        printf("LOAD_CONST '%s'\n", qstr_str(qstr));
    }
}

void emit_load_const_str(emitter_t *emit, qstr qstr, bool bytes) {
    emit_pre(emit, 1, 3);
    if (emit->pass == PASS_3) {
        printf("LOAD_CONST ");
        emit_load_const_verbatim_quoted_str(emit, qstr, bytes);
        printf("\n");
    }
}

void emit_load_const_verbatim_start(emitter_t *emit) {
    emit_pre(emit, 1, 3);
    if (emit->pass == PASS_3) {
        printf("LOAD_CONST ");
    }
}

void emit_load_const_verbatim_int(emitter_t *emit, int val) {
    if (emit->pass == PASS_3) {
        printf("%d", val);
    }
}

void emit_load_const_verbatim_str(emitter_t *emit, const char *str) {
    if (emit->pass == PASS_3) {
        printf("%s", str);
    }
}

void emit_load_const_verbatim_strn(emitter_t *emit, const char *str, int len) {
    if (emit->pass == PASS_3) {
        printf("%.*s", len, str);
    }
}

void emit_load_const_verbatim_quoted_str(emitter_t *emit, qstr qstr, bool bytes) {
    // TODO strings should be escaped before we get here
    if (emit->pass == PASS_3) {
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
        bool quote_single = false;
        if (has_single_quote && !has_double_quote) {
            printf("\"");
        } else {
            quote_single = true;
            printf("'");
        }
        for (int i = 0; i < len; i++) {
            if (str[i] == '\n') {
                printf("\\n");
            } else if (str[i] == '\\' && str[i + 1] == '\'') {
                i += 1;
                if (quote_single) {
                    printf("\\'");
                } else {
                    printf("'");
                }
            } else if (str[i] == '\'' && quote_single) {
                printf("\\'");
            } else {
                printf("%c", str[i]);
            }
        }
        if (has_single_quote && !has_double_quote) {
            printf("\"");
        } else {
            printf("'");
        }
    }
}

void emit_load_const_verbatim_end(emitter_t *emit) {
    if (emit->pass == PASS_3) {
        printf("\n");
    }
}

void emit_load_name(emitter_t *emit, qstr qstr) {
    emit_pre(emit, 1, 3);
    if (emit->pass == PASS_3) {
        printf("LOAD_NAME %s\n", qstr_str(qstr));
    }
}

void emit_load_global(emitter_t *emit, qstr qstr) {
    emit_pre(emit, 1, 3);
    if (emit->pass == PASS_3) {
        printf("LOAD_GLOBAL %s\n", qstr_str(qstr));
    }
}

void emit_load_fast(emitter_t *emit, qstr qstr, int local_num) {
    emit_pre(emit, 1, 3);
    if (emit->pass == PASS_3) {
        printf("LOAD_FAST %s\n", qstr_str(qstr));
    }
}

void emit_load_deref(emitter_t *emit, qstr qstr) {
    emit_pre(emit, 1, 3);
    if (emit->pass == PASS_3) {
        printf("LOAD_DEREF %s\n", qstr_str(qstr));
    }
}

void emit_load_closure(emitter_t *emit, qstr qstr) {
    emit_pre(emit, 1, 3);
    if (emit->pass == PASS_3) {
        printf("LOAD_CLOSURE %s\n", qstr_str(qstr));
    }
}

void emit_load_attr(emitter_t *emit, qstr qstr) {
    emit_pre(emit, 0, 3);
    if (emit->pass == PASS_3) {
        printf("LOAD_ATTR %s\n", qstr_str(qstr));
    }
}

void emit_load_method(emitter_t *emit, qstr qstr) {
    emit_load_attr(emit, qstr);
}

void emit_load_build_class(emitter_t *emit) {
    emit_pre(emit, 1, 1);
    if (emit->pass == PASS_3) {
        printf("LOAD_BUILD_CLASS\n");
    }
}

void emit_store_name(emitter_t *emit, qstr qstr) {
    emit_pre(emit, -1, 3);
    if (emit->pass == PASS_3) {
        printf("STORE_NAME %s\n", qstr_str(qstr));
    }
}

void emit_store_global(emitter_t *emit, qstr qstr) {
    emit_pre(emit, -1, 3);
    if (emit->pass == PASS_3) {
        printf("STORE_GLOBAL %s\n", qstr_str(qstr));
    }
}

void emit_store_fast(emitter_t *emit, qstr qstr, int local_num) {
    emit_pre(emit, -1, 3);
    if (emit->pass == PASS_3) {
        printf("STORE_FAST %s\n", qstr_str(qstr));
    }
}

void emit_store_deref(emitter_t *emit, qstr qstr) {
    emit_pre(emit, -1, 3);
    if (emit->pass == PASS_3) {
        printf("STORE_DEREF %s\n", qstr_str(qstr));
    }
}

void emit_store_attr(emitter_t *emit, qstr qstr) {
    emit_pre(emit, -2, 3);
    if (emit->pass == PASS_3) {
        printf("STORE_ATTR %s\n", qstr_str(qstr));
    }
}

void emit_store_locals(emitter_t *emit) {
    emit_pre(emit, -1, 1);
    if (emit->pass == PASS_3) {
        printf("STORE_LOCALS\n");
    }
}

void emit_store_subscr(emitter_t *emit) {
    emit_pre(emit, -3, 1);
    if (emit->pass == PASS_3) {
        printf("STORE_SUBSCR\n");
    }
}

void emit_delete_name(emitter_t *emit, qstr qstr) {
    emit_pre(emit, 0, 3);
    if (emit->pass == PASS_3) {
        printf("DELETE_NAME %s\n", qstr_str(qstr));
    }
}

void emit_delete_global(emitter_t *emit, qstr qstr) {
    emit_pre(emit, 0, 3);
    if (emit->pass == PASS_3) {
        printf("DELETE_GLOBAL %s\n", qstr_str(qstr));
    }
}

void emit_delete_fast(emitter_t *emit, qstr qstr, int local_num) {
    emit_pre(emit, 0, 3);
    if (emit->pass == PASS_3) {
        printf("DELETE_FAST %s\n", qstr_str(qstr));
    }
}

void emit_delete_deref(emitter_t *emit, qstr qstr) {
    emit_pre(emit, 0, 3);
    if (emit->pass == PASS_3) {
        printf("DELETE_DEREF %s\n", qstr_str(qstr));
    }
}

void emit_delete_attr(emitter_t *emit, qstr qstr) {
    emit_pre(emit, -1, 3);
    if (emit->pass == PASS_3) {
        printf("DELETE_ATTR %s\n", qstr_str(qstr));
    }
}

void emit_delete_subscr(emitter_t *emit) {
    emit_pre(emit, -2, 1);
    if (emit->pass == PASS_3) {
        printf("DELETE_SUBSCR\n");
    }
}

void emit_dup_top(emitter_t *emit) {
    emit_pre(emit, 1, 1);
    if (emit->pass == PASS_3) {
        printf("DUP_TOP\n");
    }
}

void emit_dup_top_two(emitter_t *emit) {
    emit_pre(emit, 2, 1);
    if (emit->pass == PASS_3) {
        printf("DUP_TOP_TWO\n");
    }
}

void emit_pop_top(emitter_t *emit) {
    emit_pre(emit, -1, 1);
    if (emit->pass == PASS_3) {
        printf("POP_TOP\n");
    }
}

void emit_rot_two(emitter_t *emit) {
    emit_pre(emit, 0, 1);
    if (emit->pass == PASS_3) {
        printf("ROT_TWO\n");
    }
}

void emit_rot_three(emitter_t *emit) {
    emit_pre(emit, 0, 1);
    if (emit->pass == PASS_3) {
        printf("ROT_THREE\n");
    }
}

void emit_jump(emitter_t *emit, int label) {
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

void emit_pop_jump_if_true(emitter_t *emit, int label) {
    emit_pre(emit, -1, 3);
    if (emit->pass == PASS_3) {
        printf("POP_JUMP_IF_TRUE %d\n", emit->label_offsets[label]);
    }
}

void emit_pop_jump_if_false(emitter_t *emit, int label) {
    emit_pre(emit, -1, 3);
    if (emit->pass == PASS_3) {
        printf("POP_JUMP_IF_FALSE %d\n", emit->label_offsets[label]);
    }
}

void emit_jump_if_true_or_pop(emitter_t *emit, int label) {
    emit_pre(emit, -1, 3);
    if (emit->pass == PASS_3) {
        printf("JUMP_IF_TRUE_OR_POP %d\n", emit->label_offsets[label]);
    }
}

void emit_jump_if_false_or_pop(emitter_t *emit, int label) {
    emit_pre(emit, -1, 3);
    if (emit->pass == PASS_3) {
        printf("JUMP_IF_FALSE_OR_POP %d\n", emit->label_offsets[label]);
    }
}

void emit_setup_loop(emitter_t *emit, int label) {
    emit_pre(emit, 0, 3);
    if (emit->pass == PASS_3) {
        printf("SETUP_LOOP %d\n", emit->label_offsets[label]);
    }
}

void emit_break_loop(emitter_t *emit, int label) {
    emit_pre(emit, 0, 1);
    if (emit->pass == PASS_3) {
        printf("BREAK_LOOP\n"); // CPython doesn't have label
        //printf("BREAK_LOOP %d\n", emit->label_offsets[label]);
    }
}

void emit_continue_loop(emitter_t *emit, int label) {
    emit_pre(emit, 0, 3);
    if (emit->pass == PASS_3) {
        printf("CONTINUE_LOOP %d\n", emit->label_offsets[label]);
    }
}

void emit_setup_with(emitter_t *emit, int label) {
    emit_pre(emit, 7, 3);
    if (emit->pass == PASS_3) {
        printf("SETUP_WITH %d\n", emit->label_offsets[label]);
    }
}

void emit_with_cleanup(emitter_t *emit) {
    emit_pre(emit, -7, 1);
    if (emit->pass == PASS_3) {
        printf("WITH_CLEANUP\n");
    }
}

void emit_setup_except(emitter_t *emit, int label) {
    emit_pre(emit, 6, 3);
    if (emit->pass == PASS_3) {
        printf("SETUP_EXCEPT %d\n", emit->label_offsets[label]);
    }
}

void emit_setup_finally(emitter_t *emit, int label) {
    emit_pre(emit, 6, 3);
    if (emit->pass == PASS_3) {
        printf("SETUP_FINALLY %d\n", emit->label_offsets[label]);
    }
}

void emit_end_finally(emitter_t *emit) {
    emit_pre(emit, -1, 1);
    if (emit->pass == PASS_3) {
        printf("END_FINALLY\n");
    }
}

void emit_get_iter(emitter_t *emit) {
    emit_pre(emit, 0, 1);
    if (emit->pass == PASS_3) {
        printf("GET_ITER\n");
    }
}

void emit_for_iter(emitter_t *emit, int label) {
    emit_pre(emit, 1, 3);
    if (emit->pass == PASS_3) {
        printf("FOR_ITER %d\n", emit->label_offsets[label]);
    }
}

void emit_for_iter_end(emitter_t *emit) {
    emit_pre(emit, -1, 0);
}

void emit_pop_block(emitter_t *emit) {
    emit_pre(emit, 0, 1);
    if (emit->pass == PASS_3) {
        printf("POP_BLOCK\n");
    }
}

void emit_pop_except(emitter_t *emit) {
    emit_pre(emit, 0, 1);
    if (emit->pass == PASS_3) {
        printf("POP_EXCEPT\n");
    }
}

void emit_unary_op(emitter_t *emit, rt_unary_op_t op) {
    emit_pre(emit, 0, 1);
    if (emit->pass == PASS_3) {
        switch (op) {
            case RT_UNARY_OP_NOT: printf("UNARY_NOT\n"); break;
            case RT_UNARY_OP_POSITIVE: printf("UNARY_POSITIVE\n"); break;
            case RT_UNARY_OP_NEGATIVE: printf("UNARY_NEGATIVE\n"); break;
            case RT_UNARY_OP_INVERT: printf("UNARY_INVERT\n"); break;
            default: assert(0);
        }
    }
}

void emit_binary_op(emitter_t *emit, rt_binary_op_t op) {
    emit_pre(emit, -1, 1);
    if (emit->pass == PASS_3) {
        switch (op) {
            case RT_BINARY_OP_SUBSCR: printf("BINARY_SUBSCR\n"); break;
            case RT_BINARY_OP_OR: printf("BINARY_OR\n"); break;
            case RT_BINARY_OP_XOR: printf("BINARY_XOR\n"); break;
            case RT_BINARY_OP_AND: printf("BINARY_AND\n"); break;
            case RT_BINARY_OP_LSHIFT: printf("BINARY_LSHIFT\n"); break;
            case RT_BINARY_OP_RSHIFT: printf("BINARY_RSHIFT\n"); break;
            case RT_BINARY_OP_ADD: printf("BINARY_ADD\n"); break;
            case RT_BINARY_OP_SUBTRACT: printf("BINARY_SUBTRACT\n"); break;
            case RT_BINARY_OP_MULTIPLY: printf("BINARY_MULTIPLY\n"); break;
            case RT_BINARY_OP_FLOOR_DIVIDE: printf("BINARY_FLOOR_DIVIDE\n"); break;
            case RT_BINARY_OP_TRUE_DIVIDE: printf("BINARY_TRUE_DIVIDE\n"); break;
            case RT_BINARY_OP_MODULO: printf("BINARY_MODULO\n"); break;
            case RT_BINARY_OP_POWER: printf("BINARY_POWER\n"); break;
            case RT_BINARY_OP_INPLACE_OR: printf("INPLACE_OR\n"); break;
            case RT_BINARY_OP_INPLACE_XOR: printf("INPLACE_XOR\n"); break;
            case RT_BINARY_OP_INPLACE_AND: printf("INPLACE_AND\n"); break;
            case RT_BINARY_OP_INPLACE_LSHIFT: printf("INPLACE_LSHIFT\n"); break;
            case RT_BINARY_OP_INPLACE_RSHIFT: printf("INPLACE_RSHIFT\n"); break;
            case RT_BINARY_OP_INPLACE_ADD: printf("INPLACE_ADD\n"); break;
            case RT_BINARY_OP_INPLACE_SUBTRACT: printf("INPLACE_SUBTRACT\n"); break;
            case RT_BINARY_OP_INPLACE_MULTIPLY: printf("INPLACE_MULTIPLY\n"); break;
            case RT_BINARY_OP_INPLACE_FLOOR_DIVIDE: printf("INPLACE_FLOOR_DIVIDE\n"); break;
            case RT_BINARY_OP_INPLACE_TRUE_DIVIDE: printf("INPLACE_TRUE_DIVIDE\n"); break;
            case RT_BINARY_OP_INPLACE_MODULO: printf("INPLACE_MODULO\n"); break;
            case RT_BINARY_OP_INPLACE_POWER: printf("INPLACE_POWER\n"); break;
            default: assert(0);
        }
    }
}

void emit_compare_op(emitter_t *emit, rt_compare_op_t op) {
    emit_pre(emit, -1, 3);
    if (emit->pass == PASS_3) {
        switch (op) {
            case RT_COMPARE_OP_LESS: printf("COMPARE_OP <\n"); break;
            case RT_COMPARE_OP_MORE: printf("COMPARE_OP >\n"); break;
            case RT_COMPARE_OP_EQUAL: printf("COMPARE_OP ==\n"); break;
            case RT_COMPARE_OP_LESS_EQUAL: printf("COMPARE_OP <=\n"); break;
            case RT_COMPARE_OP_MORE_EQUAL: printf("COMPARE_OP >=\n"); break;
            case RT_COMPARE_OP_NOT_EQUAL: printf("COMPARE_OP !=\n"); break;
            case RT_COMPARE_OP_IN: printf("COMPARE_OP in\n"); break;
            case RT_COMPARE_OP_NOT_IN: printf("COMPARE_OP not in\n"); break;
            case RT_COMPARE_OP_IS: printf("COMPARE_OP is\n"); break;
            case RT_COMPARE_OP_IS_NOT: printf("COMPARE_OP is not\n"); break;
            case RT_COMPARE_OP_EXCEPTION_MATCH: printf("COMPARE_OP exception match\n"); break;
            default: assert(0);
        }
    }
}

void emit_build_tuple(emitter_t *emit, int n_args) {
    emit_pre(emit, 1 - n_args, 3);
    if (emit->pass == PASS_3) {
        printf("BUILD_TUPLE %d\n", n_args);
    }
}

void emit_build_list(emitter_t *emit, int n_args) {
    emit_pre(emit, 1 - n_args, 3);
    if (emit->pass == PASS_3) {
        printf("BUILD_LIST %d\n", n_args);
    }
}

void emit_list_append(emitter_t *emit, int list_index) {
    emit_pre(emit, -1, 3);
    if (emit->pass == PASS_3) {
        printf("LIST_APPEND %d\n", list_index);
    }
}

void emit_build_map(emitter_t *emit, int n_args) {
    emit_pre(emit, 1, 3);
    if (emit->pass == PASS_3) {
        printf("BUILD_MAP %d\n", n_args);
    }
}

void emit_store_map(emitter_t *emit) {
    emit_pre(emit, -2, 1);
    if (emit->pass == PASS_3) {
        printf("STORE_MAP\n");
    }
}

void emit_map_add(emitter_t *emit, int map_index) {
    emit_pre(emit, -2, 3);
    if (emit->pass == PASS_3) {
        printf("MAP_ADD %d\n", map_index);
    }
}

void emit_build_set(emitter_t *emit, int n_args) {
    emit_pre(emit, 1 - n_args, 3);
    if (emit->pass == PASS_3) {
        printf("BUILD_SET %d\n", n_args);
    }
}

void emit_set_add(emitter_t *emit, int set_index) {
    emit_pre(emit, -1, 3);
    if (emit->pass == PASS_3) {
        printf("SET_ADD %d\n", set_index);
    }
}

void emit_build_slice(emitter_t *emit, int n_args) {
    emit_pre(emit, 1 - n_args, 3);
    if (emit->pass == PASS_3) {
        printf("BUILD_SLICE %d\n", n_args);
    }
}

void emit_unpack_sequence(emitter_t *emit, int n_args) {
    emit_pre(emit, -1 + n_args, 3);
    if (emit->pass == PASS_3) {
        printf("UNPACK_SEQUENCE %d\n", n_args);
    }
}

void emit_unpack_ex(emitter_t *emit, int n_left, int n_right) {
    emit_pre(emit, -1 + n_left + n_right + 1, 3);
    if (emit->pass == PASS_3) {
        printf("UNPACK_EX %d\n", n_left | (n_right << 8));
    }
}

void emit_call_function(emitter_t *emit, int n_positional, int n_keyword, bool have_star_arg, bool have_dbl_star_arg) {
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

void emit_call_method(emitter_t *emit, int n_positional, int n_keyword, bool have_star_arg, bool have_dbl_star_arg) {
    emit_call_function(emit, n_positional, n_keyword, have_star_arg, have_dbl_star_arg);
}

void emit_return_value(emitter_t *emit) {
    emit_pre(emit, -1, 1);
    emit->last_emit_was_return_value = true;
    if (emit->pass == PASS_3) {
        printf("RETURN_VALUE\n");
    }
}

void emit_raise_varargs(emitter_t *emit, int n_args) {
    emit_pre(emit, -n_args, 3);
    if (emit->pass == PASS_3) {
        printf("RAISE_VARARGS %d\n", n_args);
    }
}

void load_const_code_and_name(emitter_t *emit, qstr qstr) {
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

void emit_make_function(emitter_t *emit, scope_t *scope, int n_dict_params, int n_default_params) {
    load_const_code_and_name(emit, scope->simple_name);
    emit_pre(emit, -1 - n_default_params - 2 * n_dict_params, 3);
    if (emit->pass == PASS_3) {
        printf("MAKE_FUNCTION %d\n", (n_dict_params << 8) | n_default_params);
    }
}

void emit_make_closure(emitter_t *emit, scope_t *scope, int n_dict_params, int n_default_params) {
    load_const_code_and_name(emit, scope->simple_name);
    emit_pre(emit, -2 - n_default_params - 2 * n_dict_params, 3);
    if (emit->pass == PASS_3) {
        printf("MAKE_CLOSURE %d\n", (n_dict_params << 8) | n_default_params);
    }
}

void emit_yield_value(emitter_t *emit) {
    emit_pre(emit, 0, 1);
    if (emit->pass == PASS_2) {
        emit->scope->flags |= SCOPE_FLAG_GENERATOR;
    }
    if (emit->pass == PASS_3) {
        printf("YIELD_VALUE\n");
    }
}

void emit_yield_from(emitter_t *emit) {
    emit_pre(emit, -1, 1);
    if (emit->pass == PASS_2) {
        emit->scope->flags |= SCOPE_FLAG_GENERATOR;
    }
    if (emit->pass == PASS_3) {
        printf("YIELD_FROM\n");
    }
}

#endif // EMIT_DO_CPY
