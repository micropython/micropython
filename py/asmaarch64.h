/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Damien P. George
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
#ifndef MICROPY_INCLUDED_PY_ASMAARCH64_H
#define MICROPY_INCLUDED_PY_ASMAARCH64_H

#include "py/misc.h"
#include "py/asmbase.h"

#define ASM_AARCH64_REG_X0  (0)
#define ASM_AARCH64_REG_X1  (1)
#define ASM_AARCH64_REG_X2  (2)
#define ASM_AARCH64_REG_X3  (3)
#define ASM_AARCH64_REG_X4  (4)
#define ASM_AARCH64_REG_X5  (5)
#define ASM_AARCH64_REG_X6  (6)
#define ASM_AARCH64_REG_X7  (7)
#define ASM_AARCH64_REG_X8  (8)
#define ASM_AARCH64_REG_X9  (9)
#define ASM_AARCH64_REG_X10 (10)
#define ASM_AARCH64_REG_X11 (11)
#define ASM_AARCH64_REG_X12 (12)
#define ASM_AARCH64_REG_X13 (13)
#define ASM_AARCH64_REG_X14 (14)
#define ASM_AARCH64_REG_X15 (15)
#define ASM_AARCH64_REG_X16 (16)
#define ASM_AARCH64_REG_X17 (17)
#define ASM_AARCH64_REG_X18 (18)
#define ASM_AARCH64_REG_X19 (19)
#define ASM_AARCH64_REG_X20 (20)
#define ASM_AARCH64_REG_X21 (21)
#define ASM_AARCH64_REG_X22 (22)
#define ASM_AARCH64_REG_X23 (23)
#define ASM_AARCH64_REG_X24 (24)
#define ASM_AARCH64_REG_X25 (25)
#define ASM_AARCH64_REG_X26 (26)
#define ASM_AARCH64_REG_X27 (27)
#define ASM_AARCH64_REG_X28 (28)
#define ASM_AARCH64_REG_X29 (29)
#define ASM_AARCH64_REG_X30 (30)
#define ASM_AARCH64_REG_SP  (31)
#define ASM_AARCH64_REG_LR  ASM_AARCH64_REG_X30

#define ASM_AARCH64_CC_EQ (0x0)
#define ASM_AARCH64_CC_NE (0x1)
#define ASM_AARCH64_CC_CS (0x2)
#define ASM_AARCH64_CC_CC (0x3)
#define ASM_AARCH64_CC_MI (0x4)
#define ASM_AARCH64_CC_PL (0x5)
#define ASM_AARCH64_CC_VS (0x6)
#define ASM_AARCH64_CC_VC (0x7)
#define ASM_AARCH64_CC_HI (0x8)
#define ASM_AARCH64_CC_LS (0x9)
#define ASM_AARCH64_CC_GE (0xa)
#define ASM_AARCH64_CC_LT (0xb)
#define ASM_AARCH64_CC_GT (0xc)
#define ASM_AARCH64_CC_LE (0xd)
#define ASM_AARCH64_CC_AL (0xe)
#define ASM_AARCH64_CC_NV (0xf)

#define ASM_AARCH64_CC_LO ASM_AARCH64_CC_CC
#define ASM_AARCH64_CC_HS ASM_AARCH64_CC_CS

typedef struct _asm_aarch64_t {
    mp_asm_base_t base;
    uint64_t push_reglist;
    uint stack_adjust;
} asm_aarch64_t;

static inline void asm_aarch64_end_pass(asm_aarch64_t *as) {
    (void)as;
}

void asm_aarch64_entry(asm_aarch64_t *as, int num_locals);
void asm_aarch64_exit(asm_aarch64_t *as);

void asm_aarch64_bkpt(asm_aarch64_t *as);
void asm_aarch64_push(asm_aarch64_t *as, uint64_t reglist);
void asm_aarch64_pop(asm_aarch64_t *as, uint64_t reglist);

void asm_aarch64_mov_reg_reg(asm_aarch64_t *as, uint reg_dest, uint reg_src);
void asm_aarch64_mov_reg_i64_optimised(asm_aarch64_t *as, uint rd, int64_t imm);
void asm_aarch64_mov_local_reg(asm_aarch64_t *as, int local_num, uint rd);
void asm_aarch64_mov_reg_local(asm_aarch64_t *as, uint rd, int local_num);
void asm_aarch64_setcc_reg(asm_aarch64_t *as, uint rd, uint cond);

void asm_aarch64_cmp_reg_i8(asm_aarch64_t *as, uint rd, int imm);
void asm_aarch64_cmp_reg_reg(asm_aarch64_t *as, uint rd, uint rn);

void asm_aarch64_mvn_reg_reg(asm_aarch64_t *as, uint rd, uint rm);
void asm_aarch64_add_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rn, uint rm);
void asm_aarch64_add_reg_reg_imm(asm_aarch64_t *as, uint rd, uint rn, uint imm12);
void asm_aarch64_sub_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rn, uint rm);
void asm_aarch64_sub_reg_reg_imm(asm_aarch64_t *as, uint rd, uint rn, uint imm12);
void asm_aarch64_rsb_reg_reg_imm(asm_aarch64_t *as, uint rd, uint rn, uint imm);
void asm_aarch64_mul_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rn, uint rm);
void asm_aarch64_and_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rn, uint rm);
void asm_aarch64_eor_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rn, uint rm);
void asm_aarch64_orr_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rn, uint rm);
void asm_aarch64_mov_reg_local_addr(asm_aarch64_t *as, uint rd, int local_num);
void asm_aarch64_mov_reg_pcrel(asm_aarch64_t *as, uint reg_dest, uint label);
void asm_aarch64_lsl_reg_reg(asm_aarch64_t *as, uint rd, uint rs);
void asm_aarch64_lsr_reg_reg(asm_aarch64_t *as, uint rd, uint rs);
void asm_aarch64_asr_reg_reg(asm_aarch64_t *as, uint rd, uint rs);
void asm_aarch64_lsl_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rn, uint rm);
void asm_aarch64_lsr_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rn, uint rm);
void asm_aarch64_asr_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rn, uint rm);
void asm_aarch64_lsl_reg_reg_imm(asm_aarch64_t *as, uint rd, uint rn, uint shift);
void asm_aarch64_lsr_reg_reg_imm(asm_aarch64_t *as, uint rd, uint rn, uint shift);
void asm_aarch64_asr_reg_reg_imm(asm_aarch64_t *as, uint rd, uint rn, uint shift);

void asm_aarch64_ldr_reg_reg_offset(asm_aarch64_t *as, uint rd, uint rn, uint byte_offset);
void asm_aarch64_ldr32_reg_reg_offset(asm_aarch64_t *as, uint rd, uint rn, uint byte_offset);
void asm_aarch64_ldrh_reg_reg_offset(asm_aarch64_t *as, uint rd, uint rn, uint byte_offset);
void asm_aarch64_ldrb_reg_reg_offset(asm_aarch64_t *as, uint rd, uint rn, uint byte_offset);
void asm_aarch64_str_reg_reg_offset(asm_aarch64_t *as, uint rd, uint rm, uint byte_offset);
void asm_aarch64_str32_reg_reg_offset(asm_aarch64_t *as, uint rd, uint rm, uint byte_offset);
void asm_aarch64_strh_reg_reg_offset(asm_aarch64_t *as, uint rd, uint rm, uint byte_offset);
void asm_aarch64_strb_reg_reg_offset(asm_aarch64_t *as, uint rd, uint rm, uint byte_offset);

void asm_aarch64_ldr_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rm, uint rn);
void asm_aarch64_ldrh_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rm, uint rn);
void asm_aarch64_ldrb_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rm, uint rn);

void asm_aarch64_str_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rm, uint rn);
void asm_aarch64_strh_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rm, uint rn);
void asm_aarch64_strb_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rm, uint rn);



void asm_aarch64_bcc_label(asm_aarch64_t *as, int cond, uint label);
void asm_aarch64_b_label(asm_aarch64_t *as, uint label);
void asm_aarch64_bl_label(asm_aarch64_t *as, uint label);
void asm_aarch64_bl_ind(asm_aarch64_t *as, uint fun_id, uint reg_temp);
void asm_aarch64_br_reg(asm_aarch64_t *as, uint reg_src);

#define ASM_AARCH64_REG_FUN_TABLE ASM_AARCH64_REG_X28

#if GENERIC_ASM_API

#define ASM_WORD_SIZE (8)

#define REG_RET ASM_AARCH64_REG_X0
#define REG_ARG_1 ASM_AARCH64_REG_X0
#define REG_ARG_2 ASM_AARCH64_REG_X1
#define REG_ARG_3 ASM_AARCH64_REG_X2
#define REG_ARG_4 ASM_AARCH64_REG_X3

#define REG_TEMP0 ASM_AARCH64_REG_X8
#define REG_TEMP1 ASM_AARCH64_REG_X9
#define REG_TEMP2 ASM_AARCH64_REG_X10

#define REG_LOCAL_1 ASM_AARCH64_REG_X19
#define REG_LOCAL_2 ASM_AARCH64_REG_X20
#define REG_LOCAL_3 ASM_AARCH64_REG_X21
#define REG_LOCAL_NUM (3)

#define REG_FUN_TABLE ASM_AARCH64_REG_FUN_TABLE

#define ASM_T                           asm_aarch64_t
#define ASM_END_PASS                    asm_aarch64_end_pass
#define ASM_ENTRY(as, num_locals, name) asm_aarch64_entry((as), (num_locals))
#define ASM_EXIT                        asm_aarch64_exit

#define ASM_JUMP                        asm_aarch64_b_label
#define ASM_JUMP_IF_REG_ZERO(as, reg, label, bool_test) \
    do { \
        asm_aarch64_cmp_reg_i8(as, reg, 0); \
        asm_aarch64_bcc_label(as, ASM_AARCH64_CC_EQ, label); \
    } while (0)
#define ASM_JUMP_IF_REG_NONZERO(as, reg, label, bool_test) \
    do { \
        asm_aarch64_cmp_reg_i8(as, reg, 0); \
        asm_aarch64_bcc_label(as, ASM_AARCH64_CC_NE, label); \
    } while (0)
#define ASM_JUMP_IF_REG_EQ(as, reg1, reg2, label) \
    do { \
        asm_aarch64_cmp_reg_reg(as, reg1, reg2); \
        asm_aarch64_bcc_label(as, ASM_AARCH64_CC_EQ, label); \
    } while (0)
#define ASM_JUMP_REG(as, reg) asm_aarch64_br_reg((as), (reg))
#define ASM_CALL_IND(as, idx) asm_aarch64_bl_ind(as, idx, ASM_AARCH64_REG_X11)

#define ASM_MOV_LOCAL_REG(as, local_num, reg_src) asm_aarch64_mov_local_reg((as), (local_num), (reg_src))
#define ASM_MOV_REG_IMM(as, reg_dest, imm) asm_aarch64_mov_reg_i64_optimised((as), (reg_dest), (imm))
#define ASM_MOV_REG_LOCAL(as, reg_dest, local_num) asm_aarch64_mov_reg_local((as), (reg_dest), (local_num))
#define ASM_MOV_REG_REG(as, reg_dest, reg_src) asm_aarch64_mov_reg_reg((as), (reg_dest), (reg_src))
#define ASM_MOV_REG_LOCAL_ADDR(as, reg_dest, local_num) asm_aarch64_mov_reg_local_addr((as), (reg_dest), (local_num))
#define ASM_MOV_REG_PCREL(as, reg_dest, label) asm_aarch64_mov_reg_pcrel((as), (reg_dest), (label))

#define ASM_NOT_REG(as, reg_dest) asm_aarch64_mvn_reg_reg((as), (reg_dest), (reg_dest))
#define ASM_CLR_REG(as, reg_dest) asm_aarch64_eor_reg_reg_reg((as), (reg_dest), (reg_dest), (reg_dest))
#define ASM_NEG_REG(as, reg_dest) asm_aarch64_rsb_reg_reg_imm((as), (reg_dest), (reg_dest), 0)
#define ASM_LSL_REG_REG(as, reg_dest, reg_shift) asm_aarch64_lsl_reg_reg((as), (reg_dest), (reg_shift))
#define ASM_LSR_REG_REG(as, reg_dest, reg_shift) asm_aarch64_lsr_reg_reg((as), (reg_dest), (reg_shift))
#define ASM_ASR_REG_REG(as, reg_dest, reg_shift) asm_aarch64_asr_reg_reg((as), (reg_dest), (reg_shift))
#define ASM_OR_REG_REG(as, reg_dest, reg_src) asm_aarch64_orr_reg_reg_reg((as), (reg_dest), (reg_dest), (reg_src))
#define ASM_XOR_REG_REG(as, reg_dest, reg_src) asm_aarch64_eor_reg_reg_reg((as), (reg_dest), (reg_dest), (reg_src))
#define ASM_AND_REG_REG(as, reg_dest, reg_src) asm_aarch64_and_reg_reg_reg((as), (reg_dest), (reg_dest), (reg_src))
#define ASM_ADD_REG_REG(as, reg_dest, reg_src) asm_aarch64_add_reg_reg_reg((as), (reg_dest), (reg_dest), (reg_src))
#define ASM_SUB_REG_REG(as, reg_dest, reg_src) asm_aarch64_sub_reg_reg_reg((as), (reg_dest), (reg_dest), (reg_src))
#define ASM_MUL_REG_REG(as, reg_dest, reg_src) asm_aarch64_mul_reg_reg_reg((as), (reg_dest), (reg_dest), (reg_src))

#define ASM_LOAD_REG_REG_OFFSET(as, reg_dest, reg_base, word_offset) asm_aarch64_ldr_reg_reg_offset((as), (reg_dest), (reg_base), 8 * (word_offset))
#define ASM_LOAD8_REG_REG(as, reg_dest, reg_base) ASM_LOAD8_REG_REG_OFFSET((as), (reg_dest), (reg_base), 0)
#define ASM_LOAD8_REG_REG_OFFSET(as, reg_dest, reg_base, byte_offset) asm_aarch64_ldrb_reg_reg_offset((as), (reg_dest), (reg_base), (byte_offset))
#define ASM_LOAD16_REG_REG(as, reg_dest, reg_base) ASM_LOAD16_REG_REG_OFFSET((as), (reg_dest), (reg_base), 0)
#define ASM_LOAD16_REG_REG_OFFSET(as, reg_dest, reg_base, halfword_offset) asm_aarch64_ldrh_reg_reg_offset((as), (reg_dest), (reg_base), 2 * (halfword_offset))
#define ASM_LOAD32_REG_REG(as, reg_dest, reg_base) ASM_LOAD32_REG_REG_OFFSET((as), (reg_dest), (reg_base), 0)
#define ASM_LOAD32_REG_REG_OFFSET(as, reg_dest, reg_base, word_offset) asm_aarch64_ldr32_reg_reg_offset((as), (reg_dest), (reg_base), 4 * (word_offset))

#define ASM_STORE_REG_REG_OFFSET(as, reg_value, reg_base, word_offset) asm_aarch64_str_reg_reg_offset((as), (reg_value), (reg_base), 8 * (word_offset))
#define ASM_STORE8_REG_REG(as, reg_value, reg_base) ASM_STORE8_REG_REG_OFFSET((as), (reg_value), (reg_base), 0)
#define ASM_STORE8_REG_REG_OFFSET(as, reg_value, reg_base, byte_offset) asm_aarch64_strb_reg_reg_offset((as), (reg_value), (reg_base), (byte_offset))
#define ASM_STORE16_REG_REG(as, reg_value, reg_base) ASM_STORE16_REG_REG_OFFSET((as), (reg_value), (reg_base), 0)
#define ASM_STORE16_REG_REG_OFFSET(as, reg_value, reg_base, halfword_offset) asm_aarch64_strh_reg_reg_offset((as), (reg_value), (reg_base), 2 * (halfword_offset))
#define ASM_STORE32_REG_REG(as, reg_value, reg_base) ASM_STORE32_REG_REG_OFFSET((as), (reg_value), (reg_base), 0)
#define ASM_STORE32_REG_REG_OFFSET(as, reg_value, reg_base, word_offset) asm_aarch64_str32_reg_reg_offset((as), (reg_value), (reg_base), 4 * (word_offset))

#define ASM_LOAD8_REG_REG_REG(as, reg_dest, reg_base, reg_index) asm_aarch64_ldrb_reg_reg_reg((as), (reg_dest), (reg_base), (reg_index))
#define ASM_LOAD16_REG_REG_REG(as, reg_dest, reg_base, reg_index) asm_aarch64_ldrh_reg_reg_reg((as), (reg_dest), (reg_base), (reg_index))
#define ASM_LOAD32_REG_REG_REG(as, reg_dest, reg_base, reg_index) asm_aarch64_ldr_reg_reg_reg((as), (reg_dest), (reg_base), (reg_index))
#define ASM_STORE8_REG_REG_REG(as, reg_val, reg_base, reg_index) asm_aarch64_strb_reg_reg_reg((as), (reg_val), (reg_base), (reg_index))
#define ASM_STORE16_REG_REG_REG(as, reg_val, reg_base, reg_index) asm_aarch64_strh_reg_reg_reg((as), (reg_val), (reg_base), (reg_index))
#define ASM_STORE32_REG_REG_REG(as, reg_val, reg_base, reg_index) asm_aarch64_str_reg_reg_reg((as), (reg_val), (reg_base), (reg_index))

#endif // GENERIC_ASM_API

#endif // MICROPY_INCLUDED_PY_ASMAARCH64_H
