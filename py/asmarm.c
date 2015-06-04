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

#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "py/mpconfig.h"

// wrapper around everything in this file
#if MICROPY_EMIT_ARM

#include "py/asmarm.h"

#define SIGNED_FIT24(x) (((x) & 0xff800000) == 0) || (((x) & 0xff000000) == 0xff000000)

struct _asm_arm_t {
    uint pass;
    mp_uint_t code_offset;
    mp_uint_t code_size;
    byte *code_base;
    byte dummy_data[4];

    mp_uint_t max_num_labels;
    mp_uint_t *label_offsets;
    uint push_reglist;
    uint stack_adjust;
};

asm_arm_t *asm_arm_new(uint max_num_labels) {
    asm_arm_t *as;

    as = m_new0(asm_arm_t, 1);
    as->max_num_labels = max_num_labels;
    as->label_offsets = m_new(mp_uint_t, max_num_labels);

    return as;
}

void asm_arm_free(asm_arm_t *as, bool free_code) {
    if (free_code) {
        MP_PLAT_FREE_EXEC(as->code_base, as->code_size);
    }
    m_del(mp_uint_t, as->label_offsets, as->max_num_labels);
    m_del_obj(asm_arm_t, as);
}

void asm_arm_start_pass(asm_arm_t *as, uint pass) {
    if (pass == ASM_ARM_PASS_COMPUTE) {
        memset(as->label_offsets, -1, as->max_num_labels * sizeof(mp_uint_t));
    } else if (pass == ASM_ARM_PASS_EMIT) {
        MP_PLAT_ALLOC_EXEC(as->code_offset, (void**)&as->code_base, &as->code_size);
        if (as->code_base == NULL) {
            assert(0);
        }
    }
    as->pass = pass;
    as->code_offset = 0;
}

void asm_arm_end_pass(asm_arm_t *as) {
    if (as->pass == ASM_ARM_PASS_EMIT) {
#ifdef __arm__
        // flush I- and D-cache
        asm volatile(
                "0:"
                "mrc p15, 0, r15, c7, c10, 3\n"
                "bne 0b\n"
                "mov r0, #0\n"
                "mcr p15, 0, r0, c7, c7, 0\n"
                : : : "r0", "cc");
#endif
    }
}

// all functions must go through this one to emit bytes
// if as->pass < ASM_ARM_PASS_EMIT, then this function only returns a buffer of 4 bytes length
STATIC byte *asm_arm_get_cur_to_write_bytes(asm_arm_t *as, int num_bytes_to_write) {
    if (as->pass < ASM_ARM_PASS_EMIT) {
        as->code_offset += num_bytes_to_write;
        return as->dummy_data;
    } else {
        assert(as->code_offset + num_bytes_to_write <= as->code_size);
        byte *c = as->code_base + as->code_offset;
        as->code_offset += num_bytes_to_write;
        return c;
    }
}

uint asm_arm_get_code_pos(asm_arm_t *as) {
    return as->code_offset;
}

uint asm_arm_get_code_size(asm_arm_t *as) {
    return as->code_size;
}

void *asm_arm_get_code(asm_arm_t *as) {
    return as->code_base;
}

// Insert word into instruction flow
STATIC void emit(asm_arm_t *as, uint op) {
    *(uint*)asm_arm_get_cur_to_write_bytes(as, 4) = op;
}

// Insert word into instruction flow, add "ALWAYS" condition code
STATIC void emit_al(asm_arm_t *as, uint op) {
    emit(as, op | ASM_ARM_CC_AL);
}

// Basic instructions without condition code
STATIC uint asm_arm_op_push(uint reglist) {
    // stmfd sp!, {reglist}
    return 0x92d0000 | (reglist & 0xFFFF);
}

STATIC uint asm_arm_op_pop(uint reglist) {
    // ldmfd sp!, {reglist}
    return 0x8bd0000 | (reglist & 0xFFFF);
}

STATIC uint asm_arm_op_mov_reg(uint rd, uint rn) {
    // mov rd, rn
    return 0x1a00000 | (rd << 12) | rn;
}

STATIC uint asm_arm_op_mov_imm(uint rd, uint imm) {
    // mov rd, #imm
    return 0x3a00000 | (rd << 12) | imm;
}

STATIC uint asm_arm_op_mvn_imm(uint rd, uint imm) {
    // mvn rd, #imm
    return 0x3e00000 | (rd << 12) | imm;
}

STATIC uint asm_arm_op_add_imm(uint rd, uint rn, uint imm) {
    // add rd, rn, #imm
    return 0x2800000 | (rn << 16) | (rd << 12) | (imm & 0xFF);
}

STATIC uint asm_arm_op_add_reg(uint rd, uint rn, uint rm) {
    // add rd, rn, rm
    return 0x0800000 | (rn << 16) | (rd << 12) | rm;
}

STATIC uint asm_arm_op_sub_imm(uint rd, uint rn, uint imm) {
    // sub rd, rn, #imm
    return 0x2400000 | (rn << 16) | (rd << 12) | (imm & 0xFF);
}

STATIC uint asm_arm_op_sub_reg(uint rd, uint rn, uint rm) {
    // sub rd, rn, rm
    return 0x0400000 | (rn << 16) | (rd << 12) | rm;
}

STATIC uint asm_arm_op_mul_reg(uint rd, uint rm, uint rs) {
    // mul rd, rm, rs
    assert(rd != rm);
    return 0x0000090 | (rd << 16) | (rs << 8) | rm;
}

STATIC uint asm_arm_op_and_reg(uint rd, uint rn, uint rm) {
    // and rd, rn, rm
    return 0x0000000 | (rn << 16) | (rd << 12) | rm;
}

STATIC uint asm_arm_op_eor_reg(uint rd, uint rn, uint rm) {
    // eor rd, rn, rm
    return 0x0200000 | (rn << 16) | (rd << 12) | rm;
}

STATIC uint asm_arm_op_orr_reg(uint rd, uint rn, uint rm) {
    // orr rd, rn, rm
    return 0x1800000 | (rn << 16) | (rd << 12) | rm;
}

void asm_arm_bkpt(asm_arm_t *as) {
    // bkpt #0
    emit_al(as, 0x1200070); 
}

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

void asm_arm_entry(asm_arm_t *as, int num_locals) {

    if (num_locals < 0) {
        num_locals = 0;
    }

    as->stack_adjust = 0;
    as->push_reglist = 1 << ASM_ARM_REG_R1
        | 1 << ASM_ARM_REG_R2
        | 1 << ASM_ARM_REG_R3
        | 1 << ASM_ARM_REG_R4
        | 1 << ASM_ARM_REG_R5
        | 1 << ASM_ARM_REG_R6
        | 1 << ASM_ARM_REG_R7
        | 1 << ASM_ARM_REG_R8;

    // Only adjust the stack if there are more locals than usable registers
    if (num_locals > 3) {
        as->stack_adjust = num_locals * 4;
        // Align stack to 8 bytes
        if (num_locals & 1) {
            as->stack_adjust += 4;
        }
    }

    emit_al(as, asm_arm_op_push(as->push_reglist | 1 << ASM_ARM_REG_LR));
    if (as->stack_adjust > 0) {
        emit_al(as, asm_arm_op_sub_imm(ASM_ARM_REG_SP, ASM_ARM_REG_SP, as->stack_adjust));
    }
}

void asm_arm_exit(asm_arm_t *as) {
    if (as->stack_adjust > 0) {
        emit_al(as, asm_arm_op_add_imm(ASM_ARM_REG_SP, ASM_ARM_REG_SP, as->stack_adjust));
    }

    emit_al(as, asm_arm_op_pop(as->push_reglist | (1 << ASM_ARM_REG_PC)));
}

void asm_arm_push(asm_arm_t *as, uint reglist) {
    emit_al(as, asm_arm_op_push(reglist));
}

void asm_arm_pop(asm_arm_t *as, uint reglist) {
    emit_al(as, asm_arm_op_pop(reglist));
}

void asm_arm_label_assign(asm_arm_t *as, uint label) {
    assert(label < as->max_num_labels);
    if (as->pass < ASM_ARM_PASS_EMIT) {
        // assign label offset
        assert(as->label_offsets[label] == -1);
        as->label_offsets[label] = as->code_offset;
    } else {
        // ensure label offset has not changed from PASS_COMPUTE to PASS_EMIT
        assert(as->label_offsets[label] == as->code_offset);
    }
}

void asm_arm_align(asm_arm_t* as, uint align) {
    // TODO fill unused data with NOPs?
    as->code_offset = (as->code_offset + align - 1) & (~(align - 1));
}

void asm_arm_data(asm_arm_t* as, uint bytesize, uint val) {
    byte *c = asm_arm_get_cur_to_write_bytes(as, bytesize);
    // only write to the buffer in the emit pass (otherwise we overflow dummy_data)
    if (as->pass == ASM_ARM_PASS_EMIT) {
        // little endian
        for (uint i = 0; i < bytesize; i++) {
            *c++ = val;
            val >>= 8;
        }
    }
}

void asm_arm_mov_reg_reg(asm_arm_t *as, uint reg_dest, uint reg_src) {
    emit_al(as, asm_arm_op_mov_reg(reg_dest, reg_src));
}

void asm_arm_mov_reg_i32(asm_arm_t *as, uint rd, int imm) {
    // TODO: There are more variants of immediate values
    if ((imm & 0xFF) == imm) {
        emit_al(as, asm_arm_op_mov_imm(rd, imm));
    } else if (imm < 0 && imm >= -256) {
        // mvn is "move not", not "move negative"
        emit_al(as, asm_arm_op_mvn_imm(rd, ~imm));
    } else {
        //Insert immediate into code and jump over it
        emit_al(as, 0x59f0000 | (rd << 12)); // ldr rd, [pc]
        emit_al(as, 0xa000000); // b pc
        emit(as, imm);
    }
}

void asm_arm_mov_local_reg(asm_arm_t *as, int local_num, uint rd) {
    // str rd, [sp, #local_num*4]
    emit_al(as, 0x58d0000 | (rd << 12) | (local_num << 2));
}

void asm_arm_mov_reg_local(asm_arm_t *as, uint rd, int local_num) {
    // ldr rd, [sp, #local_num*4]
    emit_al(as, 0x59d0000 | (rd << 12) | (local_num << 2));
}

void asm_arm_cmp_reg_i8(asm_arm_t *as, uint rd, int imm) {
    // cmp rd, #imm
    emit_al(as, 0x3500000 | (rd << 16) | (imm & 0xFF));
}

void asm_arm_cmp_reg_reg(asm_arm_t *as, uint rd, uint rn) {
    // cmp rd, rn
    emit_al(as, 0x1500000 | (rd << 16) | rn);
}

void asm_arm_setcc_reg(asm_arm_t *as, uint rd, uint cond) {
    emit(as, asm_arm_op_mov_imm(rd, 1) | cond); // movCOND rd, #1
    emit(as, asm_arm_op_mov_imm(rd, 0) | (cond ^ (1 << 28))); // mov!COND rd, #0
}

void asm_arm_add_reg_reg_reg(asm_arm_t *as, uint rd, uint rn, uint rm) {
    // add rd, rn, rm
    emit_al(as, asm_arm_op_add_reg(rd, rn, rm));
}

void asm_arm_sub_reg_reg_reg(asm_arm_t *as, uint rd, uint rn, uint rm) {
    // sub rd, rn, rm
    emit_al(as, asm_arm_op_sub_reg(rd, rn, rm));
}

void asm_arm_mul_reg_reg_reg(asm_arm_t *as, uint rd, uint rs, uint rm) {
    // rs and rm are swapped because of restriction rd!=rm
    // mul rd, rm, rs
    emit_al(as, asm_arm_op_mul_reg(rd, rm, rs));
}

void asm_arm_and_reg_reg_reg(asm_arm_t *as, uint rd, uint rn, uint rm) {
    // and rd, rn, rm
    emit_al(as, asm_arm_op_and_reg(rd, rn, rm));
}

void asm_arm_eor_reg_reg_reg(asm_arm_t *as, uint rd, uint rn, uint rm) {
    // eor rd, rn, rm
    emit_al(as, asm_arm_op_eor_reg(rd, rn, rm));
}

void asm_arm_orr_reg_reg_reg(asm_arm_t *as, uint rd, uint rn, uint rm) {
    // orr rd, rn, rm
    emit_al(as, asm_arm_op_orr_reg(rd, rn, rm));
}

void asm_arm_mov_reg_local_addr(asm_arm_t *as, uint rd, int local_num) {
    // add rd, sp, #local_num*4
    emit_al(as, asm_arm_op_add_imm(rd, ASM_ARM_REG_SP, local_num << 2));
}

void asm_arm_lsl_reg_reg(asm_arm_t *as, uint rd, uint rs) {
    // mov rd, rd, lsl rs
    emit_al(as, 0x1a00010 | (rd << 12) | (rs << 8) | rd);
}

void asm_arm_asr_reg_reg(asm_arm_t *as, uint rd, uint rs) {
    // mov rd, rd, asr rs
    emit_al(as, 0x1a00050 | (rd << 12) | (rs << 8) | rd);
}

void asm_arm_ldr_reg_reg(asm_arm_t *as, uint rd, uint rn, uint byte_offset) {
    // ldr rd, [rn, #off]
    emit_al(as, 0x5900000 | (rn << 16) | (rd << 12) | byte_offset);
}

void asm_arm_ldrh_reg_reg(asm_arm_t *as, uint rd, uint rn) {
    // ldrh rd, [rn]
    emit_al(as, 0x1d000b0 | (rn << 16) | (rd << 12));
}

void asm_arm_ldrb_reg_reg(asm_arm_t *as, uint rd, uint rn) {
    // ldrb rd, [rn]
    emit_al(as, 0x5d00000 | (rn << 16) | (rd << 12));
}

void asm_arm_str_reg_reg(asm_arm_t *as, uint rd, uint rm, uint byte_offset) {
    // str rd, [rm, #off]
    emit_al(as, 0x5800000 | (rm << 16) | (rd << 12) | byte_offset);
}

void asm_arm_strh_reg_reg(asm_arm_t *as, uint rd, uint rm) {
    // strh rd, [rm]
    emit_al(as, 0x1c000b0 | (rm << 16) | (rd << 12));
}

void asm_arm_strb_reg_reg(asm_arm_t *as, uint rd, uint rm) {
    // strb rd, [rm]
    emit_al(as, 0x5c00000 | (rm << 16) | (rd << 12));
}

void asm_arm_str_reg_reg_reg(asm_arm_t *as, uint rd, uint rm, uint rn) {
    // str rd, [rm, rn, lsl #2]
    emit_al(as, 0x7800100 | (rm << 16) | (rd << 12) | rn);
}

void asm_arm_strh_reg_reg_reg(asm_arm_t *as, uint rd, uint rm, uint rn) {
    // strh doesn't support scaled register index
    emit_al(as, 0x1a00080 | (ASM_ARM_REG_R8 << 12) | rn); // mov r8, rn, lsl #1
    emit_al(as, 0x18000b0 | (rm << 16) | (rd << 12) | ASM_ARM_REG_R8); // strh rd, [rm, r8]
}

void asm_arm_strb_reg_reg_reg(asm_arm_t *as, uint rd, uint rm, uint rn) {
    // strb rd, [rm, rn]
    emit_al(as, 0x7c00000 | (rm << 16) | (rd << 12) | rn);
}

void asm_arm_bcc_label(asm_arm_t *as, int cond, uint label) {
    assert(label < as->max_num_labels);
    mp_uint_t dest = as->label_offsets[label];
    mp_int_t rel = dest - as->code_offset;
    rel -= 8; // account for instruction prefetch, PC is 8 bytes ahead of this instruction
    rel >>= 2; // in ARM mode the branch target is 32-bit aligned, so the 2 LSB are omitted

    if (SIGNED_FIT24(rel)) {
        emit(as, cond | 0xa000000 | (rel & 0xffffff));
    } else {
        printf("asm_arm_bcc: branch does not fit in 24 bits\n");
    }
}

void asm_arm_b_label(asm_arm_t *as, uint label) {
    asm_arm_bcc_label(as, ASM_ARM_CC_AL, label);
}

void asm_arm_bl_ind(asm_arm_t *as, void *fun_ptr, uint fun_id, uint reg_temp) {
    // If the table offset fits into the ldr instruction
    if (fun_id < (0x1000 / 4)) {
        emit_al(as, asm_arm_op_mov_reg(ASM_ARM_REG_LR, ASM_ARM_REG_PC)); // mov lr, pc
        emit_al(as, 0x597f000 | (fun_id << 2)); // ldr pc, [r7, #fun_id*4]
        return;
    }

    emit_al(as, 0x59f0004 | (reg_temp << 12)); // ldr rd, [pc, #4]
    // Set lr after fun_ptr
    emit_al(as, asm_arm_op_add_imm(ASM_ARM_REG_LR, ASM_ARM_REG_PC, 4)); // add lr, pc, #4
    emit_al(as, asm_arm_op_mov_reg(ASM_ARM_REG_PC, reg_temp)); // mov pc, reg_temp
    emit(as, (uint) fun_ptr);
}

#endif // MICROPY_EMIT_ARM
