/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
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
#ifndef MICROPY_INCLUDED_PY_ASMX86_H
#define MICROPY_INCLUDED_PY_ASMX86_H

#include "py/mpconfig.h"
#include "py/misc.h"
#include "py/asmbase.h"

// x86 cdecl calling convention is:
//  - args passed on the stack in reverse order
//  - return value in EAX
//  - caller cleans up the stack after a call
//  - stack must be aligned to 16-byte boundary before all calls
//  - EAX, ECX, EDX are caller-save
//  - EBX, ESI, EDI, EBP, ESP, EIP are callee-save

// In the functions below, argument order follows x86 docs and generally
// the destination is the first argument.
// NOTE: this is a change from the old convention used in this file and
// some functions still use the old (reverse) convention.

#define ASM_X86_REG_EAX (0)
#define ASM_X86_REG_ECX (1)
#define ASM_X86_REG_EDX (2)
#define ASM_X86_REG_EBX (3)
#define ASM_X86_REG_ESP (4)
#define ASM_X86_REG_EBP (5)
#define ASM_X86_REG_ESI (6)
#define ASM_X86_REG_EDI (7)

// x86 passes values on the stack, but the emitter is register based, so we need
// to define registers that can temporarily hold the function arguments.  They
// need to be defined here so that asm_x86_call_ind can push them onto the stack
// before the call.
#define ASM_X86_REG_ARG_1 ASM_X86_REG_EAX
#define ASM_X86_REG_ARG_2 ASM_X86_REG_ECX
#define ASM_X86_REG_ARG_3 ASM_X86_REG_EDX
#define ASM_X86_REG_ARG_4 ASM_X86_REG_EBX
#define ASM_X86_REG_ARG_5 ASM_X86_REG_ESI

// condition codes, used for jcc and setcc (despite their j-name!)
#define ASM_X86_CC_JB  (0x2) // below, unsigned
#define ASM_X86_CC_JZ  (0x4)
#define ASM_X86_CC_JE  (0x4)
#define ASM_X86_CC_JNZ (0x5)
#define ASM_X86_CC_JNE (0x5)
#define ASM_X86_CC_JL  (0xc) // less, signed
#define ASM_X86_CC_JGE (0xd) // greater or equal, signed
#define ASM_X86_CC_JLE (0xe) // less or equal, signed
#define ASM_X86_CC_JG  (0xf) // greater, signed

typedef struct _asm_x86_t {
    mp_asm_base_t base;
    int num_locals;
} asm_x86_t;

static inline void asm_x86_end_pass(asm_x86_t *as) {
    (void)as;
}

void asm_x86_mov_r32_r32(asm_x86_t* as, int dest_r32, int src_r32);
void asm_x86_mov_i32_to_r32(asm_x86_t *as, int32_t src_i32, int dest_r32);
void asm_x86_mov_i32_to_r32_aligned(asm_x86_t *as, int32_t src_i32, int dest_r32);
void asm_x86_mov_r8_to_mem8(asm_x86_t *as, int src_r32, int dest_r32, int dest_disp);
void asm_x86_mov_r16_to_mem16(asm_x86_t *as, int src_r32, int dest_r32, int dest_disp);
void asm_x86_mov_r32_to_mem32(asm_x86_t *as, int src_r32, int dest_r32, int dest_disp);
void asm_x86_mov_mem8_to_r32zx(asm_x86_t *as, int src_r32, int src_disp, int dest_r32);
void asm_x86_mov_mem16_to_r32zx(asm_x86_t *as, int src_r32, int src_disp, int dest_r32);
void asm_x86_mov_mem32_to_r32(asm_x86_t *as, int src_r32, int src_disp, int dest_r32);
void asm_x86_and_r32_r32(asm_x86_t *as, int dest_r32, int src_r32);
void asm_x86_or_r32_r32(asm_x86_t *as, int dest_r32, int src_r32);
void asm_x86_xor_r32_r32(asm_x86_t *as, int dest_r32, int src_r32);
void asm_x86_shl_r32_cl(asm_x86_t* as, int dest_r32);
void asm_x86_sar_r32_cl(asm_x86_t* as, int dest_r32);
void asm_x86_add_r32_r32(asm_x86_t* as, int dest_r32, int src_r32);
void asm_x86_sub_r32_r32(asm_x86_t* as, int dest_r32, int src_r32);
void asm_x86_mul_r32_r32(asm_x86_t* as, int dest_r32, int src_r32);
void asm_x86_cmp_r32_with_r32(asm_x86_t* as, int src_r32_a, int src_r32_b);
void asm_x86_test_r8_with_r8(asm_x86_t* as, int src_r32_a, int src_r32_b);
void asm_x86_test_r32_with_r32(asm_x86_t* as, int src_r32_a, int src_r32_b);
void asm_x86_setcc_r8(asm_x86_t* as, mp_uint_t jcc_type, int dest_r8);
void asm_x86_jmp_reg(asm_x86_t *as, int src_r86);
void asm_x86_jmp_label(asm_x86_t* as, mp_uint_t label);
void asm_x86_jcc_label(asm_x86_t* as, mp_uint_t jcc_type, mp_uint_t label);
void asm_x86_entry(asm_x86_t* as, int num_locals);
void asm_x86_exit(asm_x86_t* as);
void asm_x86_mov_arg_to_r32(asm_x86_t *as, int src_arg_num, int dest_r32);
void asm_x86_mov_local_to_r32(asm_x86_t* as, int src_local_num, int dest_r32);
void asm_x86_mov_r32_to_local(asm_x86_t* as, int src_r32, int dest_local_num);
void asm_x86_mov_local_addr_to_r32(asm_x86_t* as, int local_num, int dest_r32);
void asm_x86_mov_reg_pcrel(asm_x86_t *as, int dest_r64, mp_uint_t label);
void asm_x86_call_ind(asm_x86_t* as, void* ptr, mp_uint_t n_args, int temp_r32);

#if GENERIC_ASM_API

// The following macros provide a (mostly) arch-independent API to
// generate native code, and are used by the native emitter.

#define ASM_WORD_SIZE (4)

#define REG_RET ASM_X86_REG_EAX
#define REG_ARG_1 ASM_X86_REG_ARG_1
#define REG_ARG_2 ASM_X86_REG_ARG_2
#define REG_ARG_3 ASM_X86_REG_ARG_3
#define REG_ARG_4 ASM_X86_REG_ARG_4
#define REG_ARG_5 ASM_X86_REG_ARG_5

// caller-save, so can be used as temporaries
#define REG_TEMP0 ASM_X86_REG_EAX
#define REG_TEMP1 ASM_X86_REG_ECX
#define REG_TEMP2 ASM_X86_REG_EDX

// callee-save, so can be used as locals
#define REG_LOCAL_1 ASM_X86_REG_EBX
#define REG_LOCAL_2 ASM_X86_REG_ESI
#define REG_LOCAL_3 ASM_X86_REG_EDI
#define REG_LOCAL_NUM (3)

#define ASM_T               asm_x86_t
#define ASM_END_PASS        asm_x86_end_pass
#define ASM_ENTRY           asm_x86_entry
#define ASM_EXIT            asm_x86_exit

#define ASM_JUMP            asm_x86_jmp_label
#define ASM_JUMP_IF_REG_ZERO(as, reg, label, bool_test) \
    do { \
        if (bool_test) { \
            asm_x86_test_r8_with_r8(as, reg, reg); \
        } else { \
            asm_x86_test_r32_with_r32(as, reg, reg); \
        } \
        asm_x86_jcc_label(as, ASM_X86_CC_JZ, label); \
    } while (0)
#define ASM_JUMP_IF_REG_NONZERO(as, reg, label, bool_test) \
    do { \
        if (bool_test) { \
            asm_x86_test_r8_with_r8(as, reg, reg); \
        } else { \
            asm_x86_test_r32_with_r32(as, reg, reg); \
        } \
        asm_x86_jcc_label(as, ASM_X86_CC_JNZ, label); \
    } while (0)
#define ASM_JUMP_IF_REG_EQ(as, reg1, reg2, label) \
    do { \
        asm_x86_cmp_r32_with_r32(as, reg1, reg2); \
        asm_x86_jcc_label(as, ASM_X86_CC_JE, label); \
    } while (0)
#define ASM_JUMP_REG(as, reg) asm_x86_jmp_reg((as), (reg))
#define ASM_CALL_IND(as, ptr, idx) asm_x86_call_ind(as, ptr, mp_f_n_args[idx], ASM_X86_REG_EAX)

#define ASM_MOV_LOCAL_REG(as, local_num, reg_src) asm_x86_mov_r32_to_local((as), (reg_src), (local_num))
#define ASM_MOV_REG_IMM(as, reg_dest, imm) asm_x86_mov_i32_to_r32((as), (imm), (reg_dest))
#define ASM_MOV_REG_ALIGNED_IMM(as, reg_dest, imm) asm_x86_mov_i32_to_r32_aligned((as), (imm), (reg_dest))
#define ASM_MOV_REG_LOCAL(as, reg_dest, local_num) asm_x86_mov_local_to_r32((as), (local_num), (reg_dest))
#define ASM_MOV_REG_REG(as, reg_dest, reg_src) asm_x86_mov_r32_r32((as), (reg_dest), (reg_src))
#define ASM_MOV_REG_LOCAL_ADDR(as, reg_dest, local_num) asm_x86_mov_local_addr_to_r32((as), (local_num), (reg_dest))
#define ASM_MOV_REG_PCREL(as, reg_dest, label) asm_x86_mov_reg_pcrel((as), (reg_dest), (label))

#define ASM_LSL_REG(as, reg) asm_x86_shl_r32_cl((as), (reg))
#define ASM_ASR_REG(as, reg) asm_x86_sar_r32_cl((as), (reg))
#define ASM_OR_REG_REG(as, reg_dest, reg_src) asm_x86_or_r32_r32((as), (reg_dest), (reg_src))
#define ASM_XOR_REG_REG(as, reg_dest, reg_src) asm_x86_xor_r32_r32((as), (reg_dest), (reg_src))
#define ASM_AND_REG_REG(as, reg_dest, reg_src) asm_x86_and_r32_r32((as), (reg_dest), (reg_src))
#define ASM_ADD_REG_REG(as, reg_dest, reg_src) asm_x86_add_r32_r32((as), (reg_dest), (reg_src))
#define ASM_SUB_REG_REG(as, reg_dest, reg_src) asm_x86_sub_r32_r32((as), (reg_dest), (reg_src))
#define ASM_MUL_REG_REG(as, reg_dest, reg_src) asm_x86_mul_r32_r32((as), (reg_dest), (reg_src))

#define ASM_LOAD_REG_REG(as, reg_dest, reg_base) asm_x86_mov_mem32_to_r32((as), (reg_base), 0, (reg_dest))
#define ASM_LOAD_REG_REG_OFFSET(as, reg_dest, reg_base, word_offset) asm_x86_mov_mem32_to_r32((as), (reg_base), 4 * (word_offset), (reg_dest))
#define ASM_LOAD8_REG_REG(as, reg_dest, reg_base) asm_x86_mov_mem8_to_r32zx((as), (reg_base), 0, (reg_dest))
#define ASM_LOAD16_REG_REG(as, reg_dest, reg_base) asm_x86_mov_mem16_to_r32zx((as), (reg_base), 0, (reg_dest))
#define ASM_LOAD32_REG_REG(as, reg_dest, reg_base) asm_x86_mov_mem32_to_r32((as), (reg_base), 0, (reg_dest))

#define ASM_STORE_REG_REG(as, reg_src, reg_base) asm_x86_mov_r32_to_mem32((as), (reg_src), (reg_base), 0)
#define ASM_STORE_REG_REG_OFFSET(as, reg_src, reg_base, word_offset) asm_x86_mov_r32_to_mem32((as), (reg_src), (reg_base), 4 * (word_offset))
#define ASM_STORE8_REG_REG(as, reg_src, reg_base) asm_x86_mov_r8_to_mem8((as), (reg_src), (reg_base), 0)
#define ASM_STORE16_REG_REG(as, reg_src, reg_base) asm_x86_mov_r16_to_mem16((as), (reg_src), (reg_base), 0)
#define ASM_STORE32_REG_REG(as, reg_src, reg_base) asm_x86_mov_r32_to_mem32((as), (reg_src), (reg_base), 0)

#endif // GENERIC_ASM_API

#endif // MICROPY_INCLUDED_PY_ASMX86_H
