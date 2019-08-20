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

// redirect all printfs in this file to the platform print stream
#define printf(...) mp_printf(&mp_plat_print, __VA_ARGS__)

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
    ip = (byte*)MP_ALIGN(ip, sizeof(void*)); \
    unum = (uintptr_t)*(void**)ip; \
    ip += sizeof(void*); \
} while (0)
#define DECODE_OBJ do { \
    ip = (byte*)MP_ALIGN(ip, sizeof(mp_obj_t)); \
    unum = (mp_uint_t)*(mp_obj_t*)ip; \
    ip += sizeof(mp_obj_t); \
} while (0)

#endif

const byte *mp_showbc_code_start;
const mp_uint_t *mp_showbc_const_table;

void mp_bytecode_print(const void *descr, const byte *ip, mp_uint_t len, const mp_uint_t *const_table) {
    mp_showbc_code_start = ip;

    // get bytecode parameters
    mp_uint_t n_state = mp_decode_uint(&ip);
    mp_uint_t n_exc_stack = mp_decode_uint(&ip);
    /*mp_uint_t scope_flags =*/ ip++;
    mp_uint_t n_pos_args = *ip++;
    mp_uint_t n_kwonly_args = *ip++;
    /*mp_uint_t n_def_pos_args =*/ ip++;

    const byte *code_info = ip;
    mp_uint_t code_info_size = mp_decode_uint(&code_info);
    ip += code_info_size;

    #if MICROPY_PERSISTENT_CODE
    qstr block_name = code_info[0] | (code_info[1] << 8);
    qstr source_file = code_info[2] | (code_info[3] << 8);
    code_info += 4;
    #else
    qstr block_name = mp_decode_uint(&code_info);
    qstr source_file = mp_decode_uint(&code_info);
    #endif
    printf("File %s, code block '%s' (descriptor: %p, bytecode @%p " UINT_FMT " bytes)\n",
        qstr_str(source_file), qstr_str(block_name), descr, mp_showbc_code_start, len);

    // raw bytecode dump
    printf("Raw bytecode (code_info_size=" UINT_FMT ", bytecode_size=" UINT_FMT "):\n", code_info_size, len - code_info_size);
    for (mp_uint_t i = 0; i < len; i++) {
        if (i > 0 && i % 16 == 0) {
            printf("\n");
        }
        printf(" %02x", mp_showbc_code_start[i]);
    }
    printf("\n");

    // bytecode prelude: arg names (as qstr objects)
    printf("arg names:");
    for (mp_uint_t i = 0; i < n_pos_args + n_kwonly_args; i++) {
        printf(" %s", qstr_str(MP_OBJ_QSTR_VALUE(const_table[i])));
    }
    printf("\n");

    printf("(N_STATE " UINT_FMT ")\n", n_state);
    printf("(N_EXC_STACK " UINT_FMT ")\n", n_exc_stack);

    // for printing line number info
    const byte *bytecode_start = ip;

    // bytecode prelude: initialise closed over variables
    {
        uint local_num;
        while ((local_num = *ip++) != 255) {
            printf("(INIT_CELL %u)\n", local_num);
        }
        len -= ip - mp_showbc_code_start;
    }

    // print out line number info
    {
        mp_int_t bc = bytecode_start - ip;
        mp_uint_t source_line = 1;
        printf("  bc=" INT_FMT " line=" UINT_FMT "\n", bc, source_line);
        for (const byte* ci = code_info; *ci;) {
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
            printf("  bc=" INT_FMT " line=" UINT_FMT "\n", bc, source_line);
        }
    }
    mp_bytecode_print2(ip, len - 0, const_table);
}

const byte *mp_bytecode_print_str(const byte *ip) {
    mp_uint_t unum;
    qstr qst;

    switch (*ip++) {
        case MP_BC_LOAD_CONST_FALSE:
            printf("LOAD_CONST_FALSE");
            break;

        case MP_BC_LOAD_CONST_NONE:
            printf("LOAD_CONST_NONE");
            break;

        case MP_BC_LOAD_CONST_TRUE:
            printf("LOAD_CONST_TRUE");
            break;

        case MP_BC_LOAD_CONST_SMALL_INT: {
            mp_int_t num = 0;
            if ((ip[0] & 0x40) != 0) {
                // Number is negative
                num--;
            }
            do {
                num = (num << 7) | (*ip & 0x7f);
            } while ((*ip++ & 0x80) != 0);
            printf("LOAD_CONST_SMALL_INT " INT_FMT, num);
            break;
        }

        case MP_BC_LOAD_CONST_STRING:
            DECODE_QSTR;
            printf("LOAD_CONST_STRING '%s'", qstr_str(qst));
            break;

        case MP_BC_LOAD_CONST_OBJ:
            DECODE_OBJ;
            printf("LOAD_CONST_OBJ %p=", MP_OBJ_TO_PTR(unum));
            mp_obj_print_helper(&mp_plat_print, (mp_obj_t)unum, PRINT_REPR);
            break;

        case MP_BC_LOAD_NULL:
            printf("LOAD_NULL");
            break;

        case MP_BC_LOAD_FAST_N:
            DECODE_UINT;
            printf("LOAD_FAST_N " UINT_FMT, unum);
            break;

        case MP_BC_LOAD_DEREF:
            DECODE_UINT;
            printf("LOAD_DEREF " UINT_FMT, unum);
            break;

        case MP_BC_LOAD_NAME:
            DECODE_QSTR;
            printf("LOAD_NAME %s", qstr_str(qst));
            if (MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE) {
                printf(" (cache=%u)", *ip++);
            }
            break;

        case MP_BC_LOAD_GLOBAL:
            DECODE_QSTR;
            printf("LOAD_GLOBAL %s", qstr_str(qst));
            if (MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE) {
                printf(" (cache=%u)", *ip++);
            }
            break;

        case MP_BC_LOAD_ATTR:
            DECODE_QSTR;
            printf("LOAD_ATTR %s", qstr_str(qst));
            if (MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE) {
                printf(" (cache=%u)", *ip++);
            }
            break;

        case MP_BC_LOAD_METHOD:
            DECODE_QSTR;
            printf("LOAD_METHOD %s", qstr_str(qst));
            break;

        case MP_BC_LOAD_SUPER_METHOD:
            DECODE_QSTR;
            printf("LOAD_SUPER_METHOD %s", qstr_str(qst));
            break;

        case MP_BC_LOAD_BUILD_CLASS:
            printf("LOAD_BUILD_CLASS");
            break;

        case MP_BC_LOAD_SUBSCR:
            printf("LOAD_SUBSCR");
            break;

        case MP_BC_STORE_FAST_N:
            DECODE_UINT;
            printf("STORE_FAST_N " UINT_FMT, unum);
            break;

        case MP_BC_STORE_DEREF:
            DECODE_UINT;
            printf("STORE_DEREF " UINT_FMT, unum);
            break;

        case MP_BC_STORE_NAME:
            DECODE_QSTR;
            printf("STORE_NAME %s", qstr_str(qst));
            break;

        case MP_BC_STORE_GLOBAL:
            DECODE_QSTR;
            printf("STORE_GLOBAL %s", qstr_str(qst));
            break;

        case MP_BC_STORE_ATTR:
            DECODE_QSTR;
            printf("STORE_ATTR %s", qstr_str(qst));
            if (MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE) {
                printf(" (cache=%u)", *ip++);
            }
            break;

        case MP_BC_STORE_SUBSCR:
            printf("STORE_SUBSCR");
            break;

        case MP_BC_DELETE_FAST:
            DECODE_UINT;
            printf("DELETE_FAST " UINT_FMT, unum);
            break;

        case MP_BC_DELETE_DEREF:
            DECODE_UINT;
            printf("DELETE_DEREF " UINT_FMT, unum);
            break;

        case MP_BC_DELETE_NAME:
            DECODE_QSTR;
            printf("DELETE_NAME %s", qstr_str(qst));
            break;

        case MP_BC_DELETE_GLOBAL:
            DECODE_QSTR;
            printf("DELETE_GLOBAL %s", qstr_str(qst));
            break;

        case MP_BC_DUP_TOP:
            printf("DUP_TOP");
            break;

        case MP_BC_DUP_TOP_TWO:
            printf("DUP_TOP_TWO");
            break;

        case MP_BC_POP_TOP:
            printf("POP_TOP");
            break;

        case MP_BC_ROT_TWO:
            printf("ROT_TWO");
            break;

        case MP_BC_ROT_THREE:
            printf("ROT_THREE");
            break;

        case MP_BC_JUMP:
            DECODE_SLABEL;
            printf("JUMP " UINT_FMT, (mp_uint_t)(ip + unum - mp_showbc_code_start));
            break;

        case MP_BC_POP_JUMP_IF_TRUE:
            DECODE_SLABEL;
            printf("POP_JUMP_IF_TRUE " UINT_FMT, (mp_uint_t)(ip + unum - mp_showbc_code_start));
            break;

        case MP_BC_POP_JUMP_IF_FALSE:
            DECODE_SLABEL;
            printf("POP_JUMP_IF_FALSE " UINT_FMT, (mp_uint_t)(ip + unum - mp_showbc_code_start));
            break;

        case MP_BC_JUMP_IF_TRUE_OR_POP:
            DECODE_SLABEL;
            printf("JUMP_IF_TRUE_OR_POP " UINT_FMT, (mp_uint_t)(ip + unum - mp_showbc_code_start));
            break;

        case MP_BC_JUMP_IF_FALSE_OR_POP:
            DECODE_SLABEL;
            printf("JUMP_IF_FALSE_OR_POP " UINT_FMT, (mp_uint_t)(ip + unum - mp_showbc_code_start));
            break;

        case MP_BC_SETUP_WITH:
            DECODE_ULABEL; // loop-like labels are always forward
            printf("SETUP_WITH " UINT_FMT, (mp_uint_t)(ip + unum - mp_showbc_code_start));
            break;

        case MP_BC_WITH_CLEANUP:
            printf("WITH_CLEANUP");
            break;

        case MP_BC_UNWIND_JUMP:
            DECODE_SLABEL;
            printf("UNWIND_JUMP " UINT_FMT " %d", (mp_uint_t)(ip + unum - mp_showbc_code_start), *ip);
            ip += 1;
            break;

        case MP_BC_SETUP_EXCEPT:
            DECODE_ULABEL; // except labels are always forward
            printf("SETUP_EXCEPT " UINT_FMT, (mp_uint_t)(ip + unum - mp_showbc_code_start));
            break;

        case MP_BC_SETUP_FINALLY:
            DECODE_ULABEL; // except labels are always forward
            printf("SETUP_FINALLY " UINT_FMT, (mp_uint_t)(ip + unum - mp_showbc_code_start));
            break;

        case MP_BC_END_FINALLY:
            // if TOS is an exception, reraises the exception (3 values on TOS)
            // if TOS is an integer, does something else
            // if TOS is None, just pops it and continues
            // else error
            printf("END_FINALLY");
            break;

        case MP_BC_GET_ITER:
            printf("GET_ITER");
            break;

        case MP_BC_GET_ITER_STACK:
            printf("GET_ITER_STACK");
            break;

        case MP_BC_FOR_ITER:
            DECODE_ULABEL; // the jump offset if iteration finishes; for labels are always forward
            printf("FOR_ITER " UINT_FMT, (mp_uint_t)(ip + unum - mp_showbc_code_start));
            break;

        case MP_BC_POP_EXCEPT_JUMP:
            DECODE_ULABEL; // these labels are always forward
            printf("POP_EXCEPT_JUMP " UINT_FMT, (mp_uint_t)(ip + unum - mp_showbc_code_start));
            break;

        case MP_BC_BUILD_TUPLE:
            DECODE_UINT;
            printf("BUILD_TUPLE " UINT_FMT, unum);
            break;

        case MP_BC_BUILD_LIST:
            DECODE_UINT;
            printf("BUILD_LIST " UINT_FMT, unum);
            break;

        case MP_BC_BUILD_MAP:
            DECODE_UINT;
            printf("BUILD_MAP " UINT_FMT, unum);
            break;

        case MP_BC_STORE_MAP:
            printf("STORE_MAP");
            break;

        case MP_BC_BUILD_SET:
            DECODE_UINT;
            printf("BUILD_SET " UINT_FMT, unum);
            break;

#if MICROPY_PY_BUILTINS_SLICE
        case MP_BC_BUILD_SLICE:
            DECODE_UINT;
            printf("BUILD_SLICE " UINT_FMT, unum);
            break;
#endif

        case MP_BC_STORE_COMP:
            DECODE_UINT;
            printf("STORE_COMP " UINT_FMT, unum);
            break;

        case MP_BC_UNPACK_SEQUENCE:
            DECODE_UINT;
            printf("UNPACK_SEQUENCE " UINT_FMT, unum);
            break;

        case MP_BC_UNPACK_EX:
            DECODE_UINT;
            printf("UNPACK_EX " UINT_FMT, unum);
            break;

        case MP_BC_MAKE_FUNCTION:
            DECODE_PTR;
            printf("MAKE_FUNCTION %p", (void*)(uintptr_t)unum);
            break;

        case MP_BC_MAKE_FUNCTION_DEFARGS:
            DECODE_PTR;
            printf("MAKE_FUNCTION_DEFARGS %p", (void*)(uintptr_t)unum);
            break;

        case MP_BC_MAKE_CLOSURE: {
            DECODE_PTR;
            mp_uint_t n_closed_over = *ip++;
            printf("MAKE_CLOSURE %p " UINT_FMT, (void*)(uintptr_t)unum, n_closed_over);
            break;
        }

        case MP_BC_MAKE_CLOSURE_DEFARGS: {
            DECODE_PTR;
            mp_uint_t n_closed_over = *ip++;
            printf("MAKE_CLOSURE_DEFARGS %p " UINT_FMT, (void*)(uintptr_t)unum, n_closed_over);
            break;
        }

        case MP_BC_CALL_FUNCTION:
            DECODE_UINT;
            printf("CALL_FUNCTION n=" UINT_FMT " nkw=" UINT_FMT, unum & 0xff, (unum >> 8) & 0xff);
            break;

        case MP_BC_CALL_FUNCTION_VAR_KW:
            DECODE_UINT;
            printf("CALL_FUNCTION_VAR_KW n=" UINT_FMT " nkw=" UINT_FMT, unum & 0xff, (unum >> 8) & 0xff);
            break;

        case MP_BC_CALL_METHOD:
            DECODE_UINT;
            printf("CALL_METHOD n=" UINT_FMT " nkw=" UINT_FMT, unum & 0xff, (unum >> 8) & 0xff);
            break;

        case MP_BC_CALL_METHOD_VAR_KW:
            DECODE_UINT;
            printf("CALL_METHOD_VAR_KW n=" UINT_FMT " nkw=" UINT_FMT, unum & 0xff, (unum >> 8) & 0xff);
            break;

        case MP_BC_RETURN_VALUE:
            printf("RETURN_VALUE");
            break;

        case MP_BC_RAISE_VARARGS:
            unum = *ip++;
            printf("RAISE_VARARGS " UINT_FMT, unum);
            break;

        case MP_BC_YIELD_VALUE:
            printf("YIELD_VALUE");
            break;

        case MP_BC_YIELD_FROM:
            printf("YIELD_FROM");
            break;

        case MP_BC_IMPORT_NAME:
            DECODE_QSTR;
            printf("IMPORT_NAME '%s'", qstr_str(qst));
            break;

        case MP_BC_IMPORT_FROM:
            DECODE_QSTR;
            printf("IMPORT_FROM '%s'", qstr_str(qst));
            break;

        case MP_BC_IMPORT_STAR:
            printf("IMPORT_STAR");
            break;

        default:
            if (ip[-1] < MP_BC_LOAD_CONST_SMALL_INT_MULTI + 64) {
                printf("LOAD_CONST_SMALL_INT " INT_FMT, (mp_int_t)ip[-1] - MP_BC_LOAD_CONST_SMALL_INT_MULTI - 16);
            } else if (ip[-1] < MP_BC_LOAD_FAST_MULTI + 16) {
                printf("LOAD_FAST " UINT_FMT, (mp_uint_t)ip[-1] - MP_BC_LOAD_FAST_MULTI);
            } else if (ip[-1] < MP_BC_STORE_FAST_MULTI + 16) {
                printf("STORE_FAST " UINT_FMT, (mp_uint_t)ip[-1] - MP_BC_STORE_FAST_MULTI);
            } else if (ip[-1] < MP_BC_UNARY_OP_MULTI + MP_UNARY_OP_NUM_BYTECODE) {
                printf("UNARY_OP " UINT_FMT, (mp_uint_t)ip[-1] - MP_BC_UNARY_OP_MULTI);
            } else if (ip[-1] < MP_BC_BINARY_OP_MULTI + MP_BINARY_OP_NUM_BYTECODE) {
                mp_uint_t op = ip[-1] - MP_BC_BINARY_OP_MULTI;
                printf("BINARY_OP " UINT_FMT " %s", op, qstr_str(mp_binary_op_method_name[op]));
            } else {
                printf("code %p, byte code 0x%02x not implemented\n", ip - 1, ip[-1]);
                assert(0);
                return ip;
            }
            break;
    }

    return ip;
}

void mp_bytecode_print2(const byte *ip, size_t len, const mp_uint_t *const_table) {
    mp_showbc_code_start = ip;
    mp_showbc_const_table = const_table;
    while (ip < len + mp_showbc_code_start) {
        printf("%02u ", (uint)(ip - mp_showbc_code_start));
        ip = mp_bytecode_print_str(ip);
        printf("\n");
    }
}

#endif // MICROPY_DEBUG_PRINTERS
