/*
 * This file is part of the MicroPython project, http://micropython.org/
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
#ifndef MICROPY_INCLUDED_PY_RUNTIME0_H
#define MICROPY_INCLUDED_PY_RUNTIME0_H

// The first four must fit in 8 bits, see emitbc.c
// The remaining must fit in 16 bits, see scope.h
#define MP_SCOPE_FLAG_ALL_SIG      (0x0f)
#define MP_SCOPE_FLAG_GENERATOR    (0x01)
#define MP_SCOPE_FLAG_VARKEYWORDS  (0x02)
#define MP_SCOPE_FLAG_VARARGS      (0x04)
#define MP_SCOPE_FLAG_DEFKWARGS    (0x08)
#define MP_SCOPE_FLAG_REFGLOBALS   (0x10) // used only if native emitter enabled
#define MP_SCOPE_FLAG_HASCONSTS    (0x20) // used only if native emitter enabled
#define MP_SCOPE_FLAG_VIPERRET_POS    (6) // 3 bits used for viper return type, to pass from compiler to native emitter
#define MP_SCOPE_FLAG_VIPERRELOC   (0x10) // used only when loading viper from .mpy
#define MP_SCOPE_FLAG_VIPERRODATA  (0x20) // used only when loading viper from .mpy
#define MP_SCOPE_FLAG_VIPERBSS     (0x40) // used only when loading viper from .mpy

// types for native (viper) function signature
#define MP_NATIVE_TYPE_OBJ  (0x00)
#define MP_NATIVE_TYPE_BOOL (0x01)
#define MP_NATIVE_TYPE_INT  (0x02)
#define MP_NATIVE_TYPE_UINT (0x03)
#define MP_NATIVE_TYPE_PTR  (0x04)
#define MP_NATIVE_TYPE_PTR8 (0x05)
#define MP_NATIVE_TYPE_PTR16 (0x06)
#define MP_NATIVE_TYPE_PTR32 (0x07)

// Bytecode and runtime boundaries for unary ops
#define MP_UNARY_OP_NUM_BYTECODE    (MP_UNARY_OP_NOT + 1)
#define MP_UNARY_OP_NUM_RUNTIME     (MP_UNARY_OP_SIZEOF + 1)

// Bytecode and runtime boundaries for binary ops
#define MP_BINARY_OP_NUM_BYTECODE   (MP_BINARY_OP_POWER + 1)
#if MICROPY_PY_REVERSE_SPECIAL_METHODS
#define MP_BINARY_OP_NUM_RUNTIME    (MP_BINARY_OP_REVERSE_POWER + 1)
#else
#define MP_BINARY_OP_NUM_RUNTIME    (MP_BINARY_OP_CONTAINS + 1)
#endif

typedef enum {
    // These ops may appear in the bytecode. Changing this group
    // in any way requires changing the bytecode version.
    MP_UNARY_OP_POSITIVE,
    MP_UNARY_OP_NEGATIVE,
    MP_UNARY_OP_INVERT,
    MP_UNARY_OP_NOT,

    // Following ops cannot appear in the bytecode
    MP_UNARY_OP_BOOL, // __bool__
    MP_UNARY_OP_LEN, // __len__
    MP_UNARY_OP_HASH, // __hash__; must return a small int
    MP_UNARY_OP_ABS, // __abs__
    MP_UNARY_OP_INT, // __int__
    MP_UNARY_OP_SIZEOF, // for sys.getsizeof()
} mp_unary_op_t;

typedef enum {
    // The following 9+13+13 ops are used in bytecode and changing
    // them requires changing the bytecode version.

    // 9 relational operations, should return a bool; order of first 6 matches corresponding mp_token_kind_t
    MP_BINARY_OP_LESS,
    MP_BINARY_OP_MORE,
    MP_BINARY_OP_EQUAL,
    MP_BINARY_OP_LESS_EQUAL,
    MP_BINARY_OP_MORE_EQUAL,
    MP_BINARY_OP_NOT_EQUAL,
    MP_BINARY_OP_IN,
    MP_BINARY_OP_IS,
    MP_BINARY_OP_EXCEPTION_MATCH,

    // 13 inplace arithmetic operations; order matches corresponding mp_token_kind_t
    MP_BINARY_OP_INPLACE_OR,
    MP_BINARY_OP_INPLACE_XOR,
    MP_BINARY_OP_INPLACE_AND,
    MP_BINARY_OP_INPLACE_LSHIFT,
    MP_BINARY_OP_INPLACE_RSHIFT,
    MP_BINARY_OP_INPLACE_ADD,
    MP_BINARY_OP_INPLACE_SUBTRACT,
    MP_BINARY_OP_INPLACE_MULTIPLY,
    MP_BINARY_OP_INPLACE_MAT_MULTIPLY,
    MP_BINARY_OP_INPLACE_FLOOR_DIVIDE,
    MP_BINARY_OP_INPLACE_TRUE_DIVIDE,
    MP_BINARY_OP_INPLACE_MODULO,
    MP_BINARY_OP_INPLACE_POWER,

    // 13 normal arithmetic operations; order matches corresponding mp_token_kind_t
    MP_BINARY_OP_OR,
    MP_BINARY_OP_XOR,
    MP_BINARY_OP_AND,
    MP_BINARY_OP_LSHIFT,
    MP_BINARY_OP_RSHIFT,
    MP_BINARY_OP_ADD,
    MP_BINARY_OP_SUBTRACT,
    MP_BINARY_OP_MULTIPLY,
    MP_BINARY_OP_MAT_MULTIPLY,
    MP_BINARY_OP_FLOOR_DIVIDE,
    MP_BINARY_OP_TRUE_DIVIDE,
    MP_BINARY_OP_MODULO,
    MP_BINARY_OP_POWER,

    // Operations below this line don't appear in bytecode, they
    // just identify special methods.

    // This is not emitted by the compiler but is supported by the runtime.
    // It must follow immediately after MP_BINARY_OP_POWER.
    MP_BINARY_OP_DIVMOD,

    // The runtime will convert MP_BINARY_OP_IN to this operator with swapped args.
    // A type should implement this containment operator instead of MP_BINARY_OP_IN.
    MP_BINARY_OP_CONTAINS,

    // 13 MP_BINARY_OP_REVERSE_* operations must be in the same order as MP_BINARY_OP_*,
    // and be the last ones supported by the runtime.
    MP_BINARY_OP_REVERSE_OR,
    MP_BINARY_OP_REVERSE_XOR,
    MP_BINARY_OP_REVERSE_AND,
    MP_BINARY_OP_REVERSE_LSHIFT,
    MP_BINARY_OP_REVERSE_RSHIFT,
    MP_BINARY_OP_REVERSE_ADD,
    MP_BINARY_OP_REVERSE_SUBTRACT,
    MP_BINARY_OP_REVERSE_MULTIPLY,
    MP_BINARY_OP_REVERSE_MAT_MULTIPLY,
    MP_BINARY_OP_REVERSE_FLOOR_DIVIDE,
    MP_BINARY_OP_REVERSE_TRUE_DIVIDE,
    MP_BINARY_OP_REVERSE_MODULO,
    MP_BINARY_OP_REVERSE_POWER,

    // These 2 are not supported by the runtime and must be synthesised by the emitter
    MP_BINARY_OP_NOT_IN,
    MP_BINARY_OP_IS_NOT,
} mp_binary_op_t;

#endif // MICROPY_INCLUDED_PY_RUNTIME0_H
