#include <unistd.h>
#include <stdlib.h>
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
#include "bc0.h"

struct _emit_t {
    pass_kind_t pass;
    int stack_size;
    bool last_emit_was_return_value;

    scope_t *scope;

    uint last_source_line_offset;
    uint last_source_line;

    uint max_num_labels;
    uint *label_offsets;

    uint code_info_offset;
    uint code_info_size;
    uint byte_code_offset;
    uint byte_code_size;
    byte *code_base; // stores both byte code and code info
    byte dummy_data[8];
};

emit_t *emit_bc_new(uint max_num_labels) {
    emit_t *emit = m_new0(emit_t, 1);
    emit->max_num_labels = max_num_labels;
    emit->label_offsets = m_new(uint, emit->max_num_labels);
    return emit;
}

void emit_bc_free(emit_t *emit) {
    m_del(uint, emit->label_offsets, emit->max_num_labels);
    m_del_obj(emit_t, emit);
}

// all functions must go through this one to emit code info
static byte* emit_get_cur_to_write_code_info(emit_t* emit, int num_bytes_to_write) {
    //printf("emit %d\n", num_bytes_to_write);
    if (emit->pass < PASS_3) {
        emit->code_info_offset += num_bytes_to_write;
        return emit->dummy_data;
    } else {
        assert(emit->code_info_offset + num_bytes_to_write <= emit->code_info_size);
        byte *c = emit->code_base + emit->code_info_offset;
        emit->code_info_offset += num_bytes_to_write;
        return c;
    }
}

static void emit_write_code_info_qstr(emit_t* emit, qstr qstr) {
    byte* c = emit_get_cur_to_write_code_info(emit, 4);
    // TODO variable length encoding for qstr
    c[0] = qstr & 0xff;
    c[1] = (qstr >> 8) & 0xff;
    c[2] = (qstr >> 16) & 0xff;
    c[3] = (qstr >> 24) & 0xff;
}

static void emit_write_code_info_bytes_lines(emit_t* emit, uint bytes_to_skip, uint lines_to_skip) {
    for (; bytes_to_skip > 31; bytes_to_skip -= 31) {
        *emit_get_cur_to_write_code_info(emit, 1) = 31;
    }
    for (; lines_to_skip > 7; lines_to_skip -= 7) {
        *emit_get_cur_to_write_code_info(emit, 1) = 7 << 5;
    }
    *emit_get_cur_to_write_code_info(emit, 1) = bytes_to_skip | (lines_to_skip << 5);
}

// all functions must go through this one to emit byte code
static byte* emit_get_cur_to_write_byte_code(emit_t* emit, int num_bytes_to_write) {
    //printf("emit %d\n", num_bytes_to_write);
    if (emit->pass < PASS_3) {
        emit->byte_code_offset += num_bytes_to_write;
        return emit->dummy_data;
    } else {
        assert(emit->byte_code_offset + num_bytes_to_write <= emit->byte_code_size);
        byte *c = emit->code_base + emit->code_info_size + emit->byte_code_offset;
        emit->byte_code_offset += num_bytes_to_write;
        return c;
    }
}

static void emit_write_byte_code_byte(emit_t* emit, byte b1) {
    byte* c = emit_get_cur_to_write_byte_code(emit, 1);
    c[0] = b1;
}

static void emit_write_byte_code_byte_byte(emit_t* emit, byte b1, uint b2) {
    assert((b2 & (~0xff)) == 0);
    byte* c = emit_get_cur_to_write_byte_code(emit, 2);
    c[0] = b1;
    c[1] = b2;
}

static void emit_write_byte_code_uint(emit_t* emit, uint num) {
    if (num <= 127) { // fits in 0x7f
        // fit argument in single byte
        byte* c = emit_get_cur_to_write_byte_code(emit, 1);
        c[0] = num;
    } else if (num <= 16383) { // fits in 0x3fff
        // fit argument in two bytes
        byte* c = emit_get_cur_to_write_byte_code(emit, 2);
        c[0] = (num >> 8) | 0x80;
        c[1] = num;
    } else {
        // larger numbers not implemented/supported
        assert(0);
    }
}

// integers (for small ints) are stored as 24 bits, in excess
static void emit_write_byte_code_byte_int(emit_t* emit, byte b1, machine_int_t num) {
    num += 0x800000;
    assert(0 <= num && num <= 0xffffff);
    byte* c = emit_get_cur_to_write_byte_code(emit, 4);
    c[0] = b1;
    c[1] = num;
    c[2] = num >> 8;
    c[3] = num >> 16;
}

static void emit_write_byte_code_byte_uint(emit_t* emit, byte b, uint num) {
    emit_write_byte_code_byte(emit, b);
    emit_write_byte_code_uint(emit, num);
}

/* currently unused
static void emit_write_byte_code_byte_uint_uint(emit_t* emit, byte b, uint num1, uint num2) {
    emit_write_byte_code_byte(emit, b);
    emit_write_byte_code_byte_uint(emit, num1);
    emit_write_byte_code_byte_uint(emit, num2);
}
*/

static void emit_write_byte_code_byte_qstr(emit_t* emit, byte b, qstr qstr) {
    emit_write_byte_code_byte_uint(emit, b, qstr);
}

// unsigned labels are relative to ip following this instruction, stored as 16 bits
static void emit_write_byte_code_byte_unsigned_label(emit_t* emit, byte b1, int label) {
    uint byte_code_offset;
    if (emit->pass < PASS_3) {
        byte_code_offset = 0;
    } else {
        byte_code_offset = emit->label_offsets[label] - emit->byte_code_offset - 3;
    }
    byte* c = emit_get_cur_to_write_byte_code(emit, 3);
    c[0] = b1;
    c[1] = byte_code_offset;
    c[2] = byte_code_offset >> 8;
}

// signed labels are relative to ip following this instruction, stored as 16 bits, in excess
static void emit_write_byte_code_byte_signed_label(emit_t* emit, byte b1, int label) {
    int byte_code_offset;
    if (emit->pass < PASS_3) {
        byte_code_offset = 0;
    } else {
        byte_code_offset = emit->label_offsets[label] - emit->byte_code_offset - 3 + 0x8000;
    }
    byte* c = emit_get_cur_to_write_byte_code(emit, 3);
    c[0] = b1;
    c[1] = byte_code_offset;
    c[2] = byte_code_offset >> 8;
}

static void emit_bc_set_native_types(emit_t *emit, bool do_native_types) {
}

static void emit_bc_start_pass(emit_t *emit, pass_kind_t pass, scope_t *scope) {
    emit->pass = pass;
    emit->stack_size = 0;
    emit->last_emit_was_return_value = false;
    emit->scope = scope;
    emit->last_source_line_offset = 0;
    emit->last_source_line = 1;
    if (pass == PASS_2) {
        memset(emit->label_offsets, -1, emit->max_num_labels * sizeof(uint));
    }
    emit->byte_code_offset = 0;
    emit->code_info_offset = 0;

    // write code info size (don't know size at this stage in PASS_2 so need to use maximum space (4 bytes) to write it)
    {
        byte* c = emit_get_cur_to_write_code_info(emit, 4);
        machine_uint_t s = emit->code_info_size;
        c[0] = s & 0xff;
        c[1] = (s >> 8) & 0xff;
        c[2] = (s >> 16) & 0xff;
        c[3] = (s >> 24) & 0xff;
    }

    // code info
    emit_write_code_info_qstr(emit, scope->source_file);
    emit_write_code_info_qstr(emit, scope->simple_name);

    // prelude for initialising closed over variables
    int num_cell = 0;
    for (int i = 0; i < scope->id_info_len; i++) {
        id_info_t *id = &scope->id_info[i];
        if (id->kind == ID_INFO_KIND_CELL) {
            num_cell += 1;
        }
    }
    assert(num_cell <= 255);
    emit_write_byte_code_byte(emit, num_cell); // write number of locals that are cells
    for (int i = 0; i < scope->id_info_len; i++) {
        id_info_t *id = &scope->id_info[i];
        if (id->kind == ID_INFO_KIND_CELL) {
            emit_write_byte_code_byte(emit, id->local_num); // write the local which should be converted to a cell
        }
    }
}

static void emit_bc_end_pass(emit_t *emit) {
    // check stack is back to zero size
    if (emit->stack_size != 0) {
        printf("ERROR: stack size not back to zero; got %d\n", emit->stack_size);
    }

    emit_write_code_info_bytes_lines(emit, 0, 0); // end of line number info

    if (emit->pass == PASS_2) {
        // calculate size of code in bytes
        emit->code_info_size = emit->code_info_offset;
        emit->byte_code_size = emit->byte_code_offset;
        emit->code_base = m_new(byte, emit->code_info_size + emit->byte_code_size);

    } else if (emit->pass == PASS_3) {
        rt_assign_byte_code(emit->scope->unique_code_id, emit->code_base, emit->code_info_size + emit->byte_code_size, emit->scope->num_params, emit->scope->num_locals, emit->scope->stack_size, (emit->scope->flags & SCOPE_FLAG_GENERATOR) != 0);
    }
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

static void emit_bc_set_source_line(emit_t *emit, int source_line) {
    //printf("source: line %d -> %d  offset %d -> %d\n", emit->last_source_line, source_line, emit->last_source_line_offset, emit->byte_code_offset);
#if MICROPY_ENABLE_SOURCE_LINE
    if (source_line > emit->last_source_line) {
        uint bytes_to_skip = emit->byte_code_offset - emit->last_source_line_offset;
        uint lines_to_skip = source_line - emit->last_source_line;
        emit_write_code_info_bytes_lines(emit, bytes_to_skip, lines_to_skip);
        //printf("  %d %d\n", bytes_to_skip, lines_to_skip);
        emit->last_source_line_offset = emit->byte_code_offset;
        emit->last_source_line = source_line;
    }
#endif
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
        emit->label_offsets[l] = emit->byte_code_offset;
    } else if (emit->pass == PASS_3) {
        // ensure label offset has not changed from PASS_2 to PASS_3
        //printf("l%d: (at %d vs %d)\n", l, emit->byte_code_offset, emit->label_offsets[l]);
        assert(emit->label_offsets[l] == emit->byte_code_offset);
    }
}

static void emit_bc_import_name(emit_t *emit, qstr qstr) {
    emit_pre(emit, -1);
    emit_write_byte_code_byte_qstr(emit, MP_BC_IMPORT_NAME, qstr);
}

static void emit_bc_import_from(emit_t *emit, qstr qstr) {
    emit_pre(emit, 1);
    emit_write_byte_code_byte_qstr(emit, MP_BC_IMPORT_FROM, qstr);
}

static void emit_bc_import_star(emit_t *emit) {
    emit_pre(emit, -1);
    emit_write_byte_code_byte(emit, MP_BC_IMPORT_STAR);
}

static void emit_bc_load_const_tok(emit_t *emit, mp_token_kind_t tok) {
    emit_pre(emit, 1);
    switch (tok) {
        case MP_TOKEN_KW_FALSE: emit_write_byte_code_byte(emit, MP_BC_LOAD_CONST_FALSE); break;
        case MP_TOKEN_KW_NONE: emit_write_byte_code_byte(emit, MP_BC_LOAD_CONST_NONE); break;
        case MP_TOKEN_KW_TRUE: emit_write_byte_code_byte(emit, MP_BC_LOAD_CONST_TRUE); break;
        case MP_TOKEN_ELLIPSIS: emit_write_byte_code_byte(emit, MP_BC_LOAD_CONST_ELLIPSIS); break;
        default: assert(0);
    }
}

static void emit_bc_load_const_small_int(emit_t *emit, machine_int_t arg) {
    emit_pre(emit, 1);
    emit_write_byte_code_byte_int(emit, MP_BC_LOAD_CONST_SMALL_INT, arg);
}

static void emit_bc_load_const_int(emit_t *emit, qstr qstr) {
    emit_pre(emit, 1);
    emit_write_byte_code_byte_qstr(emit, MP_BC_LOAD_CONST_INT, qstr);
}

static void emit_bc_load_const_dec(emit_t *emit, qstr qstr) {
    emit_pre(emit, 1);
    emit_write_byte_code_byte_qstr(emit, MP_BC_LOAD_CONST_DEC, qstr);
}

static void emit_bc_load_const_id(emit_t *emit, qstr qstr) {
    emit_pre(emit, 1);
    emit_write_byte_code_byte_qstr(emit, MP_BC_LOAD_CONST_ID, qstr);
}

static void emit_bc_load_const_str(emit_t *emit, qstr qstr, bool bytes) {
    emit_pre(emit, 1);
    if (bytes) {
        emit_write_byte_code_byte_qstr(emit, MP_BC_LOAD_CONST_BYTES, qstr);
    } else {
        emit_write_byte_code_byte_qstr(emit, MP_BC_LOAD_CONST_STRING, qstr);
    }
}

static void emit_bc_load_const_verbatim_str(emit_t *emit, const char *str) {
    // not needed/supported for BC
    assert(0);
}

static void emit_bc_load_fast(emit_t *emit, qstr qstr, int local_num) {
    assert(local_num >= 0);
    emit_pre(emit, 1);
    switch (local_num) {
        case 0: emit_write_byte_code_byte(emit, MP_BC_LOAD_FAST_0); break;
        case 1: emit_write_byte_code_byte(emit, MP_BC_LOAD_FAST_1); break;
        case 2: emit_write_byte_code_byte(emit, MP_BC_LOAD_FAST_2); break;
        default: emit_write_byte_code_byte_uint(emit, MP_BC_LOAD_FAST_N, local_num); break;
    }
}

static void emit_bc_load_deref(emit_t *emit, qstr qstr, int local_num) {
    emit_pre(emit, 1);
    emit_write_byte_code_byte_uint(emit, MP_BC_LOAD_DEREF, local_num);
}

static void emit_bc_load_closure(emit_t *emit, qstr qstr, int local_num) {
    // not needed/supported for BC
    assert(0);
}

static void emit_bc_load_name(emit_t *emit, qstr qstr) {
    emit_pre(emit, 1);
    emit_write_byte_code_byte_qstr(emit, MP_BC_LOAD_NAME, qstr);
}

static void emit_bc_load_global(emit_t *emit, qstr qstr) {
    emit_pre(emit, 1);
    emit_write_byte_code_byte_qstr(emit, MP_BC_LOAD_GLOBAL, qstr);
}

static void emit_bc_load_attr(emit_t *emit, qstr qstr) {
    emit_pre(emit, 0);
    emit_write_byte_code_byte_qstr(emit, MP_BC_LOAD_ATTR, qstr);
}

static void emit_bc_load_method(emit_t *emit, qstr qstr) {
    emit_pre(emit, 1);
    emit_write_byte_code_byte_qstr(emit, MP_BC_LOAD_METHOD, qstr);
}

static void emit_bc_load_build_class(emit_t *emit) {
    emit_pre(emit, 1);
    emit_write_byte_code_byte(emit, MP_BC_LOAD_BUILD_CLASS);
}

static void emit_bc_store_fast(emit_t *emit, qstr qstr, int local_num) {
    assert(local_num >= 0);
    emit_pre(emit, -1);
    switch (local_num) {
        case 0: emit_write_byte_code_byte(emit, MP_BC_STORE_FAST_0); break;
        case 1: emit_write_byte_code_byte(emit, MP_BC_STORE_FAST_1); break;
        case 2: emit_write_byte_code_byte(emit, MP_BC_STORE_FAST_2); break;
        default: emit_write_byte_code_byte_uint(emit, MP_BC_STORE_FAST_N, local_num); break;
    }
}

static void emit_bc_store_deref(emit_t *emit, qstr qstr, int local_num) {
    emit_pre(emit, -1);
    emit_write_byte_code_byte_uint(emit, MP_BC_STORE_DEREF, local_num);
}

static void emit_bc_store_name(emit_t *emit, qstr qstr) {
    emit_pre(emit, -1);
    emit_write_byte_code_byte_qstr(emit, MP_BC_STORE_NAME, qstr);
}

static void emit_bc_store_global(emit_t *emit, qstr qstr) {
    emit_pre(emit, -1);
    emit_write_byte_code_byte_qstr(emit, MP_BC_STORE_GLOBAL, qstr);
}

static void emit_bc_store_attr(emit_t *emit, qstr qstr) {
    emit_pre(emit, -2);
    emit_write_byte_code_byte_qstr(emit, MP_BC_STORE_ATTR, qstr);
}

static void emit_bc_store_subscr(emit_t *emit) {
    emit_pre(emit, -3);
    emit_write_byte_code_byte(emit, MP_BC_STORE_SUBSCR);
}

static void emit_bc_store_locals(emit_t *emit) {
    // not needed
    emit_pre(emit, -1);
    emit_write_byte_code_byte(emit, MP_BC_POP_TOP);
}

static void emit_bc_delete_fast(emit_t *emit, qstr qstr, int local_num) {
    assert(local_num >= 0);
    emit_pre(emit, 0);
    emit_write_byte_code_byte_uint(emit, MP_BC_DELETE_FAST_N, local_num);
}

static void emit_bc_delete_deref(emit_t *emit, qstr qstr, int local_num) {
    emit_pre(emit, 0);
    emit_write_byte_code_byte_qstr(emit, MP_BC_DELETE_DEREF, local_num);
}

static void emit_bc_delete_name(emit_t *emit, qstr qstr) {
    emit_pre(emit, 0);
    emit_write_byte_code_byte_qstr(emit, MP_BC_DELETE_NAME, qstr);
}

static void emit_bc_delete_global(emit_t *emit, qstr qstr) {
    emit_pre(emit, 0);
    emit_write_byte_code_byte_qstr(emit, MP_BC_DELETE_GLOBAL, qstr);
}

static void emit_bc_delete_attr(emit_t *emit, qstr qstr) {
    emit_pre(emit, -1);
    emit_write_byte_code_byte_qstr(emit, MP_BC_DELETE_ATTR, qstr);
}

static void emit_bc_delete_subscr(emit_t *emit) {
    emit_pre(emit, -2);
    emit_write_byte_code_byte(emit, MP_BC_DELETE_SUBSCR);
}

static void emit_bc_dup_top(emit_t *emit) {
    emit_pre(emit, 1);
    emit_write_byte_code_byte(emit, MP_BC_DUP_TOP);
}

static void emit_bc_dup_top_two(emit_t *emit) {
    emit_pre(emit, 2);
    emit_write_byte_code_byte(emit, MP_BC_DUP_TOP_TWO);
}

static void emit_bc_pop_top(emit_t *emit) {
    emit_pre(emit, -1);
    emit_write_byte_code_byte(emit, MP_BC_POP_TOP);
}

static void emit_bc_rot_two(emit_t *emit) {
    emit_pre(emit, 0);
    emit_write_byte_code_byte(emit, MP_BC_ROT_TWO);
}

static void emit_bc_rot_three(emit_t *emit) {
    emit_pre(emit, 0);
    emit_write_byte_code_byte(emit, MP_BC_ROT_THREE);
}

static void emit_bc_jump(emit_t *emit, int label) {
    emit_pre(emit, 0);
    emit_write_byte_code_byte_signed_label(emit, MP_BC_JUMP, label);
}

static void emit_bc_pop_jump_if_true(emit_t *emit, int label) {
    emit_pre(emit, -1);
    emit_write_byte_code_byte_signed_label(emit, MP_BC_POP_JUMP_IF_TRUE, label);
}

static void emit_bc_pop_jump_if_false(emit_t *emit, int label) {
    emit_pre(emit, -1);
    emit_write_byte_code_byte_signed_label(emit, MP_BC_POP_JUMP_IF_FALSE, label);
}

static void emit_bc_jump_if_true_or_pop(emit_t *emit, int label) {
    emit_pre(emit, -1);
    emit_write_byte_code_byte_signed_label(emit, MP_BC_JUMP_IF_TRUE_OR_POP, label);
}

static void emit_bc_jump_if_false_or_pop(emit_t *emit, int label) {
    emit_pre(emit, -1);
    emit_write_byte_code_byte_signed_label(emit, MP_BC_JUMP_IF_FALSE_OR_POP, label);
}

static void emit_bc_setup_loop(emit_t *emit, int label) {
    emit_pre(emit, 0);
    emit_write_byte_code_byte_unsigned_label(emit, MP_BC_SETUP_LOOP, label);
}

static void emit_bc_unwind_jump(emit_t *emit, int label, int except_depth) {
    if (except_depth == 0) {
        emit_bc_jump(emit, label);
    } else {
        emit_pre(emit, 0);
        emit_write_byte_code_byte_signed_label(emit, MP_BC_UNWIND_JUMP, label);
        emit_write_byte_code_byte(emit, except_depth);
    }
}

static void emit_bc_setup_with(emit_t *emit, int label) {
    emit_pre(emit, 7);
    emit_write_byte_code_byte_unsigned_label(emit, MP_BC_SETUP_WITH, label);
}

static void emit_bc_with_cleanup(emit_t *emit) {
    emit_pre(emit, -7);
    emit_write_byte_code_byte(emit, MP_BC_WITH_CLEANUP);
}

static void emit_bc_setup_except(emit_t *emit, int label) {
    emit_pre(emit, 6);
    emit_write_byte_code_byte_unsigned_label(emit, MP_BC_SETUP_EXCEPT, label);
}

static void emit_bc_setup_finally(emit_t *emit, int label) {
    emit_pre(emit, 6);
    emit_write_byte_code_byte_unsigned_label(emit, MP_BC_SETUP_FINALLY, label);
}

static void emit_bc_end_finally(emit_t *emit) {
    emit_pre(emit, -1);
    emit_write_byte_code_byte(emit, MP_BC_END_FINALLY);
}

static void emit_bc_get_iter(emit_t *emit) {
    emit_pre(emit, 0);
    emit_write_byte_code_byte(emit, MP_BC_GET_ITER);
}

static void emit_bc_for_iter(emit_t *emit, int label) {
    emit_pre(emit, 1);
    emit_write_byte_code_byte_unsigned_label(emit, MP_BC_FOR_ITER, label);
}

static void emit_bc_for_iter_end(emit_t *emit) {
    emit_pre(emit, -1);
}

static void emit_bc_pop_block(emit_t *emit) {
    emit_pre(emit, 0);
    emit_write_byte_code_byte(emit, MP_BC_POP_BLOCK);
}

static void emit_bc_pop_except(emit_t *emit) {
    emit_pre(emit, 0);
    emit_write_byte_code_byte(emit, MP_BC_POP_EXCEPT);
}

static void emit_bc_unary_op(emit_t *emit, rt_unary_op_t op) {
    if (op == RT_UNARY_OP_NOT) {
        emit_pre(emit, 0);
        emit_write_byte_code_byte_byte(emit, MP_BC_UNARY_OP, RT_UNARY_OP_BOOL);
        emit_pre(emit, 0);
        emit_write_byte_code_byte(emit, MP_BC_NOT);
    } else {
        emit_pre(emit, 0);
        emit_write_byte_code_byte_byte(emit, MP_BC_UNARY_OP, op);
    }
}

static void emit_bc_binary_op(emit_t *emit, rt_binary_op_t op) {
    bool invert = false;
    if (op == RT_BINARY_OP_NOT_IN) {
        invert = true;
        op = RT_BINARY_OP_IN;
    } else if (op == RT_BINARY_OP_IS_NOT) {
        invert = true;
        op = RT_BINARY_OP_IS;
    }
    emit_pre(emit, -1);
    emit_write_byte_code_byte_byte(emit, MP_BC_BINARY_OP, op);
    if (invert) {
        emit_pre(emit, 0);
        emit_write_byte_code_byte(emit, MP_BC_NOT);
    }
}

static void emit_bc_build_tuple(emit_t *emit, int n_args) {
    assert(n_args >= 0);
    emit_pre(emit, 1 - n_args);
    emit_write_byte_code_byte_uint(emit, MP_BC_BUILD_TUPLE, n_args);
}

static void emit_bc_build_list(emit_t *emit, int n_args) {
    assert(n_args >= 0);
    emit_pre(emit, 1 - n_args);
    emit_write_byte_code_byte_uint(emit, MP_BC_BUILD_LIST, n_args);
}

static void emit_bc_list_append(emit_t *emit, int list_stack_index) {
    assert(list_stack_index >= 0);
    emit_pre(emit, -1);
    emit_write_byte_code_byte_uint(emit, MP_BC_LIST_APPEND, list_stack_index);
}

static void emit_bc_build_map(emit_t *emit, int n_args) {
    assert(n_args >= 0);
    emit_pre(emit, 1);
    emit_write_byte_code_byte_uint(emit, MP_BC_BUILD_MAP, n_args);
}

static void emit_bc_store_map(emit_t *emit) {
    emit_pre(emit, -2);
    emit_write_byte_code_byte(emit, MP_BC_STORE_MAP);
}

static void emit_bc_map_add(emit_t *emit, int map_stack_index) {
    assert(map_stack_index >= 0);
    emit_pre(emit, -2);
    emit_write_byte_code_byte_uint(emit, MP_BC_MAP_ADD, map_stack_index);
}

static void emit_bc_build_set(emit_t *emit, int n_args) {
    assert(n_args >= 0);
    emit_pre(emit, 1 - n_args);
    emit_write_byte_code_byte_uint(emit, MP_BC_BUILD_SET, n_args);
}

static void emit_bc_set_add(emit_t *emit, int set_stack_index) {
    assert(set_stack_index >= 0);
    emit_pre(emit, -1);
    emit_write_byte_code_byte_uint(emit, MP_BC_SET_ADD, set_stack_index);
}

static void emit_bc_build_slice(emit_t *emit, int n_args) {
    assert(n_args >= 0);
    emit_pre(emit, 1 - n_args);
    emit_write_byte_code_byte_uint(emit, MP_BC_BUILD_SLICE, n_args);
}

static void emit_bc_unpack_sequence(emit_t *emit, int n_args) {
    assert(n_args >= 0);
    emit_pre(emit, -1 + n_args);
    emit_write_byte_code_byte_uint(emit, MP_BC_UNPACK_SEQUENCE, n_args);
}

static void emit_bc_unpack_ex(emit_t *emit, int n_left, int n_right) {
    assert(n_left >=0 && n_right >= 0);
    emit_pre(emit, -1 + n_left + n_right + 1);
    emit_write_byte_code_byte_uint(emit, MP_BC_UNPACK_EX, n_left | (n_right << 8));
}

static void emit_bc_make_function(emit_t *emit, scope_t *scope, int n_dict_params, int n_default_params) {
    assert(n_dict_params == 0);
    if (n_default_params == 0) {
        emit_pre(emit, 1);
        emit_write_byte_code_byte_uint(emit, MP_BC_MAKE_FUNCTION, scope->unique_code_id);
    } else {
        emit_bc_build_tuple(emit, n_default_params);
        emit_pre(emit, 0);
        emit_write_byte_code_byte_uint(emit, MP_BC_MAKE_FUNCTION_DEFARGS, scope->unique_code_id);
    }
}

static void emit_bc_make_closure(emit_t *emit, scope_t *scope, int n_dict_params, int n_default_params) {
    assert(n_default_params == 0 && n_dict_params == 0);
    emit_pre(emit, 0);
    emit_write_byte_code_byte_uint(emit, MP_BC_MAKE_CLOSURE, scope->unique_code_id);
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
            op = MP_BC_CALL_FUNCTION_VAR_KW;
        } else {
            op = MP_BC_CALL_FUNCTION_VAR;
        }
    } else {
        if (have_dbl_star_arg) {
            op = MP_BC_CALL_FUNCTION_KW;
        } else {
            op = MP_BC_CALL_FUNCTION;
        }
    }
    emit_write_byte_code_byte_uint(emit, op, (n_keyword << 8) | n_positional); // TODO make it 2 separate uints
}

static void emit_bc_call_method(emit_t *emit, int n_positional, int n_keyword, bool have_star_arg, bool have_dbl_star_arg) {
    int s = 0;
    if (have_star_arg) {
        s += 1;
    }
    if (have_dbl_star_arg) {
        s += 1;
    }
    emit_pre(emit, -1 - n_positional - 2 * n_keyword - s);
    int op;
    if (have_star_arg) {
        if (have_dbl_star_arg) {
            op = MP_BC_CALL_METHOD_VAR_KW;
        } else {
            op = MP_BC_CALL_METHOD_VAR;
        }
    } else {
        if (have_dbl_star_arg) {
            op = MP_BC_CALL_METHOD_KW;
        } else {
            op = MP_BC_CALL_METHOD;
        }
    }
    emit_write_byte_code_byte_uint(emit, op, (n_keyword << 8) | n_positional); // TODO make it 2 separate uints
}

static void emit_bc_return_value(emit_t *emit) {
    emit_pre(emit, -1);
    emit->last_emit_was_return_value = true;
    emit_write_byte_code_byte(emit, MP_BC_RETURN_VALUE);
}

static void emit_bc_raise_varargs(emit_t *emit, int n_args) {
    assert(0 <= n_args && n_args <= 2);
    emit_pre(emit, -n_args);
    emit_write_byte_code_byte_byte(emit, MP_BC_RAISE_VARARGS, n_args);
}

static void emit_bc_yield_value(emit_t *emit) {
    emit_pre(emit, 0);
    if (emit->pass == PASS_2) {
        emit->scope->flags |= SCOPE_FLAG_GENERATOR;
    }
    emit_write_byte_code_byte(emit, MP_BC_YIELD_VALUE);
}

static void emit_bc_yield_from(emit_t *emit) {
    emit_pre(emit, -1);
    if (emit->pass == PASS_2) {
        emit->scope->flags |= SCOPE_FLAG_GENERATOR;
    }
    emit_write_byte_code_byte(emit, MP_BC_YIELD_FROM);
}

const emit_method_table_t emit_bc_method_table = {
    emit_bc_set_native_types,
    emit_bc_start_pass,
    emit_bc_end_pass,
    emit_bc_last_emit_was_return_value,
    emit_bc_get_stack_size,
    emit_bc_set_stack_size,
    emit_bc_set_source_line,

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
    emit_bc_load_const_verbatim_str,
    emit_bc_load_fast,
    emit_bc_load_deref,
    emit_bc_load_closure,
    emit_bc_load_name,
    emit_bc_load_global,
    emit_bc_load_attr,
    emit_bc_load_method,
    emit_bc_load_build_class,
    emit_bc_store_fast,
    emit_bc_store_deref,
    emit_bc_store_name,
    emit_bc_store_global,
    emit_bc_store_attr,
    emit_bc_store_subscr,
    emit_bc_store_locals,
    emit_bc_delete_fast,
    emit_bc_delete_deref,
    emit_bc_delete_name,
    emit_bc_delete_global,
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
    emit_bc_unwind_jump,
    emit_bc_unwind_jump,
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
