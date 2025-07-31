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

#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "py/mpconfig.h"

// wrapper around everything in this file
#if MICROPY_EMIT_THUMB || MICROPY_EMIT_INLINE_THUMB

#include "py/mpstate.h"
#include "py/asmthumb.h"
#include "py/misc.h"

#define UNSIGNED_FIT5(x) ((uint32_t)(x) < 32)
#define UNSIGNED_FIT7(x) ((uint32_t)(x) < 128)
#define UNSIGNED_FIT8(x) (((x) & 0xffffff00) == 0)
#define UNSIGNED_FIT16(x) (((x) & 0xffff0000) == 0)
#define SIGNED_FIT8(x) (((x) & 0xffffff80) == 0) || (((x) & 0xffffff80) == 0xffffff80)
#define SIGNED_FIT9(x) (((x) & 0xffffff00) == 0) || (((x) & 0xffffff00) == 0xffffff00)
#define SIGNED_FIT12(x) (((x) & 0xfffff800) == 0) || (((x) & 0xfffff800) == 0xfffff800)
#define SIGNED_FIT23(x) (((x) & 0xffc00000) == 0) || (((x) & 0xffc00000) == 0xffc00000)

// Note: these actually take an imm12 but the high-bit is not encoded here
#define OP_ADD_W_RRI_HI(reg_src) (0xf200 | (reg_src))
#define OP_ADD_W_RRI_LO(reg_dest, imm11) ((imm11 << 4 & 0x7000) | reg_dest << 8 | (imm11 & 0xff))
#define OP_SUB_W_RRI_HI(reg_src) (0xf2a0 | (reg_src))
#define OP_SUB_W_RRI_LO(reg_dest, imm11) ((imm11 << 4 & 0x7000) | reg_dest << 8 | (imm11 & 0xff))

#define OP_LDR_W_HI(reg_base) (0xf8d0 | (reg_base))
#define OP_LDR_W_LO(reg_dest, imm12) ((reg_dest) << 12 | (imm12))

#define OP_LDRH_W_HI(reg_base) (0xf8b0 | (reg_base))
#define OP_LDRH_W_LO(reg_dest, imm12) ((reg_dest) << 12 | (imm12))

static inline byte *asm_thumb_get_cur_to_write_bytes(asm_thumb_t *as, int n) {
    return mp_asm_base_get_cur_to_write_bytes(&as->base, n);
}

/*
static void asm_thumb_write_byte_1(asm_thumb_t *as, byte b1) {
    byte *c = asm_thumb_get_cur_to_write_bytes(as, 1);
    c[0] = b1;
}
*/

/*
#define IMM32_L0(x) ((x) & 0xff)
#define IMM32_L1(x) (((x) >> 8) & 0xff)
#define IMM32_L2(x) (((x) >> 16) & 0xff)
#define IMM32_L3(x) (((x) >> 24) & 0xff)

static void asm_thumb_write_word32(asm_thumb_t *as, int w32) {
    byte *c = asm_thumb_get_cur_to_write_bytes(as, 4);
    c[0] = IMM32_L0(w32);
    c[1] = IMM32_L1(w32);
    c[2] = IMM32_L2(w32);
    c[3] = IMM32_L3(w32);
}
*/

// rlolist is a bit map indicating desired lo-registers
#define OP_PUSH_RLIST(rlolist)      (0xb400 | (rlolist))
#define OP_PUSH_RLIST_LR(rlolist)   (0xb400 | 0x0100 | (rlolist))
#define OP_POP_RLIST(rlolist)       (0xbc00 | (rlolist))
#define OP_POP_RLIST_PC(rlolist)    (0xbc00 | 0x0100 | (rlolist))

// The number of words must fit in 7 unsigned bits
#define OP_ADD_SP(num_words) (0xb000 | (num_words))
#define OP_SUB_SP(num_words) (0xb080 | (num_words))

// locals:
//  - stored on the stack in ascending order
//  - numbered 0 through num_locals-1
//  - SP points to first local
//
//  | SP
//  v
//  l0  l1  l2  ...  l(n-1)
//  ^                ^
//  | low address    | high address in RAM

void asm_thumb_entry(asm_thumb_t *as, int num_locals) {
    assert(num_locals >= 0);

    // If this Thumb machine code is run from ARM state then add a prelude
    // to switch to Thumb state for the duration of the function.
    #if MICROPY_DYNAMIC_COMPILER || MICROPY_EMIT_ARM || (defined(__arm__) && !defined(__thumb2__) && !defined(__thumb__))
    #if MICROPY_DYNAMIC_COMPILER
    if (mp_dynamic_compiler.native_arch == MP_NATIVE_ARCH_ARMV6)
    #endif
    {
        asm_thumb_op32(as, 0x4010, 0xe92d); // push {r4, lr}
        asm_thumb_op32(as, 0xe009, 0xe28f); // add lr, pc, 8 + 1
        asm_thumb_op32(as, 0xff3e, 0xe12f); // blx lr
        asm_thumb_op32(as, 0x4010, 0xe8bd); // pop {r4, lr}
        asm_thumb_op32(as, 0xff1e, 0xe12f); // bx lr
    }
    #endif

    // work out what to push and how many extra spaces to reserve on stack
    // so that we have enough for all locals and it's aligned an 8-byte boundary
    // we push extra regs (r1, r2, r3) to help do the stack adjustment
    // we probably should just always subtract from sp, since this would be more efficient
    // for push rlist, lowest numbered register at the lowest address
    uint reglist;
    uint stack_adjust;
    // don't pop r0 because it's used for return value
    switch (num_locals) {
        case 0:
            reglist = 0xf2;
            stack_adjust = 0;
            break;

        case 1:
            reglist = 0xf2;
            stack_adjust = 0;
            break;

        case 2:
            reglist = 0xfe;
            stack_adjust = 0;
            break;

        case 3:
            reglist = 0xfe;
            stack_adjust = 0;
            break;

        default:
            reglist = 0xfe;
            stack_adjust = ((num_locals - 3) + 1) & (~1);
            break;
    }
    asm_thumb_op16(as, OP_PUSH_RLIST_LR(reglist));
    if (stack_adjust > 0) {
        if (asm_thumb_allow_armv7m(as)) {
            if (UNSIGNED_FIT7(stack_adjust)) {
                asm_thumb_op16(as, OP_SUB_SP(stack_adjust));
            } else {
                asm_thumb_op32(as, OP_SUB_W_RRI_HI(ASM_THUMB_REG_SP), OP_SUB_W_RRI_LO(ASM_THUMB_REG_SP, stack_adjust * 4));
            }
        } else {
            int adj = stack_adjust;
            // we don't expect the stack_adjust to be massive
            while (!UNSIGNED_FIT7(adj)) {
                asm_thumb_op16(as, OP_SUB_SP(127));
                adj -= 127;
            }
            asm_thumb_op16(as, OP_SUB_SP(adj));
        }
    }
    as->push_reglist = reglist;
    as->stack_adjust = stack_adjust;
}

void asm_thumb_exit(asm_thumb_t *as) {
    if (as->stack_adjust > 0) {
        if (asm_thumb_allow_armv7m(as)) {
            if (UNSIGNED_FIT7(as->stack_adjust)) {
                asm_thumb_op16(as, OP_ADD_SP(as->stack_adjust));
            } else {
                asm_thumb_op32(as, OP_ADD_W_RRI_HI(ASM_THUMB_REG_SP), OP_ADD_W_RRI_LO(ASM_THUMB_REG_SP, as->stack_adjust * 4));
            }
        } else {
            int adj = as->stack_adjust;
            // we don't expect the stack_adjust to be massive
            while (!UNSIGNED_FIT7(adj)) {
                asm_thumb_op16(as, OP_ADD_SP(127));
                adj -= 127;
            }
            asm_thumb_op16(as, OP_ADD_SP(adj));
        }
    }
    asm_thumb_op16(as, OP_POP_RLIST_PC(as->push_reglist));
}

static mp_uint_t get_label_dest(asm_thumb_t *as, uint label) {
    assert(label < as->base.max_num_labels);
    return as->base.label_offsets[label];
}

void asm_thumb_op16(asm_thumb_t *as, uint op) {
    byte *c = asm_thumb_get_cur_to_write_bytes(as, 2);
    if (c != NULL) {
        // little endian
        c[0] = op;
        c[1] = op >> 8;
    }
}

void asm_thumb_op32(asm_thumb_t *as, uint op1, uint op2) {
    byte *c = asm_thumb_get_cur_to_write_bytes(as, 4);
    if (c != NULL) {
        // little endian, op1 then op2
        c[0] = op1;
        c[1] = op1 >> 8;
        c[2] = op2;
        c[3] = op2 >> 8;
    }
}

#define OP_FORMAT_4(op, rlo_dest, rlo_src) ((op) | ((rlo_src) << 3) | (rlo_dest))

void asm_thumb_format_4(asm_thumb_t *as, uint op, uint rlo_dest, uint rlo_src) {
    assert(rlo_dest < ASM_THUMB_REG_R8);
    assert(rlo_src < ASM_THUMB_REG_R8);
    asm_thumb_op16(as, OP_FORMAT_4(op, rlo_dest, rlo_src));
}

void asm_thumb_mov_reg_reg(asm_thumb_t *as, uint reg_dest, uint reg_src) {
    uint op_lo;
    if (reg_src < 8) {
        op_lo = reg_src << 3;
    } else {
        op_lo = 0x40 | ((reg_src - 8) << 3);
    }
    if (reg_dest < 8) {
        op_lo |= reg_dest;
    } else {
        op_lo |= 0x80 | (reg_dest - 8);
    }
    // mov reg_dest, reg_src
    asm_thumb_op16(as, 0x4600 | op_lo);
}

// if loading lo half with movw, the i16 value will be zero extended into the r32 register!
void asm_thumb_mov_reg_i16(asm_thumb_t *as, uint mov_op, uint reg_dest, int i16_src) {
    assert(reg_dest < ASM_THUMB_REG_R15);
    // mov[wt] reg_dest, #i16_src
    asm_thumb_op32(as, mov_op | ((i16_src >> 1) & 0x0400) | ((i16_src >> 12) & 0xf), ((i16_src << 4) & 0x7000) | (reg_dest << 8) | (i16_src & 0xff));
}

static void asm_thumb_mov_rlo_i16(asm_thumb_t *as, uint rlo_dest, int i16_src) {
    asm_thumb_mov_rlo_i8(as, rlo_dest, (i16_src >> 8) & 0xff);
    asm_thumb_lsl_rlo_rlo_i5(as, rlo_dest, rlo_dest, 8);
    asm_thumb_add_rlo_i8(as, rlo_dest, i16_src & 0xff);
}

#define OP_B_N(byte_offset) (0xe000 | (((byte_offset) >> 1) & 0x07ff))

bool asm_thumb_b_n_label(asm_thumb_t *as, uint label) {
    mp_uint_t dest = get_label_dest(as, label);
    mp_int_t rel = dest - as->base.code_offset;
    rel -= 4; // account for instruction prefetch, PC is 4 bytes ahead of this instruction
    asm_thumb_op16(as, OP_B_N(rel));
    return as->base.pass != MP_ASM_PASS_EMIT || SIGNED_FIT12(rel);
}

#define OP_BCC_N(cond, byte_offset) (0xd000 | ((cond) << 8) | (((byte_offset) >> 1) & 0x00ff))

// all these bit-arithmetic operations need coverage testing!
#define OP_BCC_W_HI(cond, byte_offset) (0xf000 | ((cond) << 6) | (((byte_offset) >> 10) & 0x0400) | (((byte_offset) >> 14) & 0x003f))
#define OP_BCC_W_LO(byte_offset) (0x8000 | ((byte_offset) & 0x2000) | (((byte_offset) >> 1) & 0x0fff))

bool asm_thumb_bcc_nw_label(asm_thumb_t *as, int cond, uint label, bool wide) {
    mp_uint_t dest = get_label_dest(as, label);
    mp_int_t rel = dest - as->base.code_offset;
    rel -= 4; // account for instruction prefetch, PC is 4 bytes ahead of this instruction
    if (!wide) {
        asm_thumb_op16(as, OP_BCC_N(cond, rel));
        return as->base.pass != MP_ASM_PASS_EMIT || SIGNED_FIT9(rel);
    } else if (asm_thumb_allow_armv7m(as)) {
        asm_thumb_op32(as, OP_BCC_W_HI(cond, rel), OP_BCC_W_LO(rel));
        return true;
    } else {
        // this method should not be called for ARMV6M
        return false;
    }
}

#define OP_BL_HI(byte_offset) (0xf000 | (((byte_offset) >> 12) & 0x07ff))
#define OP_BL_LO(byte_offset) (0xf800 | (((byte_offset) >> 1) & 0x07ff))

bool asm_thumb_bl_label(asm_thumb_t *as, uint label) {
    mp_uint_t dest = get_label_dest(as, label);
    mp_int_t rel = dest - as->base.code_offset;
    rel -= 4; // account for instruction prefetch, PC is 4 bytes ahead of this instruction
    asm_thumb_op32(as, OP_BL_HI(rel), OP_BL_LO(rel));
    return as->base.pass != MP_ASM_PASS_EMIT || SIGNED_FIT23(rel);
}

size_t asm_thumb_mov_reg_i32(asm_thumb_t *as, uint reg_dest, mp_uint_t i32) {
    // movw, movt does it in 8 bytes
    // ldr [pc, #], dw does it in 6 bytes, but we might not reach to end of code for dw

    size_t loc = mp_asm_base_get_code_pos(&as->base);

    if (asm_thumb_allow_armv7m(as)) {
        asm_thumb_mov_reg_i16(as, ASM_THUMB_OP_MOVW, reg_dest, i32);
        asm_thumb_mov_reg_i16(as, ASM_THUMB_OP_MOVT, reg_dest, i32 >> 16);
    } else {
        // should only be called with lo reg for ARMV6M
        assert(reg_dest < ASM_THUMB_REG_R8);

        // sanity check that generated code is aligned
        assert(!as->base.code_base || !(3u & (uintptr_t)as->base.code_base));

        // basically:
        //        (nop)
        //        ldr reg_dest, _data
        //        b 1f
        // _data: .word i32
        //  1:
        if (as->base.code_offset & 2u) {
            asm_thumb_op16(as, ASM_THUMB_OP_NOP);
        }
        asm_thumb_ldr_rlo_pcrel_i8(as, reg_dest, 0);
        asm_thumb_op16(as, OP_B_N(2));
        asm_thumb_op16(as, i32 & 0xffff);
        asm_thumb_op16(as, i32 >> 16);
    }

    return loc;
}

void asm_thumb_mov_reg_i32_optimised(asm_thumb_t *as, uint reg_dest, int i32) {
    if (reg_dest < 8 && UNSIGNED_FIT8(i32)) {
        asm_thumb_mov_rlo_i8(as, reg_dest, i32);
    } else if (asm_thumb_allow_armv7m(as)) {
        if (UNSIGNED_FIT16(i32)) {
            asm_thumb_mov_reg_i16(as, ASM_THUMB_OP_MOVW, reg_dest, i32);
        } else {
            asm_thumb_mov_reg_i32(as, reg_dest, i32);
        }
    } else {
        uint rlo_dest = reg_dest;
        assert(rlo_dest < ASM_THUMB_REG_R8); // should never be called for ARMV6M

        bool negate = i32 < 0 && ((i32 + i32) & 0xffffffffu); // don't negate 0x80000000
        if (negate) {
            i32 = -i32;
        }

        uint clz = mp_clz(i32);
        uint ctz = i32 ? mp_ctz(i32) : 0;
        assert(clz + ctz <= 32);
        if (clz + ctz >= 24) {
            asm_thumb_mov_rlo_i8(as, rlo_dest, (i32 >> ctz) & 0xff);
            asm_thumb_lsl_rlo_rlo_i5(as, rlo_dest, rlo_dest, ctz);
        } else if (UNSIGNED_FIT16(i32)) {
            asm_thumb_mov_rlo_i16(as, rlo_dest, i32);
        } else {
            if (negate) {
                // no point in negating if we're storing in 32 bit anyway
                negate = false;
                i32 = -i32;
            }
            asm_thumb_mov_reg_i32(as, rlo_dest, i32);
        }
        if (negate) {
            asm_thumb_neg_rlo_rlo(as, rlo_dest, rlo_dest);
        }
    }
}

#define OP_STR_TO_SP_OFFSET(rlo_dest, word_offset) (0x9000 | ((rlo_dest) << 8) | ((word_offset) & 0x00ff))
#define OP_LDR_FROM_SP_OFFSET(rlo_dest, word_offset) (0x9800 | ((rlo_dest) << 8) | ((word_offset) & 0x00ff))

static void asm_thumb_mov_local_check(asm_thumb_t *as, int word_offset) {
    if (as->base.pass >= MP_ASM_PASS_EMIT) {
        assert(word_offset >= 0);
        if (!UNSIGNED_FIT8(word_offset)) {
            mp_raise_NotImplementedError(MP_ERROR_TEXT("too many locals for native method"));
        }
    }
}

void asm_thumb_mov_local_reg(asm_thumb_t *as, int local_num, uint rlo_src) {
    assert(rlo_src < ASM_THUMB_REG_R8);
    int word_offset = local_num;
    asm_thumb_mov_local_check(as, word_offset);
    asm_thumb_op16(as, OP_STR_TO_SP_OFFSET(rlo_src, word_offset));
}

void asm_thumb_mov_reg_local(asm_thumb_t *as, uint rlo_dest, int local_num) {
    assert(rlo_dest < ASM_THUMB_REG_R8);
    int word_offset = local_num;
    asm_thumb_mov_local_check(as, word_offset);
    asm_thumb_op16(as, OP_LDR_FROM_SP_OFFSET(rlo_dest, word_offset));
}

#define OP_ADD_REG_SP_OFFSET(rlo_dest, word_offset) (0xa800 | ((rlo_dest) << 8) | ((word_offset) & 0x00ff))

void asm_thumb_mov_reg_local_addr(asm_thumb_t *as, uint rlo_dest, int local_num) {
    assert(rlo_dest < ASM_THUMB_REG_R8);
    int word_offset = local_num;
    assert(as->base.pass < MP_ASM_PASS_EMIT || word_offset >= 0);
    asm_thumb_op16(as, OP_ADD_REG_SP_OFFSET(rlo_dest, word_offset));
}

void asm_thumb_mov_reg_pcrel(asm_thumb_t *as, uint rlo_dest, uint label) {
    mp_uint_t dest = get_label_dest(as, label);
    mp_int_t rel = dest - as->base.code_offset;
    rel |= 1; // to stay in Thumb state when jumping to this address
    if (asm_thumb_allow_armv7m(as)) {
        rel -= 6 + 4; // adjust for mov_reg_i16, sxth_rlo_rlo and then PC+4 prefetch of add_reg_reg
        asm_thumb_mov_reg_i16(as, ASM_THUMB_OP_MOVW, rlo_dest, rel); // 4 bytes
        asm_thumb_sxth_rlo_rlo(as, rlo_dest, rlo_dest); // 2 bytes
    } else {
        rel -= 8 + 4; // adjust for four instructions and then PC+4 prefetch of add_reg_reg
        // 6 bytes
        asm_thumb_mov_rlo_i16(as, rlo_dest, rel);
        // 2 bytes - not always needed, but we want to keep the size the same
        asm_thumb_sxth_rlo_rlo(as, rlo_dest, rlo_dest);
    }
    asm_thumb_add_reg_reg(as, rlo_dest, ASM_THUMB_REG_R15); // 2 bytes
}

// ARMv7-M only
static inline void asm_thumb_ldr_reg_reg_i12(asm_thumb_t *as, uint reg_dest, uint reg_base, uint word_offset) {
    asm_thumb_op32(as, OP_LDR_W_HI(reg_base), OP_LDR_W_LO(reg_dest, word_offset * 4));
}

// emits code for: reg_dest = reg_base + offset << offset_shift
static void asm_thumb_add_reg_reg_offset(asm_thumb_t *as, uint reg_dest, uint reg_base, uint offset, uint offset_shift) {
    if (reg_dest < ASM_THUMB_REG_R8 && reg_base < ASM_THUMB_REG_R8) {
        if (offset << offset_shift < 256) {
            if (reg_dest != reg_base) {
                asm_thumb_mov_reg_reg(as, reg_dest, reg_base);
            }
            asm_thumb_add_rlo_i8(as, reg_dest, offset << offset_shift);
        } else if (UNSIGNED_FIT8(offset) && reg_dest != reg_base) {
            asm_thumb_mov_rlo_i8(as, reg_dest, offset);
            asm_thumb_lsl_rlo_rlo_i5(as, reg_dest, reg_dest, offset_shift);
            asm_thumb_add_rlo_rlo_rlo(as, reg_dest, reg_dest, reg_base);
        } else if (reg_dest != reg_base) {
            asm_thumb_mov_rlo_i16(as, reg_dest, offset << offset_shift);
            asm_thumb_add_rlo_rlo_rlo(as, reg_dest, reg_dest, reg_dest);
        } else {
            uint reg_other = reg_dest ^ 7;
            asm_thumb_op16(as, OP_PUSH_RLIST((1 << reg_other)));
            asm_thumb_mov_rlo_i16(as, reg_other, offset << offset_shift);
            asm_thumb_add_rlo_rlo_rlo(as, reg_dest, reg_dest, reg_other);
            asm_thumb_op16(as, OP_POP_RLIST((1 << reg_other)));
        }
    } else {
        assert(0); // should never be called for ARMV6M
    }
}

void asm_thumb_ldr_reg_reg_i12_optimised(asm_thumb_t *as, uint reg_dest, uint reg_base, uint word_offset) {
    if (reg_dest < ASM_THUMB_REG_R8 && reg_base < ASM_THUMB_REG_R8 && UNSIGNED_FIT5(word_offset)) {
        asm_thumb_ldr_rlo_rlo_i5(as, reg_dest, reg_base, word_offset);
    } else if (asm_thumb_allow_armv7m(as)) {
        asm_thumb_ldr_reg_reg_i12(as, reg_dest, reg_base, word_offset);
    } else {
        asm_thumb_add_reg_reg_offset(as, reg_dest, reg_base, word_offset - 31, 2);
        asm_thumb_ldr_rlo_rlo_i5(as, reg_dest, reg_dest, 31);
    }
}

// ARMv7-M only
static inline void asm_thumb_ldrh_reg_reg_i12(asm_thumb_t *as, uint reg_dest, uint reg_base, uint uint16_offset) {
    asm_thumb_op32(as, OP_LDRH_W_HI(reg_base), OP_LDRH_W_LO(reg_dest, uint16_offset * 2));
}

void asm_thumb_ldrh_reg_reg_i12_optimised(asm_thumb_t *as, uint reg_dest, uint reg_base, uint uint16_offset) {
    if (reg_dest < ASM_THUMB_REG_R8 && reg_base < ASM_THUMB_REG_R8 && UNSIGNED_FIT5(uint16_offset)) {
        asm_thumb_ldrh_rlo_rlo_i5(as, reg_dest, reg_base, uint16_offset);
    } else if (asm_thumb_allow_armv7m(as)) {
        asm_thumb_ldrh_reg_reg_i12(as, reg_dest, reg_base, uint16_offset);
    } else {
        asm_thumb_add_reg_reg_offset(as, reg_dest, reg_base, uint16_offset - 31, 1);
        asm_thumb_ldrh_rlo_rlo_i5(as, reg_dest, reg_dest, 31);
    }
}

// this could be wrong, because it should have a range of +/- 16MiB...
#define OP_BW_HI(byte_offset) (0xf000 | (((byte_offset) >> 12) & 0x07ff))
#define OP_BW_LO(byte_offset) (0xb800 | (((byte_offset) >> 1) & 0x07ff))

void asm_thumb_b_label(asm_thumb_t *as, uint label) {
    mp_uint_t dest = get_label_dest(as, label);
    mp_int_t rel = dest - as->base.code_offset;
    rel -= 4; // account for instruction prefetch, PC is 4 bytes ahead of this instruction

    if (dest != (mp_uint_t)-1 && rel <= -4) {
        // is a backwards jump, so we know the size of the jump on the first pass
        // calculate rel assuming 12 bit relative jump
        if (SIGNED_FIT12(rel)) {
            asm_thumb_op16(as, OP_B_N(rel));
            return;
        }
    }

    // is a large backwards jump, or a forwards jump (that must be assumed large)

    if (asm_thumb_allow_armv7m(as)) {
        asm_thumb_op32(as, OP_BW_HI(rel), OP_BW_LO(rel));
    } else {
        if (SIGNED_FIT12(rel)) {
            // this code path has to be the same number of instructions irrespective of rel
            asm_thumb_op16(as, OP_B_N(rel));
        } else {
            asm_thumb_op16(as, ASM_THUMB_OP_NOP);
            if (dest != (mp_uint_t)-1) {
                // we have an actual branch > 12 bits; this is not handled yet
                mp_raise_NotImplementedError(MP_ERROR_TEXT("native method too big"));
            }
        }
    }
}

void asm_thumb_bcc_label(asm_thumb_t *as, int cond, uint label) {
    mp_uint_t dest = get_label_dest(as, label);
    mp_int_t rel = dest - as->base.code_offset;
    rel -= 4; // account for instruction prefetch, PC is 4 bytes ahead of this instruction

    if (dest != (mp_uint_t)-1 && rel <= -4) {
        // is a backwards jump, so we know the size of the jump on the first pass
        // calculate rel assuming 9 bit relative jump
        if (SIGNED_FIT9(rel)) {
            asm_thumb_op16(as, OP_BCC_N(cond, rel));
            return;
        }
    }

    // is a large backwards jump, or a forwards jump (that must be assumed large)

    if (asm_thumb_allow_armv7m(as)) {
        asm_thumb_op32(as, OP_BCC_W_HI(cond, rel), OP_BCC_W_LO(rel));
    } else {
        // reverse the sense of the branch to jump over a longer branch
        asm_thumb_op16(as, OP_BCC_N(cond ^ 1, 0));
        asm_thumb_b_label(as, label);
    }
}

void asm_thumb_bcc_rel9(asm_thumb_t *as, int cond, int rel) {
    rel -= 4; // account for instruction prefetch, PC is 4 bytes ahead of this instruction
    assert(SIGNED_FIT9(rel));
    asm_thumb_op16(as, OP_BCC_N(cond, rel));
}

void asm_thumb_b_rel12(asm_thumb_t *as, int rel) {
    rel -= 4; // account for instruction prefetch, PC is 4 bytes ahead of this instruction
    assert(SIGNED_FIT12(rel));
    asm_thumb_op16(as, OP_B_N(rel));
}

#define OP_BLX(reg) (0x4780 | ((reg) << 3))
#define OP_SVC(arg) (0xdf00 | (arg))

void asm_thumb_bl_ind(asm_thumb_t *as, uint fun_id, uint reg_temp) {
    // Load ptr to function from table, indexed by fun_id, then call it
    asm_thumb_ldr_reg_reg_i12_optimised(as, reg_temp, ASM_THUMB_REG_FUN_TABLE, fun_id);
    asm_thumb_op16(as, OP_BLX(reg_temp));
}

#endif // MICROPY_EMIT_THUMB || MICROPY_EMIT_INLINE_THUMB
