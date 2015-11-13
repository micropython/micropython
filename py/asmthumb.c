/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include "py/asmthumb.h"

#define UNSIGNED_FIT8(x) (((x) & 0xffffff00) == 0)
#define UNSIGNED_FIT16(x) (((x) & 0xffff0000) == 0)
#define SIGNED_FIT8(x) (((x) & 0xffffff80) == 0) || (((x) & 0xffffff80) == 0xffffff80)
#define SIGNED_FIT9(x) (((x) & 0xffffff00) == 0) || (((x) & 0xffffff00) == 0xffffff00)
#define SIGNED_FIT12(x) (((x) & 0xfffff800) == 0) || (((x) & 0xfffff800) == 0xfffff800)
#define SIGNED_FIT23(x) (((x) & 0xffc00000) == 0) || (((x) & 0xffc00000) == 0xffc00000)

struct _asm_thumb_t {
    mp_uint_t pass;
    mp_uint_t code_offset;
    mp_uint_t code_size;
    byte *code_base;
    byte dummy_data[4];

    mp_uint_t max_num_labels;
    mp_uint_t *label_offsets;
    mp_uint_t push_reglist;
    mp_uint_t stack_adjust;
};

asm_thumb_t *asm_thumb_new(uint max_num_labels) {
    asm_thumb_t *as;

    as = m_new0(asm_thumb_t, 1);
    as->max_num_labels = max_num_labels;
    as->label_offsets = m_new(mp_uint_t, max_num_labels);

    return as;
}

void asm_thumb_free(asm_thumb_t *as, bool free_code) {
    if (free_code) {
        MP_PLAT_FREE_EXEC(as->code_base, as->code_size);
    }
    m_del(mp_uint_t, as->label_offsets, as->max_num_labels);
    m_del_obj(asm_thumb_t, as);
}

void asm_thumb_start_pass(asm_thumb_t *as, uint pass) {
    if (pass == ASM_THUMB_PASS_COMPUTE) {
        memset(as->label_offsets, -1, as->max_num_labels * sizeof(mp_uint_t));
    } else if (pass == ASM_THUMB_PASS_EMIT) {
        MP_PLAT_ALLOC_EXEC(as->code_offset, (void**)&as->code_base, &as->code_size);
        if (as->code_base == NULL) {
            assert(0);
        }
        //printf("code_size: %u\n", as->code_size);
    }
    as->pass = pass;
    as->code_offset = 0;
}

void asm_thumb_end_pass(asm_thumb_t *as) {
    (void)as;
    // could check labels are resolved...
}

// all functions must go through this one to emit bytes
// if as->pass < ASM_THUMB_PASS_EMIT, then this function only returns a buffer of 4 bytes length
STATIC byte *asm_thumb_get_cur_to_write_bytes(asm_thumb_t *as, int num_bytes_to_write) {
    //printf("emit %d\n", num_bytes_to_write);
    if (as->pass < ASM_THUMB_PASS_EMIT) {
        as->code_offset += num_bytes_to_write;
        return as->dummy_data;
    } else {
        assert(as->code_offset + num_bytes_to_write <= as->code_size);
        byte *c = as->code_base + as->code_offset;
        as->code_offset += num_bytes_to_write;
        return c;
    }
}

uint asm_thumb_get_code_pos(asm_thumb_t *as) {
    return as->code_offset;
}

uint asm_thumb_get_code_size(asm_thumb_t *as) {
    return as->code_size;
}

void *asm_thumb_get_code(asm_thumb_t *as) {
    return as->code_base;
}

/*
STATIC void asm_thumb_write_byte_1(asm_thumb_t *as, byte b1) {
    byte *c = asm_thumb_get_cur_to_write_bytes(as, 1);
    c[0] = b1;
}
*/

/*
#define IMM32_L0(x) ((x) & 0xff)
#define IMM32_L1(x) (((x) >> 8) & 0xff)
#define IMM32_L2(x) (((x) >> 16) & 0xff)
#define IMM32_L3(x) (((x) >> 24) & 0xff)

STATIC void asm_thumb_write_word32(asm_thumb_t *as, int w32) {
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
    // work out what to push and how many extra spaces to reserve on stack
    // so that we have enough for all locals and it's aligned an 8-byte boundary
    // we push extra regs (r1, r2, r3) to help do the stack adjustment
    // we probably should just always subtract from sp, since this would be more efficient
    // for push rlist, lowest numbered register at the lowest address
    uint reglist;
    uint stack_adjust;
    if (num_locals < 0) {
        num_locals = 0;
    }
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
        asm_thumb_op16(as, OP_SUB_SP(stack_adjust));
    }
    as->push_reglist = reglist;
    as->stack_adjust = stack_adjust;
}

void asm_thumb_exit(asm_thumb_t *as) {
    if (as->stack_adjust > 0) {
        asm_thumb_op16(as, OP_ADD_SP(as->stack_adjust));
    }
    asm_thumb_op16(as, OP_POP_RLIST_PC(as->push_reglist));
}

void asm_thumb_label_assign(asm_thumb_t *as, uint label) {
    assert(label < as->max_num_labels);
    if (as->pass < ASM_THUMB_PASS_EMIT) {
        // assign label offset
        assert(as->label_offsets[label] == -1);
        as->label_offsets[label] = as->code_offset;
    } else {
        // ensure label offset has not changed from PASS_COMPUTE to PASS_EMIT
        //printf("l%d: (at %d=%ld)\n", label, as->label_offsets[label], as->code_offset);
        assert(as->label_offsets[label] == as->code_offset);
    }
}

void asm_thumb_align(asm_thumb_t* as, uint align) {
    // TODO fill unused data with NOPs?
    as->code_offset = (as->code_offset + align - 1) & (~(align - 1));
}

void asm_thumb_data(asm_thumb_t* as, uint bytesize, uint val) {
    byte *c = asm_thumb_get_cur_to_write_bytes(as, bytesize);
    // only write to the buffer in the emit pass (otherwise we overflow dummy_data)
    if (as->pass == ASM_THUMB_PASS_EMIT) {
        // little endian
        for (uint i = 0; i < bytesize; i++) {
            *c++ = val;
            val >>= 8;
        }
    }
}

STATIC mp_uint_t get_label_dest(asm_thumb_t *as, uint label) {
    assert(label < as->max_num_labels);
    return as->label_offsets[label];
}

void asm_thumb_op16(asm_thumb_t *as, uint op) {
    byte *c = asm_thumb_get_cur_to_write_bytes(as, 2);
    // little endian
    c[0] = op;
    c[1] = op >> 8;
}

void asm_thumb_op32(asm_thumb_t *as, uint op1, uint op2) {
    byte *c = asm_thumb_get_cur_to_write_bytes(as, 4);
    // little endian, op1 then op2
    c[0] = op1;
    c[1] = op1 >> 8;
    c[2] = op2;
    c[3] = op2 >> 8;
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

#define OP_B_N(byte_offset) (0xe000 | (((byte_offset) >> 1) & 0x07ff))

bool asm_thumb_b_n_label(asm_thumb_t *as, uint label) {
    mp_uint_t dest = get_label_dest(as, label);
    mp_int_t rel = dest - as->code_offset;
    rel -= 4; // account for instruction prefetch, PC is 4 bytes ahead of this instruction
    asm_thumb_op16(as, OP_B_N(rel));
    return as->pass != ASM_THUMB_PASS_EMIT || SIGNED_FIT12(rel);
}

#define OP_BCC_N(cond, byte_offset) (0xd000 | ((cond) << 8) | (((byte_offset) >> 1) & 0x00ff))

// all these bit arithmetics need coverage testing!
#define OP_BCC_W_HI(cond, byte_offset) (0xf000 | ((cond) << 6) | (((byte_offset) >> 10) & 0x0400) | (((byte_offset) >> 14) & 0x003f))
#define OP_BCC_W_LO(byte_offset) (0x8000 | ((byte_offset) & 0x2000) | (((byte_offset) >> 1) & 0x0fff))

bool asm_thumb_bcc_nw_label(asm_thumb_t *as, int cond, uint label, bool wide) {
    mp_uint_t dest = get_label_dest(as, label);
    mp_int_t rel = dest - as->code_offset;
    rel -= 4; // account for instruction prefetch, PC is 4 bytes ahead of this instruction
    if (!wide) {
        asm_thumb_op16(as, OP_BCC_N(cond, rel));
        return as->pass != ASM_THUMB_PASS_EMIT || SIGNED_FIT9(rel);
    } else {
        asm_thumb_op32(as, OP_BCC_W_HI(cond, rel), OP_BCC_W_LO(rel));
        return true;
    }
}

#define OP_BL_HI(byte_offset) (0xf000 | (((byte_offset) >> 12) & 0x07ff))
#define OP_BL_LO(byte_offset) (0xf800 | (((byte_offset) >> 1) & 0x07ff))

bool asm_thumb_bl_label(asm_thumb_t *as, uint label) {
    mp_uint_t dest = get_label_dest(as, label);
    mp_int_t rel = dest - as->code_offset;
    rel -= 4; // account for instruction prefetch, PC is 4 bytes ahead of this instruction
    asm_thumb_op32(as, OP_BL_HI(rel), OP_BL_LO(rel));
    return as->pass != ASM_THUMB_PASS_EMIT || SIGNED_FIT23(rel);
}

void asm_thumb_mov_reg_i32(asm_thumb_t *as, uint reg_dest, mp_uint_t i32) {
    // movw, movt does it in 8 bytes
    // ldr [pc, #], dw does it in 6 bytes, but we might not reach to end of code for dw

    asm_thumb_mov_reg_i16(as, ASM_THUMB_OP_MOVW, reg_dest, i32);
    asm_thumb_mov_reg_i16(as, ASM_THUMB_OP_MOVT, reg_dest, i32 >> 16);
}

void asm_thumb_mov_reg_i32_optimised(asm_thumb_t *as, uint reg_dest, int i32) {
    if (reg_dest < 8 && UNSIGNED_FIT8(i32)) {
        asm_thumb_mov_rlo_i8(as, reg_dest, i32);
    } else if (UNSIGNED_FIT16(i32)) {
        asm_thumb_mov_reg_i16(as, ASM_THUMB_OP_MOVW, reg_dest, i32);
    } else {
        asm_thumb_mov_reg_i32(as, reg_dest, i32);
    }
}

// i32 is stored as a full word in the code, and aligned to machine-word boundary
// TODO this is very inefficient, improve it!
void asm_thumb_mov_reg_i32_aligned(asm_thumb_t *as, uint reg_dest, int i32) {
    // align on machine-word + 2
    if ((as->code_offset & 3) == 0) {
        asm_thumb_op16(as, ASM_THUMB_OP_NOP);
    }
    // jump over the i32 value (instruction prefetch adds 2 to PC)
    asm_thumb_op16(as, OP_B_N(2));
    // store i32 on machine-word aligned boundary
    asm_thumb_data(as, 4, i32);
    // do the actual load of the i32 value
    asm_thumb_mov_reg_i32_optimised(as, reg_dest, i32);
}

#define OP_STR_TO_SP_OFFSET(rlo_dest, word_offset) (0x9000 | ((rlo_dest) << 8) | ((word_offset) & 0x00ff))
#define OP_LDR_FROM_SP_OFFSET(rlo_dest, word_offset) (0x9800 | ((rlo_dest) << 8) | ((word_offset) & 0x00ff))

void asm_thumb_mov_local_reg(asm_thumb_t *as, int local_num, uint rlo_src) {
    assert(rlo_src < ASM_THUMB_REG_R8);
    int word_offset = local_num;
    assert(as->pass < ASM_THUMB_PASS_EMIT || word_offset >= 0);
    asm_thumb_op16(as, OP_STR_TO_SP_OFFSET(rlo_src, word_offset));
}

void asm_thumb_mov_reg_local(asm_thumb_t *as, uint rlo_dest, int local_num) {
    assert(rlo_dest < ASM_THUMB_REG_R8);
    int word_offset = local_num;
    assert(as->pass < ASM_THUMB_PASS_EMIT || word_offset >= 0);
    asm_thumb_op16(as, OP_LDR_FROM_SP_OFFSET(rlo_dest, word_offset));
}

#define OP_ADD_REG_SP_OFFSET(rlo_dest, word_offset) (0xa800 | ((rlo_dest) << 8) | ((word_offset) & 0x00ff))

void asm_thumb_mov_reg_local_addr(asm_thumb_t *as, uint rlo_dest, int local_num) {
    assert(rlo_dest < ASM_THUMB_REG_R8);
    int word_offset = local_num;
    assert(as->pass < ASM_THUMB_PASS_EMIT || word_offset >= 0);
    asm_thumb_op16(as, OP_ADD_REG_SP_OFFSET(rlo_dest, word_offset));
}

// this could be wrong, because it should have a range of +/- 16MiB...
#define OP_BW_HI(byte_offset) (0xf000 | (((byte_offset) >> 12) & 0x07ff))
#define OP_BW_LO(byte_offset) (0xb800 | (((byte_offset) >> 1) & 0x07ff))

void asm_thumb_b_label(asm_thumb_t *as, uint label) {
    mp_uint_t dest = get_label_dest(as, label);
    mp_int_t rel = dest - as->code_offset;
    rel -= 4; // account for instruction prefetch, PC is 4 bytes ahead of this instruction
    if (dest != (mp_uint_t)-1 && rel <= -4) {
        // is a backwards jump, so we know the size of the jump on the first pass
        // calculate rel assuming 12 bit relative jump
        if (SIGNED_FIT12(rel)) {
            asm_thumb_op16(as, OP_B_N(rel));
        } else {
            goto large_jump;
        }
    } else {
        // is a forwards jump, so need to assume it's large
        large_jump:
        asm_thumb_op32(as, OP_BW_HI(rel), OP_BW_LO(rel));
    }
}

void asm_thumb_bcc_label(asm_thumb_t *as, int cond, uint label) {
    mp_uint_t dest = get_label_dest(as, label);
    mp_int_t rel = dest - as->code_offset;
    rel -= 4; // account for instruction prefetch, PC is 4 bytes ahead of this instruction
    if (dest != (mp_uint_t)-1 && rel <= -4) {
        // is a backwards jump, so we know the size of the jump on the first pass
        // calculate rel assuming 9 bit relative jump
        if (SIGNED_FIT9(rel)) {
            asm_thumb_op16(as, OP_BCC_N(cond, rel));
        } else {
            goto large_jump;
        }
    } else {
        // is a forwards jump, so need to assume it's large
        large_jump:
        asm_thumb_op32(as, OP_BCC_W_HI(cond, rel), OP_BCC_W_LO(rel));
    }
}

#define OP_BLX(reg) (0x4780 | ((reg) << 3))
#define OP_SVC(arg) (0xdf00 | (arg))

void asm_thumb_bl_ind(asm_thumb_t *as, void *fun_ptr, uint fun_id, uint reg_temp) {
    /* TODO make this use less bytes
    uint rlo_base = ASM_THUMB_REG_R3;
    uint rlo_dest = ASM_THUMB_REG_R7;
    uint word_offset = 4;
    asm_thumb_op16(as, 0x0000);
    asm_thumb_op16(as, 0x6800 | (word_offset << 6) | (rlo_base << 3) | rlo_dest); // ldr rlo_dest, [rlo_base, #offset]
    asm_thumb_op16(as, 0x4780 | (ASM_THUMB_REG_R9 << 3)); // blx reg
    */

    if (fun_id < 32) {
        // load ptr to function from table, indexed by fun_id (must be in range 0-31); 4 bytes
        asm_thumb_op16(as, ASM_THUMB_FORMAT_9_10_ENCODE(ASM_THUMB_FORMAT_9_LDR | ASM_THUMB_FORMAT_9_WORD_TRANSFER, reg_temp, ASM_THUMB_REG_R7, fun_id));
        asm_thumb_op16(as, OP_BLX(reg_temp));
    } else {
        // load ptr to function into register using immediate; 6 bytes
        asm_thumb_mov_reg_i32(as, reg_temp, (mp_uint_t)fun_ptr);
        asm_thumb_op16(as, OP_BLX(reg_temp));
    }
}

#endif // MICROPY_EMIT_THUMB || MICROPY_EMIT_INLINE_THUMB
