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

// Exception stack entry
typedef struct _mp_exc_stack {
    const byte *handler;
    // bit 0 is saved currently_in_except_block value
    mp_obj_t *val_sp;
    // Saved exception, valid if currently_in_except_block bit is 1
    mp_obj_t prev_exc;
    // We might only have 2 interesting cases here: SETUP_EXCEPT & SETUP_FINALLY,
    // consider storing it in bit 1 of val_sp. TODO: SETUP_WITH?
    byte opcode;
} mp_exc_stack_t;

typedef struct _mp_code_state {
    const byte *code_info;
    const byte *ip;
    mp_obj_t *sp;
    // bit 0 is saved currently_in_except_block value
    mp_exc_stack_t *exc_sp;
    uint n_state;
    // Variable-length
    mp_obj_t state[0];
    // Variable-length, never accessed by name, only as (void*)(state + n_state)
    //mp_exc_stack_t exc_state[0];
} mp_code_state;

mp_vm_return_kind_t mp_execute_bytecode(mp_code_state *code_state, volatile mp_obj_t inject_exc);
void mp_setup_code_state(mp_code_state *code_state, mp_obj_t self_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args);
void mp_bytecode_print(const void *descr, const byte *code, int len);
void mp_bytecode_print2(const byte *code, int len);

// Helper macros to access pointer with least significant bit holding a flag
#define MP_TAGPTR_PTR(x) ((void*)((mp_uint_t)(x) & ~((mp_uint_t)1)))
#define MP_TAGPTR_TAG(x) ((mp_uint_t)(x) & 1)
#define MP_TAGPTR_MAKE(ptr, tag) ((void*)((mp_uint_t)(ptr) | tag))
