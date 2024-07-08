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

#ifndef MICROPY_INCLUDED_PY_ASMRV32_H
#define MICROPY_INCLUDED_PY_ASMRV32_H

#include <assert.h>

#include "py/asmbase.h"
#include "py/emit.h"
#include "py/misc.h"
#include "py/persistentcode.h"

#define ASM_RV32_REG_X0 (0)   // Zero
#define ASM_RV32_REG_X1 (1)   // RA
#define ASM_RV32_REG_X2 (2)   // SP
#define ASM_RV32_REG_X3 (3)   // GP
#define ASM_RV32_REG_X4 (4)   // TP
#define ASM_RV32_REG_X5 (5)   // T0
#define ASM_RV32_REG_X6 (6)   // T1
#define ASM_RV32_REG_X7 (7)   // T2
#define ASM_RV32_REG_X8 (8)   // S0
#define ASM_RV32_REG_X9 (9)   // S1
#define ASM_RV32_REG_X10 (10) // A0
#define ASM_RV32_REG_X11 (11) // A1
#define ASM_RV32_REG_X12 (12) // A2
#define ASM_RV32_REG_X13 (13) // A3
#define ASM_RV32_REG_X14 (14) // A4
#define ASM_RV32_REG_X15 (15) // A5
#define ASM_RV32_REG_X16 (16) // A6
#define ASM_RV32_REG_X17 (17) // A7
#define ASM_RV32_REG_X18 (18) // S2
#define ASM_RV32_REG_X19 (19) // S3
#define ASM_RV32_REG_X20 (20) // S4
#define ASM_RV32_REG_X21 (21) // S5
#define ASM_RV32_REG_X22 (22) // S6
#define ASM_RV32_REG_X23 (23) // S7
#define ASM_RV32_REG_X24 (24) // S8
#define ASM_RV32_REG_X25 (25) // S9
#define ASM_RV32_REG_X26 (26) // S10
#define ASM_RV32_REG_X27 (27) // S11
#define ASM_RV32_REG_X28 (28) // T3
#define ASM_RV32_REG_X29 (29) // T4
#define ASM_RV32_REG_X30 (30) // T5
#define ASM_RV32_REG_X31 (31) // T6

// Alternate register names.

#define ASM_RV32_REG_ZERO (ASM_RV32_REG_X0)
#define ASM_RV32_REG_RA (ASM_RV32_REG_X1)
#define ASM_RV32_REG_SP (ASM_RV32_REG_X2)
#define ASM_RV32_REG_GP (ASM_RV32_REG_X3)
#define ASM_RV32_REG_TP (ASM_RV32_REG_X4)
#define ASM_RV32_REG_T0 (ASM_RV32_REG_X5)
#define ASM_RV32_REG_T1 (ASM_RV32_REG_X6)
#define ASM_RV32_REG_T2 (ASM_RV32_REG_X7)
#define ASM_RV32_REG_A0 (ASM_RV32_REG_X10)
#define ASM_RV32_REG_A1 (ASM_RV32_REG_X11)
#define ASM_RV32_REG_A2 (ASM_RV32_REG_X12)
#define ASM_RV32_REG_A3 (ASM_RV32_REG_X13)
#define ASM_RV32_REG_A4 (ASM_RV32_REG_X14)
#define ASM_RV32_REG_A5 (ASM_RV32_REG_X15)
#define ASM_RV32_REG_A6 (ASM_RV32_REG_X16)
#define ASM_RV32_REG_A7 (ASM_RV32_REG_X17)
#define ASM_RV32_REG_T3 (ASM_RV32_REG_X28)
#define ASM_RV32_REG_T4 (ASM_RV32_REG_X29)
#define ASM_RV32_REG_T5 (ASM_RV32_REG_X30)
#define ASM_RV32_REG_T6 (ASM_RV32_REG_X31)
#define ASM_RV32_REG_FP (ASM_RV32_REG_X8)
#define ASM_RV32_REG_S0 (ASM_RV32_REG_X8)
#define ASM_RV32_REG_S1 (ASM_RV32_REG_X9)
#define ASM_RV32_REG_S2 (ASM_RV32_REG_X18)
#define ASM_RV32_REG_S3 (ASM_RV32_REG_X19)
#define ASM_RV32_REG_S4 (ASM_RV32_REG_X20)
#define ASM_RV32_REG_S5 (ASM_RV32_REG_X21)
#define ASM_RV32_REG_S6 (ASM_RV32_REG_X22)
#define ASM_RV32_REG_S7 (ASM_RV32_REG_X23)
#define ASM_RV32_REG_S8 (ASM_RV32_REG_X24)
#define ASM_RV32_REG_S9 (ASM_RV32_REG_X25)
#define ASM_RV32_REG_S10 (ASM_RV32_REG_X26)
#define ASM_RV32_REG_S11 (ASM_RV32_REG_X27)

typedef struct _asm_rv32_t {
    // Opaque emitter state.
    mp_asm_base_t base;
    // Which registers are tainted and need saving/restoring.
    mp_uint_t saved_registers_mask;
    // How many locals must be stored on the stack.
    mp_uint_t locals_count;
    // The computed function stack size.
    mp_uint_t stack_size;
    // The stack offset where stack-based locals start to be stored.
    mp_uint_t locals_stack_offset;
} asm_rv32_t;

void asm_rv32_entry(asm_rv32_t *state, mp_uint_t locals);
void asm_rv32_exit(asm_rv32_t *state);
void asm_rv32_end_pass(asm_rv32_t *state);

////////////////////////////////////////////////////////////////////////////////

#define RV32_ENCODE_TYPE_B(op, ft3, rs1, rs2, imm) \
    ((op & 0x7F) | ((ft3 & 0x07) << 12) | ((imm & 0x800) >> 4) | \
    ((imm & 0x1E) << 7) | ((rs1 & 0x1F) << 15) | ((rs2 & 0x1F) << 20) | \
    ((imm & 0x7E0) << 20) | ((imm & 0x1000) << 19))

#define RV32_ENCODE_TYPE_I(op, ft3, rd, rs, imm) \
    ((op & 0x7F) | ((rd & 0x1F) << 7) | ((ft3 & 0x07) << 12) | \
    ((rs & 0x1F) << 15) | ((imm & 0xFFF) << 20))

#define RV32_ENCODE_TYPE_J(op, rd, imm) \
    ((op & 0x7F) | ((rd & 0x1F) << 7) | (imm & 0xFF000) | \
    ((imm & 0x800) << 9) | ((imm & 0x7FE) << 20) | ((imm & 0x100000) << 11))

#define RV32_ENCODE_TYPE_R(op, ft3, ft7, rd, rs1, rs2) \
    ((op & 0x7F) | ((rd & 0x1F) << 7) | ((ft3 & 0x07) << 12) | \
    ((rs1 & 0x1F) << 15) | ((rs2 & 0x1F) << 20) | ((ft7 & 0x7F) << 25))

#define RV32_ENCODE_TYPE_S(op, ft3, rs1, rs2, imm) \
    ((op & 0x7F) | ((imm & 0x1F) << 7) | ((ft3 & 0x07) << 12) | \
    ((rs1 & 0x1F) << 15) | ((rs2 & 0x1F) << 20) | ((imm & 0xFE0) << 20))

#define RV32_ENCODE_TYPE_U(op, rd, imm) \
    ((op & 0x7F) | ((rd & 0x1F) << 7) | (imm & 0xFFFFF000))

#define RV32_ENCODE_TYPE_CB(op, ft3, rs, imm) \
    ((op & 0x03) | ((ft3 & 0x07) << 13) | ((rs & 0x07) << 7) | \
    (((imm) & 0x100) << 4) | (((imm) & 0xC0) >> 1) | (((imm) & 0x20) >> 3) | \
    (((imm) & 0x18) << 7) | (((imm) & 0x06) << 2))

#define RV32_ENCODE_TYPE_CI(op, ft3, rd, imm) \
    ((op & 0x03) | ((ft3 & 0x07) << 13) | ((rd & 0x1F) << 7) | \
    (((imm) & 0x20) << 7) | (((imm) & 0x1F) << 2))

#define RV32_ENCODE_TYPE_CIW(op, ft3, rd, imm) \
    ((op & 0x03) | ((ft3 & 0x07) << 13) | ((rd & 0x07) << 2) | \
    ((imm & 0x3C0) << 1) | ((imm & 0x30) << 7) | \
    ((imm & 0x08) << 2) | ((imm & 0x04) << 4))

#define RV32_ENCODE_TYPE_CJ(op, ft3, imm) \
    ((op & 0x03) | ((ft3 & 0x07) << 13) | ((imm & 0x0E) << 2) | \
    ((imm & 0x300) << 1) | ((imm & 0x800) << 1) | ((imm & 0x400) >> 2) | \
    ((imm & 0x80) >> 1) | ((imm & 0x40) << 1) | ((imm & 0x20) >> 3) | \
    ((imm & 0x10) << 7))

#define RV32_ENCODE_TYPE_CL(op, ft3, rd, rs, imm) \
    ((op & 0x03) | ((ft3 & 0x07) << 13) | ((rd & 0x07) << 2) | \
    ((rs & 0x07) << 7) | ((imm & 0x40) >> 1) | ((imm & 0x38) << 7) | \
    ((imm & 0x04) << 4))

#define RV32_ENCODE_TYPE_CR(op, ft4, rs1, rs2) \
    ((op & 0x03) | ((rs2 & 0x1F) << 2) | ((rs1 & 0x1F) << 7) | ((ft4 & 0x0F) << 12))

#define RV32_ENCODE_TYPE_CSS(op, ft3, rs, imm) \
    ((op & 0x03) | ((ft3 & 0x07) << 13) | ((rs & 0x1F) << 2) | ((imm) & 0x3F) << 7)

void asm_rv32_emit_word_opcode(asm_rv32_t *state, mp_uint_t opcode);
void asm_rv32_emit_halfword_opcode(asm_rv32_t *state, mp_uint_t opcode);

// ADD RD, RS1, RS2
static inline void asm_rv32_opcode_add(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs1, mp_uint_t rs2) {
    // R: 0000000 ..... ..... 000 ..... 0110011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_R(0x33, 0x00, 0x00, rd, rs1, rs2));
}

// ADDI RD, RS, IMMEDIATE
static inline void asm_rv32_opcode_addi(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs, mp_int_t immediate) {
    // I: ............ ..... 000 ..... 0010011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_I(0x13, 0x00, rd, rs, immediate));
}

// AND RD, RS1, RS2
static inline void asm_rv32_opcode_and(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs1, mp_uint_t rs2) {
    // R: 0000000 ..... ..... 111 ..... 0110011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_R(0x33, 0x07, 0x00, rd, rs1, rs2));
}

// AUIPC RD, offset
static inline void asm_rv32_opcode_auipc(asm_rv32_t *state, mp_uint_t rd, mp_int_t offset) {
    // U: .................... ..... 0010111
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_U(0x17, rd, offset));
}

// BEQ RS1, RS2, OFFSET
static inline void asm_rv32_opcode_beq(asm_rv32_t *state, mp_uint_t rs1, mp_uint_t rs2, mp_int_t offset) {
    // B: . ...... ..... ..... 000 .... . 1100011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_B(0x63, 0x00, rs1, rs2, offset));
}

// BNE RS1, RS2, OFFSET
static inline void asm_rv32_opcode_bne(asm_rv32_t *state, mp_uint_t rs1, mp_uint_t rs2, mp_int_t offset) {
    // B: . ...... ..... ..... 001 .... . 1100011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_B(0x63, 0x01, rs1, rs2, offset));
}

// C.ADD RD, RS
static inline void asm_rv32_opcode_cadd(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs) {
    // CR: 1001 ..... ..... 10
    asm_rv32_emit_halfword_opcode(state, RV32_ENCODE_TYPE_CR(0x02, 0x09, rd, rs));
}

// C.ADDI RD, IMMEDIATE
static inline void asm_rv32_opcode_caddi(asm_rv32_t *state, mp_uint_t rd, mp_int_t immediate) {
    // CI: 000 . ..... ..... 01
    asm_rv32_emit_halfword_opcode(state, RV32_ENCODE_TYPE_CI(0x01, 0x00, rd, immediate));
}

// C.ADDI4SPN RD', IMMEDIATE
static inline void asm_rv32_opcode_caddi4spn(asm_rv32_t *state, mp_uint_t rd, mp_uint_t immediate) {
    // CIW: 000 ........ ... 00
    asm_rv32_emit_halfword_opcode(state, RV32_ENCODE_TYPE_CIW(0x00, 0x00, rd, immediate));
}

// C.BEQZ RS', IMMEDIATE
static inline void asm_rv32_opcode_cbeqz(asm_rv32_t *state, mp_uint_t rs, mp_int_t offset) {
    // CB: 110 ... ... ..... 01
    asm_rv32_emit_halfword_opcode(state, RV32_ENCODE_TYPE_CB(0x01, 0x06, rs, offset));
}

// C.BNEZ RS', IMMEDIATE
static inline void asm_rv32_opcode_cbnez(asm_rv32_t *state, mp_uint_t rs, mp_int_t offset) {
    // CB: 111 ... ... ..... 01
    asm_rv32_emit_halfword_opcode(state, RV32_ENCODE_TYPE_CB(0x01, 0x07, rs, offset));
}

// C.J OFFSET
static inline void asm_rv32_opcode_cj(asm_rv32_t *state, mp_int_t offset) {
    // CJ: 101 ........... 01
    asm_rv32_emit_halfword_opcode(state, RV32_ENCODE_TYPE_CJ(0x01, 0x05, offset));
}

// C.JALR RS
static inline void asm_rv32_opcode_cjalr(asm_rv32_t *state, mp_uint_t rs) {
    // CR: 1001 ..... 00000 10
    asm_rv32_emit_halfword_opcode(state, RV32_ENCODE_TYPE_CR(0x02, 0x09, rs, 0));
}

// C.JR RS
static inline void asm_rv32_opcode_cjr(asm_rv32_t *state, mp_uint_t rs) {
    // CR: 1000 ..... 00000 10
    asm_rv32_emit_halfword_opcode(state, RV32_ENCODE_TYPE_CR(0x02, 0x08, rs, 0));
}

// C.LI RD, IMMEDIATE
static inline void asm_rv32_opcode_cli(asm_rv32_t *state, mp_uint_t rd, mp_int_t immediate) {
    // CI: 010 . ..... ..... 01
    asm_rv32_emit_halfword_opcode(state, RV32_ENCODE_TYPE_CI(0x01, 0x02, rd, immediate));
}

// C.LUI RD, IMMEDIATE
static inline void asm_rv32_opcode_clui(asm_rv32_t *state, mp_uint_t rd, mp_int_t immediate) {
    // CI: 011 . ..... ..... 01
    asm_rv32_emit_halfword_opcode(state, RV32_ENCODE_TYPE_CI(0x01, 0x03, rd, immediate >> 12));
}

// C.LW RD', OFFSET(RS')
static inline void asm_rv32_opcode_clw(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs, mp_int_t offset) {
    // CL: 010 ... ... .. ... 00
    asm_rv32_emit_halfword_opcode(state, RV32_ENCODE_TYPE_CL(0x00, 0x02, rd, rs, offset));
}

// C.LWSP RD, OFFSET
static inline void asm_rv32_opcode_clwsp(asm_rv32_t *state, mp_uint_t rd, mp_uint_t offset) {
    // CI: 010 . ..... ..... 10
    asm_rv32_emit_halfword_opcode(state, RV32_ENCODE_TYPE_CI(0x02, 0x02, rd, ((offset & 0xC0) >> 6) | (offset & 0x3C)));
}

// C.MV RD, RS
static inline void asm_rv32_opcode_cmv(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs) {
    // CR: 1000 ..... ..... 10
    asm_rv32_emit_halfword_opcode(state, RV32_ENCODE_TYPE_CR(0x02, 0x08, rd, rs));
}

// C.SWSP RS, OFFSET
static inline void asm_rv32_opcode_cswsp(asm_rv32_t *state, mp_uint_t rs, mp_uint_t offset) {
    // CSS: 010 ...... ..... 10
    asm_rv32_emit_halfword_opcode(state, RV32_ENCODE_TYPE_CSS(0x02, 0x06, rs, ((offset & 0xC0) >> 6) | (offset & 0x3C)));
}

// JALR RD, RS, offset
static inline void asm_rv32_opcode_jalr(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs, mp_int_t offset) {
    // I: ............ ..... 000 ..... 1100111
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_I(0x67, 0x00, rd, rs, offset));
}

// LBU RD, OFFSET(RS)
static inline void asm_rv32_opcode_lbu(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs, mp_int_t offset) {
    // I: ............ ..... 100 ..... 0000011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_I(0x03, 0x04, rd, rs, offset));
}

// LHU RD, OFFSET(RS)
static inline void asm_rv32_opcode_lhu(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs, mp_int_t offset) {
    // I: ............ ..... 101 ..... 0000011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_I(0x03, 0x05, rd, rs, offset));
}

// LUI RD, immediate
static inline void asm_rv32_opcode_lui(asm_rv32_t *state, mp_uint_t rd, mp_int_t immediate) {
    // U: .................... ..... 0110111
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_U(0x37, rd, immediate));
}

// LW RD, OFFSET(RS)
static inline void asm_rv32_opcode_lw(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs, mp_int_t offset) {
    // I: ............ ..... 010 ..... 0000011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_I(0x03, 0x02, rd, rs, offset));
}

// MUL RD, RS1, RS2
static inline void asm_rv32m_opcode_mul(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs1, mp_uint_t rs2) {
    // R: 0000001 ..... ..... 000 ..... 0110011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_R(0x33, 0x00, 0x01, rd, rs1, rs2));
}

// OR RD, RS1, RS2
static inline void asm_rv32_opcode_or(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs1, mp_uint_t rs2) {
    // R: 0000000 ..... ..... 110 ..... 0110011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_R(0x33, 0x06, 0x00, rd, rs1, rs2));
}

// SLL RD, RS1, RS2
static inline void asm_rv32_opcode_sll(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs1, mp_uint_t rs2) {
    // R: 0000000 ..... ..... 001 ..... 0110011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_R(0x33, 0x01, 0x00, rd, rs1, rs2));
}

// SLLI RD, RS, IMMEDIATE
static inline void asm_rv32_opcode_slli(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs, mp_uint_t immediate) {
    // I: 0000000..... ..... 001 ..... 0010011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_I(0x13, 0x01, rd, rs, immediate & 0x1F));
}

// SRL RD, RS1, RS2
static inline void asm_rv32_opcode_srl(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs1, mp_uint_t rs2) {
    // R: 0000000 ..... ..... 101 ..... 0110011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_R(0x33, 0x05, 0x00, rd, rs1, rs2));
}

// SLT RD, RS1, RS2
static inline void asm_rv32_opcode_slt(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs1, mp_uint_t rs2) {
    // R: 0000000 ..... ..... 010 ..... 0110011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_R(0x33, 0x02, 0x00, rd, rs1, rs2));
}

// SLTU RD, RS1, RS2
static inline void asm_rv32_opcode_sltu(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs1, mp_uint_t rs2) {
    // R: 0000000 ..... ..... 011 ..... 0110011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_R(0x33, 0x03, 0x00, rd, rs1, rs2));
}

// SRA RD, RS1, RS2
static inline void asm_rv32_opcode_sra(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs1, mp_uint_t rs2) {
    // R: 0100000 ..... ..... 101 ..... 0110011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_R(0x33, 0x05, 0x20, rd, rs1, rs2));
}

// SUB RD, RS1, RS2
static inline void asm_rv32_opcode_sub(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs1, mp_uint_t rs2) {
    // R: 0100000 ..... ..... 000 ..... 0110011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_R(0x33, 0x00, 0x20, rd, rs1, rs2));
}

// SB RS2, OFFSET(RS1)
static inline void asm_rv32_opcode_sb(asm_rv32_t *state, mp_uint_t rs2, mp_uint_t rs1, mp_int_t offset) {
    // S: ....... ..... ..... 000 ..... 0100011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_S(0x23, 0x00, rs1, rs2, offset));
}

// SH RS2, OFFSET(RS1)
static inline void asm_rv32_opcode_sh(asm_rv32_t *state, mp_uint_t rs2, mp_uint_t rs1, mp_int_t offset) {
    // S: ....... ..... ..... 001 ..... 0100011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_S(0x23, 0x01, rs1, rs2, offset));
}

// SW RS2, OFFSET(RS1)
static inline void asm_rv32_opcode_sw(asm_rv32_t *state, mp_uint_t rs2, mp_uint_t rs1, mp_int_t offset) {
    // S: ....... ..... ..... 010 ..... 0100011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_S(0x23, 0x02, rs1, rs2, offset));
}

// XOR RD, RS1, RS2
static inline void asm_rv32_opcode_xor(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs1, mp_uint_t rs2) {
    // R: 0000000 ..... ..... 100 ..... 0110011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_R(0x33, 0x04, 0x00, rd, rs1, rs2));
}

// XORI RD, RS, IMMEDIATE
static inline void asm_rv32_opcode_xori(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs, mp_int_t immediate) {
    // I: ............ ..... 100 ..... 0010011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_I(0x13, 0x04, rd, rs, immediate));
}

#define ASM_WORD_SIZE (4)
#define ASM_HALFWORD_SIZE (2)

#define REG_RET ASM_RV32_REG_A0
#define REG_ARG_1 ASM_RV32_REG_A0
#define REG_ARG_2 ASM_RV32_REG_A1
#define REG_ARG_3 ASM_RV32_REG_A2
#define REG_ARG_4 ASM_RV32_REG_A3
#define REG_TEMP0 ASM_RV32_REG_T1
#define REG_TEMP1 ASM_RV32_REG_T2
#define REG_TEMP2 ASM_RV32_REG_T3
#define REG_FUN_TABLE ASM_RV32_REG_S1
#define REG_LOCAL_1 ASM_RV32_REG_S3
#define REG_LOCAL_2 ASM_RV32_REG_S4
#define REG_LOCAL_3 ASM_RV32_REG_S5

void asm_rv32_meta_comparison_eq(asm_rv32_t *state, mp_uint_t rs1, mp_uint_t rs2, mp_uint_t rd);
void asm_rv32_meta_comparison_ne(asm_rv32_t *state, mp_uint_t rs1, mp_uint_t rs2, mp_uint_t rd);
void asm_rv32_meta_comparison_lt(asm_rv32_t *state, mp_uint_t rs1, mp_uint_t rs2, mp_uint_t rd, bool unsigned_comparison);
void asm_rv32_meta_comparison_le(asm_rv32_t *state, mp_uint_t rs1, mp_uint_t rs2, mp_uint_t rd, bool unsigned_comparison);

#ifdef GENERIC_ASM_API

void asm_rv32_emit_call_ind(asm_rv32_t *state, mp_uint_t index);
void asm_rv32_emit_jump(asm_rv32_t *state, mp_uint_t label);
void asm_rv32_emit_jump_if_reg_eq(asm_rv32_t *state, mp_uint_t rs1, mp_uint_t rs2, mp_uint_t label);
void asm_rv32_emit_jump_if_reg_nonzero(asm_rv32_t *state, mp_uint_t rs, mp_uint_t label);
void asm_rv32_emit_load16_reg_reg_offset(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs, mp_int_t offset);
void asm_rv32_emit_load_reg_reg_offset(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs, mp_int_t offset);
void asm_rv32_emit_mov_local_reg(asm_rv32_t *state, mp_uint_t local, mp_uint_t rs);
void asm_rv32_emit_mov_reg_local(asm_rv32_t *state, mp_uint_t rd, mp_uint_t local);
void asm_rv32_emit_mov_reg_local_addr(asm_rv32_t *state, mp_uint_t rd, mp_uint_t local);
void asm_rv32_emit_mov_reg_pcrel(asm_rv32_t *state, mp_uint_t rd, mp_uint_t label);
void asm_rv32_emit_optimised_load_immediate(asm_rv32_t *state, mp_uint_t rd, mp_int_t immediate);
void asm_rv32_emit_optimised_xor(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs);
void asm_rv32_emit_store_reg_reg_offset(asm_rv32_t *state, mp_uint_t source, mp_uint_t base, mp_int_t offset);

#define ASM_T asm_rv32_t
#define ASM_ENTRY(state, labels) asm_rv32_entry(state, labels)
#define ASM_EXIT(state) asm_rv32_exit(state)
#define ASM_END_PASS(state) asm_rv32_end_pass(state)

#define ASM_ADD_REG_REG(state, rd, rs) asm_rv32_opcode_cadd(state, rd, rs)
#define ASM_AND_REG_REG(state, rd, rs) asm_rv32_opcode_and(state, rd, rs, rd)
#define ASM_ASR_REG_REG(state, rd, rs) asm_rv32_opcode_sra(state, rd, rd, rs)
#define ASM_CALL_IND(state, index) asm_rv32_emit_call_ind(state, index)
#define ASM_JUMP(state, label) asm_rv32_emit_jump(state, label)
#define ASM_JUMP_IF_REG_EQ(state, rs1, rs2, label) asm_rv32_emit_jump_if_reg_eq(state, rs1, rs2, label)
#define ASM_JUMP_IF_REG_NONZERO(state, rs, label, bool_test) asm_rv32_emit_jump_if_reg_nonzero(state, rs, label)
#define ASM_JUMP_IF_REG_ZERO(state, rs, label, bool_test) asm_rv32_emit_jump_if_reg_eq(state, rs, ASM_RV32_REG_ZERO, label)
#define ASM_JUMP_REG(state, rs) asm_rv32_opcode_cjr(state, rs)
#define ASM_LOAD16_REG_REG_OFFSET(state, rd, rs, offset) asm_rv32_emit_load16_reg_reg_offset(state, rd, rs, offset)
#define ASM_LOAD16_REG_REG(state, rd, rs) asm_rv32_opcode_lhu(state, rd, rs, 0)
#define ASM_LOAD32_REG_REG(state, rd, rs) ASM_LOAD_REG_REG_OFFSET(state, rd, rs, 0)
#define ASM_LOAD8_REG_REG(state, rd, rs) asm_rv32_opcode_lbu(state, rd, rs, 0)
#define ASM_LOAD_REG_REG_OFFSET(state, rd, rs, offset) asm_rv32_emit_load_reg_reg_offset(state, rd, rs, offset)
#define ASM_LOAD_REG_REG(state, rd, rs) ASM_LOAD32_REG_REG(state, rd, rs)
#define ASM_LSL_REG_REG(state, rd, rs) asm_rv32_opcode_sll(state, rd, rd, rs)
#define ASM_LSR_REG_REG(state, rd, rs) asm_rv32_opcode_srl(state, rd, rd, rs)
#define ASM_MOV_LOCAL_REG(state, local, rs) asm_rv32_emit_mov_local_reg(state, local, rs)
#define ASM_MOV_REG_IMM(state, rd, imm) asm_rv32_emit_optimised_load_immediate(state, rd, imm)
#define ASM_MOV_REG_LOCAL_ADDR(state, rd, local) asm_rv32_emit_mov_reg_local_addr(state, rd, local)
#define ASM_MOV_REG_LOCAL(state, rd, local) asm_rv32_emit_mov_reg_local(state, rd, local)
#define ASM_MOV_REG_PCREL(state, rd, label) asm_rv32_emit_mov_reg_pcrel(state, rd, label)
#define ASM_MOV_REG_REG(state, rd, rs) asm_rv32_opcode_cmv(state, rd, rs)
#define ASM_MUL_REG_REG(state, rd, rs) asm_rv32m_opcode_mul(state, rd, rd, rs)
#define ASM_NEG_REG(state, rd) asm_rv32_opcode_sub(state, rd, ASM_RV32_REG_ZERO, rd)
#define ASM_NOT_REG(state, rd) asm_rv32_opcode_xori(state, rd, rd, -1)
#define ASM_OR_REG_REG(state, rd, rs) asm_rv32_opcode_or(state, rd, rd, rs)
#define ASM_STORE16_REG_REG(state, rs1, rs2) asm_rv32_opcode_sh(state, rs1, rs2, 0)
#define ASM_STORE32_REG_REG(state, rs1, rs2) ASM_STORE_REG_REG_OFFSET(state, rs1, rs2, 0)
#define ASM_STORE8_REG_REG(state, rs1, rs2) asm_rv32_opcode_sb(state, rs1, rs2, 0)
#define ASM_STORE_REG_REG_OFFSET(state, rd, rs, offset) asm_rv32_emit_store_reg_reg_offset(state, rd, rs, offset)
#define ASM_STORE_REG_REG(state, rs1, rs2) ASM_STORE32_REG_REG(state, rs1, rs2)
#define ASM_SUB_REG_REG(state, rd, rs) asm_rv32_opcode_sub(state, rd, rd, rs)
#define ASM_XOR_REG_REG(state, rd, rs) asm_rv32_emit_optimised_xor(state, rd, rs)

#endif

#endif // MICROPY_INCLUDED_PY_ASMRV32_H
