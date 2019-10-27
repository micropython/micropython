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

// MicroPython bytecode opcodes, grouped based on the format of the opcode

#define MP_BC_MASK_FORMAT                   (0xf0)
#define MP_BC_MASK_EXTRA_BYTE               (0x9e)

#define MP_BC_FORMAT_BYTE                   (0)
#define MP_BC_FORMAT_QSTR                   (1)
#define MP_BC_FORMAT_VAR_UINT               (2)
#define MP_BC_FORMAT_OFFSET                 (3)

// Nibbles in magic number are: BB BB BB BB BB BO VV QU
#define MP_BC_FORMAT(op) ((0x000003a4 >> (2 * ((op) >> 4))) & 3)

// Load, Store, Delete, Import, Make, Build, Unpack, Call, Jump, Exception, For, sTack, Return, Yield, Op
#define MP_BC_BASE_RESERVED                 (0x00) // ----------------
#define MP_BC_BASE_QSTR_O                   (0x10) // LLLLLLSSSDDII---
#define MP_BC_BASE_VINT_E                   (0x20) // MMLLLLSSDDBBBBBB
#define MP_BC_BASE_VINT_O                   (0x30) // UUMMCCCC--------
#define MP_BC_BASE_JUMP_E                   (0x40) // J-JJJJJEEEEF----
#define MP_BC_BASE_BYTE_O                   (0x50) // LLLLSSDTTTTTEEFF
#define MP_BC_BASE_BYTE_E                   (0x60) // --BREEEYYI------
#define MP_BC_LOAD_CONST_SMALL_INT_MULTI    (0x70) // LLLLLLLLLLLLLLLL
                                         // (0x80) // LLLLLLLLLLLLLLLL
                                         // (0x90) // LLLLLLLLLLLLLLLL
                                         // (0xa0) // LLLLLLLLLLLLLLLL
#define MP_BC_LOAD_FAST_MULTI               (0xb0) // LLLLLLLLLLLLLLLL
#define MP_BC_STORE_FAST_MULTI              (0xc0) // SSSSSSSSSSSSSSSS
#define MP_BC_UNARY_OP_MULTI                (0xd0) // OOOOOOO
#define MP_BC_BINARY_OP_MULTI               (0xd7) //        OOOOOOOOO
                                         // (0xe0) // OOOOOOOOOOOOOOOO
                                         // (0xf0) // OOOOOOOOOO------

#define MP_BC_LOAD_CONST_SMALL_INT_MULTI_NUM (64)
#define MP_BC_LOAD_CONST_SMALL_INT_MULTI_EXCESS (16)
#define MP_BC_LOAD_FAST_MULTI_NUM           (16)
#define MP_BC_STORE_FAST_MULTI_NUM          (16)
#define MP_BC_UNARY_OP_MULTI_NUM            (MP_UNARY_OP_NUM_BYTECODE)
#define MP_BC_BINARY_OP_MULTI_NUM           (MP_BINARY_OP_NUM_BYTECODE)

#define MP_BC_LOAD_CONST_FALSE              (MP_BC_BASE_BYTE_O + 0x00)
#define MP_BC_LOAD_CONST_NONE               (MP_BC_BASE_BYTE_O + 0x01)
#define MP_BC_LOAD_CONST_TRUE               (MP_BC_BASE_BYTE_O + 0x02)
#define MP_BC_LOAD_CONST_SMALL_INT          (MP_BC_BASE_VINT_E + 0x02) // signed var-int
#define MP_BC_LOAD_CONST_STRING             (MP_BC_BASE_QSTR_O + 0x00) // qstr
#define MP_BC_LOAD_CONST_OBJ                (MP_BC_BASE_VINT_E + 0x03) // ptr
#define MP_BC_LOAD_NULL                     (MP_BC_BASE_BYTE_O + 0x03)

#define MP_BC_LOAD_FAST_N                   (MP_BC_BASE_VINT_E + 0x04) // uint
#define MP_BC_LOAD_DEREF                    (MP_BC_BASE_VINT_E + 0x05) // uint
#define MP_BC_LOAD_NAME                     (MP_BC_BASE_QSTR_O + 0x01) // qstr
#define MP_BC_LOAD_GLOBAL                   (MP_BC_BASE_QSTR_O + 0x02) // qstr
#define MP_BC_LOAD_ATTR                     (MP_BC_BASE_QSTR_O + 0x03) // qstr
#define MP_BC_LOAD_METHOD                   (MP_BC_BASE_QSTR_O + 0x04) // qstr
#define MP_BC_LOAD_SUPER_METHOD             (MP_BC_BASE_QSTR_O + 0x05) // qstr
#define MP_BC_LOAD_BUILD_CLASS              (MP_BC_BASE_BYTE_O + 0x04)
#define MP_BC_LOAD_SUBSCR                   (MP_BC_BASE_BYTE_O + 0x05)

#define MP_BC_STORE_FAST_N                  (MP_BC_BASE_VINT_E + 0x06) // uint
#define MP_BC_STORE_DEREF                   (MP_BC_BASE_VINT_E + 0x07) // uint
#define MP_BC_STORE_NAME                    (MP_BC_BASE_QSTR_O + 0x06) // qstr
#define MP_BC_STORE_GLOBAL                  (MP_BC_BASE_QSTR_O + 0x07) // qstr
#define MP_BC_STORE_ATTR                    (MP_BC_BASE_QSTR_O + 0x08) // qstr
#define MP_BC_STORE_SUBSCR                  (MP_BC_BASE_BYTE_O + 0x06)

#define MP_BC_DELETE_FAST                   (MP_BC_BASE_VINT_E + 0x08) // uint
#define MP_BC_DELETE_DEREF                  (MP_BC_BASE_VINT_E + 0x09) // uint
#define MP_BC_DELETE_NAME                   (MP_BC_BASE_QSTR_O + 0x09) // qstr
#define MP_BC_DELETE_GLOBAL                 (MP_BC_BASE_QSTR_O + 0x0a) // qstr

#define MP_BC_DUP_TOP                       (MP_BC_BASE_BYTE_O + 0x07)
#define MP_BC_DUP_TOP_TWO                   (MP_BC_BASE_BYTE_O + 0x08)
#define MP_BC_POP_TOP                       (MP_BC_BASE_BYTE_O + 0x09)
#define MP_BC_ROT_TWO                       (MP_BC_BASE_BYTE_O + 0x0a)
#define MP_BC_ROT_THREE                     (MP_BC_BASE_BYTE_O + 0x0b)

#define MP_BC_JUMP                          (MP_BC_BASE_JUMP_E + 0x02) // rel byte code offset, 16-bit signed, in excess
#define MP_BC_POP_JUMP_IF_TRUE              (MP_BC_BASE_JUMP_E + 0x03) // rel byte code offset, 16-bit signed, in excess
#define MP_BC_POP_JUMP_IF_FALSE             (MP_BC_BASE_JUMP_E + 0x04) // rel byte code offset, 16-bit signed, in excess
#define MP_BC_JUMP_IF_TRUE_OR_POP           (MP_BC_BASE_JUMP_E + 0x05) // rel byte code offset, 16-bit signed, in excess
#define MP_BC_JUMP_IF_FALSE_OR_POP          (MP_BC_BASE_JUMP_E + 0x06) // rel byte code offset, 16-bit signed, in excess
#define MP_BC_UNWIND_JUMP                   (MP_BC_BASE_JUMP_E + 0x00) // rel byte code offset, 16-bit signed, in excess; then a byte
#define MP_BC_SETUP_WITH                    (MP_BC_BASE_JUMP_E + 0x07) // rel byte code offset, 16-bit unsigned
#define MP_BC_SETUP_EXCEPT                  (MP_BC_BASE_JUMP_E + 0x08) // rel byte code offset, 16-bit unsigned
#define MP_BC_SETUP_FINALLY                 (MP_BC_BASE_JUMP_E + 0x09) // rel byte code offset, 16-bit unsigned
#define MP_BC_POP_EXCEPT_JUMP               (MP_BC_BASE_JUMP_E + 0x0a) // rel byte code offset, 16-bit unsigned
#define MP_BC_FOR_ITER                      (MP_BC_BASE_JUMP_E + 0x0b) // rel byte code offset, 16-bit unsigned
#define MP_BC_WITH_CLEANUP                  (MP_BC_BASE_BYTE_O + 0x0c)
#define MP_BC_END_FINALLY                   (MP_BC_BASE_BYTE_O + 0x0d)
#define MP_BC_GET_ITER                      (MP_BC_BASE_BYTE_O + 0x0e)
#define MP_BC_GET_ITER_STACK                (MP_BC_BASE_BYTE_O + 0x0f)

#define MP_BC_BUILD_TUPLE                   (MP_BC_BASE_VINT_E + 0x0a) // uint
#define MP_BC_BUILD_LIST                    (MP_BC_BASE_VINT_E + 0x0b) // uint
#define MP_BC_BUILD_MAP                     (MP_BC_BASE_VINT_E + 0x0c) // uint
#define MP_BC_STORE_MAP                     (MP_BC_BASE_BYTE_E + 0x02)
#define MP_BC_BUILD_SET                     (MP_BC_BASE_VINT_E + 0x0d) // uint
#define MP_BC_BUILD_SLICE                   (MP_BC_BASE_VINT_E + 0x0e) // uint
#define MP_BC_STORE_COMP                    (MP_BC_BASE_VINT_E + 0x0f) // uint
#define MP_BC_UNPACK_SEQUENCE               (MP_BC_BASE_VINT_O + 0x00) // uint
#define MP_BC_UNPACK_EX                     (MP_BC_BASE_VINT_O + 0x01) // uint

#define MP_BC_RETURN_VALUE                  (MP_BC_BASE_BYTE_E + 0x03)
#define MP_BC_RAISE_LAST                    (MP_BC_BASE_BYTE_E + 0x04)
#define MP_BC_RAISE_OBJ                     (MP_BC_BASE_BYTE_E + 0x05)
#define MP_BC_RAISE_FROM                    (MP_BC_BASE_BYTE_E + 0x06)
#define MP_BC_YIELD_VALUE                   (MP_BC_BASE_BYTE_E + 0x07)
#define MP_BC_YIELD_FROM                    (MP_BC_BASE_BYTE_E + 0x08)

#define MP_BC_MAKE_FUNCTION                 (MP_BC_BASE_VINT_O + 0x02) // uint
#define MP_BC_MAKE_FUNCTION_DEFARGS         (MP_BC_BASE_VINT_O + 0x03) // uint
#define MP_BC_MAKE_CLOSURE                  (MP_BC_BASE_VINT_E + 0x00) // uint; extra byte
#define MP_BC_MAKE_CLOSURE_DEFARGS          (MP_BC_BASE_VINT_E + 0x01) // uint; extra byte
#define MP_BC_CALL_FUNCTION                 (MP_BC_BASE_VINT_O + 0x04) // uint
#define MP_BC_CALL_FUNCTION_VAR_KW          (MP_BC_BASE_VINT_O + 0x05) // uint
#define MP_BC_CALL_METHOD                   (MP_BC_BASE_VINT_O + 0x06) // uint
#define MP_BC_CALL_METHOD_VAR_KW            (MP_BC_BASE_VINT_O + 0x07) // uint

#define MP_BC_IMPORT_NAME                   (MP_BC_BASE_QSTR_O + 0x0b) // qstr
#define MP_BC_IMPORT_FROM                   (MP_BC_BASE_QSTR_O + 0x0c) // qstr
#define MP_BC_IMPORT_STAR                   (MP_BC_BASE_BYTE_E + 0x09)

#endif // MICROPY_INCLUDED_PY_BC0_H
