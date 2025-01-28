/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2016 Damien P. George
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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "py/emit.h"
#include "py/asmxtensa.h"

#if MICROPY_EMIT_INLINE_XTENSA

struct _emit_inline_asm_t {
    asm_xtensa_t as;
    uint16_t pass;
    mp_obj_t *error_slot;
    mp_uint_t max_num_labels;
    qstr *label_lookup;
};

static void emit_inline_xtensa_error_msg(emit_inline_asm_t *emit, mp_rom_error_text_t msg) {
    *emit->error_slot = mp_obj_new_exception_msg(&mp_type_SyntaxError, msg);
}

static void emit_inline_xtensa_error_exc(emit_inline_asm_t *emit, mp_obj_t exc) {
    *emit->error_slot = exc;
}

emit_inline_asm_t *emit_inline_xtensa_new(mp_uint_t max_num_labels) {
    emit_inline_asm_t *emit = m_new_obj(emit_inline_asm_t);
    memset(&emit->as, 0, sizeof(emit->as));
    mp_asm_base_init(&emit->as.base, max_num_labels);
    emit->max_num_labels = max_num_labels;
    emit->label_lookup = m_new(qstr, max_num_labels);
    return emit;
}

void emit_inline_xtensa_free(emit_inline_asm_t *emit) {
    m_del(qstr, emit->label_lookup, emit->max_num_labels);
    mp_asm_base_deinit(&emit->as.base, false);
    m_del_obj(emit_inline_asm_t, emit);
}

static void emit_inline_xtensa_start_pass(emit_inline_asm_t *emit, pass_kind_t pass, mp_obj_t *error_slot) {
    emit->pass = pass;
    emit->error_slot = error_slot;
    if (emit->pass == MP_PASS_CODE_SIZE) {
        memset(emit->label_lookup, 0, emit->max_num_labels * sizeof(qstr));
    }
    mp_asm_base_start_pass(&emit->as.base, pass == MP_PASS_EMIT ? MP_ASM_PASS_EMIT : MP_ASM_PASS_COMPUTE);
    asm_xtensa_entry(&emit->as, 0);
}

static void emit_inline_xtensa_end_pass(emit_inline_asm_t *emit, mp_uint_t type_sig) {
    asm_xtensa_exit(&emit->as);
    asm_xtensa_end_pass(&emit->as);
}

static mp_uint_t emit_inline_xtensa_count_params(emit_inline_asm_t *emit, mp_uint_t n_params, mp_parse_node_t *pn_params) {
    if (n_params > 4) {
        emit_inline_xtensa_error_msg(emit, MP_ERROR_TEXT("can only have up to 4 parameters to Xtensa assembly"));
        return 0;
    }
    for (mp_uint_t i = 0; i < n_params; i++) {
        if (!MP_PARSE_NODE_IS_ID(pn_params[i])) {
            emit_inline_xtensa_error_msg(emit, MP_ERROR_TEXT("parameters must be registers in sequence a2 to a5"));
            return 0;
        }
        const char *p = qstr_str(MP_PARSE_NODE_LEAF_ARG(pn_params[i]));
        if (!(strlen(p) == 2 && p[0] == 'a' && (mp_uint_t)p[1] == '2' + i)) {
            emit_inline_xtensa_error_msg(emit, MP_ERROR_TEXT("parameters must be registers in sequence a2 to a5"));
            return 0;
        }
    }
    return n_params;
}

static bool emit_inline_xtensa_label(emit_inline_asm_t *emit, mp_uint_t label_num, qstr label_id) {
    assert(label_num < emit->max_num_labels);
    if (emit->pass == MP_PASS_CODE_SIZE) {
        // check for duplicate label on first pass
        for (uint i = 0; i < emit->max_num_labels; i++) {
            if (emit->label_lookup[i] == label_id) {
                return false;
            }
        }
    }
    emit->label_lookup[label_num] = label_id;
    mp_asm_base_label_assign(&emit->as.base, label_num);
    return true;
}

static const qstr_short_t REGISTERS[16] = {
    MP_QSTR_a0, MP_QSTR_a1, MP_QSTR_a2, MP_QSTR_a3, MP_QSTR_a4, MP_QSTR_a5, MP_QSTR_a6, MP_QSTR_a7,
    MP_QSTR_a8, MP_QSTR_a9, MP_QSTR_a10, MP_QSTR_a11, MP_QSTR_a12, MP_QSTR_a13, MP_QSTR_a14, MP_QSTR_a15
};

static mp_uint_t get_arg_reg(emit_inline_asm_t *emit, const char *op, mp_parse_node_t pn) {
    if (MP_PARSE_NODE_IS_ID(pn)) {
        qstr node_qstr = MP_PARSE_NODE_LEAF_ARG(pn);
        for (size_t i = 0; i < MP_ARRAY_SIZE(REGISTERS); i++) {
            if (node_qstr == REGISTERS[i]) {
                return i;
            }
        }
    }

    emit_inline_xtensa_error_exc(emit,
        mp_obj_new_exception_msg_varg(&mp_type_SyntaxError,
            MP_ERROR_TEXT("'%s' expects a register"), op));
    return 0;
}

static uint32_t get_arg_i(emit_inline_asm_t *emit, const char *op, mp_parse_node_t pn, int min, int max) {
    mp_obj_t o;
    if (!mp_parse_node_get_int_maybe(pn, &o)) {
        emit_inline_xtensa_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, MP_ERROR_TEXT("'%s' expects an integer"), op));
        return 0;
    }
    uint32_t i = mp_obj_get_int_truncated(o);
    if (min != max && ((int)i < min || (int)i > max)) {
        emit_inline_xtensa_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, MP_ERROR_TEXT("'%s' integer %d isn't within range %d..%d"), op, i, min, max));
        return 0;
    }
    return i;
}

static int get_arg_label(emit_inline_asm_t *emit, const char *op, mp_parse_node_t pn) {
    if (!MP_PARSE_NODE_IS_ID(pn)) {
        emit_inline_xtensa_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, MP_ERROR_TEXT("'%s' expects a label"), op));
        return 0;
    }
    qstr label_qstr = MP_PARSE_NODE_LEAF_ARG(pn);
    for (uint i = 0; i < emit->max_num_labels; i++) {
        if (emit->label_lookup[i] == label_qstr) {
            return i;
        }
    }
    // only need to have the labels on the last pass
    if (emit->pass == MP_PASS_EMIT) {
        emit_inline_xtensa_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, MP_ERROR_TEXT("label '%q' not defined"), label_qstr));
    }
    return 0;
}

#define RRR (0)
#define RRI8 (1)
#define RRI8_B (2)

typedef struct _opcode_table_3arg_t {
    uint16_t name; // actually a qstr, which should fit in 16 bits
    uint8_t type;
    uint8_t a0 : 4;
    uint8_t a1 : 4;
} opcode_table_3arg_t;

static const opcode_table_3arg_t opcode_table_3arg[] = {
    // arithmetic opcodes: reg, reg, reg
    {MP_QSTR_and_, RRR, 0, 1},
    {MP_QSTR_or_, RRR, 0, 2},
    {MP_QSTR_xor, RRR, 0, 3},
    {MP_QSTR_add, RRR, 0, 8},
    {MP_QSTR_sub, RRR, 0, 12},
    {MP_QSTR_mull, RRR, 2, 8},

    // load/store/addi opcodes: reg, reg, imm
    // upper nibble of type encodes the range of the immediate arg
    {MP_QSTR_l8ui, RRI8 | 0x10, 2, 0},
    {MP_QSTR_l16ui, RRI8 | 0x30, 2, 1},
    {MP_QSTR_l32i, RRI8 | 0x50, 2, 2},
    {MP_QSTR_s8i, RRI8 | 0x10, 2, 4},
    {MP_QSTR_s16i, RRI8 | 0x30, 2, 5},
    {MP_QSTR_s32i, RRI8 | 0x50, 2, 6},
    {MP_QSTR_l16si, RRI8 | 0x30, 2, 9},
    {MP_QSTR_addi, RRI8 | 0x00, 2, 12},

    // branch opcodes: reg, reg, label
    {MP_QSTR_ball, RRI8_B, ASM_XTENSA_CC_ALL, 0},
    {MP_QSTR_bany, RRI8_B, ASM_XTENSA_CC_ANY, 0},
    {MP_QSTR_bbc, RRI8_B, ASM_XTENSA_CC_BC, 0},
    {MP_QSTR_bbs, RRI8_B, ASM_XTENSA_CC_BS, 0},
    {MP_QSTR_beq, RRI8_B, ASM_XTENSA_CC_EQ, 0},
    {MP_QSTR_bge, RRI8_B, ASM_XTENSA_CC_GE, 0},
    {MP_QSTR_bgeu, RRI8_B, ASM_XTENSA_CC_GEU, 0},
    {MP_QSTR_blt, RRI8_B, ASM_XTENSA_CC_LT, 0},
    {MP_QSTR_bnall, RRI8_B, ASM_XTENSA_CC_NALL, 0},
    {MP_QSTR_bne, RRI8_B, ASM_XTENSA_CC_NE, 0},
    {MP_QSTR_bnone, RRI8_B, ASM_XTENSA_CC_NONE, 0},
};

static void emit_inline_xtensa_op(emit_inline_asm_t *emit, qstr op, mp_uint_t n_args, mp_parse_node_t *pn_args) {
    size_t op_len;
    const char *op_str = (const char *)qstr_data(op, &op_len);

    if (n_args == 0) {
        if (op == MP_QSTR_ret_n) {
            asm_xtensa_op_ret_n(&emit->as);
        } else {
            goto unknown_op;
        }

    } else if (n_args == 1) {
        if (op == MP_QSTR_callx0) {
            uint r0 = get_arg_reg(emit, op_str, pn_args[0]);
            asm_xtensa_op_callx0(&emit->as, r0);
        } else if (op == MP_QSTR_j) {
            int label = get_arg_label(emit, op_str, pn_args[0]);
            asm_xtensa_j_label(&emit->as, label);
        } else if (op == MP_QSTR_jx) {
            uint r0 = get_arg_reg(emit, op_str, pn_args[0]);
            asm_xtensa_op_jx(&emit->as, r0);
        } else {
            goto unknown_op;
        }

    } else if (n_args == 2) {
        uint r0 = get_arg_reg(emit, op_str, pn_args[0]);
        if (op == MP_QSTR_beqz) {
            int label = get_arg_label(emit, op_str, pn_args[1]);
            asm_xtensa_bccz_reg_label(&emit->as, ASM_XTENSA_CCZ_EQ, r0, label);
        } else if (op == MP_QSTR_bnez) {
            int label = get_arg_label(emit, op_str, pn_args[1]);
            asm_xtensa_bccz_reg_label(&emit->as, ASM_XTENSA_CCZ_NE, r0, label);
        } else if (op == MP_QSTR_mov || op == MP_QSTR_mov_n) {
            // we emit mov.n for both "mov" and "mov_n" opcodes
            uint r1 = get_arg_reg(emit, op_str, pn_args[1]);
            asm_xtensa_op_mov_n(&emit->as, r0, r1);
        } else if (op == MP_QSTR_movi) {
            // for convenience we emit l32r if the integer doesn't fit in movi
            uint32_t imm = get_arg_i(emit, op_str, pn_args[1], 0, 0);
            asm_xtensa_mov_reg_i32(&emit->as, r0, imm);
        } else {
            goto unknown_op;
        }

    } else if (n_args == 3) {
        // search table for 3 arg instructions
        for (uint i = 0; i < MP_ARRAY_SIZE(opcode_table_3arg); i++) {
            const opcode_table_3arg_t *o = &opcode_table_3arg[i];
            if (op == o->name) {
                uint r0 = get_arg_reg(emit, op_str, pn_args[0]);
                uint r1 = get_arg_reg(emit, op_str, pn_args[1]);
                if (o->type == RRR) {
                    uint r2 = get_arg_reg(emit, op_str, pn_args[2]);
                    asm_xtensa_op24(&emit->as, ASM_XTENSA_ENCODE_RRR(0, o->a0, o->a1, r0, r1, r2));
                } else if (o->type == RRI8_B) {
                    int label = get_arg_label(emit, op_str, pn_args[2]);
                    asm_xtensa_bcc_reg_reg_label(&emit->as, o->a0, r0, r1, label);
                } else {
                    int shift, min, max;
                    if ((o->type & 0xf0) == 0) {
                        shift = 0;
                        min = -128;
                        max = 127;
                    } else {
                        shift = (o->type & 0xf0) >> 5;
                        min = 0;
                        max = 0xff << shift;
                    }
                    uint32_t imm = get_arg_i(emit, op_str, pn_args[2], min, max);
                    asm_xtensa_op24(&emit->as, ASM_XTENSA_ENCODE_RRI8(o->a0, o->a1, r1, r0, (imm >> shift) & 0xff));
                }
                return;
            }
        }
        goto unknown_op;

    } else {
        goto unknown_op;
    }

    return;

unknown_op:
    emit_inline_xtensa_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, MP_ERROR_TEXT("unsupported Xtensa instruction '%s' with %d arguments"), op_str, n_args));
    return;

    /*
branch_not_in_range:
    emit_inline_xtensa_error_msg(emit, MP_ERROR_TEXT("branch not in range"));
    return;
    */
}

const emit_inline_asm_method_table_t emit_inline_xtensa_method_table = {
    #if MICROPY_DYNAMIC_COMPILER
    emit_inline_xtensa_new,
    emit_inline_xtensa_free,
    #endif

    emit_inline_xtensa_start_pass,
    emit_inline_xtensa_end_pass,
    emit_inline_xtensa_count_params,
    emit_inline_xtensa_label,
    emit_inline_xtensa_op,
};

#endif // MICROPY_EMIT_INLINE_XTENSA
