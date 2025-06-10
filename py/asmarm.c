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

#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "py/mpconfig.h"

// wrapper around everything in this file
#if MICROPY_EMIT_ARM

#include "py/asmarm.h"

#define REG_TEMP ASM_ARM_REG_R8

// Insert word into instruction flow
static void emit(asm_arm_t *as, uint op) {
    uint8_t *c = mp_asm_base_get_cur_to_write_bytes(&as->base, 4);
    if (c != NULL) {
        *(uint32_t *)c = op;
    }
}

// Insert word into instruction flow, add "ALWAYS" condition code
static void emit_al(asm_arm_t *as, uint op) {
    emit(as, op | ASM_ARM_CC_AL);
}

// Basic instructions without condition code
static uint asm_arm_op_push(uint reglist) {
    // stmfd sp!, {reglist}
    return 0x92d0000 | (reglist & 0xFFFF);
}

static uint asm_arm_op_pop(uint reglist) {
    // ldmfd sp!, {reglist}
    return 0x8bd0000 | (reglist & 0xFFFF);
}

static uint asm_arm_op_mov_reg(uint rd, uint rn) {
    // mov rd, rn
    return 0x1a00000 | (rd << 12) | rn;
}

static uint asm_arm_op_mov_imm(uint rd, uint imm) {
    // mov rd, #imm
    return 0x3a00000 | (rd << 12) | imm;
}

static uint asm_arm_op_mvn_imm(uint rd, uint imm) {
    // mvn rd, #imm
    return 0x3e00000 | (rd << 12) | imm;
}

static uint asm_arm_op_mvn_reg(uint rd, uint rm) {
    // mvn rd, rm
    return 0x1e00000 | (rd << 12) | rm;
}

static uint asm_arm_op_add_imm(uint rd, uint rn, uint imm) {
    // add rd, rn, #imm
    return 0x2800000 | (rn << 16) | (rd << 12) | (imm & 0xFF);
}

static uint asm_arm_op_add_reg(uint rd, uint rn, uint rm) {
    // add rd, rn, rm
    return 0x0800000 | (rn << 16) | (rd << 12) | rm;
}

static uint asm_arm_op_sub_imm(uint rd, uint rn, uint imm) {
    // sub rd, rn, #imm
    return 0x2400000 | (rn << 16) | (rd << 12) | (imm & 0xFF);
}

static uint asm_arm_op_sub_reg(uint rd, uint rn, uint rm) {
    // sub rd, rn, rm
    return 0x0400000 | (rn << 16) | (rd << 12) | rm;
}

static uint asm_arm_op_rsb_imm(uint rd, uint rn, uint imm) {
    // rsb rd, rn, #imm
    return 0x2600000 | (rn << 16) | (rd << 12) | (imm & 0xFF);
}

static uint asm_arm_op_mul_reg(uint rd, uint rm, uint rs) {
    // mul rd, rm, rs
    assert(rd != rm);
    return 0x0000090 | (rd << 16) | (rs << 8) | rm;
}

static uint asm_arm_op_and_reg(uint rd, uint rn, uint rm) {
    // and rd, rn, rm
    return 0x0000000 | (rn << 16) | (rd << 12) | rm;
}

static uint asm_arm_op_eor_reg(uint rd, uint rn, uint rm) {
    // eor rd, rn, rm
    return 0x0200000 | (rn << 16) | (rd << 12) | rm;
}

static uint asm_arm_op_orr_reg(uint rd, uint rn, uint rm) {
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
    assert(num_locals >= 0);

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
        if (as->stack_adjust < 0x100) {
            emit_al(as, asm_arm_op_sub_imm(ASM_ARM_REG_SP, ASM_ARM_REG_SP, as->stack_adjust));
        } else {
            asm_arm_mov_reg_i32_optimised(as, REG_TEMP, as->stack_adjust);
            emit_al(as, asm_arm_op_sub_reg(ASM_ARM_REG_SP, ASM_ARM_REG_SP, REG_TEMP));
        }
    }
}

void asm_arm_exit(asm_arm_t *as) {
    if (as->stack_adjust > 0) {
        if (as->stack_adjust < 0x100) {
            emit_al(as, asm_arm_op_add_imm(ASM_ARM_REG_SP, ASM_ARM_REG_SP, as->stack_adjust));
        } else {
            asm_arm_mov_reg_i32_optimised(as, REG_TEMP, as->stack_adjust);
            emit_al(as, asm_arm_op_add_reg(ASM_ARM_REG_SP, ASM_ARM_REG_SP, REG_TEMP));
        }
    }

    emit_al(as, asm_arm_op_pop(as->push_reglist | (1 << ASM_ARM_REG_PC)));
}

void asm_arm_push(asm_arm_t *as, uint reglist) {
    emit_al(as, asm_arm_op_push(reglist));
}

void asm_arm_pop(asm_arm_t *as, uint reglist) {
    emit_al(as, asm_arm_op_pop(reglist));
}

void asm_arm_mov_reg_reg(asm_arm_t *as, uint reg_dest, uint reg_src) {
    emit_al(as, asm_arm_op_mov_reg(reg_dest, reg_src));
}

size_t asm_arm_mov_reg_i32(asm_arm_t *as, uint rd, int imm) {
    // Insert immediate into code and jump over it
    emit_al(as, 0x59f0000 | (rd << 12)); // ldr rd, [pc]
    emit_al(as, 0xa000000); // b pc
    size_t loc = mp_asm_base_get_code_pos(&as->base);
    emit(as, imm);
    return loc;
}

void asm_arm_mov_reg_i32_optimised(asm_arm_t *as, uint rd, int imm) {
    // TODO: There are more variants of immediate values
    if ((imm & 0xFF) == imm) {
        emit_al(as, asm_arm_op_mov_imm(rd, imm));
    } else if (imm < 0 && imm >= -256) {
        // mvn is "move not", not "move negative"
        emit_al(as, asm_arm_op_mvn_imm(rd, ~imm));
    } else {
        asm_arm_mov_reg_i32(as, rd, imm);
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

void asm_arm_mvn_reg_reg(asm_arm_t *as, uint rd, uint rm) {
    // mvn rd, rm
    // computes: rd := ~rm
    emit_al(as, asm_arm_op_mvn_reg(rd, rm));
}

void asm_arm_add_reg_reg_reg(asm_arm_t *as, uint rd, uint rn, uint rm) {
    // add rd, rn, rm
    emit_al(as, asm_arm_op_add_reg(rd, rn, rm));
}

void asm_arm_rsb_reg_reg_imm(asm_arm_t *as, uint rd, uint rn, uint imm) {
    // rsb rd, rn, #imm
    // computes: rd := #imm - rn
    emit_al(as, asm_arm_op_rsb_imm(rd, rn, imm));
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
    if (local_num >= 0x40) {
        // mov temp, #local_num*4
        // add rd, sp, temp
        asm_arm_mov_reg_i32_optimised(as, REG_TEMP, local_num << 2);
        emit_al(as, asm_arm_op_add_reg(rd, ASM_ARM_REG_SP, REG_TEMP));
    } else {
        // add rd, sp, #local_num*4
        emit_al(as, asm_arm_op_add_imm(rd, ASM_ARM_REG_SP, local_num << 2));
    }
}

void asm_arm_mov_reg_pcrel(asm_arm_t *as, uint reg_dest, uint label) {
    assert(label < as->base.max_num_labels);
    mp_uint_t dest = as->base.label_offsets[label];
    mp_int_t rel = dest - as->base.code_offset;
    rel -= 12 + 8; // adjust for load of rel, and then PC+8 prefetch of add_reg_reg_reg

    // To load rel int reg_dest, insert immediate into code and jump over it
    emit_al(as, 0x59f0000 | (reg_dest << 12)); // ldr rd, [pc]
    emit_al(as, 0xa000000); // b pc
    emit(as, rel);

    // Do reg_dest += PC
    asm_arm_add_reg_reg_reg(as, reg_dest, reg_dest, ASM_ARM_REG_PC);
}

void asm_arm_lsl_reg_reg(asm_arm_t *as, uint rd, uint rs) {
    // mov rd, rd, lsl rs
    emit_al(as, 0x1a00010 | (rd << 12) | (rs << 8) | rd);
}

void asm_arm_lsr_reg_reg(asm_arm_t *as, uint rd, uint rs) {
    // mov rd, rd, lsr rs
    emit_al(as, 0x1a00030 | (rd << 12) | (rs << 8) | rd);
}

void asm_arm_asr_reg_reg(asm_arm_t *as, uint rd, uint rs) {
    // mov rd, rd, asr rs
    emit_al(as, 0x1a00050 | (rd << 12) | (rs << 8) | rd);
}

void asm_arm_ldr_reg_reg_offset(asm_arm_t *as, uint rd, uint rn, uint byte_offset) {
    if (byte_offset < 0x1000) {
        // ldr rd, [rn, #off]
        emit_al(as, 0x5900000 | (rn << 16) | (rd << 12) | byte_offset);
    } else {
        // mov temp, #off
        // ldr rd, [rn, temp]
        asm_arm_mov_reg_i32_optimised(as, REG_TEMP, byte_offset);
        emit_al(as, 0x7900000 | (rn << 16) | (rd << 12) | REG_TEMP);
    }
}

void asm_arm_ldrh_reg_reg_reg(asm_arm_t *as, uint rd, uint rm, uint rn) {
    // ldrh doesn't support scaled register index
    emit_al(as, 0x1a00080 | (REG_TEMP << 12) | rn); // mov temp, rn, lsl #1
    emit_al(as, 0x19000b0 | (rm << 16) | (rd << 12) | REG_TEMP); // ldrh rd, [rm, temp];
}

void asm_arm_ldrh_reg_reg_offset(asm_arm_t *as, uint rd, uint rn, uint byte_offset) {
    if (byte_offset < 0x100) {
        // ldrh rd, [rn, #off]
        emit_al(as, 0x1d000b0 | (rn << 16) | (rd << 12) | ((byte_offset & 0xf0) << 4) | (byte_offset & 0xf));
    } else {
        // mov temp, #off
        // ldrh rd, [rn, temp]
        asm_arm_mov_reg_i32_optimised(as, REG_TEMP, byte_offset);
        emit_al(as, 0x19000b0 | (rn << 16) | (rd << 12) | REG_TEMP);
    }
}

void asm_arm_ldrb_reg_reg_reg(asm_arm_t *as, uint rd, uint rm, uint rn) {
    // ldrb rd, [rm, rn]
    emit_al(as, 0x7d00000 | (rm << 16) | (rd << 12) | rn);
}

void asm_arm_ldrb_reg_reg_offset(asm_arm_t *as, uint rd, uint rn, uint byte_offset) {
    if (byte_offset < 0x1000) {
        // ldrb rd, [rn, #off]
        emit_al(as, 0x5d00000 | (rn << 16) | (rd << 12) | byte_offset);
    } else {
        // mov temp, #off
        // ldrb rd, [rn, temp]
        asm_arm_mov_reg_i32_optimised(as, REG_TEMP, byte_offset);
        emit_al(as, 0x7d00000 | (rn << 16) | (rd << 12) | REG_TEMP);
    }
}

void asm_arm_ldr_reg_reg_reg(asm_arm_t *as, uint rd, uint rm, uint rn) {
    // ldr rd, [rm, rn, lsl #2]
    emit_al(as, 0x7900100 | (rm << 16) | (rd << 12) | rn);
}

void asm_arm_str_reg_reg_offset(asm_arm_t *as, uint rd, uint rm, uint byte_offset) {
    if (byte_offset < 0x1000) {
        // str rd, [rm, #off]
        emit_al(as, 0x5800000 | (rm << 16) | (rd << 12) | byte_offset);
    } else {
        // mov temp, #off
        // str rd, [rm, temp]
        asm_arm_mov_reg_i32_optimised(as, REG_TEMP, byte_offset);
        emit_al(as, 0x7800000 | (rm << 16) | (rd << 12) | REG_TEMP);
    }
}

void asm_arm_strh_reg_reg_offset(asm_arm_t *as, uint rd, uint rn, uint byte_offset) {
    if (byte_offset < 0x100) {
        // strh rd, [rn, #off]
        emit_al(as, 0x1c000b0 | (rn << 16) | (rd << 12) | ((byte_offset & 0xf0) << 4) | (byte_offset & 0xf));
    } else {
        // mov temp, #off
        // strh rd, [rn, temp]
        asm_arm_mov_reg_i32_optimised(as, REG_TEMP, byte_offset);
        emit_al(as, 0x18000b0 | (rn << 16) | (rd << 12) | REG_TEMP);
    }
}

void asm_arm_strb_reg_reg_offset(asm_arm_t *as, uint rd, uint rm, uint byte_offset) {
    if (byte_offset < 0x1000) {
        // strb rd, [rm, #off]
        emit_al(as, 0x5c00000 | (rm << 16) | (rd << 12) | byte_offset);
    } else {
        // mov temp, #off
        // strb rd, [rm, temp]
        asm_arm_mov_reg_i32_optimised(as, REG_TEMP, byte_offset);
        emit_al(as, 0x7c00000 | (rm << 16) | (rd << 12) | REG_TEMP);
    }
}

void asm_arm_str_reg_reg_reg(asm_arm_t *as, uint rd, uint rm, uint rn) {
    // str rd, [rm, rn, lsl #2]
    emit_al(as, 0x7800100 | (rm << 16) | (rd << 12) | rn);
}

void asm_arm_strh_reg_reg_reg(asm_arm_t *as, uint rd, uint rm, uint rn) {
    // strh doesn't support scaled register index
    emit_al(as, 0x1a00080 | (REG_TEMP << 12) | rn); // mov temp, rn, lsl #1
    emit_al(as, 0x18000b0 | (rm << 16) | (rd << 12) | REG_TEMP); // strh rd, [rm, temp]
}

void asm_arm_strb_reg_reg_reg(asm_arm_t *as, uint rd, uint rm, uint rn) {
    // strb rd, [rm, rn]
    emit_al(as, 0x7c00000 | (rm << 16) | (rd << 12) | rn);
}

void asm_arm_bcc_label(asm_arm_t *as, int cond, uint label) {
    assert(label < as->base.max_num_labels);
    mp_uint_t dest = as->base.label_offsets[label];
    mp_int_t rel = dest - as->base.code_offset;
    rel -= 8; // account for instruction prefetch, PC is 8 bytes ahead of this instruction
    rel >>= 2; // in ARM mode the branch target is 32-bit aligned, so the 2 LSB are omitted

    if (MP_FIT_SIGNED(24, rel)) {
        emit(as, cond | 0xa000000 | (rel & 0xffffff));
    } else {
        printf("asm_arm_bcc: branch does not fit in 24 bits\n");
    }
}

void asm_arm_b_label(asm_arm_t *as, uint label) {
    asm_arm_bcc_label(as, ASM_ARM_CC_AL, label);
}

void asm_arm_bl_ind(asm_arm_t *as, uint fun_id, uint reg_temp) {
    // The table offset should fit into the ldr instruction
    assert(fun_id < (0x1000 / 4));
    emit_al(as, asm_arm_op_mov_reg(ASM_ARM_REG_LR, ASM_ARM_REG_PC)); // mov lr, pc
    emit_al(as, 0x597f000 | (fun_id << 2)); // ldr pc, [r7, #fun_id*4]
}

void asm_arm_bx_reg(asm_arm_t *as, uint reg_src) {
    emit_al(as, 0x012fff10 | reg_src);
}

#endif // MICROPY_EMIT_ARM
