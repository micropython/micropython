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

#include "py/runtime.h"

// wrapper around everything in this file
#if MICROPY_EMIT_XTENSA || MICROPY_EMIT_INLINE_XTENSA || MICROPY_EMIT_XTENSAWIN

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
    as->const_table = (uint32_t *)mp_asm_base_get_cur_to_write_bytes(&as->base, as->num_const * 4);

    // adjust the stack-pointer to store a0, a12, a13, a14, a15 and locals, 16-byte aligned
    as->stack_adjust = (((ASM_XTENSA_NUM_REGS_SAVED + num_locals) * WORD_SIZE) + 15) & ~15;
    if (SIGNED_FIT8(-as->stack_adjust)) {
        asm_xtensa_op_addi(as, ASM_XTENSA_REG_A1, ASM_XTENSA_REG_A1, -as->stack_adjust);
    } else {
        asm_xtensa_op_movi(as, ASM_XTENSA_REG_A9, as->stack_adjust);
        asm_xtensa_op_sub(as, ASM_XTENSA_REG_A1, ASM_XTENSA_REG_A1, ASM_XTENSA_REG_A9);
    }

    // save return value (a0) and callee-save registers (a12, a13, a14, a15)
    asm_xtensa_op_s32i_n(as, ASM_XTENSA_REG_A0, ASM_XTENSA_REG_A1, 0);
    for (int i = 1; i < ASM_XTENSA_NUM_REGS_SAVED; ++i) {
        asm_xtensa_op_s32i_n(as, ASM_XTENSA_REG_A11 + i, ASM_XTENSA_REG_A1, i);
    }
}

void asm_xtensa_exit(asm_xtensa_t *as) {
    // restore registers
    for (int i = ASM_XTENSA_NUM_REGS_SAVED - 1; i >= 1; --i) {
        asm_xtensa_op_l32i_n(as, ASM_XTENSA_REG_A11 + i, ASM_XTENSA_REG_A1, i);
    }
    asm_xtensa_op_l32i_n(as, ASM_XTENSA_REG_A0, ASM_XTENSA_REG_A1, 0);

    // restore stack-pointer and return
    if (SIGNED_FIT8(as->stack_adjust)) {
        asm_xtensa_op_addi(as, ASM_XTENSA_REG_A1, ASM_XTENSA_REG_A1, as->stack_adjust);
    } else {
        asm_xtensa_op_movi(as, ASM_XTENSA_REG_A9, as->stack_adjust);
        asm_xtensa_op_add_n(as, ASM_XTENSA_REG_A1, ASM_XTENSA_REG_A1, ASM_XTENSA_REG_A9);
    }

    asm_xtensa_op_ret_n(as);
}

void asm_xtensa_entry_win(asm_xtensa_t *as, int num_locals) {
    // jump over the constants
    asm_xtensa_op_j(as, as->num_const * WORD_SIZE + 4 - 4);
    mp_asm_base_get_cur_to_write_bytes(&as->base, 1); // padding/alignment byte
    as->const_table = (uint32_t *)mp_asm_base_get_cur_to_write_bytes(&as->base, as->num_const * 4);

    as->stack_adjust = 32 + ((((ASM_XTENSA_NUM_REGS_SAVED_WIN + num_locals) * WORD_SIZE) + 15) & ~15);
    asm_xtensa_op_entry(as, ASM_XTENSA_REG_A1, as->stack_adjust);
    asm_xtensa_op_s32i_n(as, ASM_XTENSA_REG_A0, ASM_XTENSA_REG_A1, 0);
}

void asm_xtensa_exit_win(asm_xtensa_t *as) {
    asm_xtensa_op_l32i_n(as, ASM_XTENSA_REG_A0, ASM_XTENSA_REG_A1, 0);
    asm_xtensa_op_retw_n(as);
}

static uint32_t get_label_dest(asm_xtensa_t *as, uint label) {
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

size_t asm_xtensa_mov_reg_i32(asm_xtensa_t *as, uint reg_dest, uint32_t i32) {
    // load the constant
    uint32_t const_table_offset = (uint8_t *)as->const_table - as->base.code_base;
    size_t loc = const_table_offset + as->cur_const * WORD_SIZE;
    asm_xtensa_op_l32r(as, reg_dest, as->base.code_offset, loc);
    // store the constant in the table
    if (as->const_table != NULL) {
        as->const_table[as->cur_const] = i32;
    }
    ++as->cur_const;
    return loc;
}

void asm_xtensa_mov_reg_i32_optimised(asm_xtensa_t *as, uint reg_dest, uint32_t i32) {
    if (-32 <= (int)i32 && (int)i32 <= 95) {
        asm_xtensa_op_movi_n(as, reg_dest, i32);
    } else if (SIGNED_FIT12(i32)) {
        asm_xtensa_op_movi(as, reg_dest, i32);
    } else {
        asm_xtensa_mov_reg_i32(as, reg_dest, i32);
    }
}

void asm_xtensa_mov_local_reg(asm_xtensa_t *as, int local_num, uint reg_src) {
    asm_xtensa_op_s32i(as, reg_src, ASM_XTENSA_REG_A1, local_num);
}

void asm_xtensa_mov_reg_local(asm_xtensa_t *as, uint reg_dest, int local_num) {
    asm_xtensa_op_l32i(as, reg_dest, ASM_XTENSA_REG_A1, local_num);
}

void asm_xtensa_mov_reg_local_addr(asm_xtensa_t *as, uint reg_dest, int local_num) {
    uint off = local_num * WORD_SIZE;
    if (SIGNED_FIT8(off)) {
        asm_xtensa_op_addi(as, reg_dest, ASM_XTENSA_REG_A1, off);
    } else {
        asm_xtensa_op_movi(as, reg_dest, off);
        asm_xtensa_op_add_n(as, reg_dest, reg_dest, ASM_XTENSA_REG_A1);
    }
}

void asm_xtensa_mov_reg_pcrel(asm_xtensa_t *as, uint reg_dest, uint label) {
    // Get relative offset from PC
    uint32_t dest = get_label_dest(as, label);
    int32_t rel = dest - as->base.code_offset;
    rel -= 3 + 3; // account for 3 bytes of movi instruction, 3 bytes call0 adjustment
    asm_xtensa_op_movi(as, reg_dest, rel); // imm has 12-bit range

    // Use call0 to get PC+3 into a0
    // call0 destination must be aligned on 4 bytes:
    //  - code_offset&3=0: off=0, pad=1
    //  - code_offset&3=1: off=0, pad=0
    //  - code_offset&3=2: off=1, pad=3
    //  - code_offset&3=3: off=1, pad=2
    uint32_t off = as->base.code_offset >> 1 & 1;
    uint32_t pad = (5 - as->base.code_offset) & 3;
    asm_xtensa_op_call0(as, off);
    mp_asm_base_get_cur_to_write_bytes(&as->base, pad);

    // Add PC to relative offset
    asm_xtensa_op_add_n(as, reg_dest, reg_dest, ASM_XTENSA_REG_A0);
}

void asm_xtensa_l32i_optimised(asm_xtensa_t *as, uint reg_dest, uint reg_base, uint word_offset) {
    if (word_offset < 16) {
        asm_xtensa_op_l32i_n(as, reg_dest, reg_base, word_offset);
    } else if (word_offset < 256) {
        asm_xtensa_op_l32i(as, reg_dest, reg_base, word_offset);
    } else {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("asm overflow"));
    }
}

void asm_xtensa_s32i_optimised(asm_xtensa_t *as, uint reg_src, uint reg_base, uint word_offset) {
    if (word_offset < 16) {
        asm_xtensa_op_s32i_n(as, reg_src, reg_base, word_offset);
    } else if (word_offset < 256) {
        asm_xtensa_op_s32i(as, reg_src, reg_base, word_offset);
    } else {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("asm overflow"));
    }
}

void asm_xtensa_call_ind(asm_xtensa_t *as, uint idx) {
    asm_xtensa_l32i_optimised(as, ASM_XTENSA_REG_A0, ASM_XTENSA_REG_FUN_TABLE, idx);
    asm_xtensa_op_callx0(as, ASM_XTENSA_REG_A0);
}

void asm_xtensa_call_ind_win(asm_xtensa_t *as, uint idx) {
    asm_xtensa_l32i_optimised(as, ASM_XTENSA_REG_A8, ASM_XTENSA_REG_FUN_TABLE_WIN, idx);
    asm_xtensa_op_callx8(as, ASM_XTENSA_REG_A8);
}

#endif // MICROPY_EMIT_XTENSA || MICROPY_EMIT_INLINE_XTENSA || MICROPY_EMIT_XTENSAWIN
