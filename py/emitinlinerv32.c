/*
 * This file is part of the MicroPython project, https://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Alessandro Gatti
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

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "py/asmrv32.h"
#include "py/emit.h"
#include "py/misc.h"

#if MICROPY_EMIT_INLINE_RV32

typedef enum {
// define rules with a compile function
#define DEF_RULE(rule, comp, kind, ...) PN_##rule,
#define DEF_RULE_NC(rule, kind, ...)
    #include "py/grammar.h"
#undef DEF_RULE
#undef DEF_RULE_NC
    PN_const_object, // special node for a constant, generic Python object
// define rules without a compile function
#define DEF_RULE(rule, comp, kind, ...)
#define DEF_RULE_NC(rule, kind, ...) PN_##rule,
    #include "py/grammar.h"
#undef DEF_RULE
#undef DEF_RULE_NC
} pn_kind_t;

struct _emit_inline_asm_t {
    asm_rv32_t as;
    uint16_t pass;
    mp_obj_t *error_slot;
    mp_uint_t max_num_labels;
    qstr *label_lookup;
};

static const qstr REGISTERS_QSTR_TABLE[] = {
    MP_QSTR_zero, MP_QSTR_ra,  MP_QSTR_sp,  MP_QSTR_gp,  MP_QSTR_tp,  MP_QSTR_t0,  MP_QSTR_t1,  MP_QSTR_t2,
    MP_QSTR_s0,   MP_QSTR_s1,  MP_QSTR_a0,  MP_QSTR_a1,  MP_QSTR_a2,  MP_QSTR_a3,  MP_QSTR_a4,  MP_QSTR_a5,
    MP_QSTR_a6,   MP_QSTR_a7,  MP_QSTR_s2,  MP_QSTR_s3,  MP_QSTR_s4,  MP_QSTR_s5,  MP_QSTR_s6,  MP_QSTR_s7,
    MP_QSTR_s8,   MP_QSTR_s9,  MP_QSTR_s10, MP_QSTR_s11, MP_QSTR_t3,  MP_QSTR_t4,  MP_QSTR_t5,  MP_QSTR_t6,
    MP_QSTR_x0,   MP_QSTR_x1,  MP_QSTR_x2,  MP_QSTR_x3,  MP_QSTR_x4,  MP_QSTR_x5,  MP_QSTR_x6,  MP_QSTR_x7,
    MP_QSTR_x8,   MP_QSTR_x9,  MP_QSTR_x10, MP_QSTR_x11, MP_QSTR_x12, MP_QSTR_x13, MP_QSTR_x14, MP_QSTR_x15,
    MP_QSTR_x16,  MP_QSTR_x17, MP_QSTR_x18, MP_QSTR_x19, MP_QSTR_x20, MP_QSTR_x21, MP_QSTR_x22, MP_QSTR_x23,
    MP_QSTR_x24,  MP_QSTR_x25, MP_QSTR_x26, MP_QSTR_x27, MP_QSTR_x28, MP_QSTR_x29, MP_QSTR_x30, MP_QSTR_x31,
};

////////////////////////////////////////////////////////////////////////////////

static inline void emit_inline_rv32_error_msg(emit_inline_asm_t *emit, mp_rom_error_text_t msg) {
    *emit->error_slot = mp_obj_new_exception_msg(&mp_type_SyntaxError, msg);
}

static inline void emit_inline_rv32_error_exc(emit_inline_asm_t *emit, mp_obj_t exc) {
    *emit->error_slot = exc;
}

emit_inline_asm_t *emit_inline_rv32_new(mp_uint_t max_num_labels) {
    emit_inline_asm_t *emit = m_new_obj(emit_inline_asm_t);
    memset(&emit->as, 0, sizeof(emit->as));
    mp_asm_base_init(&emit->as.base, max_num_labels);
    emit->max_num_labels = max_num_labels;
    emit->label_lookup = m_new(qstr, max_num_labels);
    return emit;
}

void emit_inline_rv32_free(emit_inline_asm_t *emit) {
    m_del(qstr, emit->label_lookup, emit->max_num_labels);
    mp_asm_base_deinit(&emit->as.base, false);
    m_del_obj(emit_inline_asm_t, emit);
}

static void emit_inline_rv32_start_pass(emit_inline_asm_t *emit, pass_kind_t pass, mp_obj_t *error_slot) {
    emit->pass = pass;
    emit->error_slot = error_slot;
    if (emit->pass == MP_PASS_CODE_SIZE) {
        memset(emit->label_lookup, 0, emit->max_num_labels * sizeof(qstr));
    }
    mp_asm_base_start_pass(&emit->as.base, pass == MP_PASS_EMIT ? MP_ASM_PASS_EMIT : MP_ASM_PASS_COMPUTE);
}

static void emit_inline_rv32_end_pass(emit_inline_asm_t *emit, mp_uint_t type_sig) {
    // c.jr ra
    asm_rv32_opcode_cjr(&emit->as, ASM_RV32_REG_RA);
    asm_rv32_end_pass(&emit->as);
}

static bool parse_register_node(mp_parse_node_t node, mp_uint_t *register_number, bool compressed) {
    assert(register_number != NULL && "Register number pointer is NULL.");

    if (!MP_PARSE_NODE_IS_ID(node)) {
        return false;
    }

    qstr node_qstr = MP_PARSE_NODE_LEAF_ARG(node);
    for (mp_uint_t index = 0; index < MP_ARRAY_SIZE(REGISTERS_QSTR_TABLE); index++) {
        if (node_qstr == REGISTERS_QSTR_TABLE[index]) {
            mp_uint_t number = index % RV32_AVAILABLE_REGISTERS_COUNT;
            if (!compressed || (compressed && RV32_IS_IN_C_REGISTER_WINDOW(number))) {
                *register_number = compressed ? RV32_MAP_IN_C_REGISTER_WINDOW(number) : number;
                return true;
            }
            break;
        }
    }

    return false;
}

static mp_uint_t lookup_label(emit_inline_asm_t *emit, mp_parse_node_t node, qstr *qstring) {
    assert(qstring && "qstring pointer is NULL");

    *qstring = MP_PARSE_NODE_LEAF_ARG(node);
    for (mp_uint_t label = 0; label < emit->max_num_labels; label++) {
        if (emit->label_lookup[label] == *qstring) {
            return label;
        }
    }

    return emit->max_num_labels;
}

static inline ptrdiff_t label_code_offset(emit_inline_asm_t *emit, mp_uint_t label_index) {
    return emit->as.base.label_offsets[label_index] - emit->as.base.code_offset;
}

static mp_uint_t emit_inline_rv32_count_params(emit_inline_asm_t *emit, mp_uint_t parameters_count, mp_parse_node_t *parameter_nodes) {
    // TODO: Raise this up to 8?  RV32I has 8 A-registers that are meant to
    //       be used for passing arguments.

    if (parameters_count > 4) {
        emit_inline_rv32_error_msg(emit, MP_ERROR_TEXT("can only have up to 4 parameters for RV32 assembly"));
        return 0;
    }

    mp_uint_t register_index = 0;
    for (mp_uint_t index = 0; index < parameters_count; index++) {
        bool valid_register = parse_register_node(parameter_nodes[index], &register_index, false);
        if (!valid_register || (register_index != (ASM_RV32_REG_A0 + index))) {
            emit_inline_rv32_error_msg(emit, MP_ERROR_TEXT("parameters must be registers in sequence a0 to a3"));
            return 0;
        }
    }

    return parameters_count;
}

static bool emit_inline_rv32_label(emit_inline_asm_t *emit, mp_uint_t label_num, qstr label_id) {
    assert(label_num < emit->max_num_labels);
    if (emit->pass == MP_PASS_CODE_SIZE) {
        for (mp_uint_t index = 0; index < emit->max_num_labels; index++) {
            if (emit->label_lookup[index] == label_id) {
                return false;
            }
        }
    }
    emit->label_lookup[label_num] = label_id;
    mp_asm_base_label_assign(&emit->as.base, label_num);
    return true;
}

#define CALL_RRR 0 // Register, Register, Register
#define CALL_RR  1 // Register, Register
#define CALL_RRI 2 // Register, Register, Immediate
#define CALL_RRL 3 // Register, Register, Label
#define CALL_RI  4 // Register, Immediate
#define CALL_L   5 // Label
#define CALL_R   6 // Register
#define CALL_RL  7 // Register, Label
#define CALL_N   8 // No arguments
#define CALL_I   9 // Immediate
#define CALL_RII 10 // Register, Immediate, Immediate
#define CALL_RIR 11 // Register, Immediate(Register)

#define N 0 // No argument
#define R 1 // Register
#define I 2 // Immediate
#define L 3 // Label
#define C (1 << 2) // Compressed register
#define U (1 << 2) // Unsigned immediate
#define Z (1 << 3) // Non-zero

typedef void (*call_l_t)(asm_rv32_t *state, mp_uint_t label_index);
typedef void (*call_ri_t)(asm_rv32_t *state, mp_uint_t rd, mp_int_t immediate);
typedef void (*call_rri_t)(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs1, mp_int_t immediate);
typedef void (*call_rii_t)(asm_rv32_t *state, mp_uint_t rd, mp_uint_t immediate1, mp_int_t immediate2);
typedef void (*call_rrr_t)(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs1, mp_uint_t rs2);
typedef void (*call_rr_t)(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs);
typedef void (*call_i_t)(asm_rv32_t *state, mp_int_t immediate);
typedef void (*call_r_t)(asm_rv32_t *state, mp_uint_t rd);
typedef void (*call_n_t)(asm_rv32_t *state);

typedef struct _opcode_t {
    qstr qstring;
    void *emitter;
    uint32_t calling_convention : 4;
    uint32_t argument1_kind : 4;
    uint32_t argument1_shift : 5;
    uint32_t argument2_kind : 4;
    uint32_t argument2_shift : 5;
    uint32_t argument3_kind : 4;
    uint32_t argument3_shift : 5;
    uint32_t argument1_mask;
    uint32_t argument2_mask;
    uint32_t argument3_mask;
} opcode_t;

#define opcode_li asm_rv32_emit_optimised_load_immediate

static void opcode_la(asm_rv32_t *state, mp_uint_t rd, mp_int_t displacement) {
    // This cannot be optimised for size, otherwise label addresses would move around.
    mp_uint_t upper = (mp_uint_t)displacement & 0xFFFFF000;
    mp_uint_t lower = (mp_uint_t)displacement & 0x00000FFF;
    if ((lower & 0x800) != 0) {
        upper += 0x1000;
    }
    asm_rv32_opcode_auipc(state, rd, upper);
    asm_rv32_opcode_addi(state, rd, rd, lower);
}

#define RC  (R | C)
#define IU  (I | U)
#define IZ  (I | Z)
#define IUZ (I | U | Z)

static const opcode_t OPCODES[] = {
    { MP_QSTR_add,        asm_rv32_opcode_add,       CALL_RRR, R,   0, R,   0,  R,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF },
    { MP_QSTR_addi,       asm_rv32_opcode_addi,      CALL_RRI, R,   0, R,   0,  I,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000FFF },
    { MP_QSTR_and_,       asm_rv32_opcode_and,       CALL_RRR, R,   0, R,   0,  R,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF },
    { MP_QSTR_andi,       asm_rv32_opcode_andi,      CALL_RRI, R,   0, R,   0,  I,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000FFF },
    { MP_QSTR_auipc,      asm_rv32_opcode_auipc,     CALL_RI,  R,   0, I,   12, N,  0, 0xFFFFFFFF, 0xFFFFF000, 0x00000000 },
    { MP_QSTR_beq,        asm_rv32_opcode_beq,       CALL_RRL, R,   0, R,   0,  L,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0x00001FFE },
    { MP_QSTR_bge,        asm_rv32_opcode_bge,       CALL_RRL, R,   0, R,   0,  L,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0x00001FFE },
    { MP_QSTR_bgeu,       asm_rv32_opcode_bgeu,      CALL_RRL, R,   0, R,   0,  L,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0x00001FFE },
    { MP_QSTR_blt,        asm_rv32_opcode_blt,       CALL_RRL, R,   0, R,   0,  L,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0x00001FFE },
    { MP_QSTR_bltu,       asm_rv32_opcode_bltu,      CALL_RRL, R,   0, R,   0,  L,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0x00001FFE },
    { MP_QSTR_bne,        asm_rv32_opcode_bne,       CALL_RRL, R,   0, R,   0,  L,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0x00001FFE },
    { MP_QSTR_csrrc,      asm_rv32_opcode_csrrc,     CALL_RRI, R,   0, R,   0,  IU, 0, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000FFF },
    { MP_QSTR_csrrs,      asm_rv32_opcode_csrrs,     CALL_RRI, R,   0, R,   0,  IU, 0, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000FFF },
    { MP_QSTR_csrrw,      asm_rv32_opcode_csrrw,     CALL_RRI, R,   0, R,   0,  IU, 0, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000FFF },
    { MP_QSTR_csrrci,     asm_rv32_opcode_csrrci,    CALL_RII, R,   0, IU,  0,  IU, 0, 0xFFFFFFFF, 0x00000FFF, 0x0000001F },
    { MP_QSTR_csrrsi,     asm_rv32_opcode_csrrsi,    CALL_RII, R,   0, IU,  0,  IU, 0, 0xFFFFFFFF, 0x00000FFF, 0x0000001F },
    { MP_QSTR_csrrwi,     asm_rv32_opcode_csrrwi,    CALL_RII, R,   0, IU,  0,  IU, 0, 0xFFFFFFFF, 0x00000FFF, 0x0000001F },
    { MP_QSTR_c_add,      asm_rv32_opcode_cadd,      CALL_RR,  R,   0, R,   0,  N,  0, 0xFFFFFFFE, 0xFFFFFFFE, 0x00000000 },
    { MP_QSTR_c_addi,     asm_rv32_opcode_caddi,     CALL_RI,  R,   0, IZ,  0,  N,  0, 0xFFFFFFFE, 0x0000003F, 0x00000000 },
    { MP_QSTR_c_addi4spn, asm_rv32_opcode_caddi4spn, CALL_RI,  R,   0, IUZ, 0,  N,  0, 0x0000FF00, 0x000003FC, 0x00000000 },
    { MP_QSTR_c_and,      asm_rv32_opcode_cand,      CALL_RR,  RC,  0, RC,  0,  N,  0, 0x0000FF00, 0x0000FF00, 0x00000000 },
    { MP_QSTR_c_andi,     asm_rv32_opcode_candi,     CALL_RI,  RC,  0, I,   0,  N,  0, 0x0000FF00, 0x0000003F, 0x00000000 },
    { MP_QSTR_c_beqz,     asm_rv32_opcode_cbeqz,     CALL_RL,  RC,  0, L,   0,  N,  0, 0x0000FF00, 0x000001FE, 0x00000000 },
    { MP_QSTR_c_bnez,     asm_rv32_opcode_cbnez,     CALL_RL,  RC,  0, L,   0,  N,  0, 0x0000FF00, 0x000001FE, 0x00000000 },
    { MP_QSTR_c_ebreak,   asm_rv32_opcode_cebreak,   CALL_N,   N,   0, N,   0,  N,  0, 0x00000000, 0x00000000, 0x00000000 },
    { MP_QSTR_c_j,        asm_rv32_opcode_cj,        CALL_L,   L,   0, N,   0,  N,  0, 0x00000FFE, 0x00000000, 0x00000000 },
    { MP_QSTR_c_jal,      asm_rv32_opcode_cjal,      CALL_L,   L,   0, N,   0,  N,  0, 0x00000FFE, 0x00000000, 0x00000000 },
    { MP_QSTR_c_jalr,     asm_rv32_opcode_cjalr,     CALL_R,   R,   0, N,   0,  N,  0, 0xFFFFFFFE, 0x00000000, 0x00000000 },
    { MP_QSTR_c_jr,       asm_rv32_opcode_cjr,       CALL_R,   R,   0, N,   0,  N,  0, 0xFFFFFFFE, 0x00000000, 0x00000000 },
    { MP_QSTR_c_li,       asm_rv32_opcode_cli,       CALL_RI,  R,   0, I,   0,  N,  0, 0xFFFFFFFE, 0x0000003F, 0x00000000 },
    { MP_QSTR_c_lui,      asm_rv32_opcode_clui,      CALL_RI,  R,   0, IUZ, 12, N,  0, 0xFFFFFFFA, 0x0001F800, 0x00000000 },
    { MP_QSTR_c_lw,       asm_rv32_opcode_clw,       CALL_RIR, RC,  0, I,   0,  RC, 0, 0x0000FF00, 0x0000007C, 0x0000FF00 },
    { MP_QSTR_c_lwsp,     asm_rv32_opcode_clwsp,     CALL_RI,  R,   0, I,   0,  N,  0, 0xFFFFFFFE, 0x000000FC, 0x00000000 },
    { MP_QSTR_c_mv,       asm_rv32_opcode_cmv,       CALL_RR,  R,   0, R,   0,  N,  0, 0xFFFFFFFE, 0xFFFFFFFE, 0x00000000 },
    { MP_QSTR_c_nop,      asm_rv32_opcode_cnop,      CALL_N,   N,   0, N,   0,  N,  0, 0x00000000, 0x00000000, 0x00000000 },
    { MP_QSTR_c_or,       asm_rv32_opcode_cor,       CALL_RR,  RC,  0, RC,  0,  N,  0, 0x0000FF00, 0x0000FF00, 0x00000000 },
    { MP_QSTR_c_slli,     asm_rv32_opcode_cslli,     CALL_RI,  R,   0, IU,  0,  N,  0, 0xFFFFFFFE, 0x0000001F, 0x00000000 },
    { MP_QSTR_c_srai,     asm_rv32_opcode_csrai,     CALL_RI,  RC,  0, IU,  0,  N,  0, 0x0000FF00, 0x0000001F, 0x00000000 },
    { MP_QSTR_c_srli,     asm_rv32_opcode_csrli,     CALL_RI,  RC,  0, IU,  0,  N,  0, 0x0000FF00, 0x0000001F, 0x00000000 },
    { MP_QSTR_c_sub,      asm_rv32_opcode_csub,      CALL_RR,  RC,  0, RC,  0,  N,  0, 0x0000FF00, 0x0000FF00, 0x00000000 },
    { MP_QSTR_c_sw,       asm_rv32_opcode_csw,       CALL_RIR, RC,  0, I,   0,  RC, 0, 0x0000FF00, 0x0000007C, 0x0000FF00 },
    { MP_QSTR_c_swsp,     asm_rv32_opcode_cswsp,     CALL_RI,  R,   0, I,   0,  N,  0, 0xFFFFFFFF, 0x000000FC, 0x00000000 },
    { MP_QSTR_c_xor,      asm_rv32_opcode_cxor,      CALL_RR,  RC,  0, RC,  0,  N,  0, 0x0000FF00, 0x0000FF00, 0x00000000 },
    { MP_QSTR_div,        asm_rv32_opcode_div,       CALL_RRR, R,   0, R,   0,  R,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF },
    { MP_QSTR_divu,       asm_rv32_opcode_divu,      CALL_RRR, R,   0, R,   0,  R,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF },
    { MP_QSTR_ebreak,     asm_rv32_opcode_ebreak,    CALL_N,   N,   0, N,   0,  N,  0, 0x00000000, 0x00000000, 0x00000000 },
    { MP_QSTR_ecall,      asm_rv32_opcode_ecall,     CALL_N,   N,   0, N,   0,  N,  0, 0x00000000, 0x00000000, 0x00000000 },
    { MP_QSTR_jal,        asm_rv32_opcode_jal,       CALL_RL,  R,   0, L,   0,  N,  0, 0xFFFFFFFF, 0x001FFFFE, 0x00000000 },
    { MP_QSTR_jalr,       asm_rv32_opcode_jalr,      CALL_RRI, R,   0, R,   0,  I,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000FFF },
    { MP_QSTR_la,         opcode_la,                 CALL_RL,  R,   0, L,   0,  N,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000 },
    { MP_QSTR_lb,         asm_rv32_opcode_lb,        CALL_RIR, R,   0, I,   0,  R,  0, 0xFFFFFFFF, 0x00000FFF, 0xFFFFFFFF },
    { MP_QSTR_lbu,        asm_rv32_opcode_lbu,       CALL_RIR, R,   0, I,   0,  R,  0, 0xFFFFFFFF, 0x00000FFF, 0xFFFFFFFF },
    { MP_QSTR_lh,         asm_rv32_opcode_lh,        CALL_RIR, R,   0, I,   0,  R,  0, 0xFFFFFFFF, 0x00000FFF, 0xFFFFFFFF },
    { MP_QSTR_lhu,        asm_rv32_opcode_lhu,       CALL_RIR, R,   0, I,   0,  R,  0, 0xFFFFFFFF, 0x00000FFF, 0xFFFFFFFF },
    { MP_QSTR_li,         opcode_li,                 CALL_RI,  R,   0, I,   0,  N,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000 },
    { MP_QSTR_lui,        asm_rv32_opcode_lui,       CALL_RI,  R,   0, I,   12, N,  0, 0xFFFFFFFF, 0xFFFFF000, 0x00000000 },
    { MP_QSTR_lw,         asm_rv32_opcode_lw,        CALL_RIR, R,   0, I,   0,  R,  0, 0xFFFFFFFF, 0x00000FFF, 0xFFFFFFFF },
    { MP_QSTR_mv,         asm_rv32_opcode_cmv,       CALL_RR,  R,   0, R,   0,  N,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000 },
    { MP_QSTR_mul,        asm_rv32_opcode_mul,       CALL_RRR, R,   0, R,   0,  R,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF },
    { MP_QSTR_mulh,       asm_rv32_opcode_mulh,      CALL_RRR, R,   0, R,   0,  R,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF },
    { MP_QSTR_mulhsu,     asm_rv32_opcode_mulhsu,    CALL_RRR, R,   0, R,   0,  R,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF },
    { MP_QSTR_mulhu,      asm_rv32_opcode_mulhu,     CALL_RRR, R,   0, R,   0,  R,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF },
    { MP_QSTR_or_,        asm_rv32_opcode_or,        CALL_RRR, R,   0, R,   0,  R,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF },
    { MP_QSTR_ori,        asm_rv32_opcode_ori,       CALL_RRI, R,   0, R,   0,  I,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000FFF },
    { MP_QSTR_rem,        asm_rv32_opcode_rem,       CALL_RRR, R,   0, R,   0,  R,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF },
    { MP_QSTR_remu,       asm_rv32_opcode_remu,      CALL_RRR, R,   0, R,   0,  R,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF },
    { MP_QSTR_sb,         asm_rv32_opcode_sb,        CALL_RIR, R,   0, I,   0,  R,  0, 0xFFFFFFFF, 0x00000FFF, 0xFFFFFFFF },
    { MP_QSTR_sh,         asm_rv32_opcode_sh,        CALL_RIR, R,   0, I,   0,  R,  0, 0xFFFFFFFF, 0x00000FFF, 0xFFFFFFFF },
    { MP_QSTR_sll,        asm_rv32_opcode_sll,       CALL_RRR, R,   0, R,   0,  R,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF },
    { MP_QSTR_slli,       asm_rv32_opcode_slli,      CALL_RRI, R,   0, R,   0,  IU, 0, 0xFFFFFFFF, 0xFFFFFFFF, 0x0000001F },
    { MP_QSTR_slt,        asm_rv32_opcode_slt,       CALL_RRR, R,   0, R,   0,  R,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF },
    { MP_QSTR_slti,       asm_rv32_opcode_slti,      CALL_RRI, R,   0, R,   0,  I,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000FFF },
    { MP_QSTR_sltiu,      asm_rv32_opcode_sltiu,     CALL_RRI, R,   0, R,   0,  I,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000FFF },
    { MP_QSTR_sltu,       asm_rv32_opcode_sltu,      CALL_RRR, R,   0, R,   0,  R,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF },
    { MP_QSTR_sra,        asm_rv32_opcode_sra,       CALL_RRR, R,   0, R,   0,  R,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF },
    { MP_QSTR_srai,       asm_rv32_opcode_srai,      CALL_RRI, R,   0, R,   0,  IU, 0, 0xFFFFFFFF, 0xFFFFFFFF, 0x0000001F },
    { MP_QSTR_srl,        asm_rv32_opcode_srl,       CALL_RRR, R,   0, R,   0,  R,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF },
    { MP_QSTR_srli,       asm_rv32_opcode_srli,      CALL_RRI, R,   0, R,   0,  IU, 0, 0xFFFFFFFF, 0xFFFFFFFF, 0x0000001F },
    { MP_QSTR_sub,        asm_rv32_opcode_sub,       CALL_RRR, R,   0, R,   0,  R,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF },
    { MP_QSTR_sw,         asm_rv32_opcode_sw,        CALL_RIR, R,   0, I,   0,  R,  0, 0xFFFFFFFF, 0x00000FFF, 0xFFFFFFFF },
    { MP_QSTR_xor,        asm_rv32_opcode_xor,       CALL_RRR, R,   0, R,   0,  R,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF },
    { MP_QSTR_xori,       asm_rv32_opcode_xori,      CALL_RRI, R,   0, R,   0,  I,  0, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000FFF },
};

#undef RC
#undef IU
#undef IZ
#undef IUZ

// These two checks assume the bitmasks are contiguous.

static bool is_in_signed_mask(mp_uint_t mask, mp_uint_t value) {
    mp_uint_t leading_zeroes = mp_clz(mask);
    if (leading_zeroes == 0 || leading_zeroes > 32) {
        return true;
    }
    mp_uint_t positive_mask = ~(mask & ~(1U << (31 - leading_zeroes)));
    if ((value & positive_mask) == 0) {
        return true;
    }
    mp_uint_t negative_mask = ~(mask >> 1);
    mp_uint_t trailing_zeroes = mp_ctz(mask);
    if (trailing_zeroes > 0) {
        mp_uint_t trailing_mask = (1U << trailing_zeroes) - 1;
        if ((value & trailing_mask) != 0) {
            return false;
        }
        negative_mask &= ~trailing_mask;
    }
    return (value & negative_mask) == negative_mask;
}

static inline bool is_in_unsigned_mask(mp_uint_t mask, mp_uint_t value) {
    return (value & ~mask) == 0;
}

static bool validate_integer(mp_uint_t value, mp_uint_t mask, mp_uint_t flags) {
    if (flags & U) {
        if (!is_in_unsigned_mask(mask, value)) {
            return false;
        }
    } else {
        if (!is_in_signed_mask(mask, value)) {
            return false;
        }
    }

    if ((flags & Z) && (value == 0)) {
        return false;
    }

    return true;
}

static bool validate_argument(emit_inline_asm_t *emit, qstr opcode_qstr,
    const opcode_t *opcode, mp_parse_node_t node, mp_uint_t node_index) {
    assert((node_index < 3) && "Invalid argument node number.");

    uint32_t kind = 0;
    uint32_t shift = 0;
    uint32_t mask = 0;

    switch (node_index) {
        case 0:
            kind = opcode->argument1_kind;
            shift = opcode->argument1_shift;
            mask = opcode->argument1_mask;
            break;

        case 1:
            kind = opcode->argument2_kind;
            shift = opcode->argument2_shift;
            mask = opcode->argument2_mask;
            break;

        case 2:
            kind = opcode->argument3_kind;
            shift = opcode->argument3_shift;
            mask = opcode->argument3_mask;
            break;

        default:
            break;
    }

    switch (kind & 0x03) {
        case N:
            return true;

        case R: {
            mp_uint_t register_index;
            if (!parse_register_node(node, &register_index, false)) {
                emit_inline_rv32_error_exc(emit,
                    mp_obj_new_exception_msg_varg(&mp_type_SyntaxError,
                        MP_ERROR_TEXT("opcode '%q' argument %d must be a register"),
                        opcode_qstr, node_index + 1));
                return false;
            }

            if ((mask & (1U << register_index)) == 0) {
                emit_inline_rv32_error_exc(emit,
                    mp_obj_new_exception_msg_varg(&mp_type_SyntaxError,
                        MP_ERROR_TEXT("opcode '%q' argument %d is an invalid register"),
                        opcode_qstr, node_index + 1));
                return false;
            }

            return true;
        }
        break;

        case I: {
            mp_obj_t object;
            if (!mp_parse_node_get_int_maybe(node, &object)) {
                emit_inline_rv32_error_exc(emit,
                    mp_obj_new_exception_msg_varg(&mp_type_SyntaxError,
                        MP_ERROR_TEXT("opcode '%q' argument %d must be an integer"),
                        opcode_qstr, node_index + 1));
                return false;
            }

            mp_uint_t immediate = mp_obj_get_int_truncated(object) << shift;
            if (kind & U) {
                if (!is_in_unsigned_mask(mask, immediate)) {
                    goto out_of_range;
                }
            } else {
                if (!is_in_signed_mask(mask, immediate)) {
                    goto out_of_range;
                }
            }

            if ((kind & Z) && (immediate == 0)) {
                goto zero_immediate;
            }

            return true;
        }
        break;

        case L: {
            if (!MP_PARSE_NODE_IS_ID(node)) {
                emit_inline_rv32_error_exc(emit,
                    mp_obj_new_exception_msg_varg(&mp_type_SyntaxError,
                        MP_ERROR_TEXT("opcode '%q' argument %d must be a label"),
                        opcode_qstr, node_index + 1));
                return false;
            }

            qstr qstring;
            mp_uint_t label_index = lookup_label(emit, node, &qstring);
            if (label_index >= emit->max_num_labels && emit->pass == MP_PASS_EMIT) {
                emit_inline_rv32_error_exc(emit,
                    mp_obj_new_exception_msg_varg(&mp_type_SyntaxError,
                        MP_ERROR_TEXT("opcode '%q' argument %d label '%q' is undefined"),
                        opcode_qstr, node_index + 1, qstring));
                return false;
            }

            mp_uint_t displacement = (mp_uint_t)(label_code_offset(emit, label_index));
            if (kind & U) {
                if (!is_in_unsigned_mask(mask, displacement)) {
                    goto out_of_range;
                }
            } else {
                if (!is_in_signed_mask(mask, displacement)) {
                    goto out_of_range;
                }
            }
            return true;
        }
        break;

        default:
            assert(!"Unknown argument kind");
            break;
    }

    return false;

out_of_range:
    emit_inline_rv32_error_exc(emit,
        mp_obj_new_exception_msg_varg(&mp_type_SyntaxError,
            MP_ERROR_TEXT("opcode '%q' argument %d is out of range"),
            opcode_qstr, node_index + 1));
    return false;

zero_immediate:
    emit_inline_rv32_error_exc(emit,
        mp_obj_new_exception_msg_varg(&mp_type_SyntaxError,
            MP_ERROR_TEXT("opcode '%q' argument %d must not be zero"),
            opcode_qstr, node_index + 1));
    return false;
}

static bool parse_register_offset_node(emit_inline_asm_t *emit, qstr opcode_qstr, const opcode_t *opcode_data, mp_parse_node_t node, mp_uint_t node_index, mp_parse_node_t *register_node, mp_parse_node_t *offset_node, bool *negative) {
    assert(register_node != NULL && "Register node pointer is NULL.");
    assert(offset_node != NULL && "Offset node pointer is NULL.");
    assert(negative != NULL && "Negative pointer is NULL.");

    if (!MP_PARSE_NODE_IS_STRUCT_KIND(node, PN_atom_expr_normal) && !MP_PARSE_NODE_IS_STRUCT_KIND(node, PN_factor_2)) {
        goto invalid_structure;
    }
    mp_parse_node_struct_t *node_struct = (mp_parse_node_struct_t *)node;
    *negative = false;
    if (MP_PARSE_NODE_IS_STRUCT_KIND(node, PN_factor_2)) {
        if (MP_PARSE_NODE_IS_TOKEN_KIND(node_struct->nodes[0], MP_TOKEN_OP_MINUS)) {
            *negative = true;
        } else {
            if (!MP_PARSE_NODE_IS_TOKEN_KIND(node_struct->nodes[0], MP_TOKEN_OP_PLUS)) {
                goto invalid_structure;
            }
        }
        if (!MP_PARSE_NODE_IS_STRUCT_KIND(node_struct->nodes[1], PN_atom_expr_normal)) {
            goto invalid_structure;
        }
        node_struct = (mp_parse_node_struct_t *)node_struct->nodes[1];
    }

    if (*negative) {
        // If the value is negative, RULE_atom_expr_normal's first token will be the
        // offset stripped of its negative marker; range check will then fail if the
        // default method is used, so a custom check is used instead.
        mp_obj_t object;
        if (!mp_parse_node_get_int_maybe(node_struct->nodes[0], &object)) {
            emit_inline_rv32_error_exc(emit,
                mp_obj_new_exception_msg_varg(&mp_type_SyntaxError,
                    MP_ERROR_TEXT("opcode '%q' argument %d must be an integer"),
                    opcode_qstr, 2));
            return false;
        }
        mp_uint_t value = mp_obj_get_int_truncated(object);
        value = (~value + 1) & (mp_uint_t)-1;
        if (!validate_integer(value << opcode_data->argument2_shift, opcode_data->argument2_mask, opcode_data->argument2_kind)) {
            emit_inline_rv32_error_exc(emit,
                mp_obj_new_exception_msg_varg(&mp_type_SyntaxError,
                    MP_ERROR_TEXT("opcode '%q' argument %d is out of range"),
                    opcode_qstr, 2));
            return false;
        }
    } else {
        if (!validate_argument(emit, opcode_qstr, opcode_data, node_struct->nodes[0], 1)) {
            return false;
        }
    }

    *offset_node = node_struct->nodes[0];
    node_struct = (mp_parse_node_struct_t *)node_struct->nodes[1];
    if (!validate_argument(emit, opcode_qstr, opcode_data, node_struct->nodes[0], 2)) {
        return false;
    }
    *register_node = node_struct->nodes[0];
    return true;

invalid_structure:
    emit_inline_rv32_error_exc(emit,
        mp_obj_new_exception_msg_varg(&mp_type_SyntaxError,
            MP_ERROR_TEXT("opcode '%q' argument %d must be an integer offset to a register"),
            opcode_qstr, node_index + 1));
    return false;
}

static void handle_opcode(emit_inline_asm_t *emit, qstr opcode, const opcode_t *opcode_data, mp_parse_node_t *arguments) {
    mp_uint_t rd = 0;
    mp_uint_t rs1 = 0;
    mp_uint_t rs2 = 0;

    switch (opcode_data->calling_convention) {
        case CALL_RRR: {
            parse_register_node(arguments[0], &rd, opcode_data->argument1_kind & C);
            parse_register_node(arguments[1], &rs1, opcode_data->argument2_kind & C);
            parse_register_node(arguments[2], &rs2, opcode_data->argument3_kind & C);
            ((call_rrr_t)opcode_data->emitter)(&emit->as, rd, rs1, rs2);
            break;
        }

        case CALL_RR: {
            parse_register_node(arguments[0], &rd, opcode_data->argument1_kind & C);
            parse_register_node(arguments[1], &rs1, opcode_data->argument2_kind & C);
            ((call_rr_t)opcode_data->emitter)(&emit->as, rd, rs1);
            break;
        }

        case CALL_RRI: {
            parse_register_node(arguments[0], &rd, opcode_data->argument1_kind & C);
            parse_register_node(arguments[1], &rs1, opcode_data->argument2_kind & C);
            mp_obj_t object;
            mp_parse_node_get_int_maybe(arguments[2], &object);
            mp_uint_t immediate = mp_obj_get_int_truncated(object) << opcode_data->argument3_shift;
            ((call_rri_t)opcode_data->emitter)(&emit->as, rd, rs1, immediate);
            break;
        }

        case CALL_RI: {
            parse_register_node(arguments[0], &rd, opcode_data->argument1_kind & C);
            mp_obj_t object;
            mp_parse_node_get_int_maybe(arguments[1], &object);
            mp_uint_t immediate = mp_obj_get_int_truncated(object) << opcode_data->argument2_shift;
            ((call_ri_t)opcode_data->emitter)(&emit->as, rd, immediate);
            break;
        }

        case CALL_R: {
            parse_register_node(arguments[0], &rd, opcode_data->argument1_kind & C);
            ((call_r_t)opcode_data->emitter)(&emit->as, rd);
            break;
        }

        case CALL_RRL: {
            parse_register_node(arguments[0], &rd, opcode_data->argument1_kind & C);
            parse_register_node(arguments[1], &rs1, opcode_data->argument2_kind & C);
            qstr qstring;
            mp_uint_t label_index = lookup_label(emit, arguments[2], &qstring);
            ptrdiff_t displacement = label_code_offset(emit, label_index);
            ((call_rri_t)opcode_data->emitter)(&emit->as, rd, rs1, displacement);
            break;
        }

        case CALL_RL: {
            parse_register_node(arguments[0], &rd, opcode_data->argument1_kind & C);
            qstr qstring;
            mp_uint_t label_index = lookup_label(emit, arguments[1], &qstring);
            ptrdiff_t displacement = label_code_offset(emit, label_index);
            ((call_ri_t)opcode_data->emitter)(&emit->as, rd, displacement);
            break;
        }

        case CALL_L: {
            qstr qstring;
            mp_uint_t label_index = lookup_label(emit, arguments[0], &qstring);
            ptrdiff_t displacement = label_code_offset(emit, label_index);
            ((call_i_t)opcode_data->emitter)(&emit->as, displacement);
            break;
        }

        case CALL_N:
            ((call_n_t)opcode_data->emitter)(&emit->as);
            break;

        case CALL_I: {
            mp_obj_t object;
            mp_parse_node_get_int_maybe(arguments[0], &object);
            mp_uint_t immediate = mp_obj_get_int_truncated(object) << opcode_data->argument1_shift;
            ((call_i_t)opcode_data->emitter)(&emit->as, immediate);
            break;
        }

        case CALL_RII: {
            parse_register_node(arguments[0], &rd, opcode_data->argument1_kind & C);
            mp_obj_t object;
            mp_parse_node_get_int_maybe(arguments[1], &object);
            mp_uint_t immediate1 = mp_obj_get_int_truncated(object) << opcode_data->argument2_shift;
            mp_parse_node_get_int_maybe(arguments[2], &object);
            mp_uint_t immediate2 = mp_obj_get_int_truncated(object) << opcode_data->argument3_shift;
            ((call_rii_t)opcode_data->emitter)(&emit->as, rd, immediate1, immediate2);
            break;
        }

        case CALL_RIR:
            assert(!"Should not get here.");
            break;

        default:
            assert(!"Unhandled call convention.");
            break;
    }
}

static bool handle_load_store_opcode_with_offset(emit_inline_asm_t *emit, qstr opcode, const opcode_t *opcode_data, mp_parse_node_t *argument_nodes) {
    mp_parse_node_t nodes[3] = {0};
    if (!validate_argument(emit, opcode, opcode_data, argument_nodes[0], 0)) {
        return false;
    }
    nodes[0] = argument_nodes[0];
    bool negative = false;
    if (!parse_register_offset_node(emit, opcode, opcode_data, argument_nodes[1], 1, &nodes[1], &nodes[2], &negative)) {
        return false;
    }

    mp_uint_t rd;
    mp_uint_t rs1;
    parse_register_node(nodes[0], &rd, opcode_data->argument1_kind & C);
    if (!parse_register_node(nodes[1], &rs1, opcode_data->argument3_kind & C)) {
        return false;
    }

    mp_obj_t object;
    mp_parse_node_get_int_maybe(nodes[2], &object);
    mp_uint_t immediate = mp_obj_get_int_truncated(object) << opcode_data->argument2_shift;
    if (negative) {
        immediate = (~immediate + 1) & (mp_uint_t)-1;
    }
    if (!is_in_signed_mask(opcode_data->argument2_mask, immediate)) {
        emit_inline_rv32_error_exc(emit,
            mp_obj_new_exception_msg_varg(&mp_type_SyntaxError,
                MP_ERROR_TEXT("opcode '%q' argument %d is out of range"),
                opcode, 2));
        return false;
    }

    ((call_rri_t)opcode_data->emitter)(&emit->as, rd, rs1, immediate);
    return true;
}

static void emit_inline_rv32_opcode(emit_inline_asm_t *emit, qstr opcode, mp_uint_t arguments_count, mp_parse_node_t *argument_nodes) {
    const opcode_t *opcode_data = NULL;
    for (mp_uint_t index = 0; index < MP_ARRAY_SIZE(OPCODES); index++) {
        if (OPCODES[index].qstring == opcode) {
            opcode_data = &OPCODES[index];
            break;
        }
    }

    if (!opcode_data) {
        emit_inline_rv32_error_exc(emit,
            mp_obj_new_exception_msg_varg(&mp_type_SyntaxError,
                MP_ERROR_TEXT("unsupported or unknown RV32 instruction '%q'."), opcode));
        return;
    }

    size_t required_arguments = 0;
    if (opcode_data->argument1_kind != N) {
        required_arguments++;
    }
    if (opcode_data->argument2_kind != N) {
        required_arguments++;
    }
    if (opcode_data->argument3_kind != N) {
        required_arguments++;
    }

    if (opcode_data->calling_convention != CALL_RIR) {
        if (required_arguments != arguments_count) {
            emit_inline_rv32_error_exc(emit,
                mp_obj_new_exception_msg_varg(&mp_type_SyntaxError,
                    MP_ERROR_TEXT("RV32 instruction '%q' requires %d arguments."), opcode, required_arguments));
            return;
        }
        if (required_arguments >= 1 && !validate_argument(emit, opcode, opcode_data, argument_nodes[0], 0)) {
            return;
        }
        if (required_arguments >= 2 && !validate_argument(emit, opcode, opcode_data, argument_nodes[1], 1)) {
            return;
        }
        if (required_arguments >= 3 && !validate_argument(emit, opcode, opcode_data, argument_nodes[2], 2)) {
            return;
        }
        handle_opcode(emit, opcode, opcode_data, argument_nodes);
        return;
    }

    assert(required_arguments == 3 && "Invalid arguments count for calling convention.");
    assert((opcode_data->argument2_kind & U) == 0 && "Offset must not be unsigned.");
    assert((opcode_data->argument2_kind & Z) == 0 && "Offset can be zero.");

    if (arguments_count != 2) {
        emit_inline_rv32_error_exc(emit,
            mp_obj_new_exception_msg_varg(&mp_type_SyntaxError,
                MP_ERROR_TEXT("RV32 instruction '%q' requires %d arguments."), opcode, 2));
        return;
    }

    handle_load_store_opcode_with_offset(emit, opcode, opcode_data, argument_nodes);
}

#undef N
#undef R
#undef I
#undef L
#undef C
#undef U

const emit_inline_asm_method_table_t emit_inline_rv32_method_table = {
    #if MICROPY_DYNAMIC_COMPILER
    emit_inline_rv32_new,
    emit_inline_rv32_free,
    #endif

    emit_inline_rv32_start_pass,
    emit_inline_rv32_end_pass,
    emit_inline_rv32_count_params,
    emit_inline_rv32_label,
    emit_inline_rv32_opcode,
};

#endif // MICROPY_EMIT_INLINE_RV32
