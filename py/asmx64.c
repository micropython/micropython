/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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

#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "py/mpconfig.h"

// wrapper around everything in this file
#if MICROPY_EMIT_X64

#include "py/asmx64.h"

/* all offsets are measured in multiples of 8 bytes */
#define WORD_SIZE                (8)

#define OPCODE_NOP               (0x90)
#define OPCODE_PUSH_R64          (0x50) /* +rq */
#define OPCODE_PUSH_I64          (0x68)
#define OPCODE_PUSH_M64          (0xff) /* /6 */
#define OPCODE_POP_R64           (0x58) /* +rq */
#define OPCODE_RET               (0xc3)
#define OPCODE_MOV_I8_TO_R8      (0xb0) /* +rb */
#define OPCODE_MOV_I64_TO_R64    (0xb8) /* +rq */
#define OPCODE_MOV_I32_TO_RM32   (0xc7)
#define OPCODE_MOV_R8_TO_RM8     (0x88) /* /r */
#define OPCODE_MOV_R64_TO_RM64   (0x89) /* /r */
#define OPCODE_MOV_RM64_TO_R64   (0x8b) /* /r */
#define OPCODE_MOVZX_RM8_TO_R64  (0xb6) /* 0x0f 0xb6/r */
#define OPCODE_MOVZX_RM16_TO_R64 (0xb7) /* 0x0f 0xb7/r */
#define OPCODE_LEA_MEM_TO_R64    (0x8d) /* /r */
#define OPCODE_AND_R64_TO_RM64   (0x21) /* /r */
#define OPCODE_OR_R64_TO_RM64    (0x09) /* /r */
#define OPCODE_XOR_R64_TO_RM64   (0x31) /* /r */
#define OPCODE_ADD_R64_TO_RM64   (0x01) /* /r */
#define OPCODE_ADD_I32_TO_RM32   (0x81) /* /0 */
#define OPCODE_ADD_I8_TO_RM32    (0x83) /* /0 */
#define OPCODE_SUB_R64_FROM_RM64 (0x29)
#define OPCODE_SUB_I32_FROM_RM64 (0x81) /* /5 */
#define OPCODE_SUB_I8_FROM_RM64  (0x83) /* /5 */
//#define OPCODE_SHL_RM32_BY_I8    (0xc1) /* /4 */
//#define OPCODE_SHR_RM32_BY_I8    (0xc1) /* /5 */
//#define OPCODE_SAR_RM32_BY_I8    (0xc1) /* /7 */
#define OPCODE_SHL_RM64_CL       (0xd3) /* /4 */
#define OPCODE_SAR_RM64_CL       (0xd3) /* /7 */
//#define OPCODE_CMP_I32_WITH_RM32 (0x81) /* /7 */
//#define OPCODE_CMP_I8_WITH_RM32  (0x83) /* /7 */
#define OPCODE_CMP_R64_WITH_RM64 (0x39) /* /r */
//#define OPCODE_CMP_RM32_WITH_R32 (0x3b)
#define OPCODE_TEST_R8_WITH_RM8  (0x84) /* /r */
#define OPCODE_JMP_REL8          (0xeb)
#define OPCODE_JMP_REL32         (0xe9)
#define OPCODE_JCC_REL8          (0x70) /* | jcc type */
#define OPCODE_JCC_REL32_A       (0x0f)
#define OPCODE_JCC_REL32_B       (0x80) /* | jcc type */
#define OPCODE_SETCC_RM8_A       (0x0f)
#define OPCODE_SETCC_RM8_B       (0x90) /* | jcc type, /0 */
#define OPCODE_CALL_REL32        (0xe8)
#define OPCODE_CALL_RM32         (0xff) /* /2 */
#define OPCODE_LEAVE             (0xc9)

#define MODRM_R64(x)    (((x) & 0x7) << 3)
#define MODRM_RM_DISP0  (0x00)
#define MODRM_RM_DISP8  (0x40)
#define MODRM_RM_DISP32 (0x80)
#define MODRM_RM_REG    (0xc0)
#define MODRM_RM_R64(x) ((x) & 0x7)

#define OP_SIZE_PREFIX (0x66)

#define REX_PREFIX  (0x40)
#define REX_W       (0x08)  // width
#define REX_R       (0x04)  // register
#define REX_X       (0x02)  // index
#define REX_B       (0x01)  // base
#define REX_W_FROM_R64(r64) ((r64) >> 0 & 0x08)
#define REX_R_FROM_R64(r64) ((r64) >> 1 & 0x04)
#define REX_X_FROM_R64(r64) ((r64) >> 2 & 0x02)
#define REX_B_FROM_R64(r64) ((r64) >> 3 & 0x01)

#define IMM32_L0(x) ((x) & 0xff)
#define IMM32_L1(x) (((x) >> 8) & 0xff)
#define IMM32_L2(x) (((x) >> 16) & 0xff)
#define IMM32_L3(x) (((x) >> 24) & 0xff)
#define IMM64_L4(x) (((x) >> 32) & 0xff)
#define IMM64_L5(x) (((x) >> 40) & 0xff)
#define IMM64_L6(x) (((x) >> 48) & 0xff)
#define IMM64_L7(x) (((x) >> 56) & 0xff)

#define UNSIGNED_FIT8(x) (((x) & 0xffffffffffffff00) == 0)
#define UNSIGNED_FIT32(x) (((x) & 0xffffffff00000000) == 0)
#define SIGNED_FIT8(x) (((x) & 0xffffff80) == 0) || (((x) & 0xffffff80) == 0xffffff80)

struct _asm_x64_t {
    uint pass;
    mp_uint_t code_offset;
    mp_uint_t code_size;
    byte *code_base;
    byte dummy_data[8];

    mp_uint_t max_num_labels;
    mp_uint_t *label_offsets;
    int num_locals;
};

asm_x64_t *asm_x64_new(mp_uint_t max_num_labels) {
    asm_x64_t *as;

    as = m_new0(asm_x64_t, 1);
    as->max_num_labels = max_num_labels;
    as->label_offsets = m_new(mp_uint_t, max_num_labels);

    return as;
}

void asm_x64_free(asm_x64_t *as, bool free_code) {
    if (free_code) {
        MP_PLAT_FREE_EXEC(as->code_base, as->code_size);
    }
    m_del(mp_uint_t, as->label_offsets, as->max_num_labels);
    m_del_obj(asm_x64_t, as);
}

void asm_x64_start_pass(asm_x64_t *as, uint pass) {
    if (pass == ASM_X64_PASS_COMPUTE) {
        // reset all labels
        memset(as->label_offsets, -1, as->max_num_labels * sizeof(mp_uint_t));
    } if (pass == ASM_X64_PASS_EMIT) {
        MP_PLAT_ALLOC_EXEC(as->code_offset, (void**)&as->code_base, &as->code_size);
        if (as->code_base == NULL) {
            assert(0);
        }
        //printf("code_size: %u\n", as->code_size);
    }
    as->pass = pass;
    as->code_offset = 0;
}

void asm_x64_end_pass(asm_x64_t *as) {
    // could check labels are resolved...
    (void)as;
}

// all functions must go through this one to emit bytes
STATIC byte *asm_x64_get_cur_to_write_bytes(asm_x64_t *as, int num_bytes_to_write) {
    //printf("emit %d\n", num_bytes_to_write);
    if (as->pass < ASM_X64_PASS_EMIT) {
        as->code_offset += num_bytes_to_write;
        return as->dummy_data;
    } else {
        assert(as->code_offset + num_bytes_to_write <= as->code_size);
        byte *c = as->code_base + as->code_offset;
        as->code_offset += num_bytes_to_write;
        return c;
    }
}

mp_uint_t asm_x64_get_code_pos(asm_x64_t *as) {
    return as->code_offset;
}

mp_uint_t asm_x64_get_code_size(asm_x64_t *as) {
    return as->code_size;
}

void *asm_x64_get_code(asm_x64_t *as) {
    return as->code_base;
}

STATIC void asm_x64_write_byte_1(asm_x64_t *as, byte b1) {
    byte* c = asm_x64_get_cur_to_write_bytes(as, 1);
    c[0] = b1;
}

STATIC void asm_x64_write_byte_2(asm_x64_t *as, byte b1, byte b2) {
    byte* c = asm_x64_get_cur_to_write_bytes(as, 2);
    c[0] = b1;
    c[1] = b2;
}

STATIC void asm_x64_write_byte_3(asm_x64_t *as, byte b1, byte b2, byte b3) {
    byte* c = asm_x64_get_cur_to_write_bytes(as, 3);
    c[0] = b1;
    c[1] = b2;
    c[2] = b3;
}

STATIC void asm_x64_write_word32(asm_x64_t *as, int w32) {
    byte* c = asm_x64_get_cur_to_write_bytes(as, 4);
    c[0] = IMM32_L0(w32);
    c[1] = IMM32_L1(w32);
    c[2] = IMM32_L2(w32);
    c[3] = IMM32_L3(w32);
}

STATIC void asm_x64_write_word64(asm_x64_t *as, int64_t w64) {
    byte* c = asm_x64_get_cur_to_write_bytes(as, 8);
    c[0] = IMM32_L0(w64);
    c[1] = IMM32_L1(w64);
    c[2] = IMM32_L2(w64);
    c[3] = IMM32_L3(w64);
    c[4] = IMM64_L4(w64);
    c[5] = IMM64_L5(w64);
    c[6] = IMM64_L6(w64);
    c[7] = IMM64_L7(w64);
}

// align must be a multiple of 2
void asm_x64_align(asm_x64_t* as, mp_uint_t align) {
    // TODO fill unused data with NOPs?
    as->code_offset = (as->code_offset + align - 1) & (~(align - 1));
}

void asm_x64_data(asm_x64_t* as, mp_uint_t bytesize, mp_uint_t val) {
    byte *c = asm_x64_get_cur_to_write_bytes(as, bytesize);
    // machine is little endian
    for (uint i = 0; i < bytesize; i++) {
        *c++ = val;
        val >>= 8;
    }
}

/* unused
STATIC void asm_x64_write_word32_to(asm_x64_t *as, int offset, int w32) {
    byte* c;
    assert(offset + 4 <= as->code_size);
    c = as->code_base + offset;
    c[0] = IMM32_L0(w32);
    c[1] = IMM32_L1(w32);
    c[2] = IMM32_L2(w32);
    c[3] = IMM32_L3(w32);
}
*/

STATIC void asm_x64_write_r64_disp(asm_x64_t *as, int r64, int disp_r64, int disp_offset) {
    assert(disp_r64 != ASM_X64_REG_RSP);

    if (disp_r64 == ASM_X64_REG_R12) {
        // special case for r12; not fully implemented
        assert(SIGNED_FIT8(disp_offset));
        asm_x64_write_byte_3(as, MODRM_R64(r64) | MODRM_RM_DISP8 | MODRM_RM_R64(disp_r64), 0x24, IMM32_L0(disp_offset));
        return;
    }

    if (disp_offset == 0 && disp_r64 != ASM_X64_REG_RBP) {
        asm_x64_write_byte_1(as, MODRM_R64(r64) | MODRM_RM_DISP0 | MODRM_RM_R64(disp_r64));
    } else if (SIGNED_FIT8(disp_offset)) {
        asm_x64_write_byte_2(as, MODRM_R64(r64) | MODRM_RM_DISP8 | MODRM_RM_R64(disp_r64), IMM32_L0(disp_offset));
    } else {
        asm_x64_write_byte_1(as, MODRM_R64(r64) | MODRM_RM_DISP32 | MODRM_RM_R64(disp_r64));
        asm_x64_write_word32(as, disp_offset);
    }
}

STATIC void asm_x64_generic_r64_r64(asm_x64_t *as, int dest_r64, int src_r64, int op) {
    asm_x64_write_byte_3(as, REX_PREFIX | REX_W | REX_R_FROM_R64(src_r64) | REX_B_FROM_R64(dest_r64), op, MODRM_R64(src_r64) | MODRM_RM_REG | MODRM_RM_R64(dest_r64));
}

void asm_x64_nop(asm_x64_t *as) {
    asm_x64_write_byte_1(as, OPCODE_NOP);
}

void asm_x64_push_r64(asm_x64_t *as, int src_r64) {
    if (src_r64 < 8) {
        asm_x64_write_byte_1(as, OPCODE_PUSH_R64 | src_r64);
    } else {
        asm_x64_write_byte_2(as, REX_PREFIX | REX_B, OPCODE_PUSH_R64 | (src_r64 & 7));
    }
}

/*
void asm_x64_push_i32(asm_x64_t *as, int src_i32) {
    asm_x64_write_byte_1(as, OPCODE_PUSH_I64);
    asm_x64_write_word32(as, src_i32); // will be sign extended to 64 bits
}
*/

/*
void asm_x64_push_disp(asm_x64_t *as, int src_r64, int src_offset) {
    assert(src_r64 < 8);
    asm_x64_write_byte_1(as, OPCODE_PUSH_M64);
    asm_x64_write_r64_disp(as, 6, src_r64, src_offset);
}
*/

void asm_x64_pop_r64(asm_x64_t *as, int dest_r64) {
    if (dest_r64 < 8) {
        asm_x64_write_byte_1(as, OPCODE_POP_R64 | dest_r64);
    } else {
        asm_x64_write_byte_2(as, REX_PREFIX | REX_B, OPCODE_POP_R64 | (dest_r64 & 7));
    }
}

STATIC void asm_x64_ret(asm_x64_t *as) {
    asm_x64_write_byte_1(as, OPCODE_RET);
}

void asm_x64_mov_r64_r64(asm_x64_t *as, int dest_r64, int src_r64) {
    asm_x64_generic_r64_r64(as, dest_r64, src_r64, OPCODE_MOV_R64_TO_RM64);
}

void asm_x64_mov_r8_to_mem8(asm_x64_t *as, int src_r64, int dest_r64, int dest_disp) {
    if (src_r64 < 8 && dest_r64 < 8) {
        asm_x64_write_byte_1(as, OPCODE_MOV_R8_TO_RM8);
    } else {
        asm_x64_write_byte_2(as, REX_PREFIX | REX_R_FROM_R64(src_r64) | REX_B_FROM_R64(dest_r64), OPCODE_MOV_R8_TO_RM8);
    }
    asm_x64_write_r64_disp(as, src_r64, dest_r64, dest_disp);
}

void asm_x64_mov_r16_to_mem16(asm_x64_t *as, int src_r64, int dest_r64, int dest_disp) {
    if (src_r64 < 8 && dest_r64 < 8) {
        asm_x64_write_byte_2(as, OP_SIZE_PREFIX, OPCODE_MOV_R64_TO_RM64);
    } else {
        asm_x64_write_byte_3(as, OP_SIZE_PREFIX, REX_PREFIX | REX_R_FROM_R64(src_r64) | REX_B_FROM_R64(dest_r64), OPCODE_MOV_R64_TO_RM64);
    }
    asm_x64_write_r64_disp(as, src_r64, dest_r64, dest_disp);
}

void asm_x64_mov_r32_to_mem32(asm_x64_t *as, int src_r64, int dest_r64, int dest_disp) {
    if (src_r64 < 8 && dest_r64 < 8) {
        asm_x64_write_byte_1(as, OPCODE_MOV_R64_TO_RM64);
    } else {
        asm_x64_write_byte_2(as, REX_PREFIX | REX_R_FROM_R64(src_r64) | REX_B_FROM_R64(dest_r64), OPCODE_MOV_R64_TO_RM64);
    }
    asm_x64_write_r64_disp(as, src_r64, dest_r64, dest_disp);
}

void asm_x64_mov_r64_to_mem64(asm_x64_t *as, int src_r64, int dest_r64, int dest_disp) {
    // use REX prefix for 64 bit operation
    asm_x64_write_byte_2(as, REX_PREFIX | REX_W | REX_R_FROM_R64(src_r64) | REX_B_FROM_R64(dest_r64), OPCODE_MOV_R64_TO_RM64);
    asm_x64_write_r64_disp(as, src_r64, dest_r64, dest_disp);
}

void asm_x64_mov_mem8_to_r64zx(asm_x64_t *as, int src_r64, int src_disp, int dest_r64) {
    assert(src_r64 < 8);
    if (dest_r64 < 8) {
        asm_x64_write_byte_2(as, 0x0f, OPCODE_MOVZX_RM8_TO_R64);
    } else {
        asm_x64_write_byte_3(as, REX_PREFIX | REX_R, 0x0f, OPCODE_MOVZX_RM8_TO_R64);
    }
    asm_x64_write_r64_disp(as, dest_r64, src_r64, src_disp);
}

void asm_x64_mov_mem16_to_r64zx(asm_x64_t *as, int src_r64, int src_disp, int dest_r64) {
    assert(src_r64 < 8);
    if (dest_r64 < 8) {
        asm_x64_write_byte_2(as, 0x0f, OPCODE_MOVZX_RM16_TO_R64);
    } else {
        asm_x64_write_byte_3(as, REX_PREFIX | REX_R, 0x0f, OPCODE_MOVZX_RM16_TO_R64);
    }
    asm_x64_write_r64_disp(as, dest_r64, src_r64, src_disp);
}

void asm_x64_mov_mem32_to_r64zx(asm_x64_t *as, int src_r64, int src_disp, int dest_r64) {
    assert(src_r64 < 8);
    if (dest_r64 < 8) {
        asm_x64_write_byte_1(as, OPCODE_MOV_RM64_TO_R64);
    } else {
        asm_x64_write_byte_2(as, REX_PREFIX | REX_R, OPCODE_MOV_RM64_TO_R64);
    }
    asm_x64_write_r64_disp(as, dest_r64, src_r64, src_disp);
}

void asm_x64_mov_mem64_to_r64(asm_x64_t *as, int src_r64, int src_disp, int dest_r64) {
    // use REX prefix for 64 bit operation
    asm_x64_write_byte_2(as, REX_PREFIX | REX_W | REX_R_FROM_R64(dest_r64) | REX_B_FROM_R64(src_r64), OPCODE_MOV_RM64_TO_R64);
    asm_x64_write_r64_disp(as, dest_r64, src_r64, src_disp);
}

STATIC void asm_x64_lea_disp_to_r64(asm_x64_t *as, int src_r64, int src_disp, int dest_r64) {
    // use REX prefix for 64 bit operation
    assert(src_r64 < 8);
    assert(dest_r64 < 8);
    asm_x64_write_byte_2(as, REX_PREFIX | REX_W, OPCODE_LEA_MEM_TO_R64);
    asm_x64_write_r64_disp(as, dest_r64, src_r64, src_disp);
}

/*
void asm_x64_mov_i8_to_r8(asm_x64_t *as, int src_i8, int dest_r64) {
    assert(dest_r64 < 8);
    asm_x64_write_byte_2(as, OPCODE_MOV_I8_TO_R8 | dest_r64, src_i8);
}
*/

STATIC void asm_x64_mov_i32_to_r64(asm_x64_t *as, int src_i32, int dest_r64) {
    // cpu defaults to i32 to r64, with zero extension
    if (dest_r64 < 8) {
        asm_x64_write_byte_1(as, OPCODE_MOV_I64_TO_R64 | dest_r64);
    } else {
        asm_x64_write_byte_2(as, REX_PREFIX | REX_B, OPCODE_MOV_I64_TO_R64 | (dest_r64 & 7));
    }
    asm_x64_write_word32(as, src_i32);
}

void asm_x64_mov_i64_to_r64(asm_x64_t *as, int64_t src_i64, int dest_r64) {
    // cpu defaults to i32 to r64
    // to mov i64 to r64 need to use REX prefix
    assert(dest_r64 < 8);
    asm_x64_write_byte_2(as, REX_PREFIX | REX_W, OPCODE_MOV_I64_TO_R64 | dest_r64);
    asm_x64_write_word64(as, src_i64);
}

void asm_x64_mov_i64_to_r64_optimised(asm_x64_t *as, int64_t src_i64, int dest_r64) {
    // TODO use movzx, movsx if possible
    if (UNSIGNED_FIT32(src_i64)) {
        // 5 bytes
        asm_x64_mov_i32_to_r64(as, src_i64 & 0xffffffff, dest_r64);
    } else {
        // 10 bytes
        asm_x64_mov_i64_to_r64(as, src_i64, dest_r64);
    }
}

// src_i64 is stored as a full word in the code, and aligned to machine-word boundary
void asm_x64_mov_i64_to_r64_aligned(asm_x64_t *as, int64_t src_i64, int dest_r64) {
    // mov instruction uses 2 bytes for the instruction, before the i64
    while (((as->code_offset + 2) & (WORD_SIZE - 1)) != 0) {
        asm_x64_nop(as);
    }
    asm_x64_mov_i64_to_r64(as, src_i64, dest_r64);
}

void asm_x64_and_r64_r64(asm_x64_t *as, int dest_r64, int src_r64) {
    asm_x64_generic_r64_r64(as, dest_r64, src_r64, OPCODE_AND_R64_TO_RM64);
}

void asm_x64_or_r64_r64(asm_x64_t *as, int dest_r64, int src_r64) {
    asm_x64_generic_r64_r64(as, dest_r64, src_r64, OPCODE_OR_R64_TO_RM64);
}

void asm_x64_xor_r64_r64(asm_x64_t *as, int dest_r64, int src_r64) {
    asm_x64_generic_r64_r64(as, dest_r64, src_r64, OPCODE_XOR_R64_TO_RM64);
}

void asm_x64_shl_r64_cl(asm_x64_t* as, int dest_r64) {
    asm_x64_generic_r64_r64(as, dest_r64, 4, OPCODE_SHL_RM64_CL);
}

void asm_x64_sar_r64_cl(asm_x64_t* as, int dest_r64) {
    asm_x64_generic_r64_r64(as, dest_r64, 7, OPCODE_SAR_RM64_CL);
}

void asm_x64_add_r64_r64(asm_x64_t *as, int dest_r64, int src_r64) {
    asm_x64_generic_r64_r64(as, dest_r64, src_r64, OPCODE_ADD_R64_TO_RM64);
}

void asm_x64_sub_r64_r64(asm_x64_t *as, int dest_r64, int src_r64) {
    asm_x64_generic_r64_r64(as, dest_r64, src_r64, OPCODE_SUB_R64_FROM_RM64);
}

void asm_x64_mul_r64_r64(asm_x64_t *as, int dest_r64, int src_r64) {
    // imul reg64, reg/mem64 -- 0x0f 0xaf /r
    asm_x64_write_byte_1(as, REX_PREFIX | REX_W | REX_R_FROM_R64(dest_r64) | REX_B_FROM_R64(src_r64));
    asm_x64_write_byte_3(as, 0x0f, 0xaf, MODRM_R64(dest_r64) | MODRM_RM_REG | MODRM_RM_R64(src_r64));
}

/*
void asm_x64_sub_i32_from_r32(asm_x64_t *as, int src_i32, int dest_r32) {
    if (SIGNED_FIT8(src_i32)) {
        // defaults to 32 bit operation
        asm_x64_write_byte_2(as, OPCODE_SUB_I8_FROM_RM64, MODRM_R64(5) | MODRM_RM_REG | MODRM_RM_R64(dest_r32));
        asm_x64_write_byte_1(as, src_i32 & 0xff);
    } else {
        // defaults to 32 bit operation
        asm_x64_write_byte_2(as, OPCODE_SUB_I32_FROM_RM64, MODRM_R64(5) | MODRM_RM_REG | MODRM_RM_R64(dest_r32));
        asm_x64_write_word32(as, src_i32);
    }
}
*/

STATIC void asm_x64_sub_r64_i32(asm_x64_t *as, int dest_r64, int src_i32) {
    assert(dest_r64 < 8);
    if (SIGNED_FIT8(src_i32)) {
        // use REX prefix for 64 bit operation
        asm_x64_write_byte_3(as, REX_PREFIX | REX_W, OPCODE_SUB_I8_FROM_RM64, MODRM_R64(5) | MODRM_RM_REG | MODRM_RM_R64(dest_r64));
        asm_x64_write_byte_1(as, src_i32 & 0xff);
    } else {
        // use REX prefix for 64 bit operation
        asm_x64_write_byte_3(as, REX_PREFIX | REX_W, OPCODE_SUB_I32_FROM_RM64, MODRM_R64(5) | MODRM_RM_REG | MODRM_RM_R64(dest_r64));
        asm_x64_write_word32(as, src_i32);
    }
}

/*
void asm_x64_shl_r32_by_imm(asm_x64_t *as, int r32, int imm) {
    asm_x64_write_byte_2(as, OPCODE_SHL_RM32_BY_I8, MODRM_R64(4) | MODRM_RM_REG | MODRM_RM_R64(r32));
    asm_x64_write_byte_1(as, imm);
}

void asm_x64_shr_r32_by_imm(asm_x64_t *as, int r32, int imm) {
    asm_x64_write_byte_2(as, OPCODE_SHR_RM32_BY_I8, MODRM_R64(5) | MODRM_RM_REG | MODRM_RM_R64(r32));
    asm_x64_write_byte_1(as, imm);
}

void asm_x64_sar_r32_by_imm(asm_x64_t *as, int r32, int imm) {
    asm_x64_write_byte_2(as, OPCODE_SAR_RM32_BY_I8, MODRM_R64(7) | MODRM_RM_REG | MODRM_RM_R64(r32));
    asm_x64_write_byte_1(as, imm);
}
*/

void asm_x64_cmp_r64_with_r64(asm_x64_t *as, int src_r64_a, int src_r64_b) {
    asm_x64_generic_r64_r64(as, src_r64_b, src_r64_a, OPCODE_CMP_R64_WITH_RM64);
}

/*
void asm_x64_cmp_i32_with_r32(asm_x64_t *as, int src_i32, int src_r32) {
    if (SIGNED_FIT8(src_i32)) {
        asm_x64_write_byte_2(as, OPCODE_CMP_I8_WITH_RM32, MODRM_R64(7) | MODRM_RM_REG | MODRM_RM_R64(src_r32));
        asm_x64_write_byte_1(as, src_i32 & 0xff);
    } else {
        asm_x64_write_byte_2(as, OPCODE_CMP_I32_WITH_RM32, MODRM_R64(7) | MODRM_RM_REG | MODRM_RM_R64(src_r32));
        asm_x64_write_word32(as, src_i32);
    }
}
*/

void asm_x64_test_r8_with_r8(asm_x64_t *as, int src_r64_a, int src_r64_b) {
    // TODO implement for other registers
    assert(src_r64_a == ASM_X64_REG_RAX);
    assert(src_r64_b == ASM_X64_REG_RAX);
    asm_x64_write_byte_2(as, OPCODE_TEST_R8_WITH_RM8, MODRM_R64(src_r64_a) | MODRM_RM_REG | MODRM_RM_R64(src_r64_b));
}

void asm_x64_setcc_r8(asm_x64_t *as, int jcc_type, int dest_r8) {
    assert(dest_r8 < 8);
    asm_x64_write_byte_3(as, OPCODE_SETCC_RM8_A, OPCODE_SETCC_RM8_B | jcc_type, MODRM_R64(0) | MODRM_RM_REG | MODRM_RM_R64(dest_r8));
}

void asm_x64_label_assign(asm_x64_t *as, mp_uint_t label) {
    assert(label < as->max_num_labels);
    if (as->pass < ASM_X64_PASS_EMIT) {
        // assign label offset
        assert(as->label_offsets[label] == (mp_uint_t)-1);
        as->label_offsets[label] = as->code_offset;
    } else {
        // ensure label offset has not changed from PASS_COMPUTE to PASS_EMIT
        //printf("l%d: (at %ld=%ld)\n", label, as->label_offsets[label], as->code_offset);
        assert(as->label_offsets[label] == as->code_offset);
    }
}

STATIC mp_uint_t get_label_dest(asm_x64_t *as, mp_uint_t label) {
    assert(label < as->max_num_labels);
    return as->label_offsets[label];
}

void asm_x64_jmp_label(asm_x64_t *as, mp_uint_t label) {
    mp_uint_t dest = get_label_dest(as, label);
    mp_int_t rel = dest - as->code_offset;
    if (dest != (mp_uint_t)-1 && rel < 0) {
        // is a backwards jump, so we know the size of the jump on the first pass
        // calculate rel assuming 8 bit relative jump
        rel -= 2;
        if (SIGNED_FIT8(rel)) {
            asm_x64_write_byte_2(as, OPCODE_JMP_REL8, rel & 0xff);
        } else {
            rel += 2;
            goto large_jump;
        }
    } else {
        // is a forwards jump, so need to assume it's large
        large_jump:
        rel -= 5;
        asm_x64_write_byte_1(as, OPCODE_JMP_REL32);
        asm_x64_write_word32(as, rel);
    }
}

void asm_x64_jcc_label(asm_x64_t *as, int jcc_type, mp_uint_t label) {
    mp_uint_t dest = get_label_dest(as, label);
    mp_int_t rel = dest - as->code_offset;
    if (dest != (mp_uint_t)-1 && rel < 0) {
        // is a backwards jump, so we know the size of the jump on the first pass
        // calculate rel assuming 8 bit relative jump
        rel -= 2;
        if (SIGNED_FIT8(rel)) {
            asm_x64_write_byte_2(as, OPCODE_JCC_REL8 | jcc_type, rel & 0xff);
        } else {
            rel += 2;
            goto large_jump;
        }
    } else {
        // is a forwards jump, so need to assume it's large
        large_jump:
        rel -= 6;
        asm_x64_write_byte_2(as, OPCODE_JCC_REL32_A, OPCODE_JCC_REL32_B | jcc_type);
        asm_x64_write_word32(as, rel);
    }
}

void asm_x64_entry(asm_x64_t *as, int num_locals) {
    asm_x64_push_r64(as, ASM_X64_REG_RBP);
    asm_x64_mov_r64_r64(as, ASM_X64_REG_RBP, ASM_X64_REG_RSP);
    if (num_locals < 0) {
        num_locals = 0;
    }
    num_locals |= 1; // make it odd so stack is aligned on 16 byte boundary
    asm_x64_sub_r64_i32(as, ASM_X64_REG_RSP, num_locals * WORD_SIZE);
    asm_x64_push_r64(as, ASM_X64_REG_RBX);
    asm_x64_push_r64(as, ASM_X64_REG_R12);
    asm_x64_push_r64(as, ASM_X64_REG_R13);
    as->num_locals = num_locals;
}

void asm_x64_exit(asm_x64_t *as) {
    asm_x64_pop_r64(as, ASM_X64_REG_R13);
    asm_x64_pop_r64(as, ASM_X64_REG_R12);
    asm_x64_pop_r64(as, ASM_X64_REG_RBX);
    asm_x64_write_byte_1(as, OPCODE_LEAVE);
    asm_x64_ret(as);
}

// locals:
//  - stored on the stack in ascending order
//  - numbered 0 through as->num_locals-1
//  - RBP points above the last local
//
//                          | RBP
//                          v
//  l0  l1  l2  ...  l(n-1)
//  ^                ^
//  | low address    | high address in RAM
//
STATIC int asm_x64_local_offset_from_ebp(asm_x64_t *as, int local_num) {
    return (-as->num_locals + local_num) * WORD_SIZE;
}

void asm_x64_mov_local_to_r64(asm_x64_t *as, int src_local_num, int dest_r64) {
    asm_x64_mov_mem64_to_r64(as, ASM_X64_REG_RBP, asm_x64_local_offset_from_ebp(as, src_local_num), dest_r64);
}

void asm_x64_mov_r64_to_local(asm_x64_t *as, int src_r64, int dest_local_num) {
    asm_x64_mov_r64_to_mem64(as, src_r64, ASM_X64_REG_RBP, asm_x64_local_offset_from_ebp(as, dest_local_num));
}

void asm_x64_mov_local_addr_to_r64(asm_x64_t *as, int local_num, int dest_r64) {
    int offset = asm_x64_local_offset_from_ebp(as, local_num);
    if (offset == 0) {
        asm_x64_mov_r64_r64(as, dest_r64, ASM_X64_REG_RBP);
    } else {
        asm_x64_lea_disp_to_r64(as, ASM_X64_REG_RBP, offset, dest_r64);
    }
}

/*
void asm_x64_push_local(asm_x64_t *as, int local_num) {
    asm_x64_push_disp(as, ASM_X64_REG_RBP, asm_x64_local_offset_from_ebp(as, local_num));
}

void asm_x64_push_local_addr(asm_x64_t *as, int local_num, int temp_r64) {
    asm_x64_mov_r64_r64(as, temp_r64, ASM_X64_REG_RBP);
    asm_x64_add_i32_to_r32(as, asm_x64_local_offset_from_ebp(as, local_num), temp_r64);
    asm_x64_push_r64(as, temp_r64);
}
*/

/*
   can't use these because code might be relocated when resized

void asm_x64_call(asm_x64_t *as, void* func) {
    asm_x64_sub_i32_from_r32(as, 8, ASM_X64_REG_RSP);
    asm_x64_write_byte_1(as, OPCODE_CALL_REL32);
    asm_x64_write_word32(as, func - (void*)(as->code_cur + 4));
    asm_x64_mov_r64_r64(as, ASM_X64_REG_RSP, ASM_X64_REG_RBP);
}

void asm_x64_call_i1(asm_x64_t *as, void* func, int i1) {
    asm_x64_sub_i32_from_r32(as, 8, ASM_X64_REG_RSP);
    asm_x64_sub_i32_from_r32(as, 12, ASM_X64_REG_RSP);
    asm_x64_push_i32(as, i1);
    asm_x64_write_byte_1(as, OPCODE_CALL_REL32);
    asm_x64_write_word32(as, func - (void*)(as->code_cur + 4));
    asm_x64_add_i32_to_r32(as, 16, ASM_X64_REG_RSP);
    asm_x64_mov_r64_r64(as, ASM_X64_REG_RSP, ASM_X64_REG_RBP);
}
*/

void asm_x64_call_ind(asm_x64_t *as, void *ptr, int temp_r64) {
    assert(temp_r64 < 8);
#ifdef __LP64__
    asm_x64_mov_i64_to_r64_optimised(as, (int64_t)ptr, temp_r64);
#else
    // If we get here, sizeof(int) == sizeof(void*).
    asm_x64_mov_i64_to_r64_optimised(as, (int64_t)(unsigned int)ptr, temp_r64);
#endif
    asm_x64_write_byte_2(as, OPCODE_CALL_RM32, MODRM_R64(2) | MODRM_RM_REG | MODRM_RM_R64(temp_r64));
    // this reduces code size by 2 bytes per call, but doesn't seem to speed it up at all
    // doesn't work anymore because calls are 64 bits away
    /*
    asm_x64_write_byte_1(as, OPCODE_CALL_REL32);
    asm_x64_write_word32(as, ptr - (void*)(as->code_base + as->code_offset + 4));
    */
}

#endif // MICROPY_EMIT_X64
