/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
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
#ifndef MICROPY_INCLUDED_PY_ASMXTENSA_H
#define MICROPY_INCLUDED_PY_ASMXTENSA_H

#include "py/asmbase.h"

// calling conventions:
// up to 6 args in a2-a7
// return value in a2
// PC stored in a0
// stack pointer is a1, stack full descending, is aligned to 16 bytes
// callee save: a1, a12, a13, a14, a15
// caller save: a3

#define ASM_XTENSA_REG_A0  (0)
#define ASM_XTENSA_REG_A1  (1)
#define ASM_XTENSA_REG_A2  (2)
#define ASM_XTENSA_REG_A3  (3)
#define ASM_XTENSA_REG_A4  (4)
#define ASM_XTENSA_REG_A5  (5)
#define ASM_XTENSA_REG_A6  (6)
#define ASM_XTENSA_REG_A7  (7)
#define ASM_XTENSA_REG_A8  (8)
#define ASM_XTENSA_REG_A9  (9)
#define ASM_XTENSA_REG_A10 (10)
#define ASM_XTENSA_REG_A11 (11)
#define ASM_XTENSA_REG_A12 (12)
#define ASM_XTENSA_REG_A13 (13)
#define ASM_XTENSA_REG_A14 (14)
#define ASM_XTENSA_REG_A15 (15)

// for bccz
#define ASM_XTENSA_CCZ_EQ (0)
#define ASM_XTENSA_CCZ_NE (1)

// for bcc and setcc
#define ASM_XTENSA_CC_NONE  (0)
#define ASM_XTENSA_CC_EQ    (1)
#define ASM_XTENSA_CC_LT    (2)
#define ASM_XTENSA_CC_LTU   (3)
#define ASM_XTENSA_CC_ALL   (4)
#define ASM_XTENSA_CC_BC    (5)
#define ASM_XTENSA_CC_ANY   (8)
#define ASM_XTENSA_CC_NE    (9)
#define ASM_XTENSA_CC_GE    (10)
#define ASM_XTENSA_CC_GEU   (11)
#define ASM_XTENSA_CC_NALL  (12)
#define ASM_XTENSA_CC_BS    (13)

// macros for encoding instructions (little endian versions)
#define ASM_XTENSA_ENCODE_RRR(op0, op1, op2, r, s, t) \
    ((((uint32_t)op2) << 20) | (((uint32_t)op1) << 16) | ((r) << 12) | ((s) << 8) | ((t) << 4) | (op0))
#define ASM_XTENSA_ENCODE_RRI4(op0, op1, r, s, t, imm4) \
    (((imm4) << 20) | ((op1) << 16) | ((r) << 12) | ((s) << 8) | ((t) << 4) | (op0))
#define ASM_XTENSA_ENCODE_RRI8(op0, r, s, t, imm8) \
    ((((uint32_t)imm8) << 16) | ((r) << 12) | ((s) << 8) | ((t) << 4) | (op0))
#define ASM_XTENSA_ENCODE_RI16(op0, t, imm16) \
    (((imm16) << 8) | ((t) << 4) | (op0))
#define ASM_XTENSA_ENCODE_RSR(op0, op1, op2, rs, t) \
    (((op2) << 20) | ((op1) << 16) | ((rs) << 8) | ((t) << 4) | (op0))
#define ASM_XTENSA_ENCODE_CALL(op0, n, offset) \
    (((offset) << 6) | ((n) << 4) | (op0))
#define ASM_XTENSA_ENCODE_CALLX(op0, op1, op2, r, s, m, n) \
    ((((uint32_t)op2) << 20) | (((uint32_t)op1) << 16) | ((r) << 12) | ((s) << 8) | ((m) << 6) | ((n) << 4) | (op0))
#define ASM_XTENSA_ENCODE_BRI8(op0, r, s, m, n, imm8) \
    (((imm8) << 16) | ((r) << 12) | ((s) << 8) | ((m) << 6) | ((n) << 4) | (op0))
#define ASM_XTENSA_ENCODE_BRI12(op0, s, m, n, imm12) \
    (((imm12) << 12) | ((s) << 8) | ((m) << 6) | ((n) << 4) | (op0))
#define ASM_XTENSA_ENCODE_RRRN(op0, r, s, t) \
    (((r) << 12) | ((s) << 8) | ((t) << 4) | (op0))
#define ASM_XTENSA_ENCODE_RI7(op0, s, imm7) \
    ((((imm7) & 0xf) << 12) | ((s) << 8) | ((imm7) & 0x70) | (op0))

typedef struct _asm_xtensa_t {
    mp_asm_base_t base;
    uint32_t cur_const;
    uint32_t num_const;
    uint32_t *const_table;
    uint32_t stack_adjust;
} asm_xtensa_t;

void asm_xtensa_end_pass(asm_xtensa_t *as);

void asm_xtensa_entry(asm_xtensa_t *as, int num_locals);
void asm_xtensa_exit(asm_xtensa_t *as);

void asm_xtensa_op16(asm_xtensa_t *as, uint16_t op);
void asm_xtensa_op24(asm_xtensa_t *as, uint32_t op);

// raw instructions

static inline void asm_xtensa_op_add_n(asm_xtensa_t *as, uint reg_dest, uint reg_src_a, uint reg_src_b) {
    asm_xtensa_op16(as, ASM_XTENSA_ENCODE_RRRN(10, reg_dest, reg_src_a, reg_src_b));
}

static inline void asm_xtensa_op_addi(asm_xtensa_t *as, uint reg_dest, uint reg_src, int imm8) {
    asm_xtensa_op24(as, ASM_XTENSA_ENCODE_RRI8(2, 12, reg_src, reg_dest, imm8 & 0xff));
}

static inline void asm_xtensa_op_and(asm_xtensa_t *as, uint reg_dest, uint reg_src_a, uint reg_src_b) {
    asm_xtensa_op24(as, ASM_XTENSA_ENCODE_RRR(0, 0, 1, reg_dest, reg_src_a, reg_src_b));
}

static inline void asm_xtensa_op_bcc(asm_xtensa_t *as, uint cond, uint reg_src1, uint reg_src2, int32_t rel8) {
    asm_xtensa_op24(as, ASM_XTENSA_ENCODE_RRI8(7, cond, reg_src1, reg_src2, rel8 & 0xff));
}

static inline void asm_xtensa_op_bccz(asm_xtensa_t *as, uint cond, uint reg_src, int32_t rel12) {
    asm_xtensa_op24(as, ASM_XTENSA_ENCODE_BRI12(6, reg_src, cond, 1, rel12 & 0xfff));
}

static inline void asm_xtensa_op_call0(asm_xtensa_t *as, int32_t rel18) {
    asm_xtensa_op24(as, ASM_XTENSA_ENCODE_CALL(5, 0, rel18 & 0x3ffff));
}

static inline void asm_xtensa_op_callx0(asm_xtensa_t *as, uint reg) {
    asm_xtensa_op24(as, ASM_XTENSA_ENCODE_CALLX(0, 0, 0, 0, reg, 3, 0));
}

static inline void asm_xtensa_op_j(asm_xtensa_t *as, int32_t rel18) {
    asm_xtensa_op24(as, ASM_XTENSA_ENCODE_CALL(6, 0, rel18 & 0x3ffff));
}

static inline void asm_xtensa_op_jx(asm_xtensa_t *as, uint reg) {
    asm_xtensa_op24(as, ASM_XTENSA_ENCODE_CALLX(0, 0, 0, 0, reg, 2, 2));
}

static inline void asm_xtensa_op_l8ui(asm_xtensa_t *as, uint reg_dest, uint reg_base, uint byte_offset) {
    asm_xtensa_op24(as, ASM_XTENSA_ENCODE_RRI8(2, 0, reg_base, reg_dest, byte_offset & 0xff));
}

static inline void asm_xtensa_op_l16ui(asm_xtensa_t *as, uint reg_dest, uint reg_base, uint half_word_offset) {
    asm_xtensa_op24(as, ASM_XTENSA_ENCODE_RRI8(2, 1, reg_base, reg_dest, half_word_offset & 0xff));
}

static inline void asm_xtensa_op_l32i(asm_xtensa_t *as, uint reg_dest, uint reg_base, uint word_offset) {
    asm_xtensa_op24(as, ASM_XTENSA_ENCODE_RRI8(2, 2, reg_base, reg_dest, word_offset & 0xff));
}

static inline void asm_xtensa_op_l32i_n(asm_xtensa_t *as, uint reg_dest, uint reg_base, uint word_offset) {
    asm_xtensa_op16(as, ASM_XTENSA_ENCODE_RRRN(8, word_offset & 0xf, reg_base, reg_dest));
}

static inline void asm_xtensa_op_l32r(asm_xtensa_t *as, uint reg_dest, uint32_t op_off, uint32_t dest_off) {
    asm_xtensa_op24(as, ASM_XTENSA_ENCODE_RI16(1, reg_dest, ((dest_off - ((op_off + 3) & ~3)) >> 2) & 0xffff));
}

static inline void asm_xtensa_op_mov_n(asm_xtensa_t *as, uint reg_dest, uint reg_src) {
    asm_xtensa_op16(as, ASM_XTENSA_ENCODE_RRRN(13, 0, reg_src, reg_dest));
}

static inline void asm_xtensa_op_movi(asm_xtensa_t *as, uint reg_dest, int32_t imm12) {
    asm_xtensa_op24(as, ASM_XTENSA_ENCODE_RRI8(2, 10, (imm12 >> 8) & 0xf, reg_dest, imm12 & 0xff));
}

static inline void asm_xtensa_op_movi_n(asm_xtensa_t *as, uint reg_dest, int imm4) {
    asm_xtensa_op16(as, ASM_XTENSA_ENCODE_RI7(12, reg_dest, imm4));
}

static inline void asm_xtensa_op_mull(asm_xtensa_t *as, uint reg_dest, uint reg_src_a, uint reg_src_b) {
    asm_xtensa_op24(as, ASM_XTENSA_ENCODE_RRR(0, 2, 8, reg_dest, reg_src_a, reg_src_b));
}

static inline void asm_xtensa_op_or(asm_xtensa_t *as, uint reg_dest, uint reg_src_a, uint reg_src_b) {
    asm_xtensa_op24(as, ASM_XTENSA_ENCODE_RRR(0, 0, 2, reg_dest, reg_src_a, reg_src_b));
}

static inline void asm_xtensa_op_ret_n(asm_xtensa_t *as) {
    asm_xtensa_op16(as, ASM_XTENSA_ENCODE_RRRN(13, 15, 0, 0));
}

static inline void asm_xtensa_op_s8i(asm_xtensa_t *as, uint reg_src, uint reg_base, uint byte_offset) {
    asm_xtensa_op24(as, ASM_XTENSA_ENCODE_RRI8(2, 4, reg_base, reg_src, byte_offset & 0xff));
}

static inline void asm_xtensa_op_s16i(asm_xtensa_t *as, uint reg_src, uint reg_base, uint half_word_offset) {
    asm_xtensa_op24(as, ASM_XTENSA_ENCODE_RRI8(2, 5, reg_base, reg_src, half_word_offset & 0xff));
}

static inline void asm_xtensa_op_s32i(asm_xtensa_t *as, uint reg_src, uint reg_base, uint word_offset) {
    asm_xtensa_op24(as, ASM_XTENSA_ENCODE_RRI8(2, 6, reg_base, reg_src, word_offset & 0xff));
}

static inline void asm_xtensa_op_s32i_n(asm_xtensa_t *as, uint reg_src, uint reg_base, uint word_offset) {
    asm_xtensa_op16(as, ASM_XTENSA_ENCODE_RRRN(9, word_offset & 0xf, reg_base, reg_src));
}

static inline void asm_xtensa_op_sll(asm_xtensa_t *as, uint reg_dest, uint reg_src) {
    asm_xtensa_op24(as, ASM_XTENSA_ENCODE_RRR(0, 1, 10, reg_dest, reg_src, 0));
}

static inline void asm_xtensa_op_sra(asm_xtensa_t *as, uint reg_dest, uint reg_src) {
    asm_xtensa_op24(as, ASM_XTENSA_ENCODE_RRR(0, 1, 11, reg_dest, 0, reg_src));
}

static inline void asm_xtensa_op_ssl(asm_xtensa_t *as, uint reg_src) {
    asm_xtensa_op24(as, ASM_XTENSA_ENCODE_RRR(0, 0, 4, 1, reg_src, 0));
}

static inline void asm_xtensa_op_ssr(asm_xtensa_t *as, uint reg_src) {
    asm_xtensa_op24(as, ASM_XTENSA_ENCODE_RRR(0, 0, 4, 0, reg_src, 0));
}

static inline void asm_xtensa_op_sub(asm_xtensa_t *as, uint reg_dest, uint reg_src_a, uint reg_src_b) {
    asm_xtensa_op24(as, ASM_XTENSA_ENCODE_RRR(0, 0, 12, reg_dest, reg_src_a, reg_src_b));
}

static inline void asm_xtensa_op_xor(asm_xtensa_t *as, uint reg_dest, uint reg_src_a, uint reg_src_b) {
    asm_xtensa_op24(as, ASM_XTENSA_ENCODE_RRR(0, 0, 3, reg_dest, reg_src_a, reg_src_b));
}

// convenience functions
void asm_xtensa_j_label(asm_xtensa_t *as, uint label);
void asm_xtensa_bccz_reg_label(asm_xtensa_t *as, uint cond, uint reg, uint label);
void asm_xtensa_bcc_reg_reg_label(asm_xtensa_t *as, uint cond, uint reg1, uint reg2, uint label);
void asm_xtensa_setcc_reg_reg_reg(asm_xtensa_t *as, uint cond, uint reg_dest, uint reg_src1, uint reg_src2);
void asm_xtensa_mov_reg_i32(asm_xtensa_t *as, uint reg_dest, uint32_t i32);
void asm_xtensa_mov_local_reg(asm_xtensa_t *as, int local_num, uint reg_src);
void asm_xtensa_mov_reg_local(asm_xtensa_t *as, uint reg_dest, int local_num);
void asm_xtensa_mov_reg_local_addr(asm_xtensa_t *as, uint reg_dest, int local_num);
void asm_xtensa_mov_reg_pcrel(asm_xtensa_t *as, uint reg_dest, uint label);

#if GENERIC_ASM_API

// The following macros provide a (mostly) arch-independent API to
// generate native code, and are used by the native emitter.

#define ASM_WORD_SIZE (4)

#define REG_RET ASM_XTENSA_REG_A2
#define REG_ARG_1 ASM_XTENSA_REG_A2
#define REG_ARG_2 ASM_XTENSA_REG_A3
#define REG_ARG_3 ASM_XTENSA_REG_A4
#define REG_ARG_4 ASM_XTENSA_REG_A5
#define REG_ARG_5 ASM_XTENSA_REG_A6

#define REG_TEMP0 ASM_XTENSA_REG_A2
#define REG_TEMP1 ASM_XTENSA_REG_A3
#define REG_TEMP2 ASM_XTENSA_REG_A4

#define REG_LOCAL_1 ASM_XTENSA_REG_A12
#define REG_LOCAL_2 ASM_XTENSA_REG_A13
#define REG_LOCAL_3 ASM_XTENSA_REG_A14
#define REG_LOCAL_NUM (3)

#define ASM_T               asm_xtensa_t
#define ASM_END_PASS        asm_xtensa_end_pass
#define ASM_ENTRY           asm_xtensa_entry
#define ASM_EXIT            asm_xtensa_exit

#define ASM_JUMP            asm_xtensa_j_label
#define ASM_JUMP_IF_REG_ZERO(as, reg, label, bool_test) \
    asm_xtensa_bccz_reg_label(as, ASM_XTENSA_CCZ_EQ, reg, label)
#define ASM_JUMP_IF_REG_NONZERO(as, reg, label, bool_test) \
    asm_xtensa_bccz_reg_label(as, ASM_XTENSA_CCZ_NE, reg, label)
#define ASM_JUMP_IF_REG_EQ(as, reg1, reg2, label) \
    asm_xtensa_bcc_reg_reg_label(as, ASM_XTENSA_CC_EQ, reg1, reg2, label)
#define ASM_JUMP_REG(as, reg) asm_xtensa_op_jx((as), (reg))
#define ASM_CALL_IND(as, ptr, idx) \
    do { \
        asm_xtensa_mov_reg_i32(as, ASM_XTENSA_REG_A0, (uint32_t)ptr); \
        asm_xtensa_op_callx0(as, ASM_XTENSA_REG_A0); \
    } while (0)

#define ASM_MOV_LOCAL_REG(as, local_num, reg_src) asm_xtensa_mov_local_reg((as), (local_num), (reg_src))
#define ASM_MOV_REG_IMM(as, reg_dest, imm) asm_xtensa_mov_reg_i32((as), (reg_dest), (imm))
#define ASM_MOV_REG_ALIGNED_IMM(as, reg_dest, imm) asm_xtensa_mov_reg_i32((as), (reg_dest), (imm))
#define ASM_MOV_REG_LOCAL(as, reg_dest, local_num) asm_xtensa_mov_reg_local((as), (reg_dest), (local_num))
#define ASM_MOV_REG_REG(as, reg_dest, reg_src) asm_xtensa_op_mov_n((as), (reg_dest), (reg_src))
#define ASM_MOV_REG_LOCAL_ADDR(as, reg_dest, local_num) asm_xtensa_mov_reg_local_addr((as), (reg_dest), (local_num))
#define ASM_MOV_REG_PCREL(as, reg_dest, label) asm_xtensa_mov_reg_pcrel((as), (reg_dest), (label))

#define ASM_LSL_REG_REG(as, reg_dest, reg_shift) \
    do { \
        asm_xtensa_op_ssl((as), (reg_shift)); \
        asm_xtensa_op_sll((as), (reg_dest), (reg_dest)); \
    } while (0)
#define ASM_ASR_REG_REG(as, reg_dest, reg_shift) \
    do { \
        asm_xtensa_op_ssr((as), (reg_shift)); \
        asm_xtensa_op_sra((as), (reg_dest), (reg_dest)); \
    } while (0)
#define ASM_OR_REG_REG(as, reg_dest, reg_src) asm_xtensa_op_or((as), (reg_dest), (reg_dest), (reg_src))
#define ASM_XOR_REG_REG(as, reg_dest, reg_src) asm_xtensa_op_xor((as), (reg_dest), (reg_dest), (reg_src))
#define ASM_AND_REG_REG(as, reg_dest, reg_src) asm_xtensa_op_and((as), (reg_dest), (reg_dest), (reg_src))
#define ASM_ADD_REG_REG(as, reg_dest, reg_src) asm_xtensa_op_add_n((as), (reg_dest), (reg_dest), (reg_src))
#define ASM_SUB_REG_REG(as, reg_dest, reg_src) asm_xtensa_op_sub((as), (reg_dest), (reg_dest), (reg_src))
#define ASM_MUL_REG_REG(as, reg_dest, reg_src) asm_xtensa_op_mull((as), (reg_dest), (reg_dest), (reg_src))

#define ASM_LOAD_REG_REG_OFFSET(as, reg_dest, reg_base, word_offset) asm_xtensa_op_l32i_n((as), (reg_dest), (reg_base), (word_offset))
#define ASM_LOAD8_REG_REG(as, reg_dest, reg_base) asm_xtensa_op_l8ui((as), (reg_dest), (reg_base), 0)
#define ASM_LOAD16_REG_REG(as, reg_dest, reg_base) asm_xtensa_op_l16ui((as), (reg_dest), (reg_base), 0)
#define ASM_LOAD32_REG_REG(as, reg_dest, reg_base) asm_xtensa_op_l32i_n((as), (reg_dest), (reg_base), 0)

#define ASM_STORE_REG_REG_OFFSET(as, reg_dest, reg_base, word_offset) asm_xtensa_op_s32i_n((as), (reg_dest), (reg_base), (word_offset))
#define ASM_STORE8_REG_REG(as, reg_src, reg_base) asm_xtensa_op_s8i((as), (reg_src), (reg_base), 0)
#define ASM_STORE16_REG_REG(as, reg_src, reg_base) asm_xtensa_op_s16i((as), (reg_src), (reg_base), 0)
#define ASM_STORE32_REG_REG(as, reg_src, reg_base) asm_xtensa_op_s32i_n((as), (reg_src), (reg_base), 0)

#endif // GENERIC_ASM_API

#endif // MICROPY_INCLUDED_PY_ASMXTENSA_H
