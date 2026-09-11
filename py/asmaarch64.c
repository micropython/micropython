/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Damien P. George
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

#if MICROPY_EMIT_AARCH64

#include "py/asmaarch64.h"

#define REG_TEMP ASM_AARCH64_REG_X9

static void emit(asm_aarch64_t *as, uint op) {
    uint8_t *c = mp_asm_base_get_cur_to_write_bytes(&as->base, 4);
    if (c != NULL) {
        *(uint32_t *)c = op;
    }
}

static uint asm_aarch64_op_add_reg(uint rd, uint rn, uint rm) {
    return 0x8b000000 | (rm << 16) | (rn << 5) | rd;
}

static uint asm_aarch64_op_sub_reg(uint rd, uint rn, uint rm) {
    return 0xcb000000 | (rm << 16) | (rn << 5) | rd;
}

static uint asm_aarch64_op_and_reg(uint rd, uint rn, uint rm) {
    return 0x8a000000 | (rm << 16) | (rn << 5) | rd;
}

static uint asm_aarch64_op_orr_reg(uint rd, uint rn, uint rm) {
    return 0xaa000000 | (rm << 16) | (rn << 5) | rd;
}

static uint asm_aarch64_op_eor_reg(uint rd, uint rn, uint rm) {
    return 0xca000000 | (rm << 16) | (rn << 5) | rd;
}

static uint asm_aarch64_op_mvn_reg(uint rd, uint rm) {
    return 0xaa2003e0 | (rm << 16) | rd;
}

static uint asm_aarch64_op_mov_reg(uint rd, uint rn) {
    return asm_aarch64_op_orr_reg(rd, 0x1f, rn);
}

static uint asm_aarch64_op_movz(uint rd, uint imm16, uint shift) {
    return 0xd2800000 | (shift << 21) | (imm16 << 5) | rd;
}

static uint asm_aarch64_op_movk(uint rd, uint imm16, uint shift) {
    return 0xf2800000 | (shift << 21) | (imm16 << 5) | rd;
}

static uint asm_aarch64_op_movn(uint rd, uint imm16, uint shift) {
    return 0x92800000 | (shift << 21) | (imm16 << 5) | rd;
}

static uint asm_aarch64_op_add_imm(uint rd, uint rn, uint imm12) {
    return 0x91000000 | (imm12 << 10) | (rn << 5) | rd;
}

static uint asm_aarch64_op_sub_imm(uint rd, uint rn, uint imm12);
static uint asm_aarch64_op_sub_imm(uint rd, uint rn, uint imm12) {
    return 0xd1000000 | (imm12 << 10) | (rn << 5) | rd;
}

static uint asm_aarch64_op_subs_imm(uint rd, uint rn, uint imm12) {
    return 0xf1000000 | (imm12 << 10) | (rn << 5) | rd;
}

static uint asm_aarch64_op_cmp_imm(uint rn, uint imm12) {
    return asm_aarch64_op_subs_imm(0x1f, rn, imm12);
}

static uint asm_aarch64_op_subs_reg(uint rd, uint rn, uint rm) {
    return 0xeb000000 | (rm << 16) | (rn << 5) | rd;
}

static uint asm_aarch64_op_cmp_reg(uint rn, uint rm) {
    return asm_aarch64_op_subs_reg(0x1f, rn, rm);
}

static uint asm_aarch64_op_mul_reg(uint rd, uint rn, uint rm) {
    return 0x9b007c00 | (rm << 16) | (rn << 5) | rd;
}

static uint asm_aarch64_op_ldr_reg_imm(uint rt, uint rn, uint imm12) {
    return 0xf9400000 | (imm12 << 10) | (rn << 5) | rt;
}

static uint asm_aarch64_op_ldr32_reg_imm(uint rt, uint rn, uint imm12) {
    return 0xb9400000 | (imm12 << 10) | (rn << 5) | rt;
}

static uint asm_aarch64_op_ldr_reg(uint rt, uint rn, uint rm) {
    return 0xf8606800 | (rm << 16) | (rn << 5) | rt;
}

static uint asm_aarch64_op_ldr32_reg(uint rt, uint rn, uint rm) {
    return 0xb8606800 | (rm << 16) | (rn << 5) | rt;
}

static uint asm_aarch64_op_ldrh_reg_imm(uint rt, uint rn, uint imm12) {
    return 0x79400000 | (imm12 << 10) | (rn << 5) | rt;
}

static uint asm_aarch64_op_ldrh_reg(uint rt, uint rn, uint rm) {
    return 0x78606800 | (rm << 16) | (rn << 5) | rt;
}

static uint asm_aarch64_op_ldrb_reg_imm(uint rt, uint rn, uint imm12) {
    return 0x39400000 | (imm12 << 10) | (rn << 5) | rt;
}

static uint asm_aarch64_op_ldrb_reg(uint rt, uint rn, uint rm) {
    return 0x38606800 | (rm << 16) | (rn << 5) | rt;
}

static uint asm_aarch64_op_str_reg_imm(uint rt, uint rn, uint imm12) {
    return 0xf9000000 | (imm12 << 10) | (rn << 5) | rt;
}

static uint asm_aarch64_op_str32_reg_imm(uint rt, uint rn, uint imm12) {
    return 0xb9000000 | (imm12 << 10) | (rn << 5) | rt;
}

static uint asm_aarch64_op_str_reg(uint rt, uint rn, uint rm) {
    return 0xf8206800 | (rm << 16) | (rn << 5) | rt;
}

static uint asm_aarch64_op_str32_reg(uint rt, uint rn, uint rm) {
    return 0xb8206800 | (rm << 16) | (rn << 5) | rt;
}

static uint asm_aarch64_op_strh_reg_imm(uint rt, uint rn, uint imm12) {
    return 0x79000000 | (imm12 << 10) | (rn << 5) | rt;
}

static uint asm_aarch64_op_strh_reg(uint rt, uint rn, uint rm) {
    return 0x78206800 | (rm << 16) | (rn << 5) | rt;
}

static uint asm_aarch64_op_strb_reg_imm(uint rt, uint rn, uint imm12) {
    return 0x39000000 | (imm12 << 10) | (rn << 5) | rt;
}

static uint asm_aarch64_op_strb_reg(uint rt, uint rn, uint rm) {
    return 0x38206800 | (rm << 16) | (rn << 5) | rt;
}

static uint asm_aarch64_op_stp_pre(uint rt1, uint rt2, uint rn, int imm7) {
    return 0xa9800000 | ((imm7 & 0x7f) << 15) | (rt2 << 10) | (rn << 5) | rt1;
}

static uint asm_aarch64_op_ldp_post(uint rt1, uint rt2, uint rn, int imm7) {
    return 0xa8c00000 | ((imm7 & 0x7f) << 15) | (rt2 << 10) | (rn << 5) | rt1;
}

static uint asm_aarch64_op_stp_signed(uint rt1, uint rt2, uint rn, int imm7) {
    return 0xa9000000 | ((imm7 & 0x7f) << 15) | (rt2 << 10) | (rn << 5) | rt1;
}

static uint asm_aarch64_op_ldp_signed(uint rt1, uint rt2, uint rn, int imm7) {
    return 0xa9400000 | ((imm7 & 0x7f) << 15) | (rt2 << 10) | (rn << 5) | rt1;
}

static uint asm_aarch64_op_lsl_reg(uint rd, uint rn, uint rm) {
    return 0x9ac02000 | (rm << 16) | (rn << 5) | rd;
}

static uint asm_aarch64_op_lsr_reg(uint rd, uint rn, uint rm) {
    return 0x9ac02400 | (rm << 16) | (rn << 5) | rd;
}

static uint asm_aarch64_op_asr_reg(uint rd, uint rn, uint rm) {
    return 0x9ac02800 | (rm << 16) | (rn << 5) | rd;
}

static uint asm_aarch64_op_neg_reg(uint rd, uint rn) {
    return asm_aarch64_op_sub_reg(rd, 0x1f, rn);
}

void asm_aarch64_bkpt(asm_aarch64_t *as) {
    emit(as, 0xd4200000);
}

void asm_aarch64_entry(asm_aarch64_t *as, int num_locals) {
    assert(num_locals >= 0);

    as->push_reglist = 1ULL << ASM_AARCH64_REG_X19
        | 1ULL << ASM_AARCH64_REG_X20
        | 1ULL << ASM_AARCH64_REG_X21
        | 1ULL << ASM_AARCH64_REG_X28
        | 1ULL << ASM_AARCH64_REG_X30;

    emit(as, asm_aarch64_op_stp_pre(ASM_AARCH64_REG_X29, ASM_AARCH64_REG_X30, ASM_AARCH64_REG_SP, -6));
    emit(as, asm_aarch64_op_stp_signed(ASM_AARCH64_REG_X21, ASM_AARCH64_REG_X28, ASM_AARCH64_REG_SP, 2));
    emit(as, asm_aarch64_op_stp_signed(ASM_AARCH64_REG_X19, ASM_AARCH64_REG_X20, ASM_AARCH64_REG_SP, 4));
    emit(as, asm_aarch64_op_add_imm(ASM_AARCH64_REG_X29, ASM_AARCH64_REG_SP, 0));

    as->stack_adjust = (num_locals * 8 + 15) & ~15;
    if (as->stack_adjust > 0) {
        if (as->stack_adjust < 4096) {
            emit(as, asm_aarch64_op_sub_imm(ASM_AARCH64_REG_SP, ASM_AARCH64_REG_SP, as->stack_adjust));
        } else {
            asm_aarch64_mov_reg_i64_optimised(as, REG_TEMP, as->stack_adjust);
            emit(as, asm_aarch64_op_sub_reg(ASM_AARCH64_REG_SP, ASM_AARCH64_REG_SP, REG_TEMP));
        }
    }
}

void asm_aarch64_exit(asm_aarch64_t *as) {
    if (as->stack_adjust > 0) {
        if (as->stack_adjust < 4096) {
            emit(as, asm_aarch64_op_add_imm(ASM_AARCH64_REG_SP, ASM_AARCH64_REG_SP, as->stack_adjust));
        } else {
            asm_aarch64_mov_reg_i64_optimised(as, REG_TEMP, as->stack_adjust);
            emit(as, asm_aarch64_op_add_reg(ASM_AARCH64_REG_SP, ASM_AARCH64_REG_SP, REG_TEMP));
        }
    }

    emit(as, asm_aarch64_op_ldp_signed(ASM_AARCH64_REG_X19, ASM_AARCH64_REG_X20, ASM_AARCH64_REG_SP, 4));
    emit(as, asm_aarch64_op_ldp_signed(ASM_AARCH64_REG_X21, ASM_AARCH64_REG_X28, ASM_AARCH64_REG_SP, 2));
    emit(as, asm_aarch64_op_ldp_post(ASM_AARCH64_REG_X29, ASM_AARCH64_REG_X30, ASM_AARCH64_REG_SP, 6));
    emit(as, 0xd65f03c0);
}

void asm_aarch64_push(asm_aarch64_t *as, uint64_t reglist) {
    uint regs[32];
    uint count = 0;
    for (int i = 0; i < 32; ++i) {
        if (reglist & (1ULL << i)) {
            regs[count++] = i;
        }
    }
    if (count == 0) {
        return;
    }

    uint num_slots = (count + 1) & ~1;
    uint num_pairs = num_slots / 2;

    // First pair: stp pre-index adjusts SP and stores; xzr pads odd count
    emit(as, asm_aarch64_op_stp_pre(regs[0],
        count > 1 ? regs[1] : 0x1f,
        ASM_AARCH64_REG_SP, -num_pairs * 2));

    for (uint p = 1; p < num_pairs; ++p) {
        uint r0 = (2 * p < count) ? regs[2 * p] : 0x1f;
        uint r1 = (2 * p + 1 < count) ? regs[2 * p + 1] : 0x1f;
        emit(as, asm_aarch64_op_stp_signed(r0, r1, ASM_AARCH64_REG_SP, p * 2));
    }
}

void asm_aarch64_pop(asm_aarch64_t *as, uint64_t reglist) {
    uint regs[32];
    uint count = 0;
    for (int i = 0; i < 32; ++i) {
        if (reglist & (1ULL << i)) {
            regs[count++] = i;
        }
    }
    if (count == 0) {
        return;
    }

    uint num_slots = (count + 1) & ~1;
    uint num_pairs = num_slots / 2;

    for (uint p = 0; p < num_pairs - 1; ++p) {
        uint r0 = (2 * p < count) ? regs[2 * p] : 0x1f;
        uint r1 = (2 * p + 1 < count) ? regs[2 * p + 1] : 0x1f;
        emit(as, asm_aarch64_op_ldp_signed(r0, r1, ASM_AARCH64_REG_SP, p * 2));
    }

    // Last pair: ldp post-index loads and adjusts SP
    {
        uint p = num_pairs - 1;
        uint r0 = (2 * p < count) ? regs[2 * p] : 0x1f;
        uint r1 = (2 * p + 1 < count) ? regs[2 * p + 1] : 0x1f;
        emit(as, asm_aarch64_op_ldp_post(r0, r1, ASM_AARCH64_REG_SP, num_pairs * 2));
    }
}

void asm_aarch64_mov_reg_reg(asm_aarch64_t *as, uint reg_dest, uint reg_src) {
    emit(as, asm_aarch64_op_mov_reg(reg_dest, reg_src));
}

void asm_aarch64_mov_reg_i64_optimised(asm_aarch64_t *as, uint rd, int64_t imm) {
    if (imm == 0) {
        emit(as, asm_aarch64_op_mov_reg(rd, 0x1f));
    } else if (imm > 0 && imm <= 0xffff) {
        emit(as, asm_aarch64_op_movz(rd, imm, 0));
    } else if (imm > 0 && imm <= 0xffffffff && (imm & 0xffff0000) == (uint)imm) {
        emit(as, asm_aarch64_op_movz(rd, imm >> 16, 1));
    } else if (imm < 0 && imm >= -65536) {
        emit(as, asm_aarch64_op_movn(rd, ~imm, 0));
    } else {
        // Load full 64-bit value using MOVZ + MOVK sequence.
        uint64_t u = (uint64_t)imm;
        emit(as, asm_aarch64_op_movz(rd, u & 0xffff, 0));
        if ((u >> 16) & 0xffff) {
            emit(as, asm_aarch64_op_movk(rd, (u >> 16) & 0xffff, 1));
        }
        if ((u >> 32) & 0xffff) {
            emit(as, asm_aarch64_op_movk(rd, (u >> 32) & 0xffff, 2));
        }
        if ((u >> 48) & 0xffff) {
            emit(as, asm_aarch64_op_movk(rd, (u >> 48) & 0xffff, 3));
        }
    }
}

void asm_aarch64_mov_local_reg(asm_aarch64_t *as, int local_num, uint rd) {
    emit(as, asm_aarch64_op_str_reg_imm(rd, ASM_AARCH64_REG_SP, local_num));
}

void asm_aarch64_mov_reg_local(asm_aarch64_t *as, uint rd, int local_num) {
    emit(as, asm_aarch64_op_ldr_reg_imm(rd, ASM_AARCH64_REG_SP, local_num));
}

void asm_aarch64_cmp_reg_i8(asm_aarch64_t *as, uint rd, int imm) {
    emit(as, asm_aarch64_op_cmp_imm(rd, imm));
}

void asm_aarch64_cmp_reg_reg(asm_aarch64_t *as, uint rd, uint rn) {
    emit(as, asm_aarch64_op_cmp_reg(rd, rn));
}

void asm_aarch64_setcc_reg(asm_aarch64_t *as, uint rd, uint cond) {
    // CSET Xd, cond = CSINC Xd, XZR, XZR, invert(cond)
    // if cond then Xd = 1, else Xd = 0
    emit(as, 0x9a9f07e0 | ((cond ^ 1) << 12) | rd);
}

void asm_aarch64_mvn_reg_reg(asm_aarch64_t *as, uint rd, uint rm) {
    emit(as, asm_aarch64_op_mvn_reg(rd, rm));
}

void asm_aarch64_add_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rn, uint rm) {
    emit(as, asm_aarch64_op_add_reg(rd, rn, rm));
}

void asm_aarch64_add_reg_reg_imm(asm_aarch64_t *as, uint rd, uint rn, uint imm12) {
    emit(as, asm_aarch64_op_add_imm(rd, rn, imm12));
}

void asm_aarch64_sub_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rn, uint rm) {
    emit(as, asm_aarch64_op_sub_reg(rd, rn, rm));
}

void asm_aarch64_sub_reg_reg_imm(asm_aarch64_t *as, uint rd, uint rn, uint imm12) {
    emit(as, asm_aarch64_op_sub_imm(rd, rn, imm12));
}

void asm_aarch64_rsb_reg_reg_imm(asm_aarch64_t *as, uint rd, uint rn, uint imm) {
    assert(imm == 0);
    emit(as, asm_aarch64_op_neg_reg(rd, rn));
}

void asm_aarch64_mul_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rn, uint rm) {
    emit(as, asm_aarch64_op_mul_reg(rd, rn, rm));
}

void asm_aarch64_and_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rn, uint rm) {
    emit(as, asm_aarch64_op_and_reg(rd, rn, rm));
}

void asm_aarch64_eor_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rn, uint rm) {
    emit(as, asm_aarch64_op_eor_reg(rd, rn, rm));
}

void asm_aarch64_orr_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rn, uint rm) {
    emit(as, asm_aarch64_op_orr_reg(rd, rn, rm));
}

void asm_aarch64_mov_reg_local_addr(asm_aarch64_t *as, uint rd, int local_num) {
    if (local_num >= 0 && local_num < 512) {
        emit(as, asm_aarch64_op_add_imm(rd, ASM_AARCH64_REG_SP, local_num * 8));
    } else {
        asm_aarch64_mov_reg_i64_optimised(as, REG_TEMP, local_num * 8);
        emit(as, asm_aarch64_op_add_reg(rd, ASM_AARCH64_REG_SP, REG_TEMP));
    }
}

void asm_aarch64_mov_reg_pcrel(asm_aarch64_t *as, uint reg_dest, uint label) {
    assert(label < as->base.max_num_labels);
    mp_uint_t dest = as->base.label_offsets[label];
    mp_int_t rel = (mp_int_t)dest - (mp_int_t)as->base.code_offset;

    // ADR Xd, label — PC-relative, ±1MB range.
    // Encoding: 0_immlo[1:0]_10000_immhi[18:0]_Rd[4:0]
    uint immlo = rel & 0x3;
    uint immhi = (rel >> 2) & 0x7ffff;
    emit(as, 0x10000000 | (immlo << 29) | (immhi << 5) | reg_dest);
}

void asm_aarch64_lsl_reg_reg(asm_aarch64_t *as, uint rd, uint rs) {
    emit(as, asm_aarch64_op_lsl_reg(rd, rd, rs));
}

void asm_aarch64_lsr_reg_reg(asm_aarch64_t *as, uint rd, uint rs) {
    emit(as, asm_aarch64_op_lsr_reg(rd, rd, rs));
}

void asm_aarch64_asr_reg_reg(asm_aarch64_t *as, uint rd, uint rs) {
    emit(as, asm_aarch64_op_asr_reg(rd, rd, rs));
}

void asm_aarch64_lsl_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rn, uint rm) {
    emit(as, asm_aarch64_op_lsl_reg(rd, rn, rm));
}

void asm_aarch64_lsr_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rn, uint rm) {
    emit(as, asm_aarch64_op_lsr_reg(rd, rn, rm));
}

void asm_aarch64_asr_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rn, uint rm) {
    emit(as, asm_aarch64_op_asr_reg(rd, rn, rm));
}

void asm_aarch64_lsl_reg_reg_imm(asm_aarch64_t *as, uint rd, uint rn, uint shift) {
    uint immr = (-shift) & 0x3f;
    uint imms = (63 - shift) & 0x3f;
    emit(as, 0xd3400000 | (immr << 16) | (imms << 10) | (rn << 5) | rd);
}

void asm_aarch64_lsr_reg_reg_imm(asm_aarch64_t *as, uint rd, uint rn, uint shift) {
    emit(as, 0xd340fc00 | ((shift & 0x3f) << 16) | (rn << 5) | rd);
}

void asm_aarch64_asr_reg_reg_imm(asm_aarch64_t *as, uint rd, uint rn, uint shift) {
    emit(as, 0x9340fc00 | ((shift & 0x3f) << 16) | (rn << 5) | rd);
}

void asm_aarch64_ldr_reg_reg_offset(asm_aarch64_t *as, uint rd, uint rn, uint byte_offset) {
    uint word_offset = byte_offset / 8;
    if (word_offset < 4096) {
        emit(as, asm_aarch64_op_ldr_reg_imm(rd, rn, word_offset));
    } else {
        asm_aarch64_mov_reg_i64_optimised(as, REG_TEMP, byte_offset);
        emit(as, asm_aarch64_op_ldr_reg(rd, rn, REG_TEMP));
    }
}

void asm_aarch64_ldr32_reg_reg_offset(asm_aarch64_t *as, uint rd, uint rn, uint byte_offset) {
    uint word_offset = byte_offset / 4;
    if (word_offset < 4096) {
        emit(as, asm_aarch64_op_ldr32_reg_imm(rd, rn, word_offset));
    } else {
        asm_aarch64_mov_reg_i64_optimised(as, REG_TEMP, byte_offset);
        emit(as, asm_aarch64_op_ldr32_reg(rd, rn, REG_TEMP));
    }
}

void asm_aarch64_ldrh_reg_reg_offset(asm_aarch64_t *as, uint rd, uint rn, uint byte_offset) {
    uint halfword_offset = byte_offset / 2;
    if (halfword_offset < 4096) {
        emit(as, asm_aarch64_op_ldrh_reg_imm(rd, rn, halfword_offset));
    } else {
        asm_aarch64_mov_reg_i64_optimised(as, REG_TEMP, byte_offset);
        emit(as, asm_aarch64_op_ldrh_reg(rd, rn, REG_TEMP));
    }
}

void asm_aarch64_ldrb_reg_reg_offset(asm_aarch64_t *as, uint rd, uint rn, uint byte_offset) {
    if (byte_offset < 4096) {
        emit(as, asm_aarch64_op_ldrb_reg_imm(rd, rn, byte_offset));
    } else {
        asm_aarch64_mov_reg_i64_optimised(as, REG_TEMP, byte_offset);
        emit(as, asm_aarch64_op_ldrb_reg(rd, rn, REG_TEMP));
    }
}

void asm_aarch64_str_reg_reg_offset(asm_aarch64_t *as, uint rd, uint rm, uint byte_offset) {
    uint word_offset = byte_offset / 8;
    if (word_offset < 4096) {
        emit(as, asm_aarch64_op_str_reg_imm(rd, rm, word_offset));
    } else {
        asm_aarch64_mov_reg_i64_optimised(as, REG_TEMP, byte_offset);
        emit(as, asm_aarch64_op_str_reg(rd, rm, REG_TEMP));
    }
}

void asm_aarch64_str32_reg_reg_offset(asm_aarch64_t *as, uint rd, uint rm, uint byte_offset) {
    uint word_offset = byte_offset / 4;
    if (word_offset < 4096) {
        emit(as, asm_aarch64_op_str32_reg_imm(rd, rm, word_offset));
    } else {
        asm_aarch64_mov_reg_i64_optimised(as, REG_TEMP, byte_offset);
        emit(as, asm_aarch64_op_str32_reg(rd, rm, REG_TEMP));
    }
}

void asm_aarch64_strh_reg_reg_offset(asm_aarch64_t *as, uint rd, uint rm, uint byte_offset) {
    uint halfword_offset = byte_offset / 2;
    if (halfword_offset < 4096) {
        emit(as, asm_aarch64_op_strh_reg_imm(rd, rm, halfword_offset));
    } else {
        asm_aarch64_mov_reg_i64_optimised(as, REG_TEMP, byte_offset);
        emit(as, asm_aarch64_op_strh_reg(rd, rm, REG_TEMP));
    }
}

void asm_aarch64_strb_reg_reg_offset(asm_aarch64_t *as, uint rd, uint rm, uint byte_offset) {
    if (byte_offset < 4096) {
        emit(as, asm_aarch64_op_strb_reg_imm(rd, rm, byte_offset));
    } else {
        asm_aarch64_mov_reg_i64_optimised(as, REG_TEMP, byte_offset);
        emit(as, asm_aarch64_op_strb_reg(rd, rm, REG_TEMP));
    }
}

void asm_aarch64_ldr_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rm, uint rn) {
    emit(as, 0xb8607800 | (rn << 16) | (rm << 5) | rd);
}

void asm_aarch64_ldrh_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rm, uint rn) {
    emit(as, 0x78607800 | (rn << 16) | (rm << 5) | rd);
}

void asm_aarch64_ldrb_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rm, uint rn) {
    emit(as, 0x38606800 | (rn << 16) | (rm << 5) | rd);
}

void asm_aarch64_str_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rm, uint rn) {
    emit(as, 0xb8207800 | (rn << 16) | (rm << 5) | rd);
}

void asm_aarch64_strh_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rm, uint rn) {
    emit(as, 0x78207800 | (rn << 16) | (rm << 5) | rd);
}

void asm_aarch64_strb_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rm, uint rn) {
    emit(as, 0x38206800 | (rn << 16) | (rm << 5) | rd);
}

void asm_aarch64_bcc_label(asm_aarch64_t *as, int cond, uint label) {
    assert(label < as->base.max_num_labels);
    mp_uint_t dest = as->base.label_offsets[label];
    mp_int_t rel = dest - as->base.code_offset;
    rel >>= 2;

    if (MP_FIT_SIGNED(19, rel)) {
        emit(as, 0x54000000 | (rel & 0x7ffff) << 5 | cond);
    } else {
        printf("asm_aarch64_bcc: branch does not fit in 19 bits\n");
    }
}

void asm_aarch64_b_label(asm_aarch64_t *as, uint label) {
    assert(label < as->base.max_num_labels);
    mp_uint_t dest = as->base.label_offsets[label];
    mp_int_t rel = dest - as->base.code_offset;
    rel >>= 2;

    if (MP_FIT_SIGNED(26, rel)) {
        emit(as, 0x14000000 | (rel & 0x3ffffff));
    } else {
        printf("asm_aarch64_b: branch does not fit in 26 bits\n");
    }
}

void asm_aarch64_bl_label(asm_aarch64_t *as, uint label) {
    assert(label < as->base.max_num_labels);
    mp_uint_t dest = as->base.label_offsets[label];
    mp_int_t rel = dest - as->base.code_offset;
    rel >>= 2;

    if (MP_FIT_SIGNED(26, rel)) {
        emit(as, 0x94000000 | (rel & 0x3ffffff));
    } else {
        printf("asm_aarch64_bl: branch does not fit in 26 bits\n");
    }
}

void asm_aarch64_bl_ind(asm_aarch64_t *as, uint fun_id, uint reg_temp) {
    (void)reg_temp;
    assert(fun_id < (0x1000 / 8));
    emit(as, 0xf9400390 | ((fun_id) << 10));
    emit(as, 0xd63f0200);
}

void asm_aarch64_br_reg(asm_aarch64_t *as, uint reg_src) {
    emit(as, 0xd61f0000 | (reg_src << 5));
}

#endif // MICROPY_EMIT_AARCH64
