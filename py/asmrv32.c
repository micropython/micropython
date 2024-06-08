/*
 * This file is part of the MicroPython project, https://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Alessandro Gatti
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
#include <stdlib.h>
#include <string.h>

#include "py/emit.h"
#include "py/mpconfig.h"

// wrapper around everything in this file
#if MICROPY_EMIT_RV32

#include "py/asmrv32.h"

#if MICROPY_DEBUG_VERBOSE
#define DEBUG_PRINT (1)
#define DEBUG_printf DEBUG_printf
#else
#define DEBUG_printf(...) (void)0
#endif

#ifndef MP_POPCOUNT
#ifdef _MSC_VER
#include <intrin.h>
#define MP_POPCOUNT __popcnt
#else
#if defined __has_builtin
#if __has_builtin(__builtin_popcount)
#define MP_POPCOUNT __builtin_popcount
#endif
#else
static uint32_t fallback_popcount(uint32_t value) {
    value = value - ((value >> 1) & 0x55555555);
    value = (value & 0x33333333) + ((value >> 2) & 0x33333333);
    value = (value + (value >> 4)) & 0x0F0F0F0F;
    return value * 0x01010101;
}
#define MP_POPCOUNT fallback_popcount
#endif
#endif
#endif

#define INTERNAL_TEMPORARY ASM_RV32_REG_T4
#define AVAILABLE_REGISTERS_COUNT 32

#define FIT_UNSIGNED(value, bits) (((value) & ~((1U << (bits)) - 1)) == 0)
#define FIT_SIGNED(value, bits) \
    ((((value) & ~((1U << ((bits) - 1)) - 1)) == 0) ||                                      \
    (((value) & ~((1U << ((bits) - 1)) - 1)) == ~((1U << ((bits) - 1)) - 1)))

///////////////////////////////////////////////////////////////////////////////

void asm_rv32_emit_word_opcode(asm_rv32_t *state, mp_uint_t word) {
    uint8_t *cursor = mp_asm_base_get_cur_to_write_bytes(&state->base, sizeof(uint32_t));
    if (cursor == NULL) {
        return;
    }

    #if MP_ENDIANNESS_LITTLE
    cursor[0] = word & 0xFF;
    cursor[1] = (word >> 8) & 0xFF;
    cursor[2] = (word >> 16) & 0xFF;
    cursor[3] = (word >> 24) & 0xFF;
    #else
    cursor[0] = (word >> 24) & 0xFF;
    cursor[1] = (word >> 16) & 0xFF;
    cursor[2] = (word >> 8) & 0xFF;
    cursor[3] = word & 0xFF;
    #endif
}

void asm_rv32_emit_halfword_opcode(asm_rv32_t *state, mp_uint_t word) {
    uint8_t *cursor = mp_asm_base_get_cur_to_write_bytes(&state->base, sizeof(uint16_t));
    if (cursor == NULL) {
        return;
    }

    #if MP_ENDIANNESS_LITTLE
    cursor[0] = word & 0xFF;
    cursor[1] = (word >> 8) & 0xFF;
    #else
    cursor[0] = (word >> 8) & 0xFF;
    cursor[1] = word & 0xFF;
    #endif
}

///////////////////////////////////////////////////////////////////////////////

static void split_immediate(mp_int_t immediate, mp_uint_t *upper, mp_uint_t *lower) {
    assert(upper != NULL && "Upper pointer is NULL.");
    assert(lower != NULL && "Lower pointer is NULL.");

    mp_uint_t unsigned_immediate = *((mp_uint_t *)&immediate);
    *upper = unsigned_immediate & 0xFFFFF000;
    *lower = unsigned_immediate & 0x00000FFF;

    // Turn the lower half from unsigned to signed.
    if ((*lower & 0x800) != 0) {
        *upper += 0x1000;
        *lower -= 0x1000;
    }
}

static void load_upper_immediate(asm_rv32_t *state, mp_uint_t rd, mp_uint_t immediate) {
    // if immediate fits in 17 bits and is ≠ 0:
    //   c.lui  rd, HI(immediate)
    // else:
    //   lui    rd, HI(immediate)
    if (FIT_SIGNED(immediate, 17) && ((immediate >> 12) != 0)) {
        asm_rv32_opcode_clui(state, rd, immediate);
    } else {
        asm_rv32_opcode_lui(state, rd, immediate);
    }
}

static void load_lower_immediate(asm_rv32_t *state, mp_uint_t rd, mp_uint_t immediate) {
    // WARNING: This must be executed on a register that has either been
    //          previously cleared or was the target of a LUI/C.LUI or
    //          AUIPC opcode.

    if (immediate == 0) {
        return;
    }

    // if LO(immediate) fits in 6 bits:
    //   c.addi rd, LO(immediate)
    // else:
    //   addi   rd, rd, LO(immediate)
    if (FIT_SIGNED(immediate, 6)) {
        asm_rv32_opcode_caddi(state, rd, immediate);
    } else {
        asm_rv32_opcode_addi(state, rd, rd, immediate);
    }
}

static void load_full_immediate(asm_rv32_t *state, mp_uint_t rd, mp_int_t immediate) {
    mp_uint_t upper = 0;
    mp_uint_t lower = 0;
    split_immediate(immediate, &upper, &lower);

    // if immediate fits in 17 bits:
    //   c.lui  rd, HI(immediate)
    // else:
    //   lui    rd, HI(immediate)
    // if LO(immediate) fits in 6 bits && LO(immediate) != 0:
    //   c.addi rd, LO(immediate)
    // else:
    //   addi   rd, rd, LO(immediate)
    load_upper_immediate(state, rd, upper);
    load_lower_immediate(state, rd, lower);
}

void asm_rv32_emit_optimised_load_immediate(asm_rv32_t *state, mp_uint_t rd, mp_int_t immediate) {
    if (FIT_SIGNED(immediate, 6)) {
        // c.li rd, immediate
        asm_rv32_opcode_cli(state, rd, immediate);
        return;
    }

    if (FIT_SIGNED(immediate, 12)) {
        // addi rd, zero, immediate
        asm_rv32_opcode_addi(state, rd, ASM_RV32_REG_ZERO, immediate);
        return;
    }

    load_full_immediate(state, rd, immediate);
}

// RV32 does not have dedicated push/pop opcodes, so series of loads and
// stores are generated in their place.

static void emit_registers_store(asm_rv32_t *state, mp_uint_t registers_mask) {
    mp_uint_t offset = 0;
    for (mp_uint_t register_index = 0; register_index < AVAILABLE_REGISTERS_COUNT; register_index++) {
        if (registers_mask & (1U << register_index)) {
            assert(FIT_UNSIGNED(offset >> 2, 6) && "Registers save stack offset out of range.");
            // c.swsp register, offset
            asm_rv32_opcode_cswsp(state, register_index, offset);
            offset += sizeof(uint32_t);
        }
    }
}

static void emit_registers_load(asm_rv32_t *state, mp_uint_t registers_mask) {
    mp_uint_t offset = 0;
    for (mp_uint_t register_index = 0; register_index < AVAILABLE_REGISTERS_COUNT; register_index++) {
        if (registers_mask & (1U << register_index)) {
            assert(FIT_UNSIGNED(offset >> 2, 6) && "Registers load stack offset out of range.");
            // c.lwsp register, offset
            asm_rv32_opcode_clwsp(state, register_index, offset);
            offset += sizeof(uint32_t);
        }
    }
}

static void adjust_stack(asm_rv32_t *state, mp_int_t stack_size) {
    if (stack_size == 0) {
        return;
    }

    if (FIT_SIGNED(stack_size, 6)) {
        // c.addi sp, stack_size
        asm_rv32_opcode_caddi(state, ASM_RV32_REG_SP, stack_size);
        return;
    }

    if (FIT_SIGNED(stack_size, 12)) {
        // addi sp, sp, stack_size
        asm_rv32_opcode_addi(state, ASM_RV32_REG_SP, ASM_RV32_REG_SP, stack_size);
        return;
    }

    // li    temporary, stack_size
    // c.add sp, temporary
    load_full_immediate(state, REG_TEMP0, stack_size);
    asm_rv32_opcode_cadd(state, ASM_RV32_REG_SP, REG_TEMP0);
}

// Generate a generic function entry prologue code sequence, setting up the
// stack to hold all the tainted registers and an arbitrary amount of space
// for locals.
static void emit_function_prologue(asm_rv32_t *state, mp_uint_t registers) {
    mp_uint_t registers_count = MP_POPCOUNT(registers);
    state->stack_size = (registers_count + state->locals_count) * sizeof(uint32_t);
    mp_uint_t old_saved_registers_mask = state->saved_registers_mask;
    // Move stack pointer up.
    adjust_stack(state, -state->stack_size);
    // Store registers at the top of the saved stack area.
    emit_registers_store(state, registers);
    state->locals_stack_offset = registers_count * sizeof(uint32_t);
    state->saved_registers_mask = old_saved_registers_mask;
}

// Restore registers and reset the stack pointer to its initial value.
static void emit_function_epilogue(asm_rv32_t *state, mp_uint_t registers) {
    mp_uint_t old_saved_registers_mask = state->saved_registers_mask;
    // Restore registers from the top of the stack area.
    emit_registers_load(state, registers);
    // Move stack pointer down.
    adjust_stack(state, state->stack_size);
    state->saved_registers_mask = old_saved_registers_mask;
}

///////////////////////////////////////////////////////////////////////////////

void asm_rv32_entry(asm_rv32_t *state, mp_uint_t locals) {
    state->saved_registers_mask |= (1U << REG_FUN_TABLE) | (1U << REG_LOCAL_1) | \
        (1U << REG_LOCAL_2) | (1U << REG_LOCAL_3);
    state->locals_count = locals;
    emit_function_prologue(state, state->saved_registers_mask);
}

void asm_rv32_exit(asm_rv32_t *state) {
    emit_function_epilogue(state, state->saved_registers_mask);
    // c.jr ra
    asm_rv32_opcode_cjr(state, ASM_RV32_REG_RA);
}

void asm_rv32_end_pass(asm_rv32_t *state) {
    (void)state;
}

void asm_rv32_emit_call_ind(asm_rv32_t *state, mp_uint_t index) {
    mp_uint_t offset = index * ASM_WORD_SIZE;
    state->saved_registers_mask |= (1U << ASM_RV32_REG_RA);

    if (FIT_UNSIGNED(offset, 11)) {
        // lw     temporary, offset(fun_table)
        // c.jalr temporary
        asm_rv32_opcode_lw(state, INTERNAL_TEMPORARY, REG_FUN_TABLE, offset);
        asm_rv32_opcode_cjalr(state, INTERNAL_TEMPORARY);
        return;
    }

    mp_uint_t upper = 0;
    mp_uint_t lower = 0;
    split_immediate(offset, &upper, &lower);

    // TODO: Can this clobber REG_TEMP[0:2]?

    // lui    temporary, HI(index) ; Or c.lui if possible
    // c.add  temporary, fun_table
    // lw     temporary, LO(index)(temporary)
    // c.jalr temporary
    load_upper_immediate(state, INTERNAL_TEMPORARY, upper);
    asm_rv32_opcode_cadd(state, INTERNAL_TEMPORARY, REG_FUN_TABLE);
    asm_rv32_opcode_lw(state, INTERNAL_TEMPORARY, INTERNAL_TEMPORARY, lower);
    asm_rv32_opcode_cjalr(state, INTERNAL_TEMPORARY);
}

void asm_rv32_emit_jump_if_reg_eq(asm_rv32_t *state, mp_uint_t rs1, mp_uint_t rs2, mp_uint_t label) {
    ptrdiff_t displacement = (ptrdiff_t)(state->base.label_offsets[label] - state->base.code_offset);

    // The least significant bit is ignored anyway.
    if (FIT_SIGNED(displacement, 13)) {
        // beq rs1, rs2, displacement
        asm_rv32_opcode_beq(state, rs1, rs2, displacement);
        return;
    }

    // Compensate for the initial BNE opcode.
    displacement -= ASM_WORD_SIZE;

    mp_uint_t upper = 0;
    mp_uint_t lower = 0;
    split_immediate(displacement, &upper, &lower);

    // TODO: Can this clobber REG_TEMP[0:2]?

    // bne   rs1, rs2, 12                      ; PC + 0
    // auipc temporary, HI(displacement)       ; PC + 4
    // jalr  zero, temporary, LO(displacement) ; PC + 8
    // ...                                     ; PC + 12
    asm_rv32_opcode_bne(state, rs1, rs2, 12);
    asm_rv32_opcode_auipc(state, INTERNAL_TEMPORARY, upper);
    asm_rv32_opcode_jalr(state, ASM_RV32_REG_ZERO, INTERNAL_TEMPORARY, lower);
}

void asm_rv32_emit_jump_if_reg_nonzero(asm_rv32_t *state, mp_uint_t rs, mp_uint_t label) {
    ptrdiff_t displacement = (ptrdiff_t)(state->base.label_offsets[label] - state->base.code_offset);

    // The least significant bit is ignored anyway.
    if (FIT_SIGNED(displacement, 13)) {
        // bne rs, zero, displacement
        asm_rv32_opcode_bne(state, rs, ASM_RV32_REG_ZERO, displacement);
        return;
    }

    // Compensate for the initial BEQ opcode.
    displacement -= ASM_WORD_SIZE;

    mp_uint_t upper = 0;
    mp_uint_t lower = 0;
    split_immediate(displacement, &upper, &lower);

    // TODO: Can this clobber REG_TEMP[0:2]?

    // beq   rs1, zero, 12                     ; PC + 0
    // auipc temporary, HI(displacement)       ; PC + 4
    // jalr  zero, temporary, LO(displacement) ; PC + 8
    // ...                                     ; PC + 12
    asm_rv32_opcode_beq(state, rs, ASM_RV32_REG_ZERO, 12);
    asm_rv32_opcode_auipc(state, INTERNAL_TEMPORARY, upper);
    asm_rv32_opcode_jalr(state, ASM_RV32_REG_ZERO, INTERNAL_TEMPORARY, lower);
}

void asm_rv32_emit_mov_local_reg(asm_rv32_t *state, mp_uint_t local, mp_uint_t rs) {
    mp_uint_t offset = state->locals_stack_offset + (local * ASM_WORD_SIZE);

    if (FIT_UNSIGNED(offset >> 2, 6)) {
        // c.swsp rs, offset
        asm_rv32_opcode_cswsp(state, rs, offset);
        return;
    }

    if (FIT_UNSIGNED(offset, 11)) {
        // sw rs, offset(sp)
        asm_rv32_opcode_sw(state, rs, ASM_RV32_REG_SP, offset);
        return;
    }

    mp_uint_t upper = 0;
    mp_uint_t lower = 0;
    split_immediate(offset, &upper, &lower);

    // TODO: Can this clobber REG_TEMP[0:2]?

    // lui   temporary, HI(offset) ; Or c.lui if possible
    // c.add temporary, sp
    // sw    rs, LO(offset)(temporary)
    load_upper_immediate(state, INTERNAL_TEMPORARY, upper);
    asm_rv32_opcode_cadd(state, INTERNAL_TEMPORARY, ASM_RV32_REG_SP);
    asm_rv32_opcode_sw(state, rs, INTERNAL_TEMPORARY, lower);
}

void asm_rv32_emit_mov_reg_local(asm_rv32_t *state, mp_uint_t rd, mp_uint_t local) {
    mp_uint_t offset = state->locals_stack_offset + (local * ASM_WORD_SIZE);

    if (FIT_UNSIGNED(offset >> 2, 6)) {
        // c.lwsp rd, offset
        asm_rv32_opcode_clwsp(state, rd, offset);
        return;
    }

    if (FIT_UNSIGNED(offset, 11)) {
        // lw rd, offset(sp)
        asm_rv32_opcode_lw(state, rd, ASM_RV32_REG_SP, offset);
        return;
    }

    mp_uint_t upper = 0;
    mp_uint_t lower = 0;
    split_immediate(offset, &upper, &lower);

    // lui   rd, HI(offset) ; Or c.lui if possible
    // c.add rd, sp
    // lw    rd, LO(offset)(rd)
    load_upper_immediate(state, rd, upper);
    asm_rv32_opcode_cadd(state, rd, ASM_RV32_REG_SP);
    asm_rv32_opcode_lw(state, rd, rd, lower);
}

void asm_rv32_emit_mov_reg_local_addr(asm_rv32_t *state, mp_uint_t rd, mp_uint_t local) {
    mp_uint_t offset = state->locals_stack_offset + (local * ASM_WORD_SIZE);

    if (FIT_SIGNED(offset, 11)) {
        // addi rd, sp, offset
        asm_rv32_opcode_addi(state, rd, ASM_RV32_REG_SP, offset);
        return;
    }

    // li    rd, offset
    // c.add rd, sp
    load_full_immediate(state, rd, offset);
    asm_rv32_opcode_cadd(state, rd, ASM_RV32_REG_SP);
}

void asm_rv32_emit_load_reg_reg_offset(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs, mp_int_t offset) {
    mp_int_t scaled_offset = offset * sizeof(ASM_WORD_SIZE);

    if (FIT_SIGNED(scaled_offset, 12)) {
        // lw rd, offset(rs)
        asm_rv32_opcode_lw(state, rd, rs, scaled_offset);
        return;
    }

    mp_uint_t upper = 0;
    mp_uint_t lower = 0;
    split_immediate(scaled_offset, &upper, &lower);

    // lui   rd, HI(offset) ; Or c.lui if possible
    // c.add rd, rs
    // lw    rd, LO(offset)(rd)
    load_upper_immediate(state, rd, upper);
    asm_rv32_opcode_cadd(state, rd, rs);
    asm_rv32_opcode_lw(state, rd, rd, lower);
}

void asm_rv32_emit_jump(asm_rv32_t *state, mp_uint_t label) {
    ptrdiff_t displacement = (ptrdiff_t)(state->base.label_offsets[label] - state->base.code_offset);

    // The least significant bit is ignored anyway.
    if (FIT_SIGNED(displacement, 13)) {
        // c.j displacement
        asm_rv32_opcode_cj(state, displacement);
        return;
    }

    mp_uint_t upper = 0;
    mp_uint_t lower = 0;
    split_immediate(displacement, &upper, &lower);

    // TODO: Can this clobber REG_TEMP[0:2]?

    // auipc temporary, HI(displacement)
    // jalr  zero, temporary, LO(displacement)
    asm_rv32_opcode_auipc(state, INTERNAL_TEMPORARY, upper);
    asm_rv32_opcode_jalr(state, ASM_RV32_REG_ZERO, INTERNAL_TEMPORARY, lower);
}

void asm_rv32_emit_store_reg_reg_offset(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs, mp_int_t offset) {
    mp_int_t scaled_offset = offset * ASM_WORD_SIZE;

    if (FIT_SIGNED(scaled_offset, 12)) {
        // sw rd, offset(rs)
        asm_rv32_opcode_sw(state, rd, rs, scaled_offset);
        return;
    }

    mp_uint_t upper = 0;
    mp_uint_t lower = 0;
    split_immediate(scaled_offset, &upper, &lower);

    // lui   rd, HI(offset) ; Or c.lui if possible
    // c.add rd, rs
    // sw    rd, LO(offset)(rd)
    load_upper_immediate(state, rd, upper);
    asm_rv32_opcode_cadd(state, rd, rs);
    asm_rv32_opcode_sw(state, rd, rd, lower);
}

void asm_rv32_emit_mov_reg_pcrel(asm_rv32_t *state, mp_uint_t rd, mp_uint_t label) {
    ptrdiff_t displacement = (ptrdiff_t)(state->base.label_offsets[label] - state->base.code_offset);
    mp_uint_t upper = 0;
    mp_uint_t lower = 0;
    split_immediate(displacement, &upper, &lower);

    // Compressed instructions are not used even if they may allow for code size
    // savings as the code changes size between compute and emit passes
    // otherwise.  If that happens then the assertion at asmbase.c:93 triggers
    // when built in debug mode.

    // auipc rd, HI(relative)
    // addi  rd, rd, LO(relative)
    asm_rv32_opcode_auipc(state, rd, upper);
    asm_rv32_opcode_addi(state, rd, rd, lower);
}

void asm_rv32_emit_load16_reg_reg_offset(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs, mp_int_t offset) {
    mp_int_t scaled_offset = offset * sizeof(uint16_t);

    if (FIT_SIGNED(scaled_offset, 12)) {
        // lhu rd, offset(rs)
        asm_rv32_opcode_lhu(state, rd, rs, scaled_offset);
        return;
    }

    mp_uint_t upper = 0;
    mp_uint_t lower = 0;
    split_immediate(scaled_offset, &upper, &lower);

    // lui   rd, HI(offset) ; Or c.lui if possible
    // c.add rd, rs
    // lhu   rd, LO(offset)(rd)
    load_upper_immediate(state, rd, upper);
    asm_rv32_opcode_cadd(state, rd, rs);
    asm_rv32_opcode_lhu(state, rd, rd, lower);
}

void asm_rv32_emit_optimised_xor(asm_rv32_t *state, mp_uint_t rd, mp_uint_t rs) {
    if (rs == rd) {
        // c.li rd, 0
        asm_rv32_opcode_cli(state, rd, 0);
        return;
    }

    // xor rd, rd, rs
    asm_rv32_opcode_xor(state, rd, rd, rs);
}

void asm_rv32_meta_comparison_eq(asm_rv32_t *state, mp_uint_t rs1, mp_uint_t rs2, mp_uint_t rd) {
    // c.li rd, 1       ;
    // beq  rs1, rs2, 8 ; PC + 0
    // addi rd, zero, 0 ; PC + 4
    // ...              ; PC + 8
    asm_rv32_opcode_cli(state, rd, 1);
    asm_rv32_opcode_beq(state, rs1, rs2, 8);
    asm_rv32_opcode_addi(state, rd, ASM_RV32_REG_ZERO, 0);
}

void asm_rv32_meta_comparison_ne(asm_rv32_t *state, mp_uint_t rs1, mp_uint_t rs2, mp_uint_t rd) {
    // sub  rd, rs1, rs2
    // sltu rd, zero, rd
    asm_rv32_opcode_sub(state, rd, rs1, rs2);
    asm_rv32_opcode_sltu(state, rd, ASM_RV32_REG_ZERO, rd);
}

void asm_rv32_meta_comparison_lt(asm_rv32_t *state, mp_uint_t rs1, mp_uint_t rs2, mp_uint_t rd, bool unsigned_comparison) {
    // slt(u) rd, rs1, rs2
    if (unsigned_comparison) {
        asm_rv32_opcode_sltu(state, rd, rs1, rs2);
    } else {
        asm_rv32_opcode_slt(state, rd, rs1, rs2);
    }
}

void asm_rv32_meta_comparison_le(asm_rv32_t *state, mp_uint_t rs1, mp_uint_t rs2, mp_uint_t rd, bool unsigned_comparison) {
    // c.li   rd, 1        ;
    // beq    rs1, rs2, 8  ; PC + 0
    // slt(u) rd, rs1, rs2 ; PC + 4
    // ...                 ; PC + 8
    asm_rv32_opcode_cli(state, rd, 1);
    asm_rv32_opcode_beq(state, rs1, rs2, 8);
    if (unsigned_comparison) {
        asm_rv32_opcode_sltu(state, rd, rs1, rs2);
    } else {
        asm_rv32_opcode_slt(state, rd, rs1, rs2);
    }
}

#endif // MICROPY_EMIT_RV32
