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

#if MICROPY_EMIT_AARCH64 || MICROPY_EMIT_INLINE_AARCH64

#include "py/asmaarch64.h"
#include "py/runtime.h" // for mp_raise_NotImplementedError

// Scratch register used by the helpers in this file.  See the comment on
// ASM_AARCH64_REG_TEMP_INTERNAL in py/asmaarch64.h for why X11 is safe.
#define REG_TEMP ASM_AARCH64_REG_TEMP_INTERNAL

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

static uint asm_aarch64_op_sub_imm(uint rd, uint rn, uint imm12) {
    return 0xd1000000 | (imm12 << 10) | (rn << 5) | rd;
}

static uint asm_aarch64_op_subs_imm(uint rd, uint rn, uint imm12) {
    return 0xf1000000 | (imm12 << 10) | (rn << 5) | rd;
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

// Unscaled-immediate loads/stores (LDUR/STUR family), imm9 is a signed byte
// offset in the range -256..255.
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

void asm_aarch64_bkpt(asm_aarch64_t *as) {
    emit(as, 0xd4200000);
}

// Report an out-of-range branch or ADR displacement.  Following the precedent
// set by asm_thumb_mov_local_check(), the error is only raised on the final
// pass: the earlier passes must still emit an instruction of the correct size,
// so that the code size they compute matches the size written by the emit pass
// (mp_asm_base_label_assign() asserts that label offsets do not change).
static void asm_aarch64_range_error(asm_aarch64_t *as) {
    if (as->base.pass >= MP_ASM_PASS_EMIT) {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("branch target too far in native method"));
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
        // The frame size cannot be encoded by the shifted ADD/SUB immediate
        // pair (it would need a 13-bit "hi" field).  This is unreachable for
        // functions the compiler can build -- local slots are limited to 4096
        // by asm_aarch64_mov_local_check(), which raises first -- but guard
        // it here too so a release build reports an error instead of
        // silently truncating the frame size and corrupting the stack.
        // Following asm_aarch64_mov_local_check(): raise on the emit pass,
        // and emit a (clamped) sequence of the same shape on earlier passes.
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

static void asm_aarch64_entry_common(asm_aarch64_t *as, int num_locals, bool save_all_callee_saved) {
    assert(num_locals >= 0);

    as->save_all_callee_saved = save_all_callee_saved;

    if (save_all_callee_saved) {
        // Save all callee-saved registers (x19-x28) plus fp/lr (6 pairs).
        // Used by the inline assembler, where user code may use any register.
        emit(as, asm_aarch64_op_stp_pre(ASM_AARCH64_REG_X29, ASM_AARCH64_REG_X30, ASM_AARCH64_REG_SP, -12));
        emit(as, asm_aarch64_op_stp_signed(ASM_AARCH64_REG_X19, ASM_AARCH64_REG_X20, ASM_AARCH64_REG_SP, 2));
        emit(as, asm_aarch64_op_stp_signed(ASM_AARCH64_REG_X21, ASM_AARCH64_REG_X22, ASM_AARCH64_REG_SP, 4));
        emit(as, asm_aarch64_op_stp_signed(ASM_AARCH64_REG_X23, ASM_AARCH64_REG_X24, ASM_AARCH64_REG_SP, 6));
        emit(as, asm_aarch64_op_stp_signed(ASM_AARCH64_REG_X25, ASM_AARCH64_REG_X26, ASM_AARCH64_REG_SP, 8));
        emit(as, asm_aarch64_op_stp_signed(ASM_AARCH64_REG_X27, ASM_AARCH64_REG_X28, ASM_AARCH64_REG_SP, 10));
    } else {
        // Save only the callee-saved registers used by the native emitter
        // (x19-x21 as REG_LOCAL_1-3 and x28 as REG_FUN_TABLE) plus fp/lr.
        emit(as, asm_aarch64_op_stp_pre(ASM_AARCH64_REG_X29, ASM_AARCH64_REG_X30, ASM_AARCH64_REG_SP, -6));
        emit(as, asm_aarch64_op_stp_signed(ASM_AARCH64_REG_X21, ASM_AARCH64_REG_X28, ASM_AARCH64_REG_SP, 2));
        emit(as, asm_aarch64_op_stp_signed(ASM_AARCH64_REG_X19, ASM_AARCH64_REG_X20, ASM_AARCH64_REG_SP, 4));
    }
    emit(as, asm_aarch64_op_add_imm(ASM_AARCH64_REG_X29, ASM_AARCH64_REG_SP, 0));

    as->stack_adjust = ((uint)num_locals * 8 + 15) & ~(uint)15;
    asm_aarch64_sp_adjust(as, as->stack_adjust, true);
}

void asm_aarch64_entry(asm_aarch64_t *as, int num_locals) {
    asm_aarch64_entry_common(as, num_locals, false);
}

void asm_aarch64_entry_full(asm_aarch64_t *as, int num_locals) {
    asm_aarch64_entry_common(as, num_locals, true);
}

void asm_aarch64_exit(asm_aarch64_t *as) {
    asm_aarch64_sp_adjust(as, as->stack_adjust, false);

    if (as->save_all_callee_saved) {
        emit(as, asm_aarch64_op_ldp_signed(ASM_AARCH64_REG_X27, ASM_AARCH64_REG_X28, ASM_AARCH64_REG_SP, 10));
        emit(as, asm_aarch64_op_ldp_signed(ASM_AARCH64_REG_X25, ASM_AARCH64_REG_X26, ASM_AARCH64_REG_SP, 8));
        emit(as, asm_aarch64_op_ldp_signed(ASM_AARCH64_REG_X23, ASM_AARCH64_REG_X24, ASM_AARCH64_REG_SP, 6));
        emit(as, asm_aarch64_op_ldp_signed(ASM_AARCH64_REG_X21, ASM_AARCH64_REG_X22, ASM_AARCH64_REG_SP, 4));
        emit(as, asm_aarch64_op_ldp_signed(ASM_AARCH64_REG_X19, ASM_AARCH64_REG_X20, ASM_AARCH64_REG_SP, 2));
        emit(as, asm_aarch64_op_ldp_post(ASM_AARCH64_REG_X29, ASM_AARCH64_REG_X30, ASM_AARCH64_REG_SP, 12));
    } else {
        emit(as, asm_aarch64_op_ldp_signed(ASM_AARCH64_REG_X19, ASM_AARCH64_REG_X20, ASM_AARCH64_REG_SP, 4));
        emit(as, asm_aarch64_op_ldp_signed(ASM_AARCH64_REG_X21, ASM_AARCH64_REG_X28, ASM_AARCH64_REG_SP, 2));
        emit(as, asm_aarch64_op_ldp_post(ASM_AARCH64_REG_X29, ASM_AARCH64_REG_X30, ASM_AARCH64_REG_SP, 6));
    }
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

    // Note on register 31: in the STP/LDP encodings the *base* register field
    // (Rn) selects SP, but the *transfer* register fields (Rt, Rt2) select XZR.
    // So padding an odd register count with 0x1f stores/loads the zero register
    // and leaves SP alone.  (ARM ARM: LDP/STP operand symbols are Xt1OrXZR /
    // Xt2OrXZR for the transfer registers and Xn|SP for the base.)

    // First pair: stp pre-index adjusts SP and stores; xzr pads odd count.
    emit(as, asm_aarch64_op_stp_pre(regs[0],
        count > 1 ? regs[1] : 0x1f,
        ASM_AARCH64_REG_SP, -(int)(num_pairs * 2)));

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

    if (num_pairs == 1) {
        // The only pair sits at [sp], so the post-indexed form loads it and
        // restores sp with a single instruction.
        emit(as, asm_aarch64_op_ldp_post(regs[0], count > 1 ? regs[1] : 0x1f,
            ASM_AARCH64_REG_SP, 2));
        return;
    }

    // LDP (post-indexed) always loads from [sp + 0]: its imm7 field is the
    // writeback amount, not a load displacement.  With more than one pair the
    // last pair lives at [sp + (num_pairs - 1) * 16], so it cannot use the
    // post-indexed form -- load every pair with a signed offset and restore sp
    // once at the end.  num_slots <= 32, so num_slots * 8 <= 256 always fits
    // in the ADD imm12 field and no scratch register is needed.
    for (uint p = 0; p < num_pairs; ++p) {
        uint r0 = (2 * p < count) ? regs[2 * p] : 0x1f;
        uint r1 = (2 * p + 1 < count) ? regs[2 * p + 1] : 0x1f;
        emit(as, asm_aarch64_op_ldp_signed(r0, r1, ASM_AARCH64_REG_SP, p * 2));
    }
    emit(as, asm_aarch64_op_add_imm(ASM_AARCH64_REG_SP, ASM_AARCH64_REG_SP, num_slots * 8));
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

    // Split the value into four 16-bit halfwords and count how many are
    // non-zero (num_nz) and how many are not 0xffff (num_not_ffff).  These
    // counts drive the same strategy that the compilers use to materialise
    // a 64-bit constant: a MOVZ base fills the zero halfwords for free, a
    // MOVN base fills the 0xffff halfwords for free, and MOVK patches the
    // remaining ones.
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
        // Exactly one halfword is not 0xffff: a single MOVN with the
        // inverted immediate at that position.
        for (int i = 0; i < 4; ++i) {
            if (hw[i] != 0xffff) {
                emit(as, asm_aarch64_op_movn(rd, ~hw[i] & 0xffff, i));
                break;
            }
        }
    } else {
        // General case: a MOVZ base needs num_nz instructions in total and
        // a MOVN base needs num_not_ffff, so pick the cheaper one.  The
        // base instruction is placed on the first halfword that actually
        // needs it (a zero hw0 no longer wastes an instruction), and every
        // other halfword that differs from the base default (0x0000 for
        // MOVZ, 0xffff for MOVN) is patched up with MOVK.
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

// STR/LDR (immediate, unsigned offset) scale the 12-bit field by 8, so a local
// slot number must be less than 4096.  Beyond that the field would overflow
// into the neighbouring bits of the encoding and silently access the wrong
// slot, so report it the same way asm_thumb_mov_local_check() does.
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

void asm_aarch64_cmp_reg_imm(asm_aarch64_t *as, uint rn, uint32_t imm) {
    // imm is a 12-bit value, optionally shifted left by 12 bits.
    uint shift = 0;
    if (imm > 0xfff) {
        assert((imm & 0xfff) == 0 && imm <= 0xfff000);
        imm >>= 12;
        shift = 1;
    }
    emit(as, asm_aarch64_op_subs_imm(0x1f, rn, imm) | (shift << 22));
}

void asm_aarch64_cmp_reg_reg(asm_aarch64_t *as, uint rd, uint rn) {
    emit(as, asm_aarch64_op_cmp_reg(rd, rn));
}

void asm_aarch64_setcc_reg(asm_aarch64_t *as, uint rd, uint cond) {
    assert(cond <= ASM_AARCH64_CC_NV);
    if (cond >= ASM_AARCH64_CC_AL) {
        // CSET (CSINC Xd, XZR, XZR, invert(cond)) cannot express AL or NV:
        // in AArch64 a condition field of '111x' always evaluates to true in
        // the conditional-select class, so the CSINC form would set rd to 0
        // for both AL and NV (GNU as likewise rejects "cset xd, al/nv").
        // Materialise the logical result directly instead; this is still a
        // single 4-byte instruction, so code size is pass-stable.  (The
        // native emitter only ever passes conditions 0..13.)
        emit(as, asm_aarch64_op_movz(rd, cond == ASM_AARCH64_CC_AL, 0));
        return;
    }
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

void asm_aarch64_add_reg_reg_imm(asm_aarch64_t *as, uint rd, uint rn, uint32_t imm) {
    // imm is a 12-bit value, optionally shifted left by 12 bits.
    uint shift = 0;
    if (imm > 0xfff) {
        assert((imm & 0xfff) == 0 && imm <= 0xfff000);
        imm >>= 12;
        shift = 1;
    }
    emit(as, asm_aarch64_op_add_imm(rd, rn, imm) | (shift << 22));
}

void asm_aarch64_sub_reg_reg_reg(asm_aarch64_t *as, uint rd, uint rn, uint rm) {
    emit(as, asm_aarch64_op_sub_reg(rd, rn, rm));
}

void asm_aarch64_sub_reg_reg_imm(asm_aarch64_t *as, uint rd, uint rn, uint32_t imm) {
    // imm is a 12-bit value, optionally shifted left by 12 bits.
    uint shift = 0;
    if (imm > 0xfff) {
        assert((imm & 0xfff) == 0 && imm <= 0xfff000);
        imm >>= 12;
        shift = 1;
    }
    emit(as, asm_aarch64_op_sub_imm(rd, rn, imm) | (shift << 22));
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

bool asm_aarch64_encode_bitmask_imm(uint64_t val, uint *n, uint *immr, uint *imms) {
    // A logical (bitmask) immediate is a run of S+1 consecutive ones within a
    // field of E bits (E a power of two, 2 <= E <= 64, the element neither all
    // zeros nor all ones), rotated right by R and replicated to fill 64 bits.
    // Find E, R and S for the given value, if it has such a form.
    if (val == 0 || val == ~(uint64_t)0) {
        return false;
    }

    // Find the smallest element size E such that val is a replication of its
    // lowest E bits.  E=64 always works.
    uint esize = 64;
    for (uint e = 2; e < 64; e *= 2) {
        uint64_t elem = val & (((uint64_t)1 << e) - 1);
        uint64_t rep = 0;
        for (uint b = 0; b < 64; b += e) {
            rep |= elem << b;
        }
        if (rep == val) {
            esize = e;
            break;
        }
    }

    uint64_t emask = (esize == 64) ? ~(uint64_t)0 : (((uint64_t)1 << esize) - 1);
    uint64_t elem = val & emask;

    // Count the number of one bits in the element.
    uint ones = 0;
    for (uint64_t t = elem; t != 0; t &= t - 1) {
        ++ones;
    }

    // The element must equal the pattern of `ones` consecutive one-bits
    // (starting at bit 0) rotated right by immr; find immr.
    uint64_t target = ((uint64_t)1 << ones) - 1;
    uint rot;
    for (rot = 0; rot < esize; ++rot) {
        uint64_t r = (rot == 0) ? target : (((target >> rot) | (target << (esize - rot))) & emask);
        if (r == elem) {
            break;
        }
    }
    if (rot == esize) {
        return false; // the one bits are not contiguous (circularly)
    }

    // len = log2(esize); N is only set when esize == 64.
    uint len = 0;
    while (((uint)1 << len) < esize) {
        ++len;
    }
    *n = (len == 6);
    *immr = rot & 0x3f;
    *imms = (0x3f & ~((1u << (len + 1)) - 1)) | ((ones - 1) & 0x3f);
    return true;
}

void asm_aarch64_logic_reg_reg_imm(asm_aarch64_t *as, uint op, uint rd, uint rn, uint64_t imm) {
    assert(op <= ASM_AARCH64_LOGIC_EOR);

    uint n, immr, imms;
    if (asm_aarch64_encode_bitmask_imm(imm, &n, &immr, &imms)) {
        // AND/ORR/EOR (immediate) with a bitmask immediate.
        static const uint imm_ops[3] = {0x92000000, 0xb2000000, 0xd2000000};
        emit(as, imm_ops[op] | (n << 22) | (immr << 16) | (imms << 10) | (rn << 5) | rd);
        return;
    }

    // The immediate can't be encoded directly: materialise it in a scratch
    // register and use the shifted-register form.  The scratch register must
    // not be rn (which is read after the scratch is loaded); rd is fine
    // because it is only written by the operation itself.  Note that for
    // inline-assembler code the user may hold live values in any register;
    // x9 and x10 are picked as the least surprising scratch registers.
    static const uint reg_ops[3] = {0x8a000000, 0xaa000000, 0xca000000};
    uint scratch = (rn != ASM_AARCH64_REG_X9) ? ASM_AARCH64_REG_X9 : ASM_AARCH64_REG_X10;
    asm_aarch64_mov_reg_i64_optimised(as, scratch, (int64_t)imm);
    emit(as, reg_ops[op] | (scratch << 16) | (rn << 5) | rd);
}

void asm_aarch64_mov_reg_local_addr(asm_aarch64_t *as, uint rd, int local_num) {
    assert(local_num >= 0);
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

    // ADR Xd, label — PC-relative, ±1MB range.  The code buffer is not
    // guaranteed to be page aligned, so ADRP+ADD cannot be used here.
    // Encoding: 0_immlo[1:0]_10000_immhi[18:0]_Rd[4:0]
    if (!MP_FIT_SIGNED(21, rel)) {
        // The label is out of ADR range (code larger than 1MB).  The truncated
        // encoding is still emitted so that the code size stays consistent
        // between the compute and emit passes; the error is raised below.
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

// Load/store with a constant byte offset.  The offset is signed: aligned
// offsets in the unsigned-immediate range use the scaled LDR/STR (immediate)
// form, offsets in -256..255 (of any alignment) use the unscaled LDUR/STUR
// form, and anything else is materialised in REG_TEMP (with sign extension)
// and the LDR/STR (register) form is used.
//
// The REG_TEMP fallback clobbers a register that the caller may not expect, so
// it must only be reachable from the native/viper emitter, where REG_TEMP (x11)
// is never allocated.  The inline assembler keeps every offset it passes inside
// the range of the first branch, via get_arg_offset() in
// py/emitinlineaarch64.c; the ASM_AARCH64_INLINE_ASM_MAX_OFFSET checks below
// keep the two in step.
//
// Proof that the bound cannot drift into the fallback: get_arg_offset() clamps
// the offset to 0..max and rejects any value that is not a multiple of the
// access size, and get_arg_i() clamps to 0..max.  So every offset reaching
// these functions from the inline assembler satisfies exactly the condition of
// the first branch, provided max <= 4095 * scale and max % scale == 0.
#define ASM_AARCH64_ASSERT_INLINE_BOUND(max_off, scale) \
    MP_STATIC_ASSERT((max_off) <= 4095 * (scale)); \
    MP_STATIC_ASSERT((max_off) % (scale) == 0)
ASM_AARCH64_ASSERT_INLINE_BOUND(ASM_AARCH64_INLINE_ASM_MAX_OFFSET, 8);
ASM_AARCH64_ASSERT_INLINE_BOUND(ASM_AARCH64_INLINE_ASM_MAX_OFFSET32, 4);
ASM_AARCH64_ASSERT_INLINE_BOUND(ASM_AARCH64_INLINE_ASM_MAX_OFFSET16, 2);
ASM_AARCH64_ASSERT_INLINE_BOUND(ASM_AARCH64_INLINE_ASM_MAX_OFFSET8, 1);

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

// Note on the label argument of the branch functions below: a label number
// greater than or equal to max_num_labels means "unknown label", which the
// inline assembler passes for forward references in a compute pass (the label
// is not registered yet).  An unknown label must be treated as a forward
// branch so that the emitted code size is the same in the compute and emit
// passes.
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
    // Unknown label, forwards branch, or a backwards branch out of B.cond
    // range: branch over an unconditional B with the inverted condition.
    emit(as, 0x54000000 | ((8 >> 2) << 5) | (cond ^ 1));
    asm_aarch64_b_label(as, label);
}

void asm_aarch64_b_label(asm_aarch64_t *as, uint label) {
    mp_int_t rel = 0;
    if (label < as->base.max_num_labels) {
        mp_uint_t dest = as->base.label_offsets[label];
        rel = ((mp_int_t)dest - (mp_int_t)as->base.code_offset) >> 2;
        if (!MP_FIT_SIGNED(26, rel)) {
            // Out of range; this can only happen for code larger than ±128MB.
            // The truncated encoding is still emitted so that the code size
            // stays consistent between the compute and emit passes; the error
            // is raised below.
            asm_aarch64_range_error(as);
        }
    }
    emit(as, 0x14000000 | (rel & 0x3ffffff));
}

void asm_aarch64_bl_label(asm_aarch64_t *as, uint label) {
    mp_int_t rel = 0;
    if (label < as->base.max_num_labels) {
        mp_uint_t dest = as->base.label_offsets[label];
        rel = ((mp_int_t)dest - (mp_int_t)as->base.code_offset) >> 2;
        if (!MP_FIT_SIGNED(26, rel)) {
            // Out of range; this can only happen for code larger than ±128MB.
            // The truncated encoding is still emitted so that the code size
            // stays consistent between the compute and emit passes; the error
            // is raised below.
            asm_aarch64_range_error(as);
        }
    }
    emit(as, 0x94000000 | (rel & 0x3ffffff));
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
    // Unknown label, forwards branch, or a long backwards branch: branch over
    // an unconditional B with the inverted sense.
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

#endif // MICROPY_EMIT_AARCH64 || MICROPY_EMIT_INLINE_AARCH64
