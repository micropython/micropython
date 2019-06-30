/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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
#ifndef MICROPY_INCLUDED_PY_ASMX64_H
#define MICROPY_INCLUDED_PY_ASMX64_H

#include "py/mpconfig.h"
#include "py/misc.h"
#include "py/asmbase.h"

// AMD64 calling convention is:
//  - args pass in: RDI, RSI, RDX, RCX, R08, R09
//  - return value in RAX
//  - stack must be aligned on a 16-byte boundary before all calls
//  - RAX, RCX, RDX, RSI, RDI, R08, R09, R10, R11 are caller-save
//  - RBX, RBP, R12, R13, R14, R15 are callee-save

// In the functions below, argument order follows x86 docs and generally
// the destination is the first argument.
// NOTE: this is a change from the old convention used in this file and
// some functions still use the old (reverse) convention.

#define ASM_X64_REG_RAX (0)
#define ASM_X64_REG_RCX (1)
#define ASM_X64_REG_RDX (2)
#define ASM_X64_REG_RBX (3)
#define ASM_X64_REG_RSP (4)
#define ASM_X64_REG_RBP (5)
#define ASM_X64_REG_RSI (6)
#define ASM_X64_REG_RDI (7)
#define ASM_X64_REG_R08 (8)
#define ASM_X64_REG_R09 (9)
#define ASM_X64_REG_R10 (10)
#define ASM_X64_REG_R11 (11)
#define ASM_X64_REG_R12 (12)
#define ASM_X64_REG_R13 (13)
#define ASM_X64_REG_R14 (14)
#define ASM_X64_REG_R15 (15)

// condition codes, used for jcc and setcc (despite their j-name!)
#define ASM_X64_CC_JB  (0x2) // below, unsigned
#define ASM_X64_CC_JZ  (0x4)
#define ASM_X64_CC_JE  (0x4)
#define ASM_X64_CC_JNZ (0x5)
#define ASM_X64_CC_JNE (0x5)
#define ASM_X64_CC_JL  (0xc) // less, signed
#define ASM_X64_CC_JGE (0xd) // greater or equal, signed
#define ASM_X64_CC_JLE (0xe) // less or equal, signed
#define ASM_X64_CC_JG  (0xf) // greater, signed

typedef struct _asm_x64_t {
    mp_asm_base_t base;
    int num_locals;
} asm_x64_t;

static inline void asm_x64_end_pass(asm_x64_t *as) {
    (void)as;
}

void asm_x64_nop(asm_x64_t* as);
void asm_x64_push_r64(asm_x64_t* as, int src_r64);
void asm_x64_pop_r64(asm_x64_t* as, int dest_r64);
void asm_x64_mov_r64_r64(asm_x64_t* as, int dest_r64, int src_r64);
size_t asm_x64_mov_i32_to_r64(asm_x64_t *as, int src_i32, int dest_r64);
void asm_x64_mov_i64_to_r64(asm_x64_t* as, int64_t src_i64, int dest_r64);
void asm_x64_mov_i64_to_r64_optimised(asm_x64_t *as, int64_t src_i64, int dest_r64);
void asm_x64_mov_r8_to_mem8(asm_x64_t *as, int src_r64, int dest_r64, int dest_disp);
void asm_x64_mov_r16_to_mem16(asm_x64_t *as, int src_r64, int dest_r64, int dest_disp);
void asm_x64_mov_r32_to_mem32(asm_x64_t *as, int src_r64, int dest_r64, int dest_disp);
void asm_x64_mov_r64_to_mem64(asm_x64_t *as, int src_r64, int dest_r64, int dest_disp);
void asm_x64_mov_mem8_to_r64zx(asm_x64_t *as, int src_r64, int src_disp, int dest_r64);
void asm_x64_mov_mem16_to_r64zx(asm_x64_t *as, int src_r64, int src_disp, int dest_r64);
void asm_x64_mov_mem32_to_r64zx(asm_x64_t *as, int src_r64, int src_disp, int dest_r64);
void asm_x64_mov_mem64_to_r64(asm_x64_t *as, int src_r64, int src_disp, int dest_r64);
void asm_x64_and_r64_r64(asm_x64_t *as, int dest_r64, int src_r64);
void asm_x64_or_r64_r64(asm_x64_t *as, int dest_r64, int src_r64);
void asm_x64_xor_r64_r64(asm_x64_t *as, int dest_r64, int src_r64);
void asm_x64_shl_r64_cl(asm_x64_t* as, int dest_r64);
void asm_x64_sar_r64_cl(asm_x64_t* as, int dest_r64);
void asm_x64_add_r64_r64(asm_x64_t* as, int dest_r64, int src_r64);
void asm_x64_sub_r64_r64(asm_x64_t* as, int dest_r64, int src_r64);
void asm_x64_mul_r64_r64(asm_x64_t* as, int dest_r64, int src_r64);
void asm_x64_cmp_r64_with_r64(asm_x64_t* as, int src_r64_a, int src_r64_b);
void asm_x64_test_r8_with_r8(asm_x64_t* as, int src_r64_a, int src_r64_b);
void asm_x64_test_r64_with_r64(asm_x64_t *as, int src_r64_a, int src_r64_b);
void asm_x64_setcc_r8(asm_x64_t* as, int jcc_type, int dest_r8);
void asm_x64_jmp_reg(asm_x64_t *as, int src_r64);
void asm_x64_jmp_label(asm_x64_t* as, mp_uint_t label);
void asm_x64_jcc_label(asm_x64_t* as, int jcc_type, mp_uint_t label);
void asm_x64_entry(asm_x64_t* as, int num_locals);
void asm_x64_exit(asm_x64_t* as);
void asm_x64_mov_local_to_r64(asm_x64_t* as, int src_local_num, int dest_r64);
void asm_x64_mov_r64_to_local(asm_x64_t* as, int src_r64, int dest_local_num);
void asm_x64_mov_local_addr_to_r64(asm_x64_t* as, int local_num, int dest_r64);
void asm_x64_mov_reg_pcrel(asm_x64_t *as, int dest_r64, mp_uint_t label);
void asm_x64_call_ind(asm_x64_t* as, size_t fun_id, int temp_r32);

// Holds a pointer to mp_fun_table
#define ASM_X64_REG_FUN_TABLE ASM_X64_REG_RBP

#if GENERIC_ASM_API

// The following macros provide a (mostly) arch-independent API to
// generate native code, and are used by the native emitter.

#define ASM_WORD_SIZE (8)

#define REG_RET ASM_X64_REG_RAX
#define REG_ARG_1 ASM_X64_REG_RDI
#define REG_ARG_2 ASM_X64_REG_RSI
#define REG_ARG_3 ASM_X64_REG_RDX
#define REG_ARG_4 ASM_X64_REG_RCX
#define REG_ARG_5 ASM_X64_REG_R08

// caller-save
#define REG_TEMP0 ASM_X64_REG_RAX
#define REG_TEMP1 ASM_X64_REG_RDI
#define REG_TEMP2 ASM_X64_REG_RSI

// callee-save
#define REG_LOCAL_1 ASM_X64_REG_RBX
#define REG_LOCAL_2 ASM_X64_REG_R12
#define REG_LOCAL_3 ASM_X64_REG_R13
#define REG_LOCAL_NUM (3)

// Holds a pointer to mp_fun_table
#define REG_FUN_TABLE ASM_X64_REG_FUN_TABLE

#define ASM_T               asm_x64_t
#define ASM_END_PASS        asm_x64_end_pass
#define ASM_ENTRY           asm_x64_entry
#define ASM_EXIT            asm_x64_exit

#define ASM_JUMP            asm_x64_jmp_label
#define ASM_JUMP_IF_REG_ZERO(as, reg, label, bool_test) \
    do { \
        if (bool_test) { \
            asm_x64_test_r8_with_r8((as), (reg), (reg)); \
        } else { \
            asm_x64_test_r64_with_r64((as), (reg), (reg)); \
        } \
        asm_x64_jcc_label(as, ASM_X64_CC_JZ, label); \
    } while (0)
#define ASM_JUMP_IF_REG_NONZERO(as, reg, label, bool_test) \
    do { \
        if (bool_test) { \
            asm_x64_test_r8_with_r8((as), (reg), (reg)); \
        } else { \
            asm_x64_test_r64_with_r64((as), (reg), (reg)); \
        } \
        asm_x64_jcc_label(as, ASM_X64_CC_JNZ, label); \
    } while (0)
#define ASM_JUMP_IF_REG_EQ(as, reg1, reg2, label) \
    do { \
        asm_x64_cmp_r64_with_r64(as, reg1, reg2); \
        asm_x64_jcc_label(as, ASM_X64_CC_JE, label); \
    } while (0)
#define ASM_JUMP_REG(as, reg) asm_x64_jmp_reg((as), (reg))
#define ASM_CALL_IND(as, idx) asm_x64_call_ind(as, idx, ASM_X64_REG_RAX)

#define ASM_MOV_LOCAL_REG(as, local_num, reg_src) asm_x64_mov_r64_to_local((as), (reg_src), (local_num))
#define ASM_MOV_REG_IMM(as, reg_dest, imm) asm_x64_mov_i64_to_r64_optimised((as), (imm), (reg_dest))
#define ASM_MOV_REG_IMM_FIX_U16(as, reg_dest, imm) asm_x64_mov_i32_to_r64((as), (imm), (reg_dest))
#define ASM_MOV_REG_IMM_FIX_WORD(as, reg_dest, imm) asm_x64_mov_i32_to_r64((as), (imm), (reg_dest))
#define ASM_MOV_REG_LOCAL(as, reg_dest, local_num) asm_x64_mov_local_to_r64((as), (local_num), (reg_dest))
#define ASM_MOV_REG_REG(as, reg_dest, reg_src) asm_x64_mov_r64_r64((as), (reg_dest), (reg_src))
#define ASM_MOV_REG_LOCAL_ADDR(as, reg_dest, local_num) asm_x64_mov_local_addr_to_r64((as), (local_num), (reg_dest))
#define ASM_MOV_REG_PCREL(as, reg_dest, label) asm_x64_mov_reg_pcrel((as), (reg_dest), (label))

#define ASM_LSL_REG(as, reg) asm_x64_shl_r64_cl((as), (reg))
#define ASM_ASR_REG(as, reg) asm_x64_sar_r64_cl((as), (reg))
#define ASM_OR_REG_REG(as, reg_dest, reg_src) asm_x64_or_r64_r64((as), (reg_dest), (reg_src))
#define ASM_XOR_REG_REG(as, reg_dest, reg_src) asm_x64_xor_r64_r64((as), (reg_dest), (reg_src))
#define ASM_AND_REG_REG(as, reg_dest, reg_src) asm_x64_and_r64_r64((as), (reg_dest), (reg_src))
#define ASM_ADD_REG_REG(as, reg_dest, reg_src) asm_x64_add_r64_r64((as), (reg_dest), (reg_src))
#define ASM_SUB_REG_REG(as, reg_dest, reg_src) asm_x64_sub_r64_r64((as), (reg_dest), (reg_src))
#define ASM_MUL_REG_REG(as, reg_dest, reg_src) asm_x64_mul_r64_r64((as), (reg_dest), (reg_src))

#define ASM_LOAD_REG_REG(as, reg_dest, reg_base) asm_x64_mov_mem64_to_r64((as), (reg_base), 0, (reg_dest))
#define ASM_LOAD_REG_REG_OFFSET(as, reg_dest, reg_base, word_offset) asm_x64_mov_mem64_to_r64((as), (reg_base), 8 * (word_offset), (reg_dest))
#define ASM_LOAD8_REG_REG(as, reg_dest, reg_base) asm_x64_mov_mem8_to_r64zx((as), (reg_base), 0, (reg_dest))
#define ASM_LOAD16_REG_REG(as, reg_dest, reg_base) asm_x64_mov_mem16_to_r64zx((as), (reg_base), 0, (reg_dest))
#define ASM_LOAD32_REG_REG(as, reg_dest, reg_base) asm_x64_mov_mem32_to_r64zx((as), (reg_base), 0, (reg_dest))

#define ASM_STORE_REG_REG(as, reg_src, reg_base) asm_x64_mov_r64_to_mem64((as), (reg_src), (reg_base), 0)
#define ASM_STORE_REG_REG_OFFSET(as, reg_src, reg_base, word_offset) asm_x64_mov_r64_to_mem64((as), (reg_src), (reg_base), 8 * (word_offset))
#define ASM_STORE8_REG_REG(as, reg_src, reg_base) asm_x64_mov_r8_to_mem8((as), (reg_src), (reg_base), 0)
#define ASM_STORE16_REG_REG(as, reg_src, reg_base) asm_x64_mov_r16_to_mem16((as), (reg_src), (reg_base), 0)
#define ASM_STORE32_REG_REG(as, reg_src, reg_base) asm_x64_mov_r32_to_mem32((as), (reg_src), (reg_base), 0)

#endif // GENERIC_ASM_API

#endif // MICROPY_INCLUDED_PY_ASMX64_H
