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
#ifndef __MICROPY_INCLUDED_PY_BC_H__
#define __MICROPY_INCLUDED_PY_BC_H__

#include "py/runtime.h"
#include "py/obj.h"

// bytecode layout:
//
//  n_state         : var uint
//  n_exc_stack     : var uint
//  scope_flags     : byte
//  n_pos_args      : byte          number of arguments this function takes
//  n_kwonly_args   : byte          number of keyword-only arguments this function takes
//  n_def_pos_args  : byte          number of default positional arguments
//
//  code_info_size  : var uint |    code_info_size counts bytes in this chunk
//  simple_name     : var qstr |
//  source_file     : var qstr |
//  <line number info>         |
//  <word alignment padding>   |    only needed if bytecode contains pointers
//
//  local_num0      : byte     |
//  ...             : byte     |
//  local_numN      : byte     |    N = num_cells
//  255             : byte     |    end of list sentinel
//  <bytecode>                 |
//
//
// constant table layout:
//
//  argname0        : obj (qstr)
//  ...             : obj (qstr)
//  argnameN        : obj (qstr)    N = num_pos_args + num_kwonly_args
//  const0          : obj
//  constN          : obj

// Exception stack entry
typedef struct _mp_exc_stack_t {
    const byte *handler;
    // bit 0 is saved currently_in_except_block value
    // bit 1 is whether the opcode was SETUP_WITH or SETUP_FINALLY
    mp_obj_t *val_sp;
    // Saved exception, valid if currently_in_except_block bit is 1
    mp_obj_base_t *prev_exc;
} mp_exc_stack_t;

typedef struct _mp_code_state_t {
    const byte *code_info;
    const byte *ip;
    const mp_uint_t *const_table;
    mp_obj_t *sp;
    // bit 0 is saved currently_in_except_block value
    mp_exc_stack_t *exc_sp;
    mp_obj_dict_t *old_globals;
    #if MICROPY_STACKLESS
    struct _mp_code_state_t *prev;
    #endif
    size_t n_state;
    // Variable-length
    mp_obj_t state[0];
    // Variable-length, never accessed by name, only as (void*)(state + n_state)
    //mp_exc_stack_t exc_state[0];
} mp_code_state_t;

mp_uint_t mp_decode_uint(const byte **ptr);

mp_vm_return_kind_t mp_execute_bytecode(mp_code_state_t *code_state, volatile mp_obj_t inject_exc);
mp_code_state_t *mp_obj_fun_bc_prepare_codestate(mp_obj_t func, size_t n_args, size_t n_kw, const mp_obj_t *args);
struct _mp_obj_fun_bc_t;
void mp_setup_code_state(mp_code_state_t *code_state, struct _mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, const mp_obj_t *args);
void mp_bytecode_print(const void *descr, const byte *code, mp_uint_t len, const mp_uint_t *const_table);
void mp_bytecode_print2(const byte *code, size_t len, const mp_uint_t *const_table);
const byte *mp_bytecode_print_str(const byte *ip);
#define mp_bytecode_print_inst(code, const_table) mp_bytecode_print2(code, 1, const_table)

// Helper macros to access pointer with least significant bits holding flags
#define MP_TAGPTR_PTR(x) ((void*)((uintptr_t)(x) & ~((uintptr_t)3)))
#define MP_TAGPTR_TAG0(x) ((uintptr_t)(x) & 1)
#define MP_TAGPTR_TAG1(x) ((uintptr_t)(x) & 2)
#define MP_TAGPTR_MAKE(ptr, tag) ((void*)((uintptr_t)(ptr) | (tag)))

#if MICROPY_PERSISTENT_CODE_LOAD || MICROPY_PERSISTENT_CODE_SAVE

#define MP_OPCODE_BYTE (0)
#define MP_OPCODE_QSTR (1)
#define MP_OPCODE_VAR_UINT (2)
#define MP_OPCODE_OFFSET (3)

uint mp_opcode_format(const byte *ip, size_t *opcode_size);

#endif

#endif // __MICROPY_INCLUDED_PY_BC_H__
