/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Fabian Vogt
 * Copyright (c) 2013, 2014 Damien P. George
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
#ifndef MICROPY_INCLUDED_PY_ASMARM_H
#define MICROPY_INCLUDED_PY_ASMARM_H

#include "py/misc.h"
#include "py/asmbase.h"

#define ASM_ARM_REG_R0  (0)
#define ASM_ARM_REG_R1  (1)
#define ASM_ARM_REG_R2  (2)
#define ASM_ARM_REG_R3  (3)
#define ASM_ARM_REG_R4  (4)
#define ASM_ARM_REG_R5  (5)
#define ASM_ARM_REG_R6  (6)
#define ASM_ARM_REG_R7  (7)
#define ASM_ARM_REG_R8  (8)
#define ASM_ARM_REG_R9  (9)
#define ASM_ARM_REG_R10 (10)
#define ASM_ARM_REG_R11 (11)
#define ASM_ARM_REG_R12 (12)
#define ASM_ARM_REG_R13 (13)
#define ASM_ARM_REG_R14 (14)
#define ASM_ARM_REG_R15 (15)
#define ASM_ARM_REG_SP  (ASM_ARM_REG_R13)
#define ASM_ARM_REG_LR  (ASM_ARM_REG_R14)
#define ASM_ARM_REG_PC  (ASM_ARM_REG_R15)

#define ASM_ARM_CC_EQ (0x0 << 28)
#define ASM_ARM_CC_NE (0x1 << 28)
#define ASM_ARM_CC_CS (0x2 << 28)
#define ASM_ARM_CC_CC (0x3 << 28)
#define ASM_ARM_CC_MI (0x4 << 28)
#define ASM_ARM_CC_PL (0x5 << 28)
#define ASM_ARM_CC_VS (0x6 << 28)
#define ASM_ARM_CC_VC (0x7 << 28)
#define ASM_ARM_CC_HI (0x8 << 28)
#define ASM_ARM_CC_LS (0x9 << 28)
#define ASM_ARM_CC_GE (0xa << 28)
#define ASM_ARM_CC_LT (0xb << 28)
#define ASM_ARM_CC_GT (0xc << 28)
#define ASM_ARM_CC_LE (0xd << 28)
#define ASM_ARM_CC_AL (0xe << 28)

typedef struct _asm_arm_t {
    mp_asm_base_t base;
    uint push_reglist;
    uint stack_adjust;
} asm_arm_t;

static inline void asm_arm_end_pass(asm_arm_t *as) {
    (void)as;
}

void asm_arm_entry(asm_arm_t *as, int num_locals);
void asm_arm_exit(asm_arm_t *as);

void asm_arm_bkpt(asm_arm_t *as);

// mov
void asm_arm_mov_reg_reg(asm_arm_t *as, uint reg_dest, uint reg_src);
size_t asm_arm_mov_reg_i32(asm_arm_t *as, uint rd, int imm);
void asm_arm_mov_reg_i32_optimised(asm_arm_t *as, uint rd, int imm);
void asm_arm_mov_local_reg(asm_arm_t *as, int local_num, uint rd);
void asm_arm_mov_reg_local(asm_arm_t *as, uint rd, int local_num);
void asm_arm_setcc_reg(asm_arm_t *as, uint rd, uint cond);

// compare
void asm_arm_cmp_reg_i8(asm_arm_t *as, uint rd, int imm);
void asm_arm_cmp_reg_reg(asm_arm_t *as, uint rd, uint rn);

// arithmetic
void asm_arm_add_reg_reg_reg(asm_arm_t *as, uint rd, uint rn, uint rm);
void asm_arm_sub_reg_reg_reg(asm_arm_t *as, uint rd, uint rn, uint rm);
void asm_arm_mul_reg_reg_reg(asm_arm_t *as, uint rd, uint rn, uint rm);
void asm_arm_and_reg_reg_reg(asm_arm_t *as, uint rd, uint rn, uint rm);
void asm_arm_eor_reg_reg_reg(asm_arm_t *as, uint rd, uint rn, uint rm);
void asm_arm_orr_reg_reg_reg(asm_arm_t *as, uint rd, uint rn, uint rm);
void asm_arm_mov_reg_local_addr(asm_arm_t *as, uint rd, int local_num);
void asm_arm_mov_reg_pcrel(asm_arm_t *as, uint reg_dest, uint label);
void asm_arm_lsl_reg_reg(asm_arm_t *as, uint rd, uint rs);
void asm_arm_lsr_reg_reg(asm_arm_t *as, uint rd, uint rs);
void asm_arm_asr_reg_reg(asm_arm_t *as, uint rd, uint rs);

// memory
void asm_arm_ldr_reg_reg(asm_arm_t *as, uint rd, uint rn, uint byte_offset);
void asm_arm_ldrh_reg_reg(asm_arm_t *as, uint rd, uint rn);
void asm_arm_ldrh_reg_reg_offset(asm_arm_t *as, uint rd, uint rn, uint byte_offset);
void asm_arm_ldrb_reg_reg(asm_arm_t *as, uint rd, uint rn);
void asm_arm_str_reg_reg(asm_arm_t *as, uint rd, uint rm, uint byte_offset);
void asm_arm_strh_reg_reg(asm_arm_t *as, uint rd, uint rm);
void asm_arm_strb_reg_reg(asm_arm_t *as, uint rd, uint rm);
// store to array
void asm_arm_str_reg_reg_reg(asm_arm_t *as, uint rd, uint rm, uint rn);
void asm_arm_strh_reg_reg_reg(asm_arm_t *as, uint rd, uint rm, uint rn);
void asm_arm_strb_reg_reg_reg(asm_arm_t *as, uint rd, uint rm, uint rn);

// stack
void asm_arm_push(asm_arm_t *as, uint reglist);
void asm_arm_pop(asm_arm_t *as, uint reglist);

// control flow
void asm_arm_bcc_label(asm_arm_t *as, int cond, uint label);
void asm_arm_b_label(asm_arm_t *as, uint label);
void asm_arm_bl_ind(asm_arm_t *as, uint fun_id, uint reg_temp);
void asm_arm_bx_reg(asm_arm_t *as, uint reg_src);

// Holds a pointer to mp_fun_table
#define ASM_ARM_REG_FUN_TABLE ASM_ARM_REG_R7

#if GENERIC_ASM_API

// The following macros provide a (mostly) arch-independent API to
// generate native code, and are used by the native emitter.

#define ASM_WORD_SIZE (4)

#define REG_RET ASM_ARM_REG_R0
#define REG_ARG_1 ASM_ARM_REG_R0
#define REG_ARG_2 ASM_ARM_REG_R1
#define REG_ARG_3 ASM_ARM_REG_R2
#define REG_ARG_4 ASM_ARM_REG_R3

#define REG_TEMP0 ASM_ARM_REG_R0
#define REG_TEMP1 ASM_ARM_REG_R1
#define REG_TEMP2 ASM_ARM_REG_R2

#define REG_LOCAL_1 ASM_ARM_REG_R4
#define REG_LOCAL_2 ASM_ARM_REG_R5
#define REG_LOCAL_3 ASM_ARM_REG_R6
#define REG_LOCAL_NUM (3)

// Holds a pointer to mp_fun_table
#define REG_FUN_TABLE ASM_ARM_REG_FUN_TABLE

#define ASM_T               asm_arm_t
#define ASM_END_PASS        asm_arm_end_pass
#define ASM_ENTRY           asm_arm_entry
#define ASM_EXIT            asm_arm_exit

#define ASM_JUMP            asm_arm_b_label
#define ASM_JUMP_IF_REG_ZERO(as, reg, label, bool_test) \
    do { \
        asm_arm_cmp_reg_i8(as, reg, 0); \
        asm_arm_bcc_label(as, ASM_ARM_CC_EQ, label); \
    } while (0)
#define ASM_JUMP_IF_REG_NONZERO(as, reg, label, bool_test) \
    do { \
        asm_arm_cmp_reg_i8(as, reg, 0); \
        asm_arm_bcc_label(as, ASM_ARM_CC_NE, label); \
    } while (0)
#define ASM_JUMP_IF_REG_EQ(as, reg1, reg2, label) \
    do { \
        asm_arm_cmp_reg_reg(as, reg1, reg2); \
        asm_arm_bcc_label(as, ASM_ARM_CC_EQ, label); \
    } while (0)
#define ASM_JUMP_REG(as, reg) asm_arm_bx_reg((as), (reg))
#define ASM_CALL_IND(as, idx) asm_arm_bl_ind(as, idx, ASM_ARM_REG_R3)

#define ASM_MOV_LOCAL_REG(as, local_num, reg_src) asm_arm_mov_local_reg((as), (local_num), (reg_src))
#define ASM_MOV_REG_IMM(as, reg_dest, imm) asm_arm_mov_reg_i32_optimised((as), (reg_dest), (imm))
#define ASM_MOV_REG_IMM_FIX_U16(as, reg_dest, imm) asm_arm_mov_reg_i32((as), (reg_dest), (imm))
#define ASM_MOV_REG_IMM_FIX_WORD(as, reg_dest, imm) asm_arm_mov_reg_i32((as), (reg_dest), (imm))
#define ASM_MOV_REG_LOCAL(as, reg_dest, local_num) asm_arm_mov_reg_local((as), (reg_dest), (local_num))
#define ASM_MOV_REG_REG(as, reg_dest, reg_src) asm_arm_mov_reg_reg((as), (reg_dest), (reg_src))
#define ASM_MOV_REG_LOCAL_ADDR(as, reg_dest, local_num) asm_arm_mov_reg_local_addr((as), (reg_dest), (local_num))
#define ASM_MOV_REG_PCREL(as, reg_dest, label) asm_arm_mov_reg_pcrel((as), (reg_dest), (label))

#define ASM_LSL_REG_REG(as, reg_dest, reg_shift) asm_arm_lsl_reg_reg((as), (reg_dest), (reg_shift))
#define ASM_LSR_REG_REG(as, reg_dest, reg_shift) asm_arm_lsr_reg_reg((as), (reg_dest), (reg_shift))
#define ASM_ASR_REG_REG(as, reg_dest, reg_shift) asm_arm_asr_reg_reg((as), (reg_dest), (reg_shift))
#define ASM_OR_REG_REG(as, reg_dest, reg_src) asm_arm_orr_reg_reg_reg((as), (reg_dest), (reg_dest), (reg_src))
#define ASM_XOR_REG_REG(as, reg_dest, reg_src) asm_arm_eor_reg_reg_reg((as), (reg_dest), (reg_dest), (reg_src))
#define ASM_AND_REG_REG(as, reg_dest, reg_src) asm_arm_and_reg_reg_reg((as), (reg_dest), (reg_dest), (reg_src))
#define ASM_ADD_REG_REG(as, reg_dest, reg_src) asm_arm_add_reg_reg_reg((as), (reg_dest), (reg_dest), (reg_src))
#define ASM_SUB_REG_REG(as, reg_dest, reg_src) asm_arm_sub_reg_reg_reg((as), (reg_dest), (reg_dest), (reg_src))
#define ASM_MUL_REG_REG(as, reg_dest, reg_src) asm_arm_mul_reg_reg_reg((as), (reg_dest), (reg_dest), (reg_src))

#define ASM_LOAD_REG_REG(as, reg_dest, reg_base) asm_arm_ldr_reg_reg((as), (reg_dest), (reg_base), 0)
#define ASM_LOAD_REG_REG_OFFSET(as, reg_dest, reg_base, word_offset) asm_arm_ldr_reg_reg((as), (reg_dest), (reg_base), 4 * (word_offset))
#define ASM_LOAD8_REG_REG(as, reg_dest, reg_base) asm_arm_ldrb_reg_reg((as), (reg_dest), (reg_base))
#define ASM_LOAD16_REG_REG(as, reg_dest, reg_base) asm_arm_ldrh_reg_reg((as), (reg_dest), (reg_base))
#define ASM_LOAD16_REG_REG_OFFSET(as, reg_dest, reg_base, uint16_offset) asm_arm_ldrh_reg_reg_offset((as), (reg_dest), (reg_base), 2 * (uint16_offset))
#define ASM_LOAD32_REG_REG(as, reg_dest, reg_base) asm_arm_ldr_reg_reg((as), (reg_dest), (reg_base), 0)

#define ASM_STORE_REG_REG(as, reg_value, reg_base) asm_arm_str_reg_reg((as), (reg_value), (reg_base), 0)
#define ASM_STORE_REG_REG_OFFSET(as, reg_dest, reg_base, word_offset) asm_arm_str_reg_reg((as), (reg_dest), (reg_base), 4 * (word_offset))
#define ASM_STORE8_REG_REG(as, reg_value, reg_base) asm_arm_strb_reg_reg((as), (reg_value), (reg_base))
#define ASM_STORE16_REG_REG(as, reg_value, reg_base) asm_arm_strh_reg_reg((as), (reg_value), (reg_base))
#define ASM_STORE32_REG_REG(as, reg_value, reg_base) asm_arm_str_reg_reg((as), (reg_value), (reg_base), 0)

#endif // GENERIC_ASM_API

#endif // MICROPY_INCLUDED_PY_ASMARM_H
