/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
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
#ifndef MICROPY_INCLUDED_PY_ASMTHUMB_H
#define MICROPY_INCLUDED_PY_ASMTHUMB_H

#include <assert.h>
#include "py/misc.h"
#include "py/asmbase.h"

#define ASM_THUMB_REG_R0  (0)
#define ASM_THUMB_REG_R1  (1)
#define ASM_THUMB_REG_R2  (2)
#define ASM_THUMB_REG_R3  (3)
#define ASM_THUMB_REG_R4  (4)
#define ASM_THUMB_REG_R5  (5)
#define ASM_THUMB_REG_R6  (6)
#define ASM_THUMB_REG_R7  (7)
#define ASM_THUMB_REG_R8  (8)
#define ASM_THUMB_REG_R9  (9)
#define ASM_THUMB_REG_R10 (10)
#define ASM_THUMB_REG_R11 (11)
#define ASM_THUMB_REG_R12 (12)
#define ASM_THUMB_REG_R13 (13)
#define ASM_THUMB_REG_R14 (14)
#define ASM_THUMB_REG_R15 (15)
#define ASM_THUMB_REG_SP  (ASM_THUMB_REG_R13)
#define ASM_THUMB_REG_LR  (REG_R14)

#define ASM_THUMB_CC_EQ (0x0)
#define ASM_THUMB_CC_NE (0x1)
#define ASM_THUMB_CC_CS (0x2)
#define ASM_THUMB_CC_CC (0x3)
#define ASM_THUMB_CC_MI (0x4)
#define ASM_THUMB_CC_PL (0x5)
#define ASM_THUMB_CC_VS (0x6)
#define ASM_THUMB_CC_VC (0x7)
#define ASM_THUMB_CC_HI (0x8)
#define ASM_THUMB_CC_LS (0x9)
#define ASM_THUMB_CC_GE (0xa)
#define ASM_THUMB_CC_LT (0xb)
#define ASM_THUMB_CC_GT (0xc)
#define ASM_THUMB_CC_LE (0xd)

typedef struct _asm_thumb_t {
    mp_asm_base_t base;
    uint32_t push_reglist;
    uint32_t stack_adjust;
} asm_thumb_t;

static inline void asm_thumb_end_pass(asm_thumb_t *as) {
    (void)as;
}

void asm_thumb_entry(asm_thumb_t *as, int num_locals);
void asm_thumb_exit(asm_thumb_t *as);

// argument order follows ARM, in general dest is first
// note there is a difference between movw and mov.w, and many others!

#define ASM_THUMB_OP_IT (0xbf00)
#define ASM_THUMB_OP_ITE_EQ (0xbf0c)
#define ASM_THUMB_OP_ITE_NE (0xbf14)
#define ASM_THUMB_OP_ITE_CS (0xbf2c)
#define ASM_THUMB_OP_ITE_CC (0xbf34)
#define ASM_THUMB_OP_ITE_MI (0xbf4c)
#define ASM_THUMB_OP_ITE_PL (0xbf54)
#define ASM_THUMB_OP_ITE_VS (0xbf6c)
#define ASM_THUMB_OP_ITE_VC (0xbf74)
#define ASM_THUMB_OP_ITE_HI (0xbf8c)
#define ASM_THUMB_OP_ITE_LS (0xbf94)
#define ASM_THUMB_OP_ITE_GE (0xbfac)
#define ASM_THUMB_OP_ITE_LT (0xbfb4)
#define ASM_THUMB_OP_ITE_GT (0xbfcc)
#define ASM_THUMB_OP_ITE_LE (0xbfd4)

#define ASM_THUMB_OP_NOP        (0xbf00)
#define ASM_THUMB_OP_WFI        (0xbf30)
#define ASM_THUMB_OP_CPSID_I    (0xb672) // cpsid i, disable irq
#define ASM_THUMB_OP_CPSIE_I    (0xb662) // cpsie i, enable irq

void asm_thumb_op16(asm_thumb_t *as, uint op);
void asm_thumb_op32(asm_thumb_t *as, uint op1, uint op2);

static inline void asm_thumb_it_cc(asm_thumb_t *as, uint cc, uint mask) {
    asm_thumb_op16(as, ASM_THUMB_OP_IT | (cc << 4) | mask);
}

// FORMAT 1: move shifted register

#define ASM_THUMB_FORMAT_1_LSL (0x0000)
#define ASM_THUMB_FORMAT_1_LSR (0x0800)
#define ASM_THUMB_FORMAT_1_ASR (0x1000)

#define ASM_THUMB_FORMAT_1_ENCODE(op, rlo_dest, rlo_src, offset) \
    ((op) | ((offset) << 6) | ((rlo_src) << 3) | (rlo_dest))

static inline void asm_thumb_format_1(asm_thumb_t *as, uint op, uint rlo_dest, uint rlo_src, uint offset) {
    assert(rlo_dest < ASM_THUMB_REG_R8);
    assert(rlo_src < ASM_THUMB_REG_R8);
    asm_thumb_op16(as, ASM_THUMB_FORMAT_1_ENCODE(op, rlo_dest, rlo_src, offset));
}

// FORMAT 2: add/subtract

#define ASM_THUMB_FORMAT_2_ADD (0x1800)
#define ASM_THUMB_FORMAT_2_SUB (0x1a00)
#define ASM_THUMB_FORMAT_2_REG_OPERAND (0x0000)
#define ASM_THUMB_FORMAT_2_IMM_OPERAND (0x0400)

#define ASM_THUMB_FORMAT_2_ENCODE(op, rlo_dest, rlo_src, src_b) \
    ((op) | ((src_b) << 6) | ((rlo_src) << 3) | (rlo_dest))

static inline void asm_thumb_format_2(asm_thumb_t *as, uint op, uint rlo_dest, uint rlo_src, int src_b) {
    assert(rlo_dest < ASM_THUMB_REG_R8);
    assert(rlo_src < ASM_THUMB_REG_R8);
    asm_thumb_op16(as, ASM_THUMB_FORMAT_2_ENCODE(op, rlo_dest, rlo_src, src_b));
}

static inline void asm_thumb_add_rlo_rlo_rlo(asm_thumb_t *as, uint rlo_dest, uint rlo_src_a, uint rlo_src_b) {
    asm_thumb_format_2(as, ASM_THUMB_FORMAT_2_ADD | ASM_THUMB_FORMAT_2_REG_OPERAND, rlo_dest, rlo_src_a, rlo_src_b);
}
static inline void asm_thumb_add_rlo_rlo_i3(asm_thumb_t *as, uint rlo_dest, uint rlo_src_a, int i3_src) {
    asm_thumb_format_2(as, ASM_THUMB_FORMAT_2_ADD | ASM_THUMB_FORMAT_2_IMM_OPERAND, rlo_dest, rlo_src_a, i3_src);
}
static inline void asm_thumb_sub_rlo_rlo_rlo(asm_thumb_t *as, uint rlo_dest, uint rlo_src_a, uint rlo_src_b) {
    asm_thumb_format_2(as, ASM_THUMB_FORMAT_2_SUB | ASM_THUMB_FORMAT_2_REG_OPERAND, rlo_dest, rlo_src_a, rlo_src_b);
}
static inline void asm_thumb_sub_rlo_rlo_i3(asm_thumb_t *as, uint rlo_dest, uint rlo_src_a, int i3_src) {
    asm_thumb_format_2(as, ASM_THUMB_FORMAT_2_SUB | ASM_THUMB_FORMAT_2_IMM_OPERAND, rlo_dest, rlo_src_a, i3_src);
}

// FORMAT 3: move/compare/add/subtract immediate
// These instructions all do zero extension of the i8 value

#define ASM_THUMB_FORMAT_3_MOV (0x2000)
#define ASM_THUMB_FORMAT_3_CMP (0x2800)
#define ASM_THUMB_FORMAT_3_ADD (0x3000)
#define ASM_THUMB_FORMAT_3_SUB (0x3800)
#define ASM_THUMB_FORMAT_3_LDR (0x4800)

#define ASM_THUMB_FORMAT_3_ENCODE(op, rlo, i8) ((op) | ((rlo) << 8) | (i8))

static inline void asm_thumb_format_3(asm_thumb_t *as, uint op, uint rlo, int i8) {
    assert(rlo < ASM_THUMB_REG_R8);
    asm_thumb_op16(as, ASM_THUMB_FORMAT_3_ENCODE(op, rlo, i8));
}

static inline void asm_thumb_mov_rlo_i8(asm_thumb_t *as, uint rlo, int i8) {
    asm_thumb_format_3(as, ASM_THUMB_FORMAT_3_MOV, rlo, i8);
}
static inline void asm_thumb_cmp_rlo_i8(asm_thumb_t *as, uint rlo, int i8) {
    asm_thumb_format_3(as, ASM_THUMB_FORMAT_3_CMP, rlo, i8);
}
static inline void asm_thumb_add_rlo_i8(asm_thumb_t *as, uint rlo, int i8) {
    asm_thumb_format_3(as, ASM_THUMB_FORMAT_3_ADD, rlo, i8);
}
static inline void asm_thumb_sub_rlo_i8(asm_thumb_t *as, uint rlo, int i8) {
    asm_thumb_format_3(as, ASM_THUMB_FORMAT_3_SUB, rlo, i8);
}
static inline void asm_thumb_ldr_rlo_pcrel_i8(asm_thumb_t *as, uint rlo, uint i8) {
    asm_thumb_format_3(as, ASM_THUMB_FORMAT_3_LDR, rlo, i8);
}

// FORMAT 4: ALU operations

#define ASM_THUMB_FORMAT_4_AND (0x4000)
#define ASM_THUMB_FORMAT_4_EOR (0x4040)
#define ASM_THUMB_FORMAT_4_LSL (0x4080)
#define ASM_THUMB_FORMAT_4_LSR (0x40c0)
#define ASM_THUMB_FORMAT_4_ASR (0x4100)
#define ASM_THUMB_FORMAT_4_ADC (0x4140)
#define ASM_THUMB_FORMAT_4_SBC (0x4180)
#define ASM_THUMB_FORMAT_4_ROR (0x41c0)
#define ASM_THUMB_FORMAT_4_TST (0x4200)
#define ASM_THUMB_FORMAT_4_NEG (0x4240)
#define ASM_THUMB_FORMAT_4_CMP (0x4280)
#define ASM_THUMB_FORMAT_4_CMN (0x42c0)
#define ASM_THUMB_FORMAT_4_ORR (0x4300)
#define ASM_THUMB_FORMAT_4_MUL (0x4340)
#define ASM_THUMB_FORMAT_4_BIC (0x4380)
#define ASM_THUMB_FORMAT_4_MVN (0x43c0)

void asm_thumb_format_4(asm_thumb_t *as, uint op, uint rlo_dest, uint rlo_src);

static inline void asm_thumb_cmp_rlo_rlo(asm_thumb_t *as, uint rlo_dest, uint rlo_src) {
    asm_thumb_format_4(as, ASM_THUMB_FORMAT_4_CMP, rlo_dest, rlo_src);
}
static inline void asm_thumb_mvn_rlo_rlo(asm_thumb_t *as, uint rlo_dest, uint rlo_src) {
    asm_thumb_format_4(as, ASM_THUMB_FORMAT_4_MVN, rlo_dest, rlo_src);
}
static inline void asm_thumb_neg_rlo_rlo(asm_thumb_t *as, uint rlo_dest, uint rlo_src) {
    asm_thumb_format_4(as, ASM_THUMB_FORMAT_4_NEG, rlo_dest, rlo_src);
}

// FORMAT 5: hi register operations (add, cmp, mov, bx)
// For add/cmp/mov, at least one of the args must be a high register

#define ASM_THUMB_FORMAT_5_ADD (0x4400)
#define ASM_THUMB_FORMAT_5_BX (0x4700)

#define ASM_THUMB_FORMAT_5_ENCODE(op, r_dest, r_src) \
    ((op) | ((r_dest) << 4 & 0x0080) | ((r_src) << 3) | ((r_dest) & 0x0007))

static inline void asm_thumb_format_5(asm_thumb_t *as, uint op, uint r_dest, uint r_src) {
    asm_thumb_op16(as, ASM_THUMB_FORMAT_5_ENCODE(op, r_dest, r_src));
}

static inline void asm_thumb_add_reg_reg(asm_thumb_t *as, uint r_dest, uint r_src) {
    asm_thumb_format_5(as, ASM_THUMB_FORMAT_5_ADD, r_dest, r_src);
}
static inline void asm_thumb_bx_reg(asm_thumb_t *as, uint r_src) {
    asm_thumb_format_5(as, ASM_THUMB_FORMAT_5_BX, 0, r_src);
}

// FORMAT 9: load/store with immediate offset
// For word transfers the offset must be aligned, and >>2

// FORMAT 10: load/store halfword
// The offset must be aligned, and >>1
// The load is zero extended into the register

#define ASM_THUMB_FORMAT_9_STR (0x6000)
#define ASM_THUMB_FORMAT_9_LDR (0x6800)
#define ASM_THUMB_FORMAT_9_WORD_TRANSFER (0x0000)
#define ASM_THUMB_FORMAT_9_BYTE_TRANSFER (0x1000)

#define ASM_THUMB_FORMAT_10_STRH (0x8000)
#define ASM_THUMB_FORMAT_10_LDRH (0x8800)

#define ASM_THUMB_FORMAT_9_10_ENCODE(op, rlo_dest, rlo_base, offset) \
    ((op) | (((offset) << 6) & 0x07c0) | ((rlo_base) << 3) | (rlo_dest))

static inline void asm_thumb_format_9_10(asm_thumb_t *as, uint op, uint rlo_dest, uint rlo_base, uint offset) {
    asm_thumb_op16(as, ASM_THUMB_FORMAT_9_10_ENCODE(op, rlo_dest, rlo_base, offset));
}

static inline void asm_thumb_str_rlo_rlo_i5(asm_thumb_t *as, uint rlo_src, uint rlo_base, uint word_offset) {
    asm_thumb_format_9_10(as, ASM_THUMB_FORMAT_9_STR | ASM_THUMB_FORMAT_9_WORD_TRANSFER, rlo_src, rlo_base, word_offset);
}
static inline void asm_thumb_strb_rlo_rlo_i5(asm_thumb_t *as, uint rlo_src, uint rlo_base, uint byte_offset) {
    asm_thumb_format_9_10(as, ASM_THUMB_FORMAT_9_STR | ASM_THUMB_FORMAT_9_BYTE_TRANSFER, rlo_src, rlo_base, byte_offset);
}
static inline void asm_thumb_strh_rlo_rlo_i5(asm_thumb_t *as, uint rlo_src, uint rlo_base, uint byte_offset) {
    asm_thumb_format_9_10(as, ASM_THUMB_FORMAT_10_STRH, rlo_src, rlo_base, byte_offset);
}
static inline void asm_thumb_ldr_rlo_rlo_i5(asm_thumb_t *as, uint rlo_dest, uint rlo_base, uint word_offset) {
    asm_thumb_format_9_10(as, ASM_THUMB_FORMAT_9_LDR | ASM_THUMB_FORMAT_9_WORD_TRANSFER, rlo_dest, rlo_base, word_offset);
}
static inline void asm_thumb_ldrb_rlo_rlo_i5(asm_thumb_t *as, uint rlo_dest, uint rlo_base, uint byte_offset) {
    asm_thumb_format_9_10(as, ASM_THUMB_FORMAT_9_LDR | ASM_THUMB_FORMAT_9_BYTE_TRANSFER, rlo_dest, rlo_base, byte_offset);
}
static inline void asm_thumb_ldrh_rlo_rlo_i5(asm_thumb_t *as, uint rlo_dest, uint rlo_base, uint byte_offset) {
    asm_thumb_format_9_10(as, ASM_THUMB_FORMAT_10_LDRH, rlo_dest, rlo_base, byte_offset);
}
static inline void asm_thumb_lsl_rlo_rlo_i5(asm_thumb_t *as, uint rlo_dest, uint rlo_src, uint shift) {
    asm_thumb_format_1(as, ASM_THUMB_FORMAT_1_LSL, rlo_dest, rlo_src, shift);
}
static inline void asm_thumb_asr_rlo_rlo_i5(asm_thumb_t *as, uint rlo_dest, uint rlo_src, uint shift) {
    asm_thumb_format_1(as, ASM_THUMB_FORMAT_1_ASR, rlo_dest, rlo_src, shift);
}

// FORMAT 11: sign/zero extend

#define ASM_THUMB_FORMAT_11_ENCODE(op, rlo_dest, rlo_src) \
    ((op) | ((rlo_src) << 3) | (rlo_dest))

#define ASM_THUMB_FORMAT_11_SXTH (0xb200)
#define ASM_THUMB_FORMAT_11_SXTB (0xb240)
#define ASM_THUMB_FORMAT_11_UXTH (0xb280)
#define ASM_THUMB_FORMAT_11_UXTB (0xb2c0)

static inline void asm_thumb_format_11(asm_thumb_t *as, uint op, uint rlo_dest, uint rlo_src) {
    assert(rlo_dest < ASM_THUMB_REG_R8);
    assert(rlo_src < ASM_THUMB_REG_R8);
    asm_thumb_op16(as, ASM_THUMB_FORMAT_11_ENCODE(op, rlo_dest, rlo_src));
}

static inline void asm_thumb_sxth_rlo_rlo(asm_thumb_t *as, uint rlo_dest, uint rlo_src) {
    asm_thumb_format_11(as, ASM_THUMB_FORMAT_11_SXTH, rlo_dest, rlo_src);
}

// TODO convert these to above format style

#define ASM_THUMB_OP_MOVW (0xf240)
#define ASM_THUMB_OP_MOVT (0xf2c0)

void asm_thumb_mov_reg_reg(asm_thumb_t *as, uint reg_dest, uint reg_src);

#if MICROPY_EMIT_THUMB_ARMV7M
size_t asm_thumb_mov_reg_i16(asm_thumb_t *as, uint mov_op, uint reg_dest, int i16_src);
#else
void asm_thumb_mov_rlo_i16(asm_thumb_t *as, uint rlo_dest, int i16_src);
#endif

// these return true if the destination is in range, false otherwise
bool asm_thumb_b_n_label(asm_thumb_t *as, uint label);
bool asm_thumb_bcc_nw_label(asm_thumb_t *as, int cond, uint label, bool wide);
bool asm_thumb_bl_label(asm_thumb_t *as, uint label);

size_t asm_thumb_mov_reg_i32(asm_thumb_t *as, uint reg_dest, mp_uint_t i32_src); // convenience
void asm_thumb_mov_reg_i32_optimised(asm_thumb_t *as, uint reg_dest, int i32_src); // convenience
void asm_thumb_mov_local_reg(asm_thumb_t *as, int local_num_dest, uint rlo_src); // convenience
void asm_thumb_mov_reg_local(asm_thumb_t *as, uint rlo_dest, int local_num); // convenience
void asm_thumb_mov_reg_local_addr(asm_thumb_t *as, uint rlo_dest, int local_num); // convenience
void asm_thumb_mov_reg_pcrel(asm_thumb_t *as, uint rlo_dest, uint label);

void asm_thumb_ldr_reg_reg_i12_optimised(asm_thumb_t *as, uint reg_dest, uint reg_base, uint byte_offset); // convenience

void asm_thumb_b_label(asm_thumb_t *as, uint label); // convenience: picks narrow or wide branch
void asm_thumb_bcc_label(asm_thumb_t *as, int cc, uint label); // convenience: picks narrow or wide branch
void asm_thumb_bl_ind(asm_thumb_t *as, uint fun_id, uint reg_temp); // convenience
void asm_thumb_bcc_rel9(asm_thumb_t *as, int cc, int rel);
void asm_thumb_b_rel12(asm_thumb_t *as, int rel);

// Holds a pointer to mp_fun_table
#define ASM_THUMB_REG_FUN_TABLE ASM_THUMB_REG_R7

#if defined(GENERIC_ASM_API) && GENERIC_ASM_API

// The following macros provide a (mostly) arch-independent API to
// generate native code, and are used by the native emitter.

#define ASM_WORD_SIZE (4)

#define REG_RET ASM_THUMB_REG_R0
#define REG_ARG_1 ASM_THUMB_REG_R0
#define REG_ARG_2 ASM_THUMB_REG_R1
#define REG_ARG_3 ASM_THUMB_REG_R2
#define REG_ARG_4 ASM_THUMB_REG_R3
// rest of args go on stack

#define REG_TEMP0 ASM_THUMB_REG_R0
#define REG_TEMP1 ASM_THUMB_REG_R1
#define REG_TEMP2 ASM_THUMB_REG_R2

#define REG_LOCAL_1 ASM_THUMB_REG_R4
#define REG_LOCAL_2 ASM_THUMB_REG_R5
#define REG_LOCAL_3 ASM_THUMB_REG_R6
#define REG_LOCAL_NUM (3)

#define REG_FUN_TABLE ASM_THUMB_REG_FUN_TABLE

#define ASM_T               asm_thumb_t
#define ASM_END_PASS        asm_thumb_end_pass
#define ASM_ENTRY           asm_thumb_entry
#define ASM_EXIT            asm_thumb_exit

#define ASM_JUMP            asm_thumb_b_label
#define ASM_JUMP_IF_REG_ZERO(as, reg, label, bool_test) \
    do { \
        asm_thumb_cmp_rlo_i8(as, reg, 0); \
        asm_thumb_bcc_label(as, ASM_THUMB_CC_EQ, label); \
    } while (0)
#define ASM_JUMP_IF_REG_NONZERO(as, reg, label, bool_test) \
    do { \
        asm_thumb_cmp_rlo_i8(as, reg, 0); \
        asm_thumb_bcc_label(as, ASM_THUMB_CC_NE, label); \
    } while (0)
#define ASM_JUMP_IF_REG_EQ(as, reg1, reg2, label) \
    do { \
        asm_thumb_cmp_rlo_rlo(as, reg1, reg2); \
        asm_thumb_bcc_label(as, ASM_THUMB_CC_EQ, label); \
    } while (0)
#define ASM_JUMP_REG(as, reg) asm_thumb_bx_reg((as), (reg))
#define ASM_CALL_IND(as, idx) asm_thumb_bl_ind(as, idx, ASM_THUMB_REG_R3)

#define ASM_MOV_LOCAL_REG(as, local_num, reg) asm_thumb_mov_local_reg((as), (local_num), (reg))
#define ASM_MOV_REG_IMM(as, reg_dest, imm) asm_thumb_mov_reg_i32_optimised((as), (reg_dest), (imm))
#if MICROPY_EMIT_THUMB_ARMV7M
#define ASM_MOV_REG_IMM_FIX_U16(as, reg_dest, imm) asm_thumb_mov_reg_i16((as), ASM_THUMB_OP_MOVW, (reg_dest), (imm))
#else
#define ASM_MOV_REG_IMM_FIX_U16(as, reg_dest, imm) asm_thumb_mov_rlo_i16((as), (reg_dest), (imm))
#endif
#define ASM_MOV_REG_IMM_FIX_WORD(as, reg_dest, imm) asm_thumb_mov_reg_i32((as), (reg_dest), (imm))
#define ASM_MOV_REG_LOCAL(as, reg_dest, local_num) asm_thumb_mov_reg_local((as), (reg_dest), (local_num))
#define ASM_MOV_REG_REG(as, reg_dest, reg_src) asm_thumb_mov_reg_reg((as), (reg_dest), (reg_src))
#define ASM_MOV_REG_LOCAL_ADDR(as, reg_dest, local_num) asm_thumb_mov_reg_local_addr((as), (reg_dest), (local_num))
#define ASM_MOV_REG_PCREL(as, rlo_dest, label) asm_thumb_mov_reg_pcrel((as), (rlo_dest), (label))

#define ASM_LSL_REG_REG(as, reg_dest, reg_shift) asm_thumb_format_4((as), ASM_THUMB_FORMAT_4_LSL, (reg_dest), (reg_shift))
#define ASM_LSR_REG_REG(as, reg_dest, reg_shift) asm_thumb_format_4((as), ASM_THUMB_FORMAT_4_LSR, (reg_dest), (reg_shift))
#define ASM_ASR_REG_REG(as, reg_dest, reg_shift) asm_thumb_format_4((as), ASM_THUMB_FORMAT_4_ASR, (reg_dest), (reg_shift))
#define ASM_OR_REG_REG(as, reg_dest, reg_src) asm_thumb_format_4((as), ASM_THUMB_FORMAT_4_ORR, (reg_dest), (reg_src))
#define ASM_XOR_REG_REG(as, reg_dest, reg_src) asm_thumb_format_4((as), ASM_THUMB_FORMAT_4_EOR, (reg_dest), (reg_src))
#define ASM_AND_REG_REG(as, reg_dest, reg_src) asm_thumb_format_4((as), ASM_THUMB_FORMAT_4_AND, (reg_dest), (reg_src))
#define ASM_ADD_REG_REG(as, reg_dest, reg_src) asm_thumb_add_rlo_rlo_rlo((as), (reg_dest), (reg_dest), (reg_src))
#define ASM_SUB_REG_REG(as, reg_dest, reg_src) asm_thumb_sub_rlo_rlo_rlo((as), (reg_dest), (reg_dest), (reg_src))
#define ASM_MUL_REG_REG(as, reg_dest, reg_src) asm_thumb_format_4((as), ASM_THUMB_FORMAT_4_MUL, (reg_dest), (reg_src))

#define ASM_LOAD_REG_REG(as, reg_dest, reg_base) asm_thumb_ldr_rlo_rlo_i5((as), (reg_dest), (reg_base), 0)
#define ASM_LOAD_REG_REG_OFFSET(as, reg_dest, reg_base, word_offset) asm_thumb_ldr_reg_reg_i12_optimised((as), (reg_dest), (reg_base), (word_offset))
#define ASM_LOAD8_REG_REG(as, reg_dest, reg_base) asm_thumb_ldrb_rlo_rlo_i5((as), (reg_dest), (reg_base), 0)
#define ASM_LOAD16_REG_REG(as, reg_dest, reg_base) asm_thumb_ldrh_rlo_rlo_i5((as), (reg_dest), (reg_base), 0)
#define ASM_LOAD32_REG_REG(as, reg_dest, reg_base) asm_thumb_ldr_rlo_rlo_i5((as), (reg_dest), (reg_base), 0)

#define ASM_STORE_REG_REG(as, reg_src, reg_base) asm_thumb_str_rlo_rlo_i5((as), (reg_src), (reg_base), 0)
#define ASM_STORE_REG_REG_OFFSET(as, reg_src, reg_base, word_offset) asm_thumb_str_rlo_rlo_i5((as), (reg_src), (reg_base), (word_offset))
#define ASM_STORE8_REG_REG(as, reg_src, reg_base) asm_thumb_strb_rlo_rlo_i5((as), (reg_src), (reg_base), 0)
#define ASM_STORE16_REG_REG(as, reg_src, reg_base) asm_thumb_strh_rlo_rlo_i5((as), (reg_src), (reg_base), 0)
#define ASM_STORE32_REG_REG(as, reg_src, reg_base) asm_thumb_str_rlo_rlo_i5((as), (reg_src), (reg_base), 0)

#endif // GENERIC_ASM_API

#endif // MICROPY_INCLUDED_PY_ASMTHUMB_H
