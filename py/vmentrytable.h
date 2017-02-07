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

#if __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winitializer-overrides"
#endif // __clang__

static const void *const entry_table[256] = {
    [0 ... 255] = &&entry_default,
    [MP_BC_LOAD_CONST_FALSE] = &&entry_MP_BC_LOAD_CONST_FALSE,
    [MP_BC_LOAD_CONST_NONE] = &&entry_MP_BC_LOAD_CONST_NONE,
    [MP_BC_LOAD_CONST_TRUE] = &&entry_MP_BC_LOAD_CONST_TRUE,
    [MP_BC_LOAD_CONST_SMALL_INT] = &&entry_MP_BC_LOAD_CONST_SMALL_INT,
    [MP_BC_LOAD_CONST_STRING] = &&entry_MP_BC_LOAD_CONST_STRING,
    [MP_BC_LOAD_CONST_OBJ] = &&entry_MP_BC_LOAD_CONST_OBJ,
    [MP_BC_LOAD_NULL] = &&entry_MP_BC_LOAD_NULL,
    [MP_BC_LOAD_FAST_N] = &&entry_MP_BC_LOAD_FAST_N,
    [MP_BC_LOAD_DEREF] = &&entry_MP_BC_LOAD_DEREF,
    [MP_BC_LOAD_NAME] = &&entry_MP_BC_LOAD_NAME,
    [MP_BC_LOAD_GLOBAL] = &&entry_MP_BC_LOAD_GLOBAL,
    [MP_BC_LOAD_ATTR] = &&entry_MP_BC_LOAD_ATTR,
    [MP_BC_LOAD_METHOD] = &&entry_MP_BC_LOAD_METHOD,
    [MP_BC_LOAD_BUILD_CLASS] = &&entry_MP_BC_LOAD_BUILD_CLASS,
    [MP_BC_LOAD_SUBSCR] = &&entry_MP_BC_LOAD_SUBSCR,
    [MP_BC_STORE_FAST_N] = &&entry_MP_BC_STORE_FAST_N,
    [MP_BC_STORE_DEREF] = &&entry_MP_BC_STORE_DEREF,
    [MP_BC_STORE_NAME] = &&entry_MP_BC_STORE_NAME,
    [MP_BC_STORE_GLOBAL] = &&entry_MP_BC_STORE_GLOBAL,
    [MP_BC_STORE_ATTR] = &&entry_MP_BC_STORE_ATTR,
    [MP_BC_STORE_SUBSCR] = &&entry_MP_BC_STORE_SUBSCR,
    [MP_BC_DELETE_FAST] = &&entry_MP_BC_DELETE_FAST,
    [MP_BC_DELETE_DEREF] = &&entry_MP_BC_DELETE_DEREF,
    [MP_BC_DELETE_NAME] = &&entry_MP_BC_DELETE_NAME,
    [MP_BC_DELETE_GLOBAL] = &&entry_MP_BC_DELETE_GLOBAL,
    [MP_BC_DUP_TOP] = &&entry_MP_BC_DUP_TOP,
    [MP_BC_DUP_TOP_TWO] = &&entry_MP_BC_DUP_TOP_TWO,
    [MP_BC_POP_TOP] = &&entry_MP_BC_POP_TOP,
    [MP_BC_ROT_TWO] = &&entry_MP_BC_ROT_TWO,
    [MP_BC_ROT_THREE] = &&entry_MP_BC_ROT_THREE,
    [MP_BC_JUMP] = &&entry_MP_BC_JUMP,
    [MP_BC_POP_JUMP_IF_TRUE] = &&entry_MP_BC_POP_JUMP_IF_TRUE,
    [MP_BC_POP_JUMP_IF_FALSE] = &&entry_MP_BC_POP_JUMP_IF_FALSE,
    [MP_BC_JUMP_IF_TRUE_OR_POP] = &&entry_MP_BC_JUMP_IF_TRUE_OR_POP,
    [MP_BC_JUMP_IF_FALSE_OR_POP] = &&entry_MP_BC_JUMP_IF_FALSE_OR_POP,
    [MP_BC_SETUP_WITH] = &&entry_MP_BC_SETUP_WITH,
    [MP_BC_WITH_CLEANUP] = &&entry_MP_BC_WITH_CLEANUP,
    [MP_BC_UNWIND_JUMP] = &&entry_MP_BC_UNWIND_JUMP,
    [MP_BC_SETUP_EXCEPT] = &&entry_MP_BC_SETUP_EXCEPT,
    [MP_BC_SETUP_FINALLY] = &&entry_MP_BC_SETUP_FINALLY,
    [MP_BC_END_FINALLY] = &&entry_MP_BC_END_FINALLY,
    [MP_BC_GET_ITER] = &&entry_MP_BC_GET_ITER,
    [MP_BC_FOR_ITER] = &&entry_MP_BC_FOR_ITER,
    [MP_BC_POP_BLOCK] = &&entry_MP_BC_POP_BLOCK,
    [MP_BC_POP_EXCEPT] = &&entry_MP_BC_POP_EXCEPT,
    [MP_BC_BUILD_TUPLE] = &&entry_MP_BC_BUILD_TUPLE,
    [MP_BC_BUILD_LIST] = &&entry_MP_BC_BUILD_LIST,
    [MP_BC_BUILD_MAP] = &&entry_MP_BC_BUILD_MAP,
    [MP_BC_STORE_MAP] = &&entry_MP_BC_STORE_MAP,
    #if MICROPY_PY_BUILTINS_SET
    [MP_BC_BUILD_SET] = &&entry_MP_BC_BUILD_SET,
    #endif
    #if MICROPY_PY_BUILTINS_SLICE
    [MP_BC_BUILD_SLICE] = &&entry_MP_BC_BUILD_SLICE,
    #endif
    [MP_BC_STORE_COMP] = &&entry_MP_BC_STORE_COMP,
    [MP_BC_UNPACK_SEQUENCE] = &&entry_MP_BC_UNPACK_SEQUENCE,
    [MP_BC_UNPACK_EX] = &&entry_MP_BC_UNPACK_EX,
    [MP_BC_MAKE_FUNCTION] = &&entry_MP_BC_MAKE_FUNCTION,
    [MP_BC_MAKE_FUNCTION_DEFARGS] = &&entry_MP_BC_MAKE_FUNCTION_DEFARGS,
    [MP_BC_MAKE_CLOSURE] = &&entry_MP_BC_MAKE_CLOSURE,
    [MP_BC_MAKE_CLOSURE_DEFARGS] = &&entry_MP_BC_MAKE_CLOSURE_DEFARGS,
    [MP_BC_CALL_FUNCTION] = &&entry_MP_BC_CALL_FUNCTION,
    [MP_BC_CALL_FUNCTION_VAR_KW] = &&entry_MP_BC_CALL_FUNCTION_VAR_KW,
    [MP_BC_CALL_METHOD] = &&entry_MP_BC_CALL_METHOD,
    [MP_BC_CALL_METHOD_VAR_KW] = &&entry_MP_BC_CALL_METHOD_VAR_KW,
    [MP_BC_RETURN_VALUE] = &&entry_MP_BC_RETURN_VALUE,
    [MP_BC_RAISE_VARARGS] = &&entry_MP_BC_RAISE_VARARGS,
    [MP_BC_YIELD_VALUE] = &&entry_MP_BC_YIELD_VALUE,
    [MP_BC_YIELD_FROM] = &&entry_MP_BC_YIELD_FROM,
    [MP_BC_IMPORT_NAME] = &&entry_MP_BC_IMPORT_NAME,
    [MP_BC_IMPORT_FROM] = &&entry_MP_BC_IMPORT_FROM,
    [MP_BC_IMPORT_STAR] = &&entry_MP_BC_IMPORT_STAR,
    [MP_BC_LOAD_CONST_SMALL_INT_MULTI ... MP_BC_LOAD_CONST_SMALL_INT_MULTI + 63] = &&entry_MP_BC_LOAD_CONST_SMALL_INT_MULTI,
    [MP_BC_LOAD_FAST_MULTI ... MP_BC_LOAD_FAST_MULTI + 15] = &&entry_MP_BC_LOAD_FAST_MULTI,
    [MP_BC_STORE_FAST_MULTI ... MP_BC_STORE_FAST_MULTI + 15] = &&entry_MP_BC_STORE_FAST_MULTI,
    [MP_BC_UNARY_OP_MULTI ... MP_BC_UNARY_OP_MULTI + 6] = &&entry_MP_BC_UNARY_OP_MULTI,
    [MP_BC_BINARY_OP_MULTI ... MP_BC_BINARY_OP_MULTI + 35] = &&entry_MP_BC_BINARY_OP_MULTI,
};

#if __clang__
#pragma clang diagnostic pop
#endif // __clang__
