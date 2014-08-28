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

#include "mpconfig.h"
#include "misc.h"
#include "asmarm.h"

// wrapper around everything in this file
#if MICROPY_EMIT_ARM

#define SIGNED_FIT24(x) (((x) & 0xff800000) == 0) || (((x) & 0xff000000) == 0xff000000)

struct _asm_arm_t {
    uint pass;
    uint code_offset;
    uint code_size;
    byte *code_base;
    byte dummy_data[4];

    uint max_num_labels;
    int *label_offsets;
    int num_locals;
    uint push_reglist;
    uint stack_adjust;
};

asm_arm_t *asm_arm_new(uint max_num_labels) {
    asm_arm_t *as;

    as = m_new0(asm_arm_t, 1);
    as->max_num_labels = max_num_labels;
    as->label_offsets = m_new(int, max_num_labels);

    return as;
}

void asm_arm_free(asm_arm_t *as, bool free_code) {
    if (free_code) {
        m_del(byte, as->code_base, as->code_size);
    }

    m_del_obj(asm_arm_t, as);
}

void asm_arm_start_pass(asm_arm_t *as, uint pass) {
    as->pass = pass;
    as->code_offset = 0;
    if (pass == ASM_ARM_PASS_COMPUTE) {
        memset(as->label_offsets, -1, as->max_num_labels * sizeof(int));
    }
}

void asm_arm_end_pass(asm_arm_t *as) {
    if (as->pass == ASM_ARM_PASS_COMPUTE) {
        // calculate size of code in bytes
        as->code_size = as->code_offset;
        as->code_base = m_new(byte, as->code_size);
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
    emit(as, op | ARM_CC_AL);
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

void asm_arm_bkpt(asm_arm_t *as) {
    // bkpt #0
    emit_al(as, 0x1200070); 
}

// locals:
//  - stored on the stack in ascending order
//  - numbered 0 through as->num_locals-1
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
    as->num_locals = num_locals;
    as->push_reglist = 1 << REG_R1 | 1 << REG_R2 | 1 << REG_R3 | 1 << REG_R4
            | 1 << REG_R5 | 1 << REG_R6 | 1 << REG_R7 | 1 << REG_R8;

    // Only adjust the stack if there are more locals than usable registers
    if(num_locals > 3) {
        as->stack_adjust = num_locals * 4;
        // Align stack to 8 bytes
        if(as->num_locals & 1)
            as->stack_adjust += 4;
    }

    emit_al(as, asm_arm_op_push(as->push_reglist | 1 << REG_LR));
    if (as->stack_adjust > 0) {
        emit_al(as, asm_arm_op_sub_imm(REG_SP, REG_SP, as->stack_adjust));
    }
}

void asm_arm_exit(asm_arm_t *as) {
    if (as->stack_adjust > 0) {
        emit_al(as, asm_arm_op_add_imm(REG_SP, REG_SP, as->stack_adjust));
    }

    emit_al(as, asm_arm_op_pop(as->push_reglist | (1 << REG_PC)));
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
    } else if (imm < 0 && ((-imm) & 0xFF) == -imm) {
        emit_al(as, asm_arm_op_mvn_imm(rd, -imm));
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

void asm_arm_less_op(asm_arm_t *as, uint rd, uint rn) {
    asm_arm_cmp_reg_reg(as, rd, rn); // cmp rd, rn
    emit(as, asm_arm_op_mov_imm(REG_RET, 1) | ARM_CC_LT); // movlt REG_RET, #1
    emit(as, asm_arm_op_mov_imm(REG_RET, 0) | ARM_CC_GE); // movge REG_RET, #0
}

void asm_arm_add_reg(asm_arm_t *as, uint rd, uint rn, uint rm) {
    // add rd, rn, rm
    emit_al(as, asm_arm_op_add_reg(rd, rn, rm));
}

void asm_arm_mov_reg_local_addr(asm_arm_t *as, uint rd, int local_num) {
    // add rd, sp, #local_num*4
    emit_al(as, asm_arm_op_add_imm(rd, REG_SP, local_num << 2));
}

void asm_arm_bcc_label(asm_arm_t *as, int cond, uint label) {
    assert(label < as->max_num_labels);
    int dest = as->label_offsets[label];
    int rel = dest - as->code_offset;
    rel -= 8; // account for instruction prefetch, PC is 8 bytes ahead of this instruction
    rel >>= 2; // in ARM mode the branch target is 32-bit aligned, so the 2 LSB are omitted

    if (SIGNED_FIT24(rel)) {
        emit(as, cond | 0xa000000 | (rel & 0xffffff));
    } else {
        printf("asm_arm_bcc: branch does not fit in 24 bits\n");
    }
}

void asm_arm_b_label(asm_arm_t *as, uint label) {
    asm_arm_bcc_label(as, ARM_CC_AL, label);
}

void asm_arm_bl_ind(asm_arm_t *as, void *fun_ptr, uint fun_id, uint reg_temp) {
    // If the table offset fits into the ldr instruction
    if(fun_id < (0x1000 / 4)) {
        emit_al(as, asm_arm_op_mov_reg(REG_LR, REG_PC)); // mov lr, pc
        emit_al(as, 0x597f000 | (fun_id << 2)); // ldr pc, [r7, #fun_id*4]
        return;
    }
    
    emit_al(as, 0x59f0004 | (reg_temp << 12)); // ldr rd, [pc, #4]
    // Set lr after fun_ptr
    emit_al(as, asm_arm_op_add_imm(REG_LR, REG_PC, 4)); // add lr, pc, #4
    emit_al(as, asm_arm_op_mov_reg(REG_PC, reg_temp)); // mov pc, reg_temp
    emit(as, (uint) fun_ptr);
}

#endif // MICROPY_EMIT_ARM
