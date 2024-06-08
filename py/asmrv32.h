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

#define RV32_ENCODE_TYPE_B(op, ft3, rs1, rs2, imm)                                              \
    ((op & 0b1111111) | ((ft3 & 0b111) << 12) | ((imm & 0b100000000000) >> 4) |  \
    ((imm & 0b11110) << 7) | ((rs1 & 0b11111) << 15) |                          \
    ((rs2 & 0b11111) << 20) | ((imm & 0b11111100000) << 20) |                   \
    ((imm & 0b1000000000000) << 19))

#define RV32_ENCODE_TYPE_I(op, ft3, rd, rs, imm)                               \
    ((op & 0b1111111) | ((rd & 0b11111) << 7) | ((ft3 & 0b111) << 12) |          \
    ((rs & 0b11111) << 15) | ((imm & 0b111111111111) << 20))

#define RV32_ENCODE_TYPE_J(op, rd, imm)                                        \
    ((op & 0b1111111) | ((rd & 0b11111) << 7) | (imm & 0b11111111000000000000) | \
    ((imm & 0b100000000000) << 9) | ((imm & 0b11111111110) << 20) |             \
    ((imm & 0b100000000000000000000) << 11))

#define RV32_ENCODE_TYPE_R(op, ft3, ft7, rd, rs1, rs2)                         \
    ((op & 0b1111111) | ((rd & 0b11111) << 7) | ((ft3 & 0b111) << 12) |          \
    ((rs1 & 0b11111) << 15) | ((rs2 & 0b11111) << 20) |                         \
    ((ft7 & 0b1111111) << 25))

#define RV32_ENCODE_TYPE_S(op, ft3, rs1, rs2, imm)                             \
    ((op & 0b1111111) | ((imm & 0b11111) << 7) | ((ft3 & 0b111) << 12) |         \
    ((rs1 & 0b11111) << 15) | ((rs2 & 0b11111) << 20) |                         \
    ((imm & 0b111111100000) << 20))

#define RV32_ENCODE_TYPE_U(op, rd, imm)                                        \
    ((op & 0b1111111) | ((rd & 0b11111) << 7) |                                  \
    (imm & 0b11111111111111111111000000000000))

#define RV32_ENCODE_TYPE_CI(op, ft3, rd, imm) \
    ((op & 0b11) | ((ft3 & 0b111) << 13) | ((rd & 0b11111) << 7) | \
    (((imm) & 0b100000) << 7) | (((imm) & 0b11111) << 2))

#define RV32_ENCODE_TYPE_CJ(op, ft3, imm) \
    ((op & 0b11) | ((ft3 & 0b111) << 13) | \
    ((imm & 0b1110) << 2) | ((imm & 0b1100000000) << 1) | \
    ((imm & 0b100000000000) << 1) | ((imm & 0b10000000000) >> 2) | \
    ((imm & 0b10000000) >> 1) | ((imm & 0b1000000) << 1) | \
    ((imm & 0b100000) >> 3) | ((imm & 0b10000) << 7))

#define RV32_ENCODE_TYPE_CR(op, ft4, rs1, rs2) \
    ((op & 0b11) | ((rs2 & 0b11111) << 2) | ((rs1 & 0b11111) << 7) | \
    ((ft4 & 0b1111) << 12))

#define RV32_ENCODE_TYPE_CSS(op, ft3, rs, imm) \
    ((op & 0b11) | ((ft3 & 0b111) << 13) | ((rs & 0b11111) << 2) | ((imm) & 0b111111) << 7)

void asm_rv32_emit_word_opcode(asm_rv32_t *state, mp_uint_t opcode);
void asm_rv32_emit_halfword_opcode(asm_rv32_t *state, mp_uint_t opcode);

// ADD RD, RS1, RS2
static inline void asm_rv32_opcode_add(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs1, mp_uint_t rs2) {
    // R: 0000000 ..... ..... 000 ..... 0110011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_R(0b0110011, 0b000, 0b0000000, rd, rs1, rs2));
}

// ADDI RD, RS, IMMEDIATE
static inline void asm_rv32_opcode_addi(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs, mp_int_t immediate) {
    // I: ............ ..... 000 ..... 0010011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_I(0b0010011, 0b000, rd, rs, immediate));
}

// AND RD, RS1, RS2
static inline void asm_rv32_opcode_and(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs1, mp_uint_t rs2) {
    // R: 0000000 ..... ..... 111 ..... 0110011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_R(0b0110011, 0b111, 0b0000000, rd, rs1, rs2));
}

// AUIPC RD, offset
static inline void asm_rv32_opcode_auipc(asm_rv32_t *state, mp_uint_t rd, mp_int_t offset) {
    // U: .................... ..... 0010111
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_U(0b0010111, rd, offset));
}

// BEQ RS1, RS2, OFFSET
static inline void asm_rv32_opcode_beq(asm_rv32_t *state, mp_uint_t rs1, mp_uint_t rs2, mp_int_t offset) {
    // B: . ...... ..... ..... 000 .... . 1100011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_B(0b1100011, 0b000, rs1, rs2, offset));
}

// BNE RS1, RS2, OFFSET
static inline void asm_rv32_opcode_bne(asm_rv32_t *state, mp_uint_t rs1, mp_uint_t rs2, mp_int_t offset) {
    // B: . ...... ..... ..... 001 .... . 1100011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_B(0b1100011, 0b001, rs1, rs2, offset));
}

// C.ADD RD, RS
static inline void asm_rv32_opcode_cadd(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs) {
    // CR: 1001 ..... ..... 10
    asm_rv32_emit_halfword_opcode(state, RV32_ENCODE_TYPE_CR(0b10, 0b1001, rd, rs));
}

// C.ADDI RD, IMMEDIATE
static inline void asm_rv32_opcode_caddi(asm_rv32_t *state, mp_uint_t rd, mp_int_t immediate) {
    // CI: 000 . ..... ..... 01
    asm_rv32_emit_halfword_opcode(state, RV32_ENCODE_TYPE_CI(0b01, 0b000, rd, immediate));
}

// C.J OFFSET
static inline void asm_rv32_opcode_cj(asm_rv32_t *state, mp_uint_t offset) {
    // CJ: 101 ........... 01
    asm_rv32_emit_halfword_opcode(state, RV32_ENCODE_TYPE_CJ(0b01, 0b001, offset));
}

// C.JALR RS
static inline void asm_rv32_opcode_cjalr(asm_rv32_t *state, mp_uint_t rs) {
    // CR: 1001 ..... 00000 10
    asm_rv32_emit_halfword_opcode(state, RV32_ENCODE_TYPE_CR(0b10, 0b1001, rs, 0));
}

// C.JR RS
static inline void asm_rv32_opcode_cjr(asm_rv32_t *state, mp_uint_t rs) {
    // CR: 1000 ..... 00000 10
    asm_rv32_emit_halfword_opcode(state, RV32_ENCODE_TYPE_CR(0b10, 0b1000, rs, 0));
}

// C.LI RD, IMMEDIATE
static inline void asm_rv32_opcode_cli(asm_rv32_t *state, mp_uint_t rd, mp_int_t immediate) {
    // CI: 010 . ..... ..... 01
    asm_rv32_emit_halfword_opcode(state, RV32_ENCODE_TYPE_CI(0b01, 0b010, rd, immediate));
}

// C.LUI RD, IMMEDIATE
static inline void asm_rv32_opcode_clui(asm_rv32_t *state, mp_uint_t rd, mp_int_t immediate) {
    // CI: 011 . ..... ..... 01
    asm_rv32_emit_halfword_opcode(state, RV32_ENCODE_TYPE_CI(0b01, 0b011, rd, immediate >> 12));
}

// C.LWSP RD, OFFSET
static inline void asm_rv32_opcode_clwsp(asm_rv32_t *state, mp_uint_t rd, mp_uint_t offset) {
    // CI: 010 . ..... ..... 10
    asm_rv32_emit_halfword_opcode(state, RV32_ENCODE_TYPE_CI(0b10, 0b010, rd, ((offset & 0b11000000) >> 6) | (offset & 0b111100)));
}

// C.MV RD, RS
static inline void asm_rv32_opcode_cmv(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs) {
    // CR: 1000 ..... ..... 10
    asm_rv32_emit_halfword_opcode(state, RV32_ENCODE_TYPE_CR(0b10, 0b1000, rd, rs));
}

// C.SWSP RS, OFFSET
static inline void asm_rv32_opcode_cswsp(asm_rv32_t *state, mp_uint_t rs, mp_uint_t offset) {
    // CSS: 010 ...... ..... 10
    asm_rv32_emit_halfword_opcode(state, RV32_ENCODE_TYPE_CSS(0b10, 0b110, rs, ((offset & 0b11000000) >> 6) | (offset & 0b111100)));
}

// JALR RD, RS, offset
static inline void asm_rv32_opcode_jalr(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs, mp_int_t offset) {
    // I: ............ ..... 000 ..... 1100111
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_I(0b1100111, 0b000, rd, rs, offset));
}

// LBU RD, OFFSET(RS)
static inline void asm_rv32_opcode_lbu(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs, mp_int_t offset) {
    // I: ............ ..... 100 ..... 0000011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_I(0b0000011, 0b100, rd, rs, offset));
}

// LHU RD, OFFSET(RS)
static inline void asm_rv32_opcode_lhu(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs, mp_int_t offset) {
    // I: ............ ..... 101 ..... 0000011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_I(0b0000011, 0b101, rd, rs, offset));
}

// LUI RD, immediate
static inline void asm_rv32_opcode_lui(asm_rv32_t *state, mp_uint_t rd, mp_int_t immediate) {
    // U: .................... ..... 0110111
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_U(0b0110111, rd, immediate));
}

// LW RD, OFFSET(RS)
static inline void asm_rv32_opcode_lw(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs, mp_int_t offset) {
    // I: ............ ..... 010 ..... 0000011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_I(0b0000011, 0b010, rd, rs, offset));
}

// MUL RD, RS1, RS2
static inline void asm_rv32m_opcode_mul(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs1, mp_uint_t rs2) {
    // R: 0000001 ..... ..... 000 ..... 0110011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_R(0b0110011, 0b000, 0b0000001, rd, rs1, rs2));
}

// OR RD, RS1, RS2
static inline void asm_rv32_opcode_or(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs1, mp_uint_t rs2) {
    // R: 0000000 ..... ..... 110 ..... 0110011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_R(0b0110011, 0b110, 0b0000000, rd, rs1, rs2));
}

// SLL RD, RS1, RS2
static inline void asm_rv32_opcode_sll(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs1, mp_uint_t rs2) {
    // R: 0000000 ..... ..... 001 ..... 0110011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_R(0b0110011, 0b001, 0b0000000, rd, rs1, rs2));
}

// SLLI RD, RS, IMMEDIATE
static inline void asm_rv32_opcode_slli(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs, mp_uint_t immediate) {
    // I: 0000000..... ..... 001 ..... 0010011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_I(0b0010011, 0b001, rd, rs, immediate & 0x1F));
}

// SRL RD, RS1, RS2
static inline void asm_rv32_opcode_srl(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs1, mp_uint_t rs2) {
    // R: 0000000 ..... ..... 101 ..... 0110011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_R(0b0110011, 0b101, 0b0000000, rd, rs1, rs2));
}

// SLT RD, RS1, RS2
static inline void asm_rv32_opcode_slt(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs1, mp_uint_t rs2) {
    // R: 0000000 ..... ..... 010 ..... 0110011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_R(0b0110011, 0b010, 0b0000000, rd, rs1, rs2));
}

// SLTU RD, RS1, RS2
static inline void asm_rv32_opcode_sltu(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs1, mp_uint_t rs2) {
    // R: 0000000 ..... ..... 011 ..... 0110011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_R(0b0110011, 0b011, 0b0000000, rd, rs1, rs2));
}

// SRA RD, RS1, RS2
static inline void asm_rv32_opcode_sra(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs1, mp_uint_t rs2) {
    // R: 0100000 ..... ..... 101 ..... 0110011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_R(0b0110011, 0b101, 0b0100000, rd, rs1, rs2));
}

// SUB RD, RS1, RS2
static inline void asm_rv32_opcode_sub(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs1, mp_uint_t rs2) {
    // R: 0100000 ..... ..... 000 ..... 0110011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_R(0b0110011, 0b000, 0b0100000, rd, rs1, rs2));
}

// SB RS2, OFFSET(RS1)
static inline void asm_rv32_opcode_sb(asm_rv32_t *state, mp_uint_t rs2, mp_uint_t rs1, mp_int_t offset) {
    // S: ....... ..... ..... 000 ..... 0100011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_S(0b0100011, 0b000, rs1, rs2, offset));
}

// SH RS2, OFFSET(RS1)
static inline void asm_rv32_opcode_sh(asm_rv32_t *state, mp_uint_t rs2, mp_uint_t rs1, mp_int_t offset) {
    // S: ....... ..... ..... 001 ..... 0100011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_S(0b0100011, 0b001, rs1, rs2, offset));
}

// SW RS2, OFFSET(RS1)
static inline void asm_rv32_opcode_sw(asm_rv32_t *state, mp_uint_t rs2, mp_uint_t rs1, mp_int_t offset) {
    // S: ....... ..... ..... 010 ..... 0100011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_S(0b0100011, 0b010, rs1, rs2, offset));
}

// XOR RD, RS1, RS2
static inline void asm_rv32_opcode_xor(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs1, mp_uint_t rs2) {
    // R: 0000000 ..... ..... 100 ..... 0110011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_R(0b0110011, 0b100, 0b0000000, rd, rs1, rs2));
}

// XORI RD, RS, IMMEDIATE
static inline void asm_rv32_opcode_xori(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs, mp_int_t immediate) {
    // I: ............ ..... 100 ..... 0010011
    asm_rv32_emit_word_opcode(state, RV32_ENCODE_TYPE_I(0b0010011, 0b100, rd, rs, immediate));
}

#define ASM_WORD_SIZE (4)

#define REG_RET ASM_RV32_REG_A0
#define REG_ARG_1 ASM_RV32_REG_A0
#define REG_ARG_2 ASM_RV32_REG_A1
#define REG_ARG_3 ASM_RV32_REG_A2
#define REG_ARG_4 ASM_RV32_REG_A3
#define REG_TEMP0 ASM_RV32_REG_T1
#define REG_TEMP1 ASM_RV32_REG_T2
#define REG_TEMP2 ASM_RV32_REG_T3
// S0 may be used as the frame pointer by the compiler.
#define REG_FUN_TABLE ASM_RV32_REG_S2
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
#define ASM_JUMP_REG(state, rs) asm_rv32_opcode_jalr(state, ASM_RV32_REG_ZERO, rs, 0)
#define ASM_LOAD16_REG_REG_OFFSET(state, rd, rs, offset) asm_rv32_emit_load16_reg_reg_offset(state, rd, rs, offset)
#define ASM_LOAD16_REG_REG(state, rd, rs) asm_rv32_opcode_lhu(state, rd, rs, 0)
#define ASM_LOAD32_REG_REG(state, rd, rs) asm_rv32_opcode_lw(state, rd, rs, 0)
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
#define ASM_STORE32_REG_REG(state, rs1, rs2) asm_rv32_opcode_sw(state, rs1, rs2, 0)
#define ASM_STORE8_REG_REG(state, rs1, rs2) asm_rv32_opcode_sb(state, rs1, rs2, 0)
#define ASM_STORE_REG_REG_OFFSET(state, rd, rs, offset) asm_rv32_emit_store_reg_reg_offset(state, rd, rs, offset)
#define ASM_STORE_REG_REG(state, rs1, rs2) ASM_STORE32_REG_REG(state, rs1, rs2)
#define ASM_SUB_REG_REG(state, rd, rs) asm_rv32_opcode_sub(state, rd, rd, rs)
#define ASM_XOR_REG_REG(state, rd, rs) asm_rv32_emit_optimised_xor(state, rd, rs)

#endif

#endif // MICROPY_INCLUDED_PY_ASMRV32_H
