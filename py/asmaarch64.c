/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 MicroPython Contributors
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

#include <assert.h>
#include <string.h>

#include "py/mpconfig.h"

#if MICROPY_EMIT_AARCH64

#include "py/asmaarch64.h"
#include "py/runtime.h" // for mp_raise_NotImplementedError

// Internal scratch register; see ASM_AARCH64_REG_TEMP_INTERNAL in py/asmaarch64.h.
#define REG_TEMP ASM_AARCH64_REG_TEMP_INTERNAL

static void emit(asm_aarch64_t *as, uint op) {
    uint8_t *c = mp_asm_base_get_cur_to_write_bytes(&as->base, 4);
    if (c != NULL) {
        // Write the bytes little-endian explicitly, so the emitter also works
        // on big-endian hosts (as in asmthumb.c and asmrv32.c).
        c[0] = op;
        c[1] = op >> 8;
        c[2] = op >> 16;
        c[3] = op >> 24;
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

static uint asm_aarch64_op_sub_imm(uint rd, uint rn, uint imm12) {
    return 0xd1000000 | (imm12 << 10) | (rn << 5) | rd;
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

// LDUR/STUR family; imm9 is a signed byte offset in -256..255.
static uint asm_aarch64_op_ldst_ur(uint base, uint rt, uint rn, int imm9) {
    return base | ((uint)(imm9 & 0x1ff) << 12) | (rn << 5) | rt;
}

#define ASM_AARCH64_OP_LDUR   (0xf8400000)
#define ASM_AARCH64_OP_LDUR32 (0xb8400000)
#define ASM_AARCH64_OP_LDURH  (0x78400000)
#define ASM_AARCH64_OP_LDURB  (0x38400000)
#define ASM_AARCH64_OP_STUR   (0xf8000000)
#define ASM_AARCH64_OP_STUR32 (0xb8000000)
#define ASM_AARCH64_OP_STURH  (0x78000000)
#define ASM_AARCH64_OP_STURB  (0x38000000)

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

// Report an out-of-range branch or ADR displacement.  Only raise on the emit
// pass: earlier passes must emit same-size code so the computed size matches
// (following asm_thumb_mov_local_check()).
static void asm_aarch64_range_error(asm_aarch64_t *as) {
    if (as->base.pass >= MP_ASM_PASS_EMIT) {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("branch or constant target too far in native method"));
    }
}

static void asm_aarch64_sp_adjust(asm_aarch64_t *as, uint amount, bool is_sub) {
    if (amount == 0) {
        return;
    }
    assert(amount % 16 == 0);
    uint hi = amount >> 12;
    uint lo = amount & 0xfff;
    if (hi > 4095) {
        // Frame size too large for the shifted ADD/SUB immediate pair.
        // Unreachable in practice (asm_aarch64_mov_local_check() raises
        // first), but guard so a release build reports an error instead of
        // silently truncating the frame size.  Raise on the emit pass only;
        // earlier passes emit a clamped sequence of the same size.
        if (as->base.pass >= MP_ASM_PASS_EMIT) {
            mp_raise_NotImplementedError(MP_ERROR_TEXT("too many locals for native method"));
        }
        hi &= 0xfff;
    }
    if (hi != 0) {
        assert(hi <= 4095);
        emit(as, (is_sub
            ? asm_aarch64_op_sub_imm(ASM_AARCH64_REG_SP, ASM_AARCH64_REG_SP, hi)
            : asm_aarch64_op_add_imm(ASM_AARCH64_REG_SP, ASM_AARCH64_REG_SP, hi)) | (1 << 22));
    }
    if (lo != 0) {
        emit(as, is_sub
            ? asm_aarch64_op_sub_imm(ASM_AARCH64_REG_SP, ASM_AARCH64_REG_SP, lo)
            : asm_aarch64_op_add_imm(ASM_AARCH64_REG_SP, ASM_AARCH64_REG_SP, lo));
    }
}

void asm_aarch64_entry(asm_aarch64_t *as, int num_locals) {
    assert(num_locals >= 0);

    // Save callee-saved registers used by the emitter (x19-x21 = REG_LOCAL_1-3,
    // x28 = REG_FUN_TABLE) plus fp/lr.
    emit(as, asm_aarch64_op_stp_pre(ASM_AARCH64_REG_X29, ASM_AARCH64_REG_X30, ASM_AARCH64_REG_SP, -6));
    emit(as, asm_aarch64_op_stp_signed(ASM_AARCH64_REG_X21, ASM_AARCH64_REG_X28, ASM_AARCH64_REG_SP, 2));
    emit(as, asm_aarch64_op_stp_signed(ASM_AARCH64_REG_X19, ASM_AARCH64_REG_X20, ASM_AARCH64_REG_SP, 4));
    emit(as, asm_aarch64_op_add_imm(ASM_AARCH64_REG_X29, ASM_AARCH64_REG_SP, 0));

    as->stack_adjust = ((uint)num_locals * 8 + 15) & ~(uint)15;
    asm_aarch64_sp_adjust(as, as->stack_adjust, true);
}

void asm_aarch64_exit(asm_aarch64_t *as) {
    asm_aarch64_sp_adjust(as, as->stack_adjust, false);

    emit(as, asm_aarch64_op_ldp_signed(ASM_AARCH64_REG_X19, ASM_AARCH64_REG_X20, ASM_AARCH64_REG_SP, 4));
    emit(as, asm_aarch64_op_ldp_signed(ASM_AARCH64_REG_X21, ASM_AARCH64_REG_X28, ASM_AARCH64_REG_SP, 2));
    emit(as, asm_aarch64_op_ldp_post(ASM_AARCH64_REG_X29, ASM_AARCH64_REG_X30, ASM_AARCH64_REG_SP, 6));
    emit(as, 0xd65f03c0);
}

void asm_aarch64_mov_reg_reg(asm_aarch64_t *as, uint reg_dest, uint reg_src) {
    emit(as, asm_aarch64_op_mov_reg(reg_dest, reg_src));
}

void asm_aarch64_mov_reg_i64_optimised(asm_aarch64_t *as, uint rd, int64_t imm) {
    uint64_t u = (uint64_t)imm;

    if (u == 0) {
        emit(as, asm_aarch64_op_mov_reg(rd, 0x1f)); // MOV Rd, XZR
        return;
    }

    // Split the value into 16-bit halfwords and count the non-zero ones
    // (num_nz) and the ones that are not 0xffff (num_not_ffff).  A MOVZ base
    // fills zero halfwords for free, a MOVN base fills 0xffff halfwords for
    // free, and MOVK patches the rest -- the strategy compilers use.
    uint16_t hw[4];
    int num_nz = 0;
    int num_not_ffff = 0;
    for (int i = 0; i < 4; ++i) {
        hw[i] = (u >> (i * 16)) & 0xffff;
        if (hw[i] != 0) {
            ++num_nz;
        }
        if (hw[i] != 0xffff) {
            ++num_not_ffff;
        }
    }

    if (num_nz == 1) {
        // Exactly one non-zero halfword: a single MOVZ at that position.
        for (int i = 0; i < 4; ++i) {
            if (hw[i] != 0) {
                emit(as, asm_aarch64_op_movz(rd, hw[i], i));
                break;
            }
        }
    } else if (num_not_ffff == 0) {
        // All halfwords are 0xffff (imm == -1): a single MOVN #0.
        emit(as, asm_aarch64_op_movn(rd, 0, 0));
    } else if (num_not_ffff == 1) {
        // Exactly one halfword is not 0xffff: a single MOVN of the inverse.
        for (int i = 0; i < 4; ++i) {
            if (hw[i] != 0xffff) {
                emit(as, asm_aarch64_op_movn(rd, ~hw[i] & 0xffff, i));
                break;
            }
        }
    } else {
        // General case: pick the cheaper base (MOVZ costs num_nz instructions,
        // MOVN costs num_not_ffff), placed on the first halfword that needs it;
        // patch the other non-default halfwords with MOVK.
        bool movn_base = num_not_ffff < num_nz;
        bool emitted_base = false;
        for (int i = 0; i < 4; ++i) {
            if (movn_base ? (hw[i] == 0xffff) : (hw[i] == 0)) {
                continue; // already correct once the base is emitted
            }
            if (!emitted_base) {
                emitted_base = true;
                if (movn_base) {
                    emit(as, asm_aarch64_op_movn(rd, ~hw[i] & 0xffff, i));
                } else {
                    emit(as, asm_aarch64_op_movz(rd, hw[i], i));
                }
            } else {
                emit(as, asm_aarch64_op_movk(rd, hw[i], i));
            }
        }
    }
}

// LDR/STR (immediate) scale the 12-bit field by 8, so slot numbers must stay
// below 4096 or the encoding overflows into neighbouring bits; report as in
// asm_thumb_mov_local_check().
static void asm_aarch64_mov_local_check(asm_aarch64_t *as, int local_num) {
    assert(local_num >= 0);
    if (as->base.pass >= MP_ASM_PASS_EMIT && (uint)local_num >= 4096) {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("too many locals for native method"));
    }
}

void asm_aarch64_mov_local_reg(asm_aarch64_t *as, int local_num, uint rd) {
    asm_aarch64_mov_local_check(as, local_num);
    emit(as, asm_aarch64_op_str_reg_imm(rd, ASM_AARCH64_REG_SP, local_num & 0xfff));
}

void asm_aarch64_mov_reg_local(asm_aarch64_t *as, uint rd, int local_num) {
    asm_aarch64_mov_local_check(as, local_num);
    emit(as, asm_aarch64_op_ldr_reg_imm(rd, ASM_AARCH64_REG_SP, local_num & 0xfff));
}

void asm_aarch64_cmp_reg_reg(asm_aarch64_t *as, uint rd, uint rn) {
    emit(as, asm_aarch64_op_cmp_reg(rd, rn));
}

void asm_aarch64_setcc_reg(asm_aarch64_t *as, uint rd, uint cond) {
    assert(cond <= ASM_AARCH64_CC_NV);
    if (cond >= ASM_AARCH64_CC_AL) {
        // CSINC cannot express AL/NV (a '111x' condition is always true in the
        // conditional-select class), so materialise the result with MOVZ
        // instead.  Still one instruction, so the code size is pass-stable.
        // (The native emitter only ever passes conditions 0..13.)
        emit(as, asm_aarch64_op_movz(rd, cond == ASM_AARCH64_CC_AL, 0));
        return;
    }
    // CSET Xd, cond = CSINC Xd, XZR, XZR, invert(cond)
    emit(as, 0x9a9f07e0 | ((cond ^ 1) << 12) | rd);
}

void asm_aarch64_mvn_reg_reg(asm_aarch64_t *as, uint rd, uint rm) {
    emit(as, asm_aarch64_op_mvn_reg(rd, rm));
}

void asm_aarch64_add_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rn, uint rm) {
    emit(as, asm_aarch64_op_add_reg(rd, rn, rm));
}

void asm_aarch64_sub_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rn, uint rm) {
    emit(as, asm_aarch64_op_sub_reg(rd, rn, rm));
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
    asm_aarch64_mov_local_check(as, local_num);
    uint byte_off = (uint)local_num * 8;
    uint hi = byte_off >> 12;
    uint lo = byte_off & 0xfff;
    if (hi == 0) {
        emit(as, asm_aarch64_op_add_imm(rd, ASM_AARCH64_REG_SP, lo));
        return;
    }
    assert(hi <= 4095);
    emit(as, asm_aarch64_op_add_imm(rd, ASM_AARCH64_REG_SP, hi) | (1 << 22));
    if (lo != 0) {
        emit(as, asm_aarch64_op_add_imm(rd, rd, lo));
    }
}

void asm_aarch64_mov_reg_pcrel(asm_aarch64_t *as, uint reg_dest, uint label) {
    assert(label < as->base.max_num_labels);
    mp_uint_t dest = as->base.label_offsets[label];
    mp_int_t rel = (mp_int_t)dest - (mp_int_t)as->base.code_offset;

    // ADR Xd, label: PC-relative, ±1MB range.  The code buffer is not
    // guaranteed to be page aligned, so ADRP+ADD cannot be used here.
    // Encoding: 0_immlo[1:0]_10000_immhi[18:0]_Rd[4:0]
    if (!MP_FIT_SIGNED(21, rel)) {
        // Out of ADR range; still emit a same-size truncated encoding so the
        // passes agree, and raise below.
        asm_aarch64_range_error(as);
    }
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

// Load/store with a signed constant byte offset: scaled LDR/STR (immediate)
// for aligned in-range offsets, LDUR/STUR for -256..255, otherwise the offset
// is materialised in REG_TEMP and the LDR/STR (register) form is used.
// Clobbering REG_TEMP (x11) is safe; see ASM_AARCH64_REG_TEMP_INTERNAL.
void asm_aarch64_ldr_reg_reg_offset(asm_aarch64_t *as, uint rd, uint rn, mp_int_t byte_offset) {
    if (byte_offset >= 0 && (byte_offset & 7) == 0 && byte_offset <= 4095 * 8) {
        emit(as, asm_aarch64_op_ldr_reg_imm(rd, rn, (uint)(byte_offset / 8)));
    } else if (-256 <= byte_offset && byte_offset <= 255) {
        emit(as, asm_aarch64_op_ldst_ur(ASM_AARCH64_OP_LDUR, rd, rn, (int)byte_offset));
    } else {
        asm_aarch64_mov_reg_i64_optimised(as, REG_TEMP, byte_offset);
        emit(as, asm_aarch64_op_ldr_reg(rd, rn, REG_TEMP));
    }
}

void asm_aarch64_ldr32_reg_reg_offset(asm_aarch64_t *as, uint rd, uint rn, mp_int_t byte_offset) {
    if (byte_offset >= 0 && (byte_offset & 3) == 0 && byte_offset <= 4095 * 4) {
        emit(as, asm_aarch64_op_ldr32_reg_imm(rd, rn, (uint)(byte_offset / 4)));
    } else if (-256 <= byte_offset && byte_offset <= 255) {
        emit(as, asm_aarch64_op_ldst_ur(ASM_AARCH64_OP_LDUR32, rd, rn, (int)byte_offset));
    } else {
        asm_aarch64_mov_reg_i64_optimised(as, REG_TEMP, byte_offset);
        emit(as, asm_aarch64_op_ldr32_reg(rd, rn, REG_TEMP));
    }
}

void asm_aarch64_ldrh_reg_reg_offset(asm_aarch64_t *as, uint rd, uint rn, mp_int_t byte_offset) {
    if (byte_offset >= 0 && (byte_offset & 1) == 0 && byte_offset <= 4095 * 2) {
        emit(as, asm_aarch64_op_ldrh_reg_imm(rd, rn, (uint)(byte_offset / 2)));
    } else if (-256 <= byte_offset && byte_offset <= 255) {
        emit(as, asm_aarch64_op_ldst_ur(ASM_AARCH64_OP_LDURH, rd, rn, (int)byte_offset));
    } else {
        asm_aarch64_mov_reg_i64_optimised(as, REG_TEMP, byte_offset);
        emit(as, asm_aarch64_op_ldrh_reg(rd, rn, REG_TEMP));
    }
}

void asm_aarch64_ldrb_reg_reg_offset(asm_aarch64_t *as, uint rd, uint rn, mp_int_t byte_offset) {
    if (byte_offset >= 0 && byte_offset <= 4095) {
        emit(as, asm_aarch64_op_ldrb_reg_imm(rd, rn, (uint)byte_offset));
    } else if (-256 <= byte_offset && byte_offset < 0) {
        emit(as, asm_aarch64_op_ldst_ur(ASM_AARCH64_OP_LDURB, rd, rn, (int)byte_offset));
    } else {
        asm_aarch64_mov_reg_i64_optimised(as, REG_TEMP, byte_offset);
        emit(as, asm_aarch64_op_ldrb_reg(rd, rn, REG_TEMP));
    }
}

void asm_aarch64_str_reg_reg_offset(asm_aarch64_t *as, uint rd, uint rm, mp_int_t byte_offset) {
    if (byte_offset >= 0 && (byte_offset & 7) == 0 && byte_offset <= 4095 * 8) {
        emit(as, asm_aarch64_op_str_reg_imm(rd, rm, (uint)(byte_offset / 8)));
    } else if (-256 <= byte_offset && byte_offset <= 255) {
        emit(as, asm_aarch64_op_ldst_ur(ASM_AARCH64_OP_STUR, rd, rm, (int)byte_offset));
    } else {
        asm_aarch64_mov_reg_i64_optimised(as, REG_TEMP, byte_offset);
        emit(as, asm_aarch64_op_str_reg(rd, rm, REG_TEMP));
    }
}

void asm_aarch64_str32_reg_reg_offset(asm_aarch64_t *as, uint rd, uint rm, mp_int_t byte_offset) {
    if (byte_offset >= 0 && (byte_offset & 3) == 0 && byte_offset <= 4095 * 4) {
        emit(as, asm_aarch64_op_str32_reg_imm(rd, rm, (uint)(byte_offset / 4)));
    } else if (-256 <= byte_offset && byte_offset <= 255) {
        emit(as, asm_aarch64_op_ldst_ur(ASM_AARCH64_OP_STUR32, rd, rm, (int)byte_offset));
    } else {
        asm_aarch64_mov_reg_i64_optimised(as, REG_TEMP, byte_offset);
        emit(as, asm_aarch64_op_str32_reg(rd, rm, REG_TEMP));
    }
}

void asm_aarch64_strh_reg_reg_offset(asm_aarch64_t *as, uint rd, uint rm, mp_int_t byte_offset) {
    if (byte_offset >= 0 && (byte_offset & 1) == 0 && byte_offset <= 4095 * 2) {
        emit(as, asm_aarch64_op_strh_reg_imm(rd, rm, (uint)(byte_offset / 2)));
    } else if (-256 <= byte_offset && byte_offset <= 255) {
        emit(as, asm_aarch64_op_ldst_ur(ASM_AARCH64_OP_STURH, rd, rm, (int)byte_offset));
    } else {
        asm_aarch64_mov_reg_i64_optimised(as, REG_TEMP, byte_offset);
        emit(as, asm_aarch64_op_strh_reg(rd, rm, REG_TEMP));
    }
}

void asm_aarch64_strb_reg_reg_offset(asm_aarch64_t *as, uint rd, uint rm, mp_int_t byte_offset) {
    if (byte_offset >= 0 && byte_offset <= 4095) {
        emit(as, asm_aarch64_op_strb_reg_imm(rd, rm, (uint)byte_offset));
    } else if (-256 <= byte_offset && byte_offset < 0) {
        emit(as, asm_aarch64_op_ldst_ur(ASM_AARCH64_OP_STURB, rd, rm, (int)byte_offset));
    } else {
        asm_aarch64_mov_reg_i64_optimised(as, REG_TEMP, byte_offset);
        emit(as, asm_aarch64_op_strb_reg(rd, rm, REG_TEMP));
    }
}

void asm_aarch64_ldr32_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rm, uint rn) {
    emit(as, 0xb8607800 | (rn << 16) | (rm << 5) | rd);
}

void asm_aarch64_ldrh_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rm, uint rn) {
    emit(as, 0x78607800 | (rn << 16) | (rm << 5) | rd);
}

void asm_aarch64_ldrb_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rm, uint rn) {
    emit(as, 0x38606800 | (rn << 16) | (rm << 5) | rd);
}

void asm_aarch64_str32_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rm, uint rn) {
    emit(as, 0xb8207800 | (rn << 16) | (rm << 5) | rd);
}

void asm_aarch64_strh_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rm, uint rn) {
    emit(as, 0x78207800 | (rn << 16) | (rm << 5) | rd);
}

void asm_aarch64_strb_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rm, uint rn) {
    emit(as, 0x38206800 | (rn << 16) | (rm << 5) | rd);
}

// In the branch functions below, a label number >= max_num_labels, or one not
// yet assigned in the current pass, means "unknown".  An unknown label must be
// treated as a forward branch so the emitted size matches across passes.
void asm_aarch64_bcc_label(asm_aarch64_t *as, int cond, uint label) {
    if (label < as->base.max_num_labels) {
        mp_uint_t dest = as->base.label_offsets[label];
        if (dest != (mp_uint_t)-1) {
            mp_int_t rel = (mp_int_t)dest - (mp_int_t)as->base.code_offset;
            if (rel < 0 && MP_FIT_SIGNED(19, rel >> 2)) {
                // Backwards branch within range: single B.cond.
                emit(as, 0x54000000 | (((rel >> 2) & 0x7ffff) << 5) | cond);
                return;
            }
        }
    }
    // Unknown, forward, or out-of-range backward branch: jump over a plain B
    // with the inverted condition.
    emit(as, 0x54000000 | ((8 >> 2) << 5) | (cond ^ 1));
    asm_aarch64_b_label(as, label);
}

void asm_aarch64_b_label(asm_aarch64_t *as, uint label) {
    mp_int_t rel = 0;
    if (label < as->base.max_num_labels) {
        mp_uint_t dest = as->base.label_offsets[label];
        rel = ((mp_int_t)dest - (mp_int_t)as->base.code_offset) >> 2;
        if (!MP_FIT_SIGNED(26, rel)) {
            // Out of range (code larger than ±128MB); emit a same-size
            // truncated encoding and raise below.
            asm_aarch64_range_error(as);
        }
    }
    emit(as, 0x14000000 | (rel & 0x3ffffff));
}

static void asm_aarch64_cbz_nz_label(asm_aarch64_t *as, uint reg, uint label, bool nz) {
    if (label < as->base.max_num_labels) {
        mp_uint_t dest = as->base.label_offsets[label];
        if (dest != (mp_uint_t)-1) {
            mp_int_t rel = (mp_int_t)dest - (mp_int_t)as->base.code_offset;
            if (rel < 0 && MP_FIT_SIGNED(19, rel >> 2)) {
                // Backwards branch within range: single CBZ/CBNZ.
                emit(as, (nz ? 0xb5000000 : 0xb4000000) | (((rel >> 2) & 0x7ffff) << 5) | reg);
                return;
            }
        }
    }
    // Unknown, forward, or long backward branch: jump over a plain B with the
    // inverted sense.
    emit(as, (nz ? 0xb4000000 : 0xb5000000) | ((8 >> 2) << 5) | reg);
    asm_aarch64_b_label(as, label);
}

void asm_aarch64_cbz_label(asm_aarch64_t *as, uint reg, uint label) {
    asm_aarch64_cbz_nz_label(as, reg, label, false);
}

void asm_aarch64_cbnz_label(asm_aarch64_t *as, uint reg, uint label) {
    asm_aarch64_cbz_nz_label(as, reg, label, true);
}

void asm_aarch64_bl_ind(asm_aarch64_t *as, uint fun_id) {
    assert(fun_id < (0x1000 / 8));
    emit(as, 0xf9400390 | ((fun_id) << 10)); // ldr x16, [x28, #fun_id * 8]
    emit(as, 0xd63f0200);                    // blr x16
}

void asm_aarch64_br_reg(asm_aarch64_t *as, uint reg_src) {
    emit(as, 0xd61f0000 | (reg_src << 5));
}

#endif // MICROPY_EMIT_AARCH64
