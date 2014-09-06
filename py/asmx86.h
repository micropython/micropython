/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#define ASM_X86_PASS_COMPUTE (1)
#define ASM_X86_PASS_EMIT    (2)

#define REG_EAX (0)
#define REG_ECX (1)
#define REG_EDX (2)
#define REG_EBX (3)
#define REG_ESP (4)
#define REG_EBP (5)
#define REG_ESI (6)
#define REG_EDI (7)

// condition codes, used for jcc and setcc (despite their j-name!)
#define ASM_X86_CC_JB  (0x2) // below, unsigned
#define ASM_X86_CC_JZ  (0x4)
#define ASM_X86_CC_JE  (0x4)
#define ASM_X86_CC_JNZ (0x5)
#define ASM_X86_CC_JNE (0x5)
#define ASM_X86_CC_JL  (0xc) // less, signed

#define REG_RET REG_EAX
#define REG_ARG_1 REG_EDI
#define REG_ARG_2 REG_ESI
#define REG_ARG_3 REG_EDX

typedef struct _asm_x86_t asm_x86_t;

asm_x86_t* asm_x86_new(mp_uint_t max_num_labels);
void asm_x86_free(asm_x86_t* as, bool free_code);
void asm_x86_start_pass(asm_x86_t *as, mp_uint_t pass);
void asm_x86_end_pass(asm_x86_t *as);
mp_uint_t asm_x86_get_code_size(asm_x86_t* as);
void* asm_x86_get_code(asm_x86_t* as);

void asm_x86_mov_r32_to_r32(asm_x86_t* as, int src_r32, int dest_r32);
void asm_x86_mov_i32_to_r32(asm_x86_t *as, int32_t src_i32, int dest_r32);
void asm_x86_mov_i32_to_r32_aligned(asm_x86_t *as, int32_t src_i32, int dest_r32);
void asm_x86_xor_r32_to_r32(asm_x86_t *as, int src_r32, int dest_r32);
void asm_x86_add_r32_to_r32(asm_x86_t* as, int src_r32, int dest_r32);
void asm_x86_cmp_r32_with_r32(asm_x86_t* as, int src_r32_a, int src_r32_b);
void asm_x86_test_r8_with_r8(asm_x86_t* as, int src_r32_a, int src_r32_b);
void asm_x86_setcc_r8(asm_x86_t* as, mp_uint_t jcc_type, int dest_r8);
void asm_x86_label_assign(asm_x86_t* as, mp_uint_t label);
void asm_x86_jmp_label(asm_x86_t* as, mp_uint_t label);
void asm_x86_jcc_label(asm_x86_t* as, mp_uint_t jcc_type, mp_uint_t label);
void asm_x86_entry(asm_x86_t* as, mp_uint_t num_locals);
void asm_x86_exit(asm_x86_t* as);
void asm_x86_mov_local_to_r32(asm_x86_t* as, int src_local_num, int dest_r32);
void asm_x86_mov_r32_to_local(asm_x86_t* as, int src_r32, int dest_local_num);
void asm_x86_mov_local_addr_to_r32(asm_x86_t* as, int local_num, int dest_r32);
void asm_x86_call_ind(asm_x86_t* as, void* ptr, mp_uint_t n_args, int temp_r32);
