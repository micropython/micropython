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

#include <stdio.h>
#include <assert.h>

#include "py/bc0.h"
#include "py/bc.h"

#if MICROPY_DEBUG_PRINTERS

#define DECODE_UINT { \
        unum = 0; \
        do { \
            unum = (unum << 7) + (*ip & 0x7f); \
        } while ((*ip++ & 0x80) != 0); \
}
#define DECODE_ULABEL do { unum = (ip[0] | (ip[1] << 8)); ip += 2; } while (0)
#define DECODE_SLABEL do { unum = (ip[0] | (ip[1] << 8)) - 0x8000; ip += 2; } while (0)

#if MICROPY_PERSISTENT_CODE

#define DECODE_QSTR \
    qst = ip[0] | ip[1] << 8; \
    ip += 2;
#define DECODE_PTR \
    DECODE_UINT; \
    unum = mp_showbc_const_table[unum]
#define DECODE_OBJ \
    DECODE_UINT; \
    unum = mp_showbc_const_table[unum]

#else

#define DECODE_QSTR { \
        qst = 0; \
        do { \
            qst = (qst << 7) + (*ip & 0x7f); \
        } while ((*ip++ & 0x80) != 0); \
}
#define DECODE_PTR do { \
        ip = (byte *)MP_ALIGN(ip, sizeof(void *)); \
        unum = (uintptr_t)*(void **)ip; \
        ip += sizeof(void *); \
} while (0)
#define DECODE_OBJ do { \
        ip = (byte *)MP_ALIGN(ip, sizeof(mp_obj_t)); \
        unum = (mp_uint_t)*(mp_obj_t *)ip; \
        ip += sizeof(mp_obj_t); \
} while (0)

#endif

const byte *mp_showbc_code_start;
const mp_uint_t *mp_showbc_const_table;

void mp_bytecode_print(const mp_print_t *print, const void *descr, const byte *ip, mp_uint_t len, const mp_uint_t *const_table) {
    mp_showbc_code_start = ip;

    // Decode prelude
    MP_BC_PRELUDE_SIG_DECODE(ip);
    MP_BC_PRELUDE_SIZE_DECODE(ip);
    const byte *code_info = ip;

    #if MICROPY_PERSISTENT_CODE
    qstr block_name = code_info[0] | (code_info[1] << 8);
    qstr source_file = code_info[2] | (code_info[3] << 8);
    code_info += 4;
    #else
    qstr block_name = mp_decode_uint(&code_info);
    qstr source_file = mp_decode_uint(&code_info);
    #endif
    mp_printf(print, "File %s, code block '%s' (descriptor: %p, bytecode @%p " UINT_FMT " bytes)\n",
        qstr_str(source_file), qstr_str(block_name), descr, mp_showbc_code_start, len);

    // raw bytecode dump
    size_t prelude_size = ip - mp_showbc_code_start + n_info + n_cell;
    mp_printf(print, "Raw bytecode (code_info_size=" UINT_FMT ", bytecode_size=" UINT_FMT "):\n",
        prelude_size, len - prelude_size);
    for (mp_uint_t i = 0; i < len; i++) {
        if (i > 0 && i % 16 == 0) {
            mp_printf(print, "\n");
        }
        mp_printf(print, " %02x", mp_showbc_code_start[i]);
    }
    mp_printf(print, "\n");

    // bytecode prelude: arg names (as qstr objects)
    mp_printf(print, "arg names:");
    for (mp_uint_t i = 0; i < n_pos_args + n_kwonly_args; i++) {
        mp_printf(print, " %s", qstr_str(MP_OBJ_QSTR_VALUE(const_table[i])));
    }
    mp_printf(print, "\n");

    mp_printf(print, "(N_STATE %u)\n", (unsigned)n_state);
    mp_printf(print, "(N_EXC_STACK %u)\n", (unsigned)n_exc_stack);

    // skip over code_info
    ip += n_info;

    // bytecode prelude: initialise closed over variables
    for (size_t i = 0; i < n_cell; ++i) {
        uint local_num = *ip++;
        mp_printf(print, "(INIT_CELL %u)\n", local_num);
    }

    // print out line number info
    {
        mp_int_t bc = 0;
        mp_uint_t source_line = 1;
        mp_printf(print, "  bc=" INT_FMT " line=" UINT_FMT "\n", bc, source_line);
        for (const byte *ci = code_info; *ci;) {
            if ((ci[0] & 0x80) == 0) {
                // 0b0LLBBBBB encoding
                bc += ci[0] & 0x1f;
                source_line += ci[0] >> 5;
                ci += 1;
            } else {
                // 0b1LLLBBBB 0bLLLLLLLL encoding (l's LSB in second byte)
                bc += ci[0] & 0xf;
                source_line += ((ci[0] << 4) & 0x700) | ci[1];
                ci += 2;
            }
            mp_printf(print, "  bc=" INT_FMT " line=" UINT_FMT "\n", bc, source_line);
        }
    }
    mp_bytecode_print2(print, ip, len - prelude_size, const_table);
}

const byte *mp_bytecode_print_str(const mp_print_t *print, const byte *ip) {
    mp_uint_t unum;
    qstr qst;

    switch (*ip++) {
        case MP_BC_LOAD_CONST_FALSE:
            mp_printf(print, "LOAD_CONST_FALSE");
            break;

        case MP_BC_LOAD_CONST_NONE:
            mp_printf(print, "LOAD_CONST_NONE");
            break;

        case MP_BC_LOAD_CONST_TRUE:
            mp_printf(print, "LOAD_CONST_TRUE");
            break;

        case MP_BC_LOAD_CONST_SMALL_INT: {
            mp_int_t num = 0;
            if ((ip[0] & 0x40) != 0) {
                // Number is negative
                num--;
            }
            do {
                num = ((mp_uint_t)num << 7) | (*ip & 0x7f);
            } while ((*ip++ & 0x80) != 0);
            mp_printf(print, "LOAD_CONST_SMALL_INT " INT_FMT, num);
            break;
        }

        case MP_BC_LOAD_CONST_STRING:
            DECODE_QSTR;
            mp_printf(print, "LOAD_CONST_STRING '%s'", qstr_str(qst));
            break;

        case MP_BC_LOAD_CONST_OBJ:
            DECODE_OBJ;
            mp_printf(print, "LOAD_CONST_OBJ %p=", MP_OBJ_TO_PTR(unum));
            mp_obj_print_helper(print, (mp_obj_t)unum, PRINT_REPR);
            break;

        case MP_BC_LOAD_NULL:
            mp_printf(print, "LOAD_NULL");
            break;

        case MP_BC_LOAD_FAST_N:
            DECODE_UINT;
            mp_printf(print, "LOAD_FAST_N " UINT_FMT, unum);
            break;

        case MP_BC_LOAD_DEREF:
            DECODE_UINT;
            mp_printf(print, "LOAD_DEREF " UINT_FMT, unum);
            break;

        case MP_BC_LOAD_NAME:
            DECODE_QSTR;
            mp_printf(print, "LOAD_NAME %s", qstr_str(qst));
            break;

        case MP_BC_LOAD_GLOBAL:
            DECODE_QSTR;
            mp_printf(print, "LOAD_GLOBAL %s", qstr_str(qst));
            break;

        case MP_BC_LOAD_ATTR:
            DECODE_QSTR;
            mp_printf(print, "LOAD_ATTR %s", qstr_str(qst));
            break;

        case MP_BC_LOAD_METHOD:
            DECODE_QSTR;
            mp_printf(print, "LOAD_METHOD %s", qstr_str(qst));
            break;

        case MP_BC_LOAD_SUPER_METHOD:
            DECODE_QSTR;
            mp_printf(print, "LOAD_SUPER_METHOD %s", qstr_str(qst));
            break;

        case MP_BC_LOAD_BUILD_CLASS:
            mp_printf(print, "LOAD_BUILD_CLASS");
            break;

        case MP_BC_LOAD_SUBSCR:
            mp_printf(print, "LOAD_SUBSCR");
            break;

        case MP_BC_STORE_FAST_N:
            DECODE_UINT;
            mp_printf(print, "STORE_FAST_N " UINT_FMT, unum);
            break;

        case MP_BC_STORE_DEREF:
            DECODE_UINT;
            mp_printf(print, "STORE_DEREF " UINT_FMT, unum);
            break;

        case MP_BC_STORE_NAME:
            DECODE_QSTR;
            mp_printf(print, "STORE_NAME %s", qstr_str(qst));
            break;

        case MP_BC_STORE_GLOBAL:
            DECODE_QSTR;
            mp_printf(print, "STORE_GLOBAL %s", qstr_str(qst));
            break;

        case MP_BC_STORE_ATTR:
            DECODE_QSTR;
            mp_printf(print, "STORE_ATTR %s", qstr_str(qst));
            break;

        case MP_BC_STORE_SUBSCR:
            mp_printf(print, "STORE_SUBSCR");
            break;

        case MP_BC_DELETE_FAST:
            DECODE_UINT;
            mp_printf(print, "DELETE_FAST " UINT_FMT, unum);
            break;

        case MP_BC_DELETE_DEREF:
            DECODE_UINT;
            mp_printf(print, "DELETE_DEREF " UINT_FMT, unum);
            break;

        case MP_BC_DELETE_NAME:
            DECODE_QSTR;
            mp_printf(print, "DELETE_NAME %s", qstr_str(qst));
            break;

        case MP_BC_DELETE_GLOBAL:
            DECODE_QSTR;
            mp_printf(print, "DELETE_GLOBAL %s", qstr_str(qst));
            break;

        case MP_BC_DUP_TOP:
            mp_printf(print, "DUP_TOP");
            break;

        case MP_BC_DUP_TOP_TWO:
            mp_printf(print, "DUP_TOP_TWO");
            break;

        case MP_BC_POP_TOP:
            mp_printf(print, "POP_TOP");
            break;

        case MP_BC_ROT_TWO:
            mp_printf(print, "ROT_TWO");
            break;

        case MP_BC_ROT_THREE:
            mp_printf(print, "ROT_THREE");
            break;

        case MP_BC_JUMP:
            DECODE_SLABEL;
            mp_printf(print, "JUMP " UINT_FMT, (mp_uint_t)(ip + unum - mp_showbc_code_start));
            break;

        case MP_BC_POP_JUMP_IF_TRUE:
            DECODE_SLABEL;
            mp_printf(print, "POP_JUMP_IF_TRUE " UINT_FMT, (mp_uint_t)(ip + unum - mp_showbc_code_start));
            break;

        case MP_BC_POP_JUMP_IF_FALSE:
            DECODE_SLABEL;
            mp_printf(print, "POP_JUMP_IF_FALSE " UINT_FMT, (mp_uint_t)(ip + unum - mp_showbc_code_start));
            break;

        case MP_BC_JUMP_IF_TRUE_OR_POP:
            DECODE_SLABEL;
            mp_printf(print, "JUMP_IF_TRUE_OR_POP " UINT_FMT, (mp_uint_t)(ip + unum - mp_showbc_code_start));
            break;

        case MP_BC_JUMP_IF_FALSE_OR_POP:
            DECODE_SLABEL;
            mp_printf(print, "JUMP_IF_FALSE_OR_POP " UINT_FMT, (mp_uint_t)(ip + unum - mp_showbc_code_start));
            break;

        case MP_BC_SETUP_WITH:
            DECODE_ULABEL; // loop-like labels are always forward
            mp_printf(print, "SETUP_WITH " UINT_FMT, (mp_uint_t)(ip + unum - mp_showbc_code_start));
            break;

        case MP_BC_WITH_CLEANUP:
            mp_printf(print, "WITH_CLEANUP");
            break;

        case MP_BC_UNWIND_JUMP:
            DECODE_SLABEL;
            mp_printf(print, "UNWIND_JUMP " UINT_FMT " %d", (mp_uint_t)(ip + unum - mp_showbc_code_start), *ip);
            ip += 1;
            break;

        case MP_BC_SETUP_EXCEPT:
            DECODE_ULABEL; // except labels are always forward
            mp_printf(print, "SETUP_EXCEPT " UINT_FMT, (mp_uint_t)(ip + unum - mp_showbc_code_start));
            break;

        case MP_BC_SETUP_FINALLY:
            DECODE_ULABEL; // except labels are always forward
            mp_printf(print, "SETUP_FINALLY " UINT_FMT, (mp_uint_t)(ip + unum - mp_showbc_code_start));
            break;

        case MP_BC_END_FINALLY:
            // if TOS is an exception, reraises the exception (3 values on TOS)
            // if TOS is an integer, does something else
            // if TOS is None, just pops it and continues
            // else error
            mp_printf(print, "END_FINALLY");
            break;

        case MP_BC_GET_ITER:
            mp_printf(print, "GET_ITER");
            break;

        case MP_BC_GET_ITER_STACK:
            mp_printf(print, "GET_ITER_STACK");
            break;

        case MP_BC_FOR_ITER:
            DECODE_ULABEL; // the jump offset if iteration finishes; for labels are always forward
            mp_printf(print, "FOR_ITER " UINT_FMT, (mp_uint_t)(ip + unum - mp_showbc_code_start));
            break;

        case MP_BC_POP_EXCEPT_JUMP:
            DECODE_ULABEL; // these labels are always forward
            mp_printf(print, "POP_EXCEPT_JUMP " UINT_FMT, (mp_uint_t)(ip + unum - mp_showbc_code_start));
            break;

        case MP_BC_BUILD_TUPLE:
            DECODE_UINT;
            mp_printf(print, "BUILD_TUPLE " UINT_FMT, unum);
            break;

        case MP_BC_BUILD_LIST:
            DECODE_UINT;
            mp_printf(print, "BUILD_LIST " UINT_FMT, unum);
            break;

        case MP_BC_BUILD_MAP:
            DECODE_UINT;
            mp_printf(print, "BUILD_MAP " UINT_FMT, unum);
            break;

        case MP_BC_STORE_MAP:
            mp_printf(print, "STORE_MAP");
            break;

        case MP_BC_BUILD_SET:
            DECODE_UINT;
            mp_printf(print, "BUILD_SET " UINT_FMT, unum);
            break;

        #if MICROPY_PY_BUILTINS_SLICE
        case MP_BC_BUILD_SLICE:
            DECODE_UINT;
            mp_printf(print, "BUILD_SLICE " UINT_FMT, unum);
            break;
        #endif

        case MP_BC_STORE_COMP:
            DECODE_UINT;
            mp_printf(print, "STORE_COMP " UINT_FMT, unum);
            break;

        case MP_BC_UNPACK_SEQUENCE:
            DECODE_UINT;
            mp_printf(print, "UNPACK_SEQUENCE " UINT_FMT, unum);
            break;

        case MP_BC_UNPACK_EX:
            DECODE_UINT;
            mp_printf(print, "UNPACK_EX " UINT_FMT, unum);
            break;

        case MP_BC_MAKE_FUNCTION:
            DECODE_PTR;
            mp_printf(print, "MAKE_FUNCTION %p", (void *)(uintptr_t)unum);
            break;

        case MP_BC_MAKE_FUNCTION_DEFARGS:
            DECODE_PTR;
            mp_printf(print, "MAKE_FUNCTION_DEFARGS %p", (void *)(uintptr_t)unum);
            break;

        case MP_BC_MAKE_CLOSURE: {
            DECODE_PTR;
            mp_uint_t n_closed_over = *ip++;
            mp_printf(print, "MAKE_CLOSURE %p " UINT_FMT, (void *)(uintptr_t)unum, n_closed_over);
            break;
        }

        case MP_BC_MAKE_CLOSURE_DEFARGS: {
            DECODE_PTR;
            mp_uint_t n_closed_over = *ip++;
            mp_printf(print, "MAKE_CLOSURE_DEFARGS %p " UINT_FMT, (void *)(uintptr_t)unum, n_closed_over);
            break;
        }

        case MP_BC_CALL_FUNCTION:
            DECODE_UINT;
            mp_printf(print, "CALL_FUNCTION n=" UINT_FMT " nkw=" UINT_FMT, unum & 0xff, (unum >> 8) & 0xff);
            break;

        case MP_BC_CALL_FUNCTION_VAR_KW:
            DECODE_UINT;
            mp_printf(print, "CALL_FUNCTION_VAR_KW n=" UINT_FMT " nkw=" UINT_FMT, unum & 0xff, (unum >> 8) & 0xff);
            break;

        case MP_BC_CALL_METHOD:
            DECODE_UINT;
            mp_printf(print, "CALL_METHOD n=" UINT_FMT " nkw=" UINT_FMT, unum & 0xff, (unum >> 8) & 0xff);
            break;

        case MP_BC_CALL_METHOD_VAR_KW:
            DECODE_UINT;
            mp_printf(print, "CALL_METHOD_VAR_KW n=" UINT_FMT " nkw=" UINT_FMT, unum & 0xff, (unum >> 8) & 0xff);
            break;

        case MP_BC_RETURN_VALUE:
            mp_printf(print, "RETURN_VALUE");
            break;

        case MP_BC_RAISE_LAST:
            mp_printf(print, "RAISE_LAST");
            break;

        case MP_BC_RAISE_OBJ:
            mp_printf(print, "RAISE_OBJ");
            break;

        case MP_BC_RAISE_FROM:
            mp_printf(print, "RAISE_FROM");
            break;

        case MP_BC_YIELD_VALUE:
            mp_printf(print, "YIELD_VALUE");
            break;

        case MP_BC_YIELD_FROM:
            mp_printf(print, "YIELD_FROM");
            break;

        case MP_BC_IMPORT_NAME:
            DECODE_QSTR;
            mp_printf(print, "IMPORT_NAME '%s'", qstr_str(qst));
            break;

        case MP_BC_IMPORT_FROM:
            DECODE_QSTR;
            mp_printf(print, "IMPORT_FROM '%s'", qstr_str(qst));
            break;

        case MP_BC_IMPORT_STAR:
            mp_printf(print, "IMPORT_STAR");
            break;

        default:
            if (ip[-1] < MP_BC_LOAD_CONST_SMALL_INT_MULTI + 64) {
                mp_printf(print, "LOAD_CONST_SMALL_INT " INT_FMT, (mp_int_t)ip[-1] - MP_BC_LOAD_CONST_SMALL_INT_MULTI - 16);
            } else if (ip[-1] < MP_BC_LOAD_FAST_MULTI + 16) {
                mp_printf(print, "LOAD_FAST " UINT_FMT, (mp_uint_t)ip[-1] - MP_BC_LOAD_FAST_MULTI);
            } else if (ip[-1] < MP_BC_STORE_FAST_MULTI + 16) {
                mp_printf(print, "STORE_FAST " UINT_FMT, (mp_uint_t)ip[-1] - MP_BC_STORE_FAST_MULTI);
            } else if (ip[-1] < MP_BC_UNARY_OP_MULTI + MP_UNARY_OP_NUM_BYTECODE) {
                mp_uint_t op = ip[-1] - MP_BC_UNARY_OP_MULTI;
                mp_printf(print, "UNARY_OP " UINT_FMT " %s", op, qstr_str(mp_unary_op_method_name[op]));
            } else if (ip[-1] < MP_BC_BINARY_OP_MULTI + MP_BINARY_OP_NUM_BYTECODE) {
                mp_uint_t op = ip[-1] - MP_BC_BINARY_OP_MULTI;
                mp_printf(print, "BINARY_OP " UINT_FMT " %s", op, qstr_str(mp_binary_op_method_name[op]));
            } else {
                mp_printf(print, "code %p, byte code 0x%02x not implemented\n", ip - 1, ip[-1]);
                assert(0);
                return ip;
            }
            break;
    }

    return ip;
}

void mp_bytecode_print2(const mp_print_t *print, const byte *ip, size_t len, const mp_uint_t *const_table) {
    mp_showbc_code_start = ip;
    mp_showbc_const_table = const_table;
    while (ip < len + mp_showbc_code_start) {
        mp_printf(print, "%02u ", (uint)(ip - mp_showbc_code_start));
        ip = mp_bytecode_print_str(print, ip);
        mp_printf(print, "\n");
    }
}

#endif // MICROPY_DEBUG_PRINTERS
