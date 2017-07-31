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
#ifndef MICROPY_INCLUDED_PY_BC0_H
#define MICROPY_INCLUDED_PY_BC0_H

// MicroPython byte-codes.
// The comment at the end of the line (if it exists) tells the arguments to the byte-code.

#define MP_BC_LOAD_CONST_FALSE   (0x10)
#define MP_BC_LOAD_CONST_NONE    (0x11)
#define MP_BC_LOAD_CONST_TRUE    (0x12)
#define MP_BC_LOAD_CONST_SMALL_INT   (0x14) // signed var-int
#define MP_BC_LOAD_CONST_STRING  (0x16) // qstr
#define MP_BC_LOAD_CONST_OBJ     (0x17) // ptr
#define MP_BC_LOAD_NULL          (0x18)

#define MP_BC_LOAD_FAST_N        (0x19) // uint
#define MP_BC_LOAD_DEREF         (0x1a) // uint
#define MP_BC_LOAD_NAME          (0x1b) // qstr
#define MP_BC_LOAD_GLOBAL        (0x1c) // qstr
#define MP_BC_LOAD_ATTR          (0x1d) // qstr
#define MP_BC_LOAD_METHOD        (0x1e) // qstr
#define MP_BC_LOAD_SUPER_METHOD  (0x1f) // qstr
#define MP_BC_LOAD_BUILD_CLASS   (0x20)
#define MP_BC_LOAD_SUBSCR        (0x21)

#define MP_BC_STORE_FAST_N       (0x22) // uint
#define MP_BC_STORE_DEREF        (0x23) // uint
#define MP_BC_STORE_NAME         (0x24) // qstr
#define MP_BC_STORE_GLOBAL       (0x25) // qstr
#define MP_BC_STORE_ATTR         (0x26) // qstr
#define MP_BC_STORE_SUBSCR       (0x27)

#define MP_BC_DELETE_FAST        (0x28) // uint
#define MP_BC_DELETE_DEREF       (0x29) // uint
#define MP_BC_DELETE_NAME        (0x2a) // qstr
#define MP_BC_DELETE_GLOBAL      (0x2b) // qstr

#define MP_BC_DUP_TOP            (0x30)
#define MP_BC_DUP_TOP_TWO        (0x31)
#define MP_BC_POP_TOP            (0x32)
#define MP_BC_ROT_TWO            (0x33)
#define MP_BC_ROT_THREE          (0x34)

#define MP_BC_JUMP               (0x35) // rel byte code offset, 16-bit signed, in excess
#define MP_BC_POP_JUMP_IF_TRUE   (0x36) // rel byte code offset, 16-bit signed, in excess
#define MP_BC_POP_JUMP_IF_FALSE  (0x37) // rel byte code offset, 16-bit signed, in excess
#define MP_BC_JUMP_IF_TRUE_OR_POP    (0x38) // rel byte code offset, 16-bit signed, in excess
#define MP_BC_JUMP_IF_FALSE_OR_POP   (0x39) // rel byte code offset, 16-bit signed, in excess
#define MP_BC_SETUP_WITH         (0x3d) // rel byte code offset, 16-bit unsigned
#define MP_BC_WITH_CLEANUP       (0x3e)
#define MP_BC_SETUP_EXCEPT       (0x3f) // rel byte code offset, 16-bit unsigned
#define MP_BC_SETUP_FINALLY      (0x40) // rel byte code offset, 16-bit unsigned
#define MP_BC_END_FINALLY        (0x41)
#define MP_BC_GET_ITER           (0x42)
#define MP_BC_FOR_ITER           (0x43) // rel byte code offset, 16-bit unsigned
#define MP_BC_POP_BLOCK          (0x44)
#define MP_BC_POP_EXCEPT         (0x45)
#define MP_BC_UNWIND_JUMP        (0x46) // rel byte code offset, 16-bit signed, in excess; then a byte
#define MP_BC_GET_ITER_STACK     (0x47)

#define MP_BC_BUILD_TUPLE        (0x50) // uint
#define MP_BC_BUILD_LIST         (0x51) // uint
#define MP_BC_BUILD_MAP          (0x53) // uint
#define MP_BC_STORE_MAP          (0x54)
#define MP_BC_BUILD_SET          (0x56) // uint
#define MP_BC_BUILD_SLICE        (0x58) // uint
#define MP_BC_STORE_COMP         (0x57) // uint
#define MP_BC_UNPACK_SEQUENCE    (0x59) // uint
#define MP_BC_UNPACK_EX          (0x5a) // uint

#define MP_BC_RETURN_VALUE       (0x5b)
#define MP_BC_RAISE_VARARGS      (0x5c) // byte
#define MP_BC_YIELD_VALUE        (0x5d)
#define MP_BC_YIELD_FROM         (0x5e)

#define MP_BC_MAKE_FUNCTION         (0x60) // uint
#define MP_BC_MAKE_FUNCTION_DEFARGS (0x61) // uint
#define MP_BC_MAKE_CLOSURE          (0x62) // uint
#define MP_BC_MAKE_CLOSURE_DEFARGS  (0x63) // uint
#define MP_BC_CALL_FUNCTION         (0x64) // uint
#define MP_BC_CALL_FUNCTION_VAR_KW  (0x65) // uint
#define MP_BC_CALL_METHOD           (0x66) // uint
#define MP_BC_CALL_METHOD_VAR_KW    (0x67) // uint

#define MP_BC_IMPORT_NAME        (0x68) // qstr
#define MP_BC_IMPORT_FROM        (0x69) // qstr
#define MP_BC_IMPORT_STAR        (0x6a)

#define MP_BC_LOAD_CONST_SMALL_INT_MULTI (0x70) // + N(64)
#define MP_BC_LOAD_FAST_MULTI            (0xb0) // + N(16)
#define MP_BC_STORE_FAST_MULTI           (0xc0) // + N(16)
#define MP_BC_UNARY_OP_MULTI             (0xd0) // + op(7)
#define MP_BC_BINARY_OP_MULTI            (0xd7) // + op(36)

#endif // MICROPY_INCLUDED_PY_BC0_H
