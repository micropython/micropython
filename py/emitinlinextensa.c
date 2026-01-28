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

#include "py/persistentcode.h"

static inline bool emit_windowed_code() {
    #if MICROPY_DYNAMIC_COMPILER
    return mp_dynamic_compiler.native_arch == MP_NATIVE_ARCH_XTENSAWIN;
    #elif MICROPY_EMIT_XTENSAWIN
    return true;
    #else
    return false;
    #endif
}

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
    if (emit_windowed_code()) {
        asm_xtensa_entry_win(&emit->as, 0);
    } else {
        asm_xtensa_entry(&emit->as, 0);
    }
}

static void emit_inline_xtensa_end_pass(emit_inline_asm_t *emit, mp_uint_t type_sig) {
    if (emit_windowed_code()) {
        asm_xtensa_exit_win(&emit->as);
    } else {
        asm_xtensa_exit(&emit->as);
    }
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

static const qstr_short_t BRANCH_OPCODE_NAMES[] = {
    MP_QSTR_bnone, MP_QSTR_beq,  MP_QSTR_blt,   MP_QSTR_bltu, MP_QSTR_ball,
    MP_QSTR_bbc,   MP_QSTR_,     MP_QSTR_,      MP_QSTR_bany, MP_QSTR_bne,
    MP_QSTR_bge,   MP_QSTR_bgeu, MP_QSTR_bnall, MP_QSTR_bbs,
};

#define RRR_R0 (1 << 4)
#define RRR_R1 (2 << 4)
#define RRR_R2 (3 << 4)

#define RRR    (0)
#define RRI8   (1)

static const struct opcode_entry_t {
    qstr_short_t name;
    uint16_t operands : 2;
    uint16_t op2 : 4;
    uint16_t op1 : 4;
    uint16_t op0 : 4;
    // 2 bits available here
    uint32_t r : 6;
    uint32_t s : 6;
    uint32_t t : 6;
    uint32_t shift : 4;
    uint32_t kind : 1;
    // 13 bits available here
} OPCODE_TABLE[] = {
    { MP_QSTR_abs_,   2,  6, 0, 0, RRR_R0, 1,      RRR_R1, 0, RRR  },
    { MP_QSTR_add,    3,  8, 0, 0, RRR_R0, RRR_R1, RRR_R2, 0, RRR  },
    { MP_QSTR_addi,   3,  0, 0, 2, 12,     RRR_R1, RRR_R0, 0, RRI8 },
    { MP_QSTR_addx2,  3,  9, 0, 0, RRR_R0, RRR_R1, RRR_R2, 0, RRR  },
    { MP_QSTR_addx4,  3, 10, 0, 0, RRR_R0, RRR_R1, RRR_R2, 0, RRR  },
    { MP_QSTR_addx8,  3, 11, 0, 0, RRR_R0, RRR_R1, RRR_R2, 0, RRR  },
    { MP_QSTR_and_,   3,  1, 0, 0, RRR_R0, RRR_R1, RRR_R2, 0, RRR  },
    { MP_QSTR_callx0, 1,  0, 0, 0, 0,      RRR_R0, 12,     0, RRR  },
    { MP_QSTR_jx,     1,  0, 0, 0, 0,      RRR_R0, 10,     0, RRR  },
    { MP_QSTR_l16si,  3,  0, 0, 2, 9,      RRR_R1, RRR_R0, 3, RRI8 },
    { MP_QSTR_l16ui,  3,  0, 0, 2, 1,      RRR_R1, RRR_R0, 3, RRI8 },
    { MP_QSTR_l32i,   3,  0, 0, 2, 2,      RRR_R1, RRR_R0, 5, RRI8 },
    { MP_QSTR_l8ui,   3,  0, 0, 2, 0,      RRR_R1, RRR_R0, 1, RRI8 },
    { MP_QSTR_mull,   3,  8, 2, 0, RRR_R0, RRR_R1, RRR_R2, 0, RRR  },
    { MP_QSTR_neg,    2,  6, 0, 0, RRR_R0, 0,      RRR_R1, 0, RRR  },
    { MP_QSTR_nop,    0,  0, 0, 0, 2,      0,      15,     0, RRR  },
    { MP_QSTR_nsa,    2,  4, 0, 0, 14,     RRR_R1, RRR_R0, 0, RRR  },
    { MP_QSTR_nsau,   2,  4, 0, 0, 15,     RRR_R1, RRR_R0, 0, RRR  },
    { MP_QSTR_or_,    3,  2, 0, 0, RRR_R0, RRR_R1, RRR_R2, 0, RRR  },
    { MP_QSTR_s16i,   3,  0, 0, 2, 5,      RRR_R1, RRR_R0, 3, RRI8 },
    { MP_QSTR_s32i,   3,  0, 0, 2, 6,      RRR_R1, RRR_R0, 5, RRI8 },
    { MP_QSTR_s8i,    3,  0, 0, 2, 4,      RRR_R1, RRR_R0, 1, RRI8 },
    { MP_QSTR_sll,    2, 10, 1, 0, RRR_R0, RRR_R1, 0,      0, RRR  },
    { MP_QSTR_sra,    2, 11, 1, 0, RRR_R0, 0,      RRR_R1, 0, RRR  },
    { MP_QSTR_src,    3,  8, 1, 0, RRR_R0, RRR_R1, RRR_R2, 0, RRR  },
    { MP_QSTR_srl,    2,  9, 1, 0, RRR_R0, 0,      RRR_R1, 0, RRR  },
    { MP_QSTR_ssa8b,  1,  4, 0, 0, 3,      RRR_R0, 0,      0, RRR  },
    { MP_QSTR_ssa8l,  1,  4, 0, 0, 2,      RRR_R0, 0,      0, RRR  },
    { MP_QSTR_ssl,    1,  4, 0, 0, 1,      RRR_R0, 0,      0, RRR  },
    { MP_QSTR_ssr,    1,  4, 0, 0, 0,      RRR_R0, 0,      0, RRR  },
    { MP_QSTR_sub,    3, 12, 0, 0, RRR_R0, RRR_R1, RRR_R2, 0, RRR  },
    { MP_QSTR_subx2,  3, 13, 0, 0, RRR_R0, RRR_R1, RRR_R2, 0, RRR  },
    { MP_QSTR_subx4,  3, 14, 0, 0, RRR_R0, RRR_R1, RRR_R2, 0, RRR  },
    { MP_QSTR_subx8,  3, 15, 0, 0, RRR_R0, RRR_R1, RRR_R2, 0, RRR  },
    { MP_QSTR_xor,    3,  3, 0, 0, RRR_R0, RRR_R1, RRR_R2, 0, RRR  },
    #if MICROPY_EMIT_INLINE_XTENSA_UNCOMMON_OPCODES
    { MP_QSTR_dsync,  0,  0, 0, 0, 2,      0,      3,      0, RRR  },
    { MP_QSTR_esync,  0,  0, 0, 0, 2,      0,      2,      0, RRR  },
    { MP_QSTR_extw,   0,  0, 0, 0, 2,      0,      13,     0, RRR  },
    { MP_QSTR_ill,    0,  0, 0, 0, 2,      0,      0,      0, RRR  },
    { MP_QSTR_isync,  0,  0, 0, 0, 2,      0,      0,      0, RRR  },
    { MP_QSTR_memw,   0,  0, 0, 0, 2,      0,      12,     0, RRR  },
    { MP_QSTR_rer,    2,  4, 0, 0, 6,      RRR_R1, RRR_R0, 0, RRR  },
    { MP_QSTR_rsync,  0,  0, 0, 0, 2,      0,      1,      0, RRR  },
    { MP_QSTR_wer,    2,  4, 0, 0, 7,      RRR_R1, RRR_R0, 0, RRR  },
    #endif
};

// The index of the first four qstrs matches the CCZ condition value to be
// embedded into the opcode.
static const qstr_short_t BCCZ_OPCODES[] = {
    MP_QSTR_beqz, MP_QSTR_bnez, MP_QSTR_bltz, MP_QSTR_bgez,
    MP_QSTR_beqz_n, MP_QSTR_bnez_n
};

static void emit_inline_xtensa_op(emit_inline_asm_t *emit, qstr op, mp_uint_t n_args, mp_parse_node_t *pn_args) {
    size_t op_len;
    const char *op_str = (const char *)qstr_data(op, &op_len);

    for (size_t index = 0; index < MP_ARRAY_SIZE(OPCODE_TABLE); index++) {
        const struct opcode_entry_t *entry = &OPCODE_TABLE[index];
        if (entry->name == op) {
            if (n_args != entry->operands) {
                goto unknown_op;
            }

            uint32_t op24 = ((entry->r & 0x0F) << 12) | ((entry->s & 0x0F) << 8) | ((entry->t & 0x0F) << 4) | entry->op0;
            if (entry->kind == RRR) {
                op24 |= (entry->op2 << 20) | (entry->op1 << 16);
            } else {
                int min = 0;
                int max = 0;
                int shift = entry->shift;
                if (shift > 0) {
                    shift >>= 1;
                    max = 0xFF << shift;
                } else {
                    min = -128;
                    max = 127;
                }
                uint32_t immediate = get_arg_i(emit, op_str, pn_args[2], min, max);
                op24 |= ((immediate >> shift) & 0xFF) << 16;
            }
            if (entry->r >= RRR_R0) {
                op24 |= get_arg_reg(emit, op_str, pn_args[(entry->r >> 4) - 1]) << 12;
            }
            if (entry->s >= RRR_R0) {
                op24 |= get_arg_reg(emit, op_str, pn_args[(entry->s >> 4) - 1]) << 8;
            }
            if (entry->t >= RRR_R0) {
                op24 |= get_arg_reg(emit, op_str, pn_args[(entry->t >> 4) - 1]) << 4;
            }
            asm_xtensa_op24(&emit->as, op24);
            return;
        }
    }

    if (n_args == 0) {
        if (op == MP_QSTR_ret_n || op == MP_QSTR_ret) {
            asm_xtensa_op_ret_n(&emit->as);
            return;
        } else if (op == MP_QSTR_nop_n) {
            asm_xtensa_op16(&emit->as, 0xF03D);
            return;
        }
        goto unknown_op;
    } else if (n_args == 1) {
        if (op == MP_QSTR_j) {
            int label = get_arg_label(emit, op_str, pn_args[0]);
            asm_xtensa_j_label(&emit->as, label);
        } else if (op == MP_QSTR_ssai) {
            mp_uint_t sa = get_arg_i(emit, op_str, pn_args[0], 0, 31);
            asm_xtensa_op24(&emit->as, ASM_XTENSA_ENCODE_RRR(0, 0, 4, 4, sa & 0x0F, (sa >> 4) & 0x01));
        } else if (op == MP_QSTR_call0) {
            mp_uint_t label = get_arg_label(emit, op_str, pn_args[0]);
            asm_xtensa_call0(&emit->as, label);
        #if MICROPY_EMIT_INLINE_XTENSA_UNCOMMON_OPCODES
        } else if (op == MP_QSTR_fsync) {
            mp_uint_t imm3 = get_arg_i(emit, op_str, pn_args[0], 0, 7);
            asm_xtensa_op24(&emit->as, ASM_XTENSA_ENCODE_RRR(0, 0, 0, 2, 8 | imm3, 0));
        } else if (op == MP_QSTR_ill_n) {
            asm_xtensa_op16(&emit->as, 0xF06D);
        #endif
        } else {
            goto unknown_op;
        }
    } else if (n_args == 2) {
        uint r0 = get_arg_reg(emit, op_str, pn_args[0]);
        for (size_t index = 0; index < MP_ARRAY_SIZE(BCCZ_OPCODES); index++) {
            if (op == BCCZ_OPCODES[index]) {
                mp_uint_t label = get_arg_label(emit, op_str, pn_args[1]);
                asm_xtensa_bccz_reg_label(&emit->as, index & 0x03, r0, label);
                return;
            }
        }
        if (op == MP_QSTR_mov || op == MP_QSTR_mov_n) {
            // we emit mov.n for both "mov" and "mov_n" opcodes
            uint r1 = get_arg_reg(emit, op_str, pn_args[1]);
            asm_xtensa_op_mov_n(&emit->as, r0, r1);
        } else if (op == MP_QSTR_movi) {
            // for convenience we emit l32r if the integer doesn't fit in movi
            uint32_t imm = get_arg_i(emit, op_str, pn_args[1], 0, 0);
            asm_xtensa_mov_reg_i32(&emit->as, r0, imm);
        } else if (op == MP_QSTR_l32r) {
            mp_uint_t label = get_arg_label(emit, op_str, pn_args[1]);
            asm_xtensa_l32r(&emit->as, r0, label);
        } else if (op == MP_QSTR_movi_n) {
            mp_int_t imm = get_arg_i(emit, op_str, pn_args[1], -32, 95);
            asm_xtensa_op_movi_n(&emit->as, r0, imm);
        } else
        #if MICROPY_EMIT_INLINE_XTENSA_UNCOMMON_OPCODES
        if (op == MP_QSTR_rsr) {
            mp_uint_t sr = get_arg_i(emit, op_str, pn_args[1], 0, 255);
            asm_xtensa_op24(&emit->as, ASM_XTENSA_ENCODE_RSR(0, 3, 0, sr, r0));
        } else if (op == MP_QSTR_rur) {
            mp_uint_t imm8 = get_arg_i(emit, op_str, pn_args[1], 0, 255);
            asm_xtensa_op24(&emit->as, ASM_XTENSA_ENCODE_RRR(0, 3, 14, r0, (imm8 >> 4) & 0x0F, imm8 & 0x0F));
        } else if (op == MP_QSTR_wsr) {
            mp_uint_t sr = get_arg_i(emit, op_str, pn_args[1], 0, 255);
            asm_xtensa_op24(&emit->as, ASM_XTENSA_ENCODE_RSR(0, 3, 1, sr, r0));
        } else if (op == MP_QSTR_wur) {
            mp_uint_t sr = get_arg_i(emit, op_str, pn_args[1], 0, 255);
            asm_xtensa_op24(&emit->as, ASM_XTENSA_ENCODE_RSR(0, 3, 15, sr, r0));
        } else if (op == MP_QSTR_xsr) {
            mp_uint_t sr = get_arg_i(emit, op_str, pn_args[1], 0, 255);
            asm_xtensa_op24(&emit->as, ASM_XTENSA_ENCODE_RSR(0, 1, 6, sr, r0));
        } else
        #endif
        {
            goto unknown_op;
        }
    } else if (n_args == 3) {
        for (size_t index = 0; index < MP_ARRAY_SIZE(BRANCH_OPCODE_NAMES); index++) {
            if (BRANCH_OPCODE_NAMES[index] == op) {
                int r0 = get_arg_reg(emit, op_str, pn_args[0]);
                int r1 = get_arg_reg(emit, op_str, pn_args[1]);
                int label = get_arg_label(emit, op_str, pn_args[2]);
                asm_xtensa_bcc_reg_reg_label(&emit->as, index, r0, r1, label);
                return;
            }
        }

        if (op == MP_QSTR_add_n) {
            mp_uint_t r0 = get_arg_reg(emit, op_str, pn_args[0]);
            mp_uint_t r1 = get_arg_reg(emit, op_str, pn_args[1]);
            mp_uint_t r2 = get_arg_reg(emit, op_str, pn_args[2]);
            asm_xtensa_op16(&emit->as, ASM_XTENSA_ENCODE_RRRN(10, r0, r1, r2));
        } else if (op == MP_QSTR_addi_n) {
            mp_uint_t r0 = get_arg_reg(emit, op_str, pn_args[0]);
            mp_uint_t r1 = get_arg_reg(emit, op_str, pn_args[1]);
            mp_int_t imm4 = get_arg_i(emit, op_str, pn_args[2], -1, 15);
            asm_xtensa_op16(&emit->as, ASM_XTENSA_ENCODE_RRRN(11, r0, r1, (imm4 != 0 ? imm4 : -1)));
        } else if (op == MP_QSTR_addmi) {
            mp_uint_t r0 = get_arg_reg(emit, op_str, pn_args[0]);
            mp_uint_t r1 = get_arg_reg(emit, op_str, pn_args[1]);
            mp_int_t imm8 = get_arg_i(emit, op_str, pn_args[2], -128 * 256, 127 * 256);
            if ((imm8 & 0xFF) != 0) {
                emit_inline_xtensa_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, MP_ERROR_TEXT("%d is not a multiple of %d"), imm8, 256));
            } else {
                asm_xtensa_op24(&emit->as, ASM_XTENSA_ENCODE_RRI8(2, 13, r1, r0, imm8 >> 8));
            }
        } else if (op == MP_QSTR_bbci) {
            mp_uint_t r0 = get_arg_reg(emit, op_str, pn_args[0]);
            mp_uint_t bit = get_arg_i(emit, op_str, pn_args[1], 0, 31);
            mp_int_t label = get_arg_label(emit, op_str, pn_args[2]);
            asm_xtensa_bit_branch(&emit->as, r0, bit, label, 6);
        } else if (op == MP_QSTR_bbsi) {
            mp_uint_t r0 = get_arg_reg(emit, op_str, pn_args[0]);
            mp_uint_t bit = get_arg_i(emit, op_str, pn_args[1], 0, 31);
            mp_uint_t label = get_arg_label(emit, op_str, pn_args[2]);
            asm_xtensa_bit_branch(&emit->as, r0, bit, label, 14);
        } else if (op == MP_QSTR_slli) {
            mp_uint_t r0 = get_arg_reg(emit, op_str, pn_args[0]);
            mp_uint_t r1 = get_arg_reg(emit, op_str, pn_args[1]);
            mp_uint_t bits = 32 - get_arg_i(emit, op_str, pn_args[2], 1, 31);
            asm_xtensa_op24(&emit->as, ASM_XTENSA_ENCODE_RRR(0, 1, 0 | ((bits >> 4) & 0x01), r0, r1, bits & 0x0F));
        } else if (op == MP_QSTR_srai) {
            mp_uint_t r0 = get_arg_reg(emit, op_str, pn_args[0]);
            mp_uint_t r1 = get_arg_reg(emit, op_str, pn_args[1]);
            mp_uint_t bits = get_arg_i(emit, op_str, pn_args[2], 0, 31);
            asm_xtensa_op24(&emit->as, ASM_XTENSA_ENCODE_RRR(0, 1, 2 | ((bits >> 4) & 0x01), r0, bits & 0x0F, r1));
        } else if (op == MP_QSTR_srli) {
            mp_uint_t r0 = get_arg_reg(emit, op_str, pn_args[0]);
            mp_uint_t r1 = get_arg_reg(emit, op_str, pn_args[1]);
            mp_uint_t bits = get_arg_i(emit, op_str, pn_args[2], 0, 15);
            asm_xtensa_op24(&emit->as, ASM_XTENSA_ENCODE_RRR(0, 1, 4, r0, bits, r1));
        } else if (op == MP_QSTR_l32i_n) {
            mp_uint_t r0 = get_arg_reg(emit, op_str, pn_args[0]);
            mp_uint_t r1 = get_arg_reg(emit, op_str, pn_args[1]);
            mp_uint_t imm = get_arg_i(emit, op_str, pn_args[2], 0, 60);
            if ((imm & 0x03) != 0) {
                emit_inline_xtensa_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, MP_ERROR_TEXT("%d is not a multiple of %d"), imm, 4));
            } else {
                asm_xtensa_op_l32i_n(&emit->as, r0, r1, imm >> 2);
            }
        } else if (op == MP_QSTR_s32i_n) {
            mp_uint_t r0 = get_arg_reg(emit, op_str, pn_args[0]);
            mp_uint_t r1 = get_arg_reg(emit, op_str, pn_args[1]);
            mp_uint_t imm = get_arg_i(emit, op_str, pn_args[2], 0, 60);
            if ((imm & 0x03) != 0) {
                emit_inline_xtensa_error_exc(emit, mp_obj_new_exception_msg_varg(&mp_type_SyntaxError, MP_ERROR_TEXT("%d is not a multiple of %d"), imm, 4));
            } else {
                asm_xtensa_op_s32i_n(&emit->as, r0, r1, imm >> 2);
            }
        } else {
            goto unknown_op;
        }

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
