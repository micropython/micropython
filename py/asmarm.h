/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#define ASM_ARM_PASS_COMPUTE (1)
#define ASM_ARM_PASS_EMIT    (2)

#define REG_R0  (0)
#define REG_R1  (1)
#define REG_R2  (2)
#define REG_R3  (3)
#define REG_R4  (4)
#define REG_R5  (5)
#define REG_R6  (6)
#define REG_R7  (7)
#define REG_R8  (8)
#define REG_R9  (9)
#define REG_R10 (10)
#define REG_R11 (11)
#define REG_R12 (12)
#define REG_R13 (13)
#define REG_R14 (14)
#define REG_R15 (15)
#define REG_SP  (REG_R13)
#define REG_LR  (REG_R14)
#define REG_PC  (REG_R15)

#define ARM_CC_EQ (0x0 << 28)
#define ARM_CC_NE (0x1 << 28)
#define ARM_CC_CS (0x2 << 28)
#define ARM_CC_CC (0x3 << 28)
#define ARM_CC_MI (0x4 << 28)
#define ARM_CC_PL (0x5 << 28)
#define ARM_CC_VS (0x6 << 28)
#define ARM_CC_VC (0x7 << 28)
#define ARM_CC_HI (0x8 << 28)
#define ARM_CC_LS (0x9 << 28)
#define ARM_CC_GE (0xa << 28)
#define ARM_CC_LT (0xb << 28)
#define ARM_CC_GT (0xc << 28)
#define ARM_CC_LE (0xd << 28)
#define ARM_CC_AL (0xe << 28)

typedef struct _asm_arm_t asm_arm_t;

asm_arm_t *asm_arm_new(uint max_num_labels);
void asm_arm_free(asm_arm_t *as, bool free_code);
void asm_arm_start_pass(asm_arm_t *as, uint pass);
void asm_arm_end_pass(asm_arm_t *as);
uint asm_arm_get_code_size(asm_arm_t *as);
void *asm_arm_get_code(asm_arm_t *as);

void asm_arm_entry(asm_arm_t *as, int num_locals);
void asm_arm_exit(asm_arm_t *as);
void asm_arm_label_assign(asm_arm_t *as, uint label);

void asm_arm_align(asm_arm_t* as, uint align);
void asm_arm_data(asm_arm_t* as, uint bytesize, uint val);

void asm_arm_bkpt(asm_arm_t *as);
void asm_arm_mov_reg_reg(asm_arm_t *as, uint reg_dest, uint reg_src);
void asm_arm_mov_reg_i32(asm_arm_t *as, uint rd, int imm);
void asm_arm_mov_local_reg(asm_arm_t *as, int local_num, uint rd);
void asm_arm_mov_reg_local(asm_arm_t *as, uint rd, int local_num);

void asm_arm_cmp_reg_i8(asm_arm_t *as, uint rd, int imm);
void asm_arm_cmp_reg_reg(asm_arm_t *as, uint rd, uint rn);
void asm_arm_less_op(asm_arm_t *as, uint rd, uint rn);
void asm_arm_add_reg(asm_arm_t *as, uint rd, uint rn, uint rm);
void asm_arm_mov_reg_local_addr(asm_arm_t *as, uint rd, int local_num);

void asm_arm_bcc_label(asm_arm_t *as, int cond, uint label);
void asm_arm_b_label(asm_arm_t *as, uint label);
void asm_arm_bl_ind(asm_arm_t *as, void *fun_ptr, uint fun_id, uint reg_temp);

