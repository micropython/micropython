/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
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

#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "py/mpconfig.h"

// wrapper around everything in this file
#if MICROPY_EMIT_X86

#include "py/asmx86.h"

/* all offsets are measured in multiples of 4 bytes */
#define WORD_SIZE                (4)

#define OPCODE_NOP               (0x90)
#define OPCODE_PUSH_R32          (0x50)
//#define OPCODE_PUSH_I32          (0x68)
//#define OPCODE_PUSH_M32          (0xff) /* /6 */
#define OPCODE_POP_R32           (0x58)
#define OPCODE_RET               (0xc3)
//#define OPCODE_MOV_I8_TO_R8      (0xb0) /* +rb */
#define OPCODE_MOV_I32_TO_R32    (0xb8)
//#define OPCODE_MOV_I32_TO_RM32   (0xc7)
#define OPCODE_MOV_R8_TO_RM8     (0x88) /* /r */
#define OPCODE_MOV_R32_TO_RM32   (0x89) /* /r */
#define OPCODE_MOV_RM32_TO_R32   (0x8b) /* /r */
#define OPCODE_MOVZX_RM8_TO_R32  (0xb6) /* 0x0f 0xb6/r */
#define OPCODE_MOVZX_RM16_TO_R32 (0xb7) /* 0x0f 0xb7/r */
#define OPCODE_LEA_MEM_TO_R32    (0x8d) /* /r */
#define OPCODE_AND_R32_TO_RM32   (0x21) /* /r */
#define OPCODE_OR_R32_TO_RM32    (0x09) /* /r */
#define OPCODE_XOR_R32_TO_RM32   (0x31) /* /r */
#define OPCODE_ADD_R32_TO_RM32   (0x01)
#define OPCODE_ADD_I32_TO_RM32   (0x81) /* /0 */
#define OPCODE_ADD_I8_TO_RM32    (0x83) /* /0 */
#define OPCODE_SUB_R32_FROM_RM32 (0x29)
#define OPCODE_SUB_I32_FROM_RM32 (0x81) /* /5 */
#define OPCODE_SUB_I8_FROM_RM32  (0x83) /* /5 */
//#define OPCODE_SHL_RM32_BY_I8    (0xc1) /* /4 */
//#define OPCODE_SHR_RM32_BY_I8    (0xc1) /* /5 */
//#define OPCODE_SAR_RM32_BY_I8    (0xc1) /* /7 */
#define OPCODE_SHL_RM32_CL       (0xd3) /* /4 */
#define OPCODE_SAR_RM32_CL       (0xd3) /* /7 */
//#define OPCODE_CMP_I32_WITH_RM32 (0x81) /* /7 */
//#define OPCODE_CMP_I8_WITH_RM32  (0x83) /* /7 */
#define OPCODE_CMP_R32_WITH_RM32 (0x39)
//#define OPCODE_CMP_RM32_WITH_R32 (0x3b)
#define OPCODE_TEST_R8_WITH_RM8  (0x84) /* /r */
#define OPCODE_TEST_R32_WITH_RM32 (0x85) /* /r */
#define OPCODE_JMP_REL8          (0xeb)
#define OPCODE_JMP_REL32         (0xe9)
#define OPCODE_JMP_RM32          (0xff) /* /4 */
#define OPCODE_JCC_REL8          (0x70) /* | jcc type */
#define OPCODE_JCC_REL32_A       (0x0f)
#define OPCODE_JCC_REL32_B       (0x80) /* | jcc type */
#define OPCODE_SETCC_RM8_A       (0x0f)
#define OPCODE_SETCC_RM8_B       (0x90) /* | jcc type, /0 */
#define OPCODE_CALL_REL32        (0xe8)
#define OPCODE_CALL_RM32         (0xff) /* /2 */
#define OPCODE_LEAVE             (0xc9)

#define MODRM_R32(x)    ((x) << 3)
#define MODRM_RM_DISP0  (0x00)
#define MODRM_RM_DISP8  (0x40)
#define MODRM_RM_DISP32 (0x80)
#define MODRM_RM_REG    (0xc0)
#define MODRM_RM_R32(x) (x)

#define OP_SIZE_PREFIX (0x66)

#define IMM32_L0(x) ((x) & 0xff)
#define IMM32_L1(x) (((x) >> 8) & 0xff)
#define IMM32_L2(x) (((x) >> 16) & 0xff)
#define IMM32_L3(x) (((x) >> 24) & 0xff)

#define SIGNED_FIT8(x) (((x) & 0xffffff80) == 0) || (((x) & 0xffffff80) == 0xffffff80)

STATIC void asm_x86_write_byte_1(asm_x86_t *as, byte b1) {
    byte* c = mp_asm_base_get_cur_to_write_bytes(&as->base, 1);
    if (c != NULL) {
        c[0] = b1;
    }
}

STATIC void asm_x86_write_byte_2(asm_x86_t *as, byte b1, byte b2) {
    byte* c = mp_asm_base_get_cur_to_write_bytes(&as->base, 2);
    if (c != NULL) {
        c[0] = b1;
        c[1] = b2;
    }
}

STATIC void asm_x86_write_byte_3(asm_x86_t *as, byte b1, byte b2, byte b3) {
    byte* c = mp_asm_base_get_cur_to_write_bytes(&as->base, 3);
    if (c != NULL) {
        c[0] = b1;
        c[1] = b2;
        c[2] = b3;
    }
}

STATIC void asm_x86_write_word32(asm_x86_t *as, int w32) {
    byte* c = mp_asm_base_get_cur_to_write_bytes(&as->base, 4);
    if (c != NULL) {
        c[0] = IMM32_L0(w32);
        c[1] = IMM32_L1(w32);
        c[2] = IMM32_L2(w32);
        c[3] = IMM32_L3(w32);
    }
}

STATIC void asm_x86_write_r32_disp(asm_x86_t *as, int r32, int disp_r32, int disp_offset) {
    uint8_t rm_disp;
    if (disp_offset == 0 && disp_r32 != ASM_X86_REG_EBP) {
        rm_disp = MODRM_RM_DISP0;
    } else if (SIGNED_FIT8(disp_offset)) {
        rm_disp = MODRM_RM_DISP8;
    } else {
        rm_disp = MODRM_RM_DISP32;
    }
    asm_x86_write_byte_1(as, MODRM_R32(r32) | rm_disp | MODRM_RM_R32(disp_r32));
    if (disp_r32 == ASM_X86_REG_ESP) {
        // Special case for esp, it needs a SIB byte
        asm_x86_write_byte_1(as, 0x24);
    }
    if (rm_disp == MODRM_RM_DISP8) {
        asm_x86_write_byte_1(as, IMM32_L0(disp_offset));
    } else if (rm_disp == MODRM_RM_DISP32) {
        asm_x86_write_word32(as, disp_offset);
    }
}

STATIC void asm_x86_generic_r32_r32(asm_x86_t *as, int dest_r32, int src_r32, int op) {
    asm_x86_write_byte_2(as, op, MODRM_R32(src_r32) | MODRM_RM_REG | MODRM_RM_R32(dest_r32));
}

#if 0
STATIC void asm_x86_nop(asm_x86_t *as) {
    asm_x86_write_byte_1(as, OPCODE_NOP);
}
#endif

STATIC void asm_x86_push_r32(asm_x86_t *as, int src_r32) {
    asm_x86_write_byte_1(as, OPCODE_PUSH_R32 | src_r32);
}

#if 0
void asm_x86_push_i32(asm_x86_t *as, int src_i32) {
    asm_x86_write_byte_1(as, OPCODE_PUSH_I32);
    asm_x86_write_word32(as, src_i32);
}

void asm_x86_push_disp(asm_x86_t *as, int src_r32, int src_offset) {
    asm_x86_write_byte_1(as, OPCODE_PUSH_M32);
    asm_x86_write_r32_disp(as, 6, src_r32, src_offset);
}
#endif

STATIC void asm_x86_pop_r32(asm_x86_t *as, int dest_r32) {
    asm_x86_write_byte_1(as, OPCODE_POP_R32 | dest_r32);
}

STATIC void asm_x86_ret(asm_x86_t *as) {
    asm_x86_write_byte_1(as, OPCODE_RET);
}

void asm_x86_mov_r32_r32(asm_x86_t *as, int dest_r32, int src_r32) {
    asm_x86_generic_r32_r32(as, dest_r32, src_r32, OPCODE_MOV_R32_TO_RM32);
}

void asm_x86_mov_r8_to_mem8(asm_x86_t *as, int src_r32, int dest_r32, int dest_disp) {
    asm_x86_write_byte_1(as, OPCODE_MOV_R8_TO_RM8);
    asm_x86_write_r32_disp(as, src_r32, dest_r32, dest_disp);
}

void asm_x86_mov_r16_to_mem16(asm_x86_t *as, int src_r32, int dest_r32, int dest_disp) {
    asm_x86_write_byte_2(as, OP_SIZE_PREFIX, OPCODE_MOV_R32_TO_RM32);
    asm_x86_write_r32_disp(as, src_r32, dest_r32, dest_disp);
}

void asm_x86_mov_r32_to_mem32(asm_x86_t *as, int src_r32, int dest_r32, int dest_disp) {
    asm_x86_write_byte_1(as, OPCODE_MOV_R32_TO_RM32);
    asm_x86_write_r32_disp(as, src_r32, dest_r32, dest_disp);
}

void asm_x86_mov_mem8_to_r32zx(asm_x86_t *as, int src_r32, int src_disp, int dest_r32) {
    asm_x86_write_byte_2(as, 0x0f, OPCODE_MOVZX_RM8_TO_R32);
    asm_x86_write_r32_disp(as, dest_r32, src_r32, src_disp);
}

void asm_x86_mov_mem16_to_r32zx(asm_x86_t *as, int src_r32, int src_disp, int dest_r32) {
    asm_x86_write_byte_2(as, 0x0f, OPCODE_MOVZX_RM16_TO_R32);
    asm_x86_write_r32_disp(as, dest_r32, src_r32, src_disp);
}

void asm_x86_mov_mem32_to_r32(asm_x86_t *as, int src_r32, int src_disp, int dest_r32) {
    asm_x86_write_byte_1(as, OPCODE_MOV_RM32_TO_R32);
    asm_x86_write_r32_disp(as, dest_r32, src_r32, src_disp);
}

STATIC void asm_x86_lea_disp_to_r32(asm_x86_t *as, int src_r32, int src_disp, int dest_r32) {
    asm_x86_write_byte_1(as, OPCODE_LEA_MEM_TO_R32);
    asm_x86_write_r32_disp(as, dest_r32, src_r32, src_disp);
}

#if 0
void asm_x86_mov_i8_to_r8(asm_x86_t *as, int src_i8, int dest_r32) {
    asm_x86_write_byte_2(as, OPCODE_MOV_I8_TO_R8 | dest_r32, src_i8);
}
#endif

size_t asm_x86_mov_i32_to_r32(asm_x86_t *as, int32_t src_i32, int dest_r32) {
    asm_x86_write_byte_1(as, OPCODE_MOV_I32_TO_R32 | dest_r32);
    size_t loc = mp_asm_base_get_code_pos(&as->base);
    asm_x86_write_word32(as, src_i32);
    return loc;
}

void asm_x86_and_r32_r32(asm_x86_t *as, int dest_r32, int src_r32) {
    asm_x86_generic_r32_r32(as, dest_r32, src_r32, OPCODE_AND_R32_TO_RM32);
}

void asm_x86_or_r32_r32(asm_x86_t *as, int dest_r32, int src_r32) {
    asm_x86_generic_r32_r32(as, dest_r32, src_r32, OPCODE_OR_R32_TO_RM32);
}

void asm_x86_xor_r32_r32(asm_x86_t *as, int dest_r32, int src_r32) {
    asm_x86_generic_r32_r32(as, dest_r32, src_r32, OPCODE_XOR_R32_TO_RM32);
}

void asm_x86_shl_r32_cl(asm_x86_t* as, int dest_r32) {
    asm_x86_generic_r32_r32(as, dest_r32, 4, OPCODE_SHL_RM32_CL);
}

void asm_x86_sar_r32_cl(asm_x86_t* as, int dest_r32) {
    asm_x86_generic_r32_r32(as, dest_r32, 7, OPCODE_SAR_RM32_CL);
}

void asm_x86_add_r32_r32(asm_x86_t *as, int dest_r32, int src_r32) {
    asm_x86_generic_r32_r32(as, dest_r32, src_r32, OPCODE_ADD_R32_TO_RM32);
}

STATIC void asm_x86_add_i32_to_r32(asm_x86_t *as, int src_i32, int dest_r32) {
    if (SIGNED_FIT8(src_i32)) {
        asm_x86_write_byte_2(as, OPCODE_ADD_I8_TO_RM32, MODRM_R32(0) | MODRM_RM_REG | MODRM_RM_R32(dest_r32));
        asm_x86_write_byte_1(as, src_i32 & 0xff);
    } else {
        asm_x86_write_byte_2(as, OPCODE_ADD_I32_TO_RM32, MODRM_R32(0) | MODRM_RM_REG | MODRM_RM_R32(dest_r32));
        asm_x86_write_word32(as, src_i32);
    }
}

void asm_x86_sub_r32_r32(asm_x86_t *as, int dest_r32, int src_r32) {
    asm_x86_generic_r32_r32(as, dest_r32, src_r32, OPCODE_SUB_R32_FROM_RM32);
}

STATIC void asm_x86_sub_r32_i32(asm_x86_t *as, int dest_r32, int src_i32) {
    if (SIGNED_FIT8(src_i32)) {
        // defaults to 32 bit operation
        asm_x86_write_byte_2(as, OPCODE_SUB_I8_FROM_RM32, MODRM_R32(5) | MODRM_RM_REG | MODRM_RM_R32(dest_r32));
        asm_x86_write_byte_1(as, src_i32 & 0xff);
    } else {
        // defaults to 32 bit operation
        asm_x86_write_byte_2(as, OPCODE_SUB_I32_FROM_RM32, MODRM_R32(5) | MODRM_RM_REG | MODRM_RM_R32(dest_r32));
        asm_x86_write_word32(as, src_i32);
    }
}

void asm_x86_mul_r32_r32(asm_x86_t *as, int dest_r32, int src_r32) {
    // imul reg32, reg/mem32 -- 0x0f 0xaf /r
    asm_x86_write_byte_3(as, 0x0f, 0xaf, MODRM_R32(dest_r32) | MODRM_RM_REG | MODRM_RM_R32(src_r32));
}

#if 0
/* shifts not tested */
void asm_x86_shl_r32_by_imm(asm_x86_t *as, int r32, int imm) {
    asm_x86_write_byte_2(as, OPCODE_SHL_RM32_BY_I8, MODRM_R32(4) | MODRM_RM_REG | MODRM_RM_R32(r32));
    asm_x86_write_byte_1(as, imm);
}

void asm_x86_shr_r32_by_imm(asm_x86_t *as, int r32, int imm) {
    asm_x86_write_byte_2(as, OPCODE_SHR_RM32_BY_I8, MODRM_R32(5) | MODRM_RM_REG | MODRM_RM_R32(r32));
    asm_x86_write_byte_1(as, imm);
}

void asm_x86_sar_r32_by_imm(asm_x86_t *as, int r32, int imm) {
    asm_x86_write_byte_2(as, OPCODE_SAR_RM32_BY_I8, MODRM_R32(7) | MODRM_RM_REG | MODRM_RM_R32(r32));
    asm_x86_write_byte_1(as, imm);
}
#endif

void asm_x86_cmp_r32_with_r32(asm_x86_t *as, int src_r32_a, int src_r32_b) {
    asm_x86_generic_r32_r32(as, src_r32_b, src_r32_a, OPCODE_CMP_R32_WITH_RM32);
}

#if 0
void asm_x86_cmp_i32_with_r32(asm_x86_t *as, int src_i32, int src_r32) {
    if (SIGNED_FIT8(src_i32)) {
        asm_x86_write_byte_2(as, OPCODE_CMP_I8_WITH_RM32, MODRM_R32(7) | MODRM_RM_REG | MODRM_RM_R32(src_r32));
        asm_x86_write_byte_1(as, src_i32 & 0xff);
    } else {
        asm_x86_write_byte_2(as, OPCODE_CMP_I32_WITH_RM32, MODRM_R32(7) | MODRM_RM_REG | MODRM_RM_R32(src_r32));
        asm_x86_write_word32(as, src_i32);
    }
}
#endif

void asm_x86_test_r8_with_r8(asm_x86_t *as, int src_r32_a, int src_r32_b) {
    asm_x86_write_byte_2(as, OPCODE_TEST_R8_WITH_RM8, MODRM_R32(src_r32_a) | MODRM_RM_REG | MODRM_RM_R32(src_r32_b));
}

void asm_x86_test_r32_with_r32(asm_x86_t *as, int src_r32_a, int src_r32_b) {
    asm_x86_generic_r32_r32(as, src_r32_b, src_r32_a, OPCODE_TEST_R32_WITH_RM32);
}

void asm_x86_setcc_r8(asm_x86_t *as, mp_uint_t jcc_type, int dest_r8) {
    asm_x86_write_byte_3(as, OPCODE_SETCC_RM8_A, OPCODE_SETCC_RM8_B | jcc_type, MODRM_R32(0) | MODRM_RM_REG | MODRM_RM_R32(dest_r8));
}

void asm_x86_jmp_reg(asm_x86_t *as, int src_r32) {
    asm_x86_write_byte_2(as, OPCODE_JMP_RM32, MODRM_R32(4) | MODRM_RM_REG | MODRM_RM_R32(src_r32));
}

STATIC mp_uint_t get_label_dest(asm_x86_t *as, mp_uint_t label) {
    assert(label < as->base.max_num_labels);
    return as->base.label_offsets[label];
}

void asm_x86_jmp_label(asm_x86_t *as, mp_uint_t label) {
    mp_uint_t dest = get_label_dest(as, label);
    mp_int_t rel = dest - as->base.code_offset;
    if (dest != (mp_uint_t)-1 && rel < 0) {
        // is a backwards jump, so we know the size of the jump on the first pass
        // calculate rel assuming 8 bit relative jump
        rel -= 2;
        if (SIGNED_FIT8(rel)) {
            asm_x86_write_byte_2(as, OPCODE_JMP_REL8, rel & 0xff);
        } else {
            rel += 2;
            goto large_jump;
        }
    } else {
        // is a forwards jump, so need to assume it's large
        large_jump:
        rel -= 5;
        asm_x86_write_byte_1(as, OPCODE_JMP_REL32);
        asm_x86_write_word32(as, rel);
    }
}

void asm_x86_jcc_label(asm_x86_t *as, mp_uint_t jcc_type, mp_uint_t label) {
    mp_uint_t dest = get_label_dest(as, label);
    mp_int_t rel = dest - as->base.code_offset;
    if (dest != (mp_uint_t)-1 && rel < 0) {
        // is a backwards jump, so we know the size of the jump on the first pass
        // calculate rel assuming 8 bit relative jump
        rel -= 2;
        if (SIGNED_FIT8(rel)) {
            asm_x86_write_byte_2(as, OPCODE_JCC_REL8 | jcc_type, rel & 0xff);
        } else {
            rel += 2;
            goto large_jump;
        }
    } else {
        // is a forwards jump, so need to assume it's large
        large_jump:
        rel -= 6;
        asm_x86_write_byte_2(as, OPCODE_JCC_REL32_A, OPCODE_JCC_REL32_B | jcc_type);
        asm_x86_write_word32(as, rel);
    }
}

void asm_x86_entry(asm_x86_t *as, int num_locals) {
    assert(num_locals >= 0);
    asm_x86_push_r32(as, ASM_X86_REG_EBP);
    asm_x86_push_r32(as, ASM_X86_REG_EBX);
    asm_x86_push_r32(as, ASM_X86_REG_ESI);
    asm_x86_push_r32(as, ASM_X86_REG_EDI);
    num_locals |= 3; // make it odd so stack is aligned on 16 byte boundary
    asm_x86_sub_r32_i32(as, ASM_X86_REG_ESP, num_locals * WORD_SIZE);
    as->num_locals = num_locals;
}

void asm_x86_exit(asm_x86_t *as) {
    asm_x86_sub_r32_i32(as, ASM_X86_REG_ESP, -as->num_locals * WORD_SIZE);
    asm_x86_pop_r32(as, ASM_X86_REG_EDI);
    asm_x86_pop_r32(as, ASM_X86_REG_ESI);
    asm_x86_pop_r32(as, ASM_X86_REG_EBX);
    asm_x86_pop_r32(as, ASM_X86_REG_EBP);
    asm_x86_ret(as);
}

STATIC int asm_x86_arg_offset_from_esp(asm_x86_t *as, size_t arg_num) {
    // Above esp are: locals, 4 saved registers, return eip, arguments
    return (as->num_locals + 4 + 1 + arg_num) * WORD_SIZE;
}

#if 0
void asm_x86_push_arg(asm_x86_t *as, int src_arg_num) {
    asm_x86_push_disp(as, ASM_X86_REG_ESP, asm_x86_arg_offset_from_esp(as, src_arg_num));
}
#endif

void asm_x86_mov_arg_to_r32(asm_x86_t *as, int src_arg_num, int dest_r32) {
    asm_x86_mov_mem32_to_r32(as, ASM_X86_REG_ESP, asm_x86_arg_offset_from_esp(as, src_arg_num), dest_r32);
}

#if 0
void asm_x86_mov_r32_to_arg(asm_x86_t *as, int src_r32, int dest_arg_num) {
    asm_x86_mov_r32_to_mem32(as, src_r32, ASM_X86_REG_ESP, asm_x86_arg_offset_from_esp(as, dest_arg_num));
}
#endif

// locals:
//  - stored on the stack in ascending order
//  - numbered 0 through as->num_locals-1
//  - ESP points to the first local
//
//  | ESP
//  v
//  l0  l1  l2  ...  l(n-1)
//  ^                ^
//  | low address    | high address in RAM
//
STATIC int asm_x86_local_offset_from_esp(asm_x86_t *as, int local_num) {
    (void)as;
    // Stack is full descending, ESP points to local0
    return local_num * WORD_SIZE;
}

void asm_x86_mov_local_to_r32(asm_x86_t *as, int src_local_num, int dest_r32) {
    asm_x86_mov_mem32_to_r32(as, ASM_X86_REG_ESP, asm_x86_local_offset_from_esp(as, src_local_num), dest_r32);
}

void asm_x86_mov_r32_to_local(asm_x86_t *as, int src_r32, int dest_local_num) {
    asm_x86_mov_r32_to_mem32(as, src_r32, ASM_X86_REG_ESP, asm_x86_local_offset_from_esp(as, dest_local_num));
}

void asm_x86_mov_local_addr_to_r32(asm_x86_t *as, int local_num, int dest_r32) {
    int offset = asm_x86_local_offset_from_esp(as, local_num);
    if (offset == 0) {
        asm_x86_mov_r32_r32(as, dest_r32, ASM_X86_REG_ESP);
    } else {
        asm_x86_lea_disp_to_r32(as, ASM_X86_REG_ESP, offset, dest_r32);
    }
}

void asm_x86_mov_reg_pcrel(asm_x86_t *as, int dest_r32, mp_uint_t label) {
    asm_x86_write_byte_1(as, OPCODE_CALL_REL32);
    asm_x86_write_word32(as, 0);
    mp_uint_t dest = get_label_dest(as, label);
    mp_int_t rel = dest - as->base.code_offset;
    asm_x86_pop_r32(as, dest_r32);
    // PC rel is usually a forward reference, so need to assume it's large
    asm_x86_write_byte_2(as, OPCODE_ADD_I32_TO_RM32, MODRM_R32(0) | MODRM_RM_REG | MODRM_RM_R32(dest_r32));
    asm_x86_write_word32(as, rel);
}

#if 0
void asm_x86_push_local(asm_x86_t *as, int local_num) {
    asm_x86_push_disp(as, ASM_X86_REG_ESP, asm_x86_local_offset_from_esp(as, local_num));
}

void asm_x86_push_local_addr(asm_x86_t *as, int local_num, int temp_r32)
{
    asm_x86_mov_r32_r32(as, temp_r32, ASM_X86_REG_ESP);
    asm_x86_add_i32_to_r32(as, asm_x86_local_offset_from_esp(as, local_num), temp_r32);
    asm_x86_push_r32(as, temp_r32);
}
#endif

void asm_x86_call_ind(asm_x86_t *as, size_t fun_id, mp_uint_t n_args, int temp_r32) {
    assert(n_args <= 4);

    // Align stack on 16-byte boundary during the call
    unsigned int align = ((n_args + 3) & ~3) - n_args;
    if (align) {
        asm_x86_sub_r32_i32(as, ASM_X86_REG_ESP, align * WORD_SIZE);
    }

    if (n_args > 3) {
        asm_x86_push_r32(as, ASM_X86_REG_ARG_4);
    }
    if (n_args > 2) {
        asm_x86_push_r32(as, ASM_X86_REG_ARG_3);
    }
    if (n_args > 1) {
        asm_x86_push_r32(as, ASM_X86_REG_ARG_2);
    }
    if (n_args > 0) {
        asm_x86_push_r32(as, ASM_X86_REG_ARG_1);
    }

    // Load the pointer to the function and make the call
    asm_x86_mov_mem32_to_r32(as, ASM_X86_REG_FUN_TABLE, fun_id * WORD_SIZE, temp_r32);
    asm_x86_write_byte_2(as, OPCODE_CALL_RM32, MODRM_R32(2) | MODRM_RM_REG | MODRM_RM_R32(temp_r32));

    // the caller must clean up the stack
    if (n_args > 0) {
        asm_x86_add_i32_to_r32(as, (n_args + align) * WORD_SIZE, ASM_X86_REG_ESP);
    }
}

#endif // MICROPY_EMIT_X86
