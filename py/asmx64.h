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

#define ASM_X64_PASS_COMPUTE (1)
#define ASM_X64_PASS_EMIT    (2)

#define REG_RAX (0)
#define REG_RCX (1)
#define REG_RDX (2)
#define REG_RBX (3)
#define REG_RSP (4)
#define REG_RBP (5)
#define REG_RSI (6)
#define REG_RDI (7)

// condition codes, used for jcc and setcc (despite their j-name!)
#define JCC_JB  (0x2) // below, unsigned
#define JCC_JZ  (0x4)
#define JCC_JE  (0x4)
#define JCC_JNZ (0x5)
#define JCC_JNE (0x5)
#define JCC_JL  (0xc) // less, signed

#define REG_RET REG_RAX
#define REG_ARG_1 REG_RDI
#define REG_ARG_2 REG_RSI
#define REG_ARG_3 REG_RDX

typedef struct _asm_x64_t asm_x64_t;

asm_x64_t* asm_x64_new(uint max_num_labels);
void asm_x64_free(asm_x64_t* as, bool free_code);
void asm_x64_start_pass(asm_x64_t *as, uint pass);
void asm_x64_end_pass(asm_x64_t *as);
uint asm_x64_get_code_size(asm_x64_t* as);
void* asm_x64_get_code(asm_x64_t* as);

void asm_x64_nop(asm_x64_t* as);
void asm_x64_push_r64(asm_x64_t* as, int src_r64);
void asm_x64_push_i32(asm_x64_t* as, int src_i32); // will be sign extended to 64 bits
void asm_x64_push_disp(asm_x64_t* as, int src_r32, int src_offset);
void asm_x64_pop_r64(asm_x64_t* as, int dest_r64);
void asm_x64_mov_r64_to_r64(asm_x64_t* as, int src_r64, int dest_r64);
void asm_x64_mov_r32_to_disp(asm_x64_t* as, int src_r32, int dest_r32, int dest_disp);
void asm_x64_mov_disp_to_r32(asm_x64_t* as, int src_r32, int src_disp, int dest_r32);
void asm_x64_mov_i32_to_r64(asm_x64_t* as, int src_i32, int dest_r64);
void asm_x64_mov_i64_to_r64(asm_x64_t* as, int64_t src_i64, int dest_r64);
void asm_x64_mov_i32_to_disp(asm_x64_t* as, int src_i32, int dest_r32, int dest_disp);
void asm_x64_mov_i64_to_r64_optimised(asm_x64_t *as, int64_t src_i64, int dest_r64);
void asm_x64_mov_i64_to_r64_aligned(asm_x64_t *as, int64_t src_i64, int dest_r64);
void asm_x64_xor_r64_to_r64(asm_x64_t *as, int src_r64, int dest_r64);
void asm_x64_add_r64_to_r64(asm_x64_t* as, int src_r64, int dest_r64);
void asm_x64_add_i32_to_r32(asm_x64_t* as, int src_i32, int dest_r32);
void asm_x64_sub_r32_from_r32(asm_x64_t* as, int src_r32, int dest_r32);
void asm_x64_sub_i32_from_r32(asm_x64_t* as, int src_i32, int dest_r32);
void asm_x64_shl_r32_by_imm(asm_x64_t* as, int r32, int imm);
void asm_x64_shr_r32_by_imm(asm_x64_t* as, int r32, int imm);
void asm_x64_sar_r32_by_imm(asm_x64_t* as, int r32, int imm);
void asm_x64_cmp_r64_with_r64(asm_x64_t* as, int src_r64_a, int src_r64_b);
void asm_x64_cmp_r32_with_disp(asm_x64_t* as, int src_r32_a, int src_r32_b, int src_disp_b);
void asm_x64_cmp_disp_with_r32(asm_x64_t* as, int src_r32_a, int src_disp_a, int src_r32_b);
void asm_x64_cmp_i32_with_r32(asm_x64_t* as, int src_i32, int src_r32);
void asm_x64_test_r8_with_r8(asm_x64_t* as, int src_r64_a, int src_r64_b);
void asm_x64_setcc_r8(asm_x64_t* as, int jcc_type, int dest_r8);
void asm_x64_label_assign(asm_x64_t* as, int label);
void asm_x64_jmp_label(asm_x64_t* as, int label);
void asm_x64_jcc_label(asm_x64_t* as, int jcc_type, int label);
void asm_x64_entry(asm_x64_t* as, int num_locals);
void asm_x64_exit(asm_x64_t* as);
void asm_x64_push_arg(asm_x64_t* as, int src_arg_num);
void asm_x64_mov_arg_to_r32(asm_x64_t* as, int src_arg_num, int dest_r32);
void asm_x64_mov_r32_to_arg(asm_x64_t* as, int src_r32, int dest_arg_num);
void asm_x64_mov_local_to_r64(asm_x64_t* as, int src_local_num, int dest_r64);
void asm_x64_mov_r64_to_local(asm_x64_t* as, int src_r64, int dest_local_num);
void asm_x64_mov_local_addr_to_r64(asm_x64_t* as, int local_num, int dest_r64);
void asm_x64_push_local(asm_x64_t* as, int local_num);
void asm_x64_push_local_addr(asm_x64_t* as, int local_num, int temp_r32);
void asm_x64_call_ind(asm_x64_t* as, void* ptr, int temp_r32);
