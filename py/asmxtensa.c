/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
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

#include "py/mpconfig.h"

// wrapper around everything in this file
#if MICROPY_EMIT_XTENSA || MICROPY_EMIT_INLINE_XTENSA

#include "py/asmxtensa.h"

#define WORD_SIZE (4)
#define SIGNED_FIT8(x) ((((x) & 0xffffff80) == 0) || (((x) & 0xffffff80) == 0xffffff80))
#define SIGNED_FIT12(x) ((((x) & 0xfffff800) == 0) || (((x) & 0xfffff800) == 0xfffff800))

void asm_xtensa_end_pass(asm_xtensa_t *as) {
    as->num_const = as->cur_const;
    as->cur_const = 0;

    #if 0
    // make a hex dump of the machine code
    if (as->base.pass == MP_ASM_PASS_EMIT) {
        uint8_t *d = as->base.code_base;
        printf("XTENSA ASM:");
        for (int i = 0; i < ((as->base.code_size + 15) & ~15); ++i) {
            if (i % 16 == 0) {
                printf("\n%08x:", (uint32_t)&d[i]);
            }
            if (i % 2 == 0) {
                printf(" ");
            }
            printf("%02x", d[i]);
        }
        printf("\n");
    }
    #endif
}

void asm_xtensa_entry(asm_xtensa_t *as, int num_locals) {
    // jump over the constants
    asm_xtensa_op_j(as, as->num_const * WORD_SIZE + 4 - 4);
    mp_asm_base_get_cur_to_write_bytes(&as->base, 1); // padding/alignment byte
    as->const_table = (uint32_t*)mp_asm_base_get_cur_to_write_bytes(&as->base, as->num_const * 4);

    // adjust the stack-pointer to store a0, a12, a13, a14 and locals, 16-byte aligned
    as->stack_adjust = (((4 + num_locals) * WORD_SIZE) + 15) & ~15;
    asm_xtensa_op_addi(as, ASM_XTENSA_REG_A1, ASM_XTENSA_REG_A1, -as->stack_adjust);

    // save return value (a0) and callee-save registers (a12, a13, a14)
    asm_xtensa_op_s32i_n(as, ASM_XTENSA_REG_A0, ASM_XTENSA_REG_A1, 0);
    asm_xtensa_op_s32i_n(as, ASM_XTENSA_REG_A12, ASM_XTENSA_REG_A1, 1);
    asm_xtensa_op_s32i_n(as, ASM_XTENSA_REG_A13, ASM_XTENSA_REG_A1, 2);
    asm_xtensa_op_s32i_n(as, ASM_XTENSA_REG_A14, ASM_XTENSA_REG_A1, 3);
}

void asm_xtensa_exit(asm_xtensa_t *as) {
    // restore registers
    asm_xtensa_op_l32i_n(as, ASM_XTENSA_REG_A14, ASM_XTENSA_REG_A1, 3);
    asm_xtensa_op_l32i_n(as, ASM_XTENSA_REG_A13, ASM_XTENSA_REG_A1, 2);
    asm_xtensa_op_l32i_n(as, ASM_XTENSA_REG_A12, ASM_XTENSA_REG_A1, 1);
    asm_xtensa_op_l32i_n(as, ASM_XTENSA_REG_A0, ASM_XTENSA_REG_A1, 0);

    // restore stack-pointer and return
    asm_xtensa_op_addi(as, ASM_XTENSA_REG_A1, ASM_XTENSA_REG_A1, as->stack_adjust);
    asm_xtensa_op_ret_n(as);
}

STATIC uint32_t get_label_dest(asm_xtensa_t *as, uint label) {
    assert(label < as->base.max_num_labels);
    return as->base.label_offsets[label];
}

void asm_xtensa_op16(asm_xtensa_t *as, uint16_t op) {
    uint8_t *c = mp_asm_base_get_cur_to_write_bytes(&as->base, 2);
    if (c != NULL) {
        c[0] = op;
        c[1] = op >> 8;
    }
}

void asm_xtensa_op24(asm_xtensa_t *as, uint32_t op) {
    uint8_t *c = mp_asm_base_get_cur_to_write_bytes(&as->base, 3);
    if (c != NULL) {
        c[0] = op;
        c[1] = op >> 8;
        c[2] = op >> 16;
    }
}

void asm_xtensa_j_label(asm_xtensa_t *as, uint label) {
    uint32_t dest = get_label_dest(as, label);
    int32_t rel = dest - as->base.code_offset - 4;
    // we assume rel, as a signed int, fits in 18-bits
    asm_xtensa_op_j(as, rel);
}

void asm_xtensa_bccz_reg_label(asm_xtensa_t *as, uint cond, uint reg, uint label) {
    uint32_t dest = get_label_dest(as, label);
    int32_t rel = dest - as->base.code_offset - 4;
    if (as->base.pass == MP_ASM_PASS_EMIT && !SIGNED_FIT12(rel)) {
        printf("ERROR: xtensa bccz out of range\n");
    }
    asm_xtensa_op_bccz(as, cond, reg, rel);
}

void asm_xtensa_bcc_reg_reg_label(asm_xtensa_t *as, uint cond, uint reg1, uint reg2, uint label) {
    uint32_t dest = get_label_dest(as, label);
    int32_t rel = dest - as->base.code_offset - 4;
    if (as->base.pass == MP_ASM_PASS_EMIT && !SIGNED_FIT8(rel)) {
        printf("ERROR: xtensa bcc out of range\n");
    }
    asm_xtensa_op_bcc(as, cond, reg1, reg2, rel);
}

// convenience function; reg_dest must be different from reg_src[12]
void asm_xtensa_setcc_reg_reg_reg(asm_xtensa_t *as, uint cond, uint reg_dest, uint reg_src1, uint reg_src2) {
    asm_xtensa_op_movi_n(as, reg_dest, 1);
    asm_xtensa_op_bcc(as, cond, reg_src1, reg_src2, 1);
    asm_xtensa_op_movi_n(as, reg_dest, 0);
}

void asm_xtensa_mov_reg_i32(asm_xtensa_t *as, uint reg_dest, uint32_t i32) {
    if (SIGNED_FIT12(i32)) {
        asm_xtensa_op_movi(as, reg_dest, i32);
    } else {
        // load the constant
        asm_xtensa_op_l32r(as, reg_dest, as->base.code_offset, 4 + as->cur_const * WORD_SIZE);
        // store the constant in the table
        if (as->const_table != NULL) {
            as->const_table[as->cur_const] = i32;
        }
        ++as->cur_const;
    }
}

void asm_xtensa_mov_local_reg(asm_xtensa_t *as, int local_num, uint reg_src) {
    asm_xtensa_op_s32i(as, reg_src, ASM_XTENSA_REG_A1, 4 + local_num);
}

void asm_xtensa_mov_reg_local(asm_xtensa_t *as, uint reg_dest, int local_num) {
    asm_xtensa_op_l32i(as, reg_dest, ASM_XTENSA_REG_A1, 4 + local_num);
}

void asm_xtensa_mov_reg_local_addr(asm_xtensa_t *as, uint reg_dest, int local_num) {
    asm_xtensa_op_mov_n(as, reg_dest, ASM_XTENSA_REG_A1);
    asm_xtensa_op_addi(as, reg_dest, reg_dest, (4 + local_num) * WORD_SIZE);
}

#endif // MICROPY_EMIT_XTENSA || MICROPY_EMIT_INLINE_XTENSA
