/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
 * SPDX-FileCopyrightText: Copyright (c) 2014 Paul Sokolovsky
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
#ifndef MICROPY_INCLUDED_PY_BC_H
#define MICROPY_INCLUDED_PY_BC_H

#include "py/runtime.h"
#include "py/objfun.h"

// bytecode layout:
//
//  func signature  : var uint
//      contains six values interleaved bit-wise as: xSSSSEAA [xFSSKAED repeated]
//          x = extension           another byte follows
//          S = n_state - 1         number of entries in Python value stack
//          E = n_exc_stack         number of entries in exception stack
//          F = scope_flags         four bits of flags, MP_SCOPE_FLAG_xxx
//          A = n_pos_args          number of arguments this function takes
//          K = n_kwonly_args       number of keyword-only arguments this function takes
//          D = n_def_pos_args      number of default positional arguments
//
//  prelude size    : var uint
//      contains two values interleaved bit-wise as: xIIIIIIC repeated
//          x = extension           another byte follows
//          I = n_info              number of bytes in source info section
//          C = n_cells             number of bytes/cells in closure section
//
//  source info section:
//      simple_name : var qstr
//      source_file : var qstr
//      <line number info>
//
//  closure section:
//      local_num0  : byte
//      ...         : byte
//      local_numN  : byte          N = n_cells-1
//
//  <word alignment padding>        only needed if bytecode contains pointers
//
//  <bytecode>
//
//
// constant table layout:
//
//  argname0        : obj (qstr)
//  ...             : obj (qstr)
//  argnameN        : obj (qstr)    N = num_pos_args + num_kwonly_args
//  const0          : obj
//  constN          : obj

#define MP_BC_PRELUDE_SIG_ENCODE(S, E, scope, out_byte, out_env) \
    do {                                                            \
        /*// Get values to store in prelude */                      \
        size_t F = scope->scope_flags & MP_SCOPE_FLAG_ALL_SIG;      \
        size_t A = scope->num_pos_args;                             \
        size_t K = scope->num_kwonly_args;                          \
        size_t D = scope->num_def_pos_args;                         \
                                                                \
        /* Adjust S to shrink range, to compress better */          \
        S -= 1;                                                     \
                                                                \
        /* Encode prelude */                                        \
        /* xSSSSEAA */                                              \
        uint8_t z = (S & 0xf) << 3 | (E & 1) << 2 | (A & 3);        \
        S >>= 4;                                                    \
        E >>= 1;                                                    \
        A >>= 2;                                                    \
        while (S | E | F | A | K | D) {                             \
            out_byte(out_env, 0x80 | z);                            \
            /* xFSSKAED */                                          \
            z = (F & 1) << 6 | (S & 3) << 4 | (K & 1) << 3          \
                | (A & 1) << 2 | (E & 1) << 1 | (D & 1);            \
            S >>= 2;                                                \
            E >>= 1;                                                \
            F >>= 1;                                                \
            A >>= 1;                                                \
            K >>= 1;                                                \
            D >>= 1;                                                \
        }                                                           \
        out_byte(out_env, z);                                       \
    } while (0)

#define MP_BC_PRELUDE_SIG_DECODE_INTO(ip, S, E, F, A, K, D)     \
    do {                                                            \
        uint8_t z = *(ip)++;                                        \
        /* xSSSSEAA */                                              \
        S = (z >> 3) & 0xf;                                         \
        E = (z >> 2) & 0x1;                                         \
        F = 0;                                                      \
        A = z & 0x3;                                                \
        K = 0;                                                      \
        D = 0;                                                      \
        for (unsigned n = 0; z & 0x80; ++n) {                       \
            z = *(ip)++;                                            \
            /* xFSSKAED */                                          \
            S |= (z & 0x30) << (2 * n);                             \
            E |= (z & 0x02) << n;                                   \
            F |= ((z & 0x40) >> 6) << n;                            \
            A |= (z & 0x4) << n;                                    \
            K |= ((z & 0x08) >> 3) << n;                            \
            D |= (z & 0x1) << n;                                    \
        }                                                           \
        S += 1;                                                     \
    } while (0)

#define MP_BC_PRELUDE_SIG_DECODE(ip) \
    size_t n_state, n_exc_stack, scope_flags, n_pos_args, n_kwonly_args, n_def_pos_args; \
    MP_BC_PRELUDE_SIG_DECODE_INTO(ip, n_state, n_exc_stack, scope_flags, n_pos_args, n_kwonly_args, n_def_pos_args)

#define MP_BC_PRELUDE_SIZE_ENCODE(I, C, out_byte, out_env)      \
    do {                                                            \
        /* Encode bit-wise as: xIIIIIIC */                          \
        uint8_t z = 0;                                              \
        do {                                                        \
            z = (I & 0x3f) << 1 | (C & 1);                          \
            C >>= 1;                                                \
            I >>= 6;                                                \
            if (C | I) {                                            \
                z |= 0x80;                                          \
            }                                                       \
            out_byte(out_env, z);                                   \
        } while (C | I);                                            \
    } while (0)

#define MP_BC_PRELUDE_SIZE_DECODE_INTO(ip, I, C)                \
    do {                                                            \
        uint8_t z;                                                  \
        C = 0;                                                      \
        I = 0;                                                      \
        for (unsigned n = 0;; ++n) {                                \
            z = *(ip)++;                                            \
            /* xIIIIIIC */                                          \
            C |= (z & 1) << n;                                      \
            I |= ((z & 0x7e) >> 1) << (6 * n);                      \
            if (!(z & 0x80)) {                                      \
                break;                                              \
            }                                                       \
        }                                                           \
    } while (0)

#define MP_BC_PRELUDE_SIZE_DECODE(ip) \
    size_t n_info, n_cell; \
    MP_BC_PRELUDE_SIZE_DECODE_INTO(ip, n_info, n_cell)

// Sentinel value for mp_code_state_t.exc_sp_idx
#define MP_CODE_STATE_EXC_SP_IDX_SENTINEL ((uint16_t)-1)

// To convert mp_code_state_t.exc_sp_idx to/from a pointer to mp_exc_stack_t
#define MP_CODE_STATE_EXC_SP_IDX_FROM_PTR(exc_stack, exc_sp) ((exc_sp) + 1 - (exc_stack))
#define MP_CODE_STATE_EXC_SP_IDX_TO_PTR(exc_stack, exc_sp_idx) ((exc_stack) + (exc_sp_idx) - 1)

typedef struct _mp_bytecode_prelude_t {
    uint n_state;
    uint n_exc_stack;
    uint scope_flags;
    uint n_pos_args;
    uint n_kwonly_args;
    uint n_def_pos_args;
    qstr qstr_block_name;
    qstr qstr_source_file;
    const byte *line_info;
    const byte *opcodes;
} mp_bytecode_prelude_t;

// Exception stack entry
typedef struct _mp_exc_stack_t {
    const byte *handler;
    // bit 0 is currently unused
    // bit 1 is whether the opcode was SETUP_WITH or SETUP_FINALLY
    mp_obj_t *val_sp;
    // Saved exception
    mp_obj_base_t *prev_exc;
} mp_exc_stack_t;

typedef struct _mp_code_state_t {
    // The fun_bc entry points to the underlying function object that is being executed.
    // It is needed to access the start of bytecode and the const_table.
    // It is also needed to prevent the GC from reclaiming the bytecode during execution,
    // because the ip pointer below will always point to the interior of the bytecode.
    mp_obj_fun_bc_t *fun_bc;
    const byte *ip;
    mp_obj_t *sp;
    uint16_t n_state;
    uint16_t exc_sp_idx;
    mp_obj_dict_t *old_globals;
    #if MICROPY_STACKLESS
    struct _mp_code_state_t *prev;
    #endif
    #if MICROPY_PY_SYS_SETTRACE
    struct _mp_code_state_t *prev_state;
    struct _mp_obj_frame_t *frame;
    #endif
    // Variable-length
    mp_obj_t state[0];
    // Variable-length, never accessed by name, only as (void*)(state + n_state)
    // mp_exc_stack_t exc_state[0];
} mp_code_state_t;

mp_uint_t mp_decode_uint(const byte **ptr);
mp_uint_t mp_decode_uint_value(const byte *ptr);
const byte *mp_decode_uint_skip(const byte *ptr);

mp_vm_return_kind_t mp_execute_bytecode(mp_code_state_t *code_state, volatile mp_obj_t inject_exc);
mp_code_state_t *mp_obj_fun_bc_prepare_codestate(mp_obj_t func, size_t n_args, size_t n_kw, const mp_obj_t *args);
void mp_setup_code_state(mp_code_state_t *code_state, size_t n_args, size_t n_kw, const mp_obj_t *args);
void mp_bytecode_print(const mp_print_t *print, const void *descr, const byte *code, mp_uint_t len, const mp_uint_t *const_table);
void mp_bytecode_print2(const mp_print_t *print, const byte *code, size_t len, const mp_uint_t *const_table);
const byte *mp_bytecode_print_str(const mp_print_t *print, const byte *ip);
#define mp_bytecode_print_inst(print, code, const_table) mp_bytecode_print2(print, code, 1, const_table)

// Helper macros to access pointer with least significant bits holding flags
#define MP_TAGPTR_PTR(x) ((void *)((uintptr_t)(x) & ~((uintptr_t)3)))
#define MP_TAGPTR_TAG0(x) ((uintptr_t)(x) & 1)
#define MP_TAGPTR_TAG1(x) ((uintptr_t)(x) & 2)
#define MP_TAGPTR_MAKE(ptr, tag) ((void *)((uintptr_t)(ptr) | (tag)))

#if MICROPY_PERSISTENT_CODE_LOAD || MICROPY_PERSISTENT_CODE_SAVE

uint mp_opcode_format(const byte *ip, size_t *opcode_size, bool count_var_uint);

#endif

static inline size_t mp_bytecode_get_source_line(const byte *line_info, size_t bc_offset) {
    size_t source_line = 1;
    size_t c;
    while ((c = *line_info)) {
        size_t b, l;
        if ((c & 0x80) == 0) {
            // 0b0LLBBBBB encoding
            b = c & 0x1f;
            l = c >> 5;
            line_info += 1;
        } else {
            // 0b1LLLBBBB 0bLLLLLLLL encoding (l's LSB in second byte)
            b = c & 0xf;
            l = ((c << 4) & 0x700) | line_info[1];
            line_info += 2;
        }
        if (bc_offset >= b) {
            bc_offset -= b;
            source_line += l;
        } else {
            // found source line corresponding to bytecode offset
            break;
        }
    }
    return source_line;
}

#endif // MICROPY_INCLUDED_PY_BC_H
