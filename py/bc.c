/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
 * Copyright (c) 2014 Paul Sokolovsky
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

#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "py/nlr.h"
#include "py/objfun.h"
#include "py/runtime0.h"
#include "py/bc0.h"
#include "py/bc.h"

#if 0 // print debugging info
#define DEBUG_PRINT (1)
#else // don't print debugging info
#define DEBUG_PRINT (0)
#define DEBUG_printf(...) (void)0
#endif

mp_uint_t mp_decode_uint(const byte **ptr) {
    mp_uint_t unum = 0;
    byte val;
    const byte *p = *ptr;
    do {
        val = *p++;
        unum = (unum << 7) | (val & 0x7f);
    } while ((val & 0x80) != 0);
    *ptr = p;
    return unum;
}

// This function is used to help reduce stack usage at the caller, for the case when
// the caller doesn't need to increase the ptr argument.  If ptr is a local variable
// and the caller uses mp_decode_uint(&ptr) instead of this function, then the compiler
// must allocate a slot on the stack for ptr, and this slot cannot be reused for
// anything else in the function because the pointer may have been stored in a global
// and reused later in the function.
mp_uint_t mp_decode_uint_value(const byte *ptr) {
    return mp_decode_uint(&ptr);
}

// This function is used to help reduce stack usage at the caller, for the case when
// the caller doesn't need the actual value and just wants to skip over it.
const byte *mp_decode_uint_skip(const byte *ptr) {
    while ((*ptr++) & 0x80) {
    }
    return ptr;
}

STATIC NORETURN void fun_pos_args_mismatch(mp_obj_fun_bc_t *f, size_t expected, size_t given) {
#if MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE
    // generic message, used also for other argument issues
    (void)f;
    (void)expected;
    (void)given;
    mp_arg_error_terse_mismatch();
#elif MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_NORMAL
    (void)f;
    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError,
        "function takes %d positional arguments but %d were given", expected, given));
#elif MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_DETAILED
    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError,
        "%q() takes %d positional arguments but %d were given",
        mp_obj_fun_get_name(MP_OBJ_FROM_PTR(f)), expected, given));
#endif
}

#if DEBUG_PRINT
STATIC void dump_args(const mp_obj_t *a, size_t sz) {
    DEBUG_printf("%p: ", a);
    for (size_t i = 0; i < sz; i++) {
        DEBUG_printf("%p ", a[i]);
    }
    DEBUG_printf("\n");
}
#else
#define dump_args(...) (void)0
#endif

// On entry code_state should be allocated somewhere (stack/heap) and
// contain the following valid entries:
//    - code_state->fun_bc should contain a pointer to the function object
//    - code_state->ip should contain the offset in bytes from the pointer
//      code_state->fun_bc->bytecode to the entry n_state (0 for bytecode, non-zero for native)
void mp_setup_code_state(mp_code_state_t *code_state, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // This function is pretty complicated.  It's main aim is to be efficient in speed and RAM
    // usage for the common case of positional only args.

    // get the function object that we want to set up (could be bytecode or native code)
    mp_obj_fun_bc_t *self = code_state->fun_bc;

    // ip comes in as an offset into bytecode, so turn it into a true pointer
    code_state->ip = self->bytecode + (size_t)code_state->ip;

    #if MICROPY_STACKLESS
    code_state->prev = NULL;
    #endif

    // get params
    size_t n_state = mp_decode_uint(&code_state->ip);
    code_state->ip = mp_decode_uint_skip(code_state->ip); // skip n_exc_stack
    size_t scope_flags = *code_state->ip++;
    size_t n_pos_args = *code_state->ip++;
    size_t n_kwonly_args = *code_state->ip++;
    size_t n_def_pos_args = *code_state->ip++;

    code_state->sp = &code_state->state[0] - 1;
    code_state->exc_sp = (mp_exc_stack_t*)(code_state->state + n_state) - 1;

    // zero out the local stack to begin with
    memset(code_state->state, 0, n_state * sizeof(*code_state->state));

    const mp_obj_t *kwargs = args + n_args;

    // var_pos_kw_args points to the stack where the var-args tuple, and var-kw dict, should go (if they are needed)
    mp_obj_t *var_pos_kw_args = &code_state->state[n_state - 1 - n_pos_args - n_kwonly_args];

    // check positional arguments

    if (n_args > n_pos_args) {
        // given more than enough arguments
        if ((scope_flags & MP_SCOPE_FLAG_VARARGS) == 0) {
            fun_pos_args_mismatch(self, n_pos_args, n_args);
        }
        // put extra arguments in varargs tuple
        *var_pos_kw_args-- = mp_obj_new_tuple(n_args - n_pos_args, args + n_pos_args);
        n_args = n_pos_args;
    } else {
        if ((scope_flags & MP_SCOPE_FLAG_VARARGS) != 0) {
            DEBUG_printf("passing empty tuple as *args\n");
            *var_pos_kw_args-- = mp_const_empty_tuple;
        }
        // Apply processing and check below only if we don't have kwargs,
        // otherwise, kw handling code below has own extensive checks.
        if (n_kw == 0 && (scope_flags & MP_SCOPE_FLAG_DEFKWARGS) == 0) {
            if (n_args >= (size_t)(n_pos_args - n_def_pos_args)) {
                // given enough arguments, but may need to use some default arguments
                for (size_t i = n_args; i < n_pos_args; i++) {
                    code_state->state[n_state - 1 - i] = self->extra_args[i - (n_pos_args - n_def_pos_args)];
                }
            } else {
                fun_pos_args_mismatch(self, n_pos_args - n_def_pos_args, n_args);
            }
        }
    }

    // copy positional args into state
    for (size_t i = 0; i < n_args; i++) {
        code_state->state[n_state - 1 - i] = args[i];
    }

    // check keyword arguments

    if (n_kw != 0 || (scope_flags & MP_SCOPE_FLAG_DEFKWARGS) != 0) {
        DEBUG_printf("Initial args: ");
        dump_args(code_state->state + n_state - n_pos_args - n_kwonly_args, n_pos_args + n_kwonly_args);

        mp_obj_t dict = MP_OBJ_NULL;
        if ((scope_flags & MP_SCOPE_FLAG_VARKEYWORDS) != 0) {
            dict = mp_obj_new_dict(n_kw); // TODO: better go conservative with 0?
            *var_pos_kw_args = dict;
        }

        // get pointer to arg_names array
        const mp_obj_t *arg_names = (const mp_obj_t*)self->const_table;

        for (size_t i = 0; i < n_kw; i++) {
            // the keys in kwargs are expected to be qstr objects
            mp_obj_t wanted_arg_name = kwargs[2 * i];
            for (size_t j = 0; j < n_pos_args + n_kwonly_args; j++) {
                if (wanted_arg_name == arg_names[j]) {
                    if (code_state->state[n_state - 1 - j] != MP_OBJ_NULL) {
                        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError,
                            "function got multiple values for argument '%q'", MP_OBJ_QSTR_VALUE(wanted_arg_name)));
                    }
                    code_state->state[n_state - 1 - j] = kwargs[2 * i + 1];
                    goto continue2;
                }
            }
            // Didn't find name match with positional args
            if ((scope_flags & MP_SCOPE_FLAG_VARKEYWORDS) == 0) {
                if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
                    mp_raise_TypeError("unexpected keyword argument");
                } else {
                    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError,
                        "unexpected keyword argument '%q'", MP_OBJ_QSTR_VALUE(wanted_arg_name)));
                }
            }
            mp_obj_dict_store(dict, kwargs[2 * i], kwargs[2 * i + 1]);
continue2:;
        }

        DEBUG_printf("Args with kws flattened: ");
        dump_args(code_state->state + n_state - n_pos_args - n_kwonly_args, n_pos_args + n_kwonly_args);

        // fill in defaults for positional args
        mp_obj_t *d = &code_state->state[n_state - n_pos_args];
        mp_obj_t *s = &self->extra_args[n_def_pos_args - 1];
        for (size_t i = n_def_pos_args; i > 0; i--, d++, s--) {
            if (*d == MP_OBJ_NULL) {
                *d = *s;
            }
        }

        DEBUG_printf("Args after filling default positional: ");
        dump_args(code_state->state + n_state - n_pos_args - n_kwonly_args, n_pos_args + n_kwonly_args);

        // Check that all mandatory positional args are specified
        while (d < &code_state->state[n_state]) {
            if (*d++ == MP_OBJ_NULL) {
                nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError,
                    "function missing required positional argument #%d", &code_state->state[n_state] - d));
            }
        }

        // Check that all mandatory keyword args are specified
        // Fill in default kw args if we have them
        for (size_t i = 0; i < n_kwonly_args; i++) {
            if (code_state->state[n_state - 1 - n_pos_args - i] == MP_OBJ_NULL) {
                mp_map_elem_t *elem = NULL;
                if ((scope_flags & MP_SCOPE_FLAG_DEFKWARGS) != 0) {
                    elem = mp_map_lookup(&((mp_obj_dict_t*)MP_OBJ_TO_PTR(self->extra_args[n_def_pos_args]))->map, arg_names[n_pos_args + i], MP_MAP_LOOKUP);
                }
                if (elem != NULL) {
                    code_state->state[n_state - 1 - n_pos_args - i] = elem->value;
                } else {
                    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError,
                        "function missing required keyword argument '%q'", MP_OBJ_QSTR_VALUE(arg_names[n_pos_args + i])));
                }
            }
        }

    } else {
        // no keyword arguments given
        if (n_kwonly_args != 0) {
            mp_raise_TypeError("function missing keyword-only argument");
        }
        if ((scope_flags & MP_SCOPE_FLAG_VARKEYWORDS) != 0) {
            *var_pos_kw_args = mp_obj_new_dict(0);
        }
    }

    // get the ip and skip argument names
    const byte *ip = code_state->ip;

    // jump over code info (source file and line-number mapping)
    ip += mp_decode_uint_value(ip);

    // bytecode prelude: initialise closed over variables
    size_t local_num;
    while ((local_num = *ip++) != 255) {
        code_state->state[n_state - 1 - local_num] =
            mp_obj_new_cell(code_state->state[n_state - 1 - local_num]);
    }

    // now that we skipped over the prelude, set the ip for the VM
    code_state->ip = ip;

    DEBUG_printf("Calling: n_pos_args=%d, n_kwonly_args=%d\n", n_pos_args, n_kwonly_args);
    dump_args(code_state->state + n_state - n_pos_args - n_kwonly_args, n_pos_args + n_kwonly_args);
    dump_args(code_state->state, n_state);
}

#if MICROPY_PERSISTENT_CODE_LOAD || MICROPY_PERSISTENT_CODE_SAVE

// The following table encodes the number of bytes that a specific opcode
// takes up.  There are 3 special opcodes that always have an extra byte:
//     MP_BC_MAKE_CLOSURE
//     MP_BC_MAKE_CLOSURE_DEFARGS
//     MP_BC_RAISE_VARARGS
// There are 4 special opcodes that have an extra byte only when
// MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE is enabled:
//     MP_BC_LOAD_NAME
//     MP_BC_LOAD_GLOBAL
//     MP_BC_LOAD_ATTR
//     MP_BC_STORE_ATTR
#define OC4(a, b, c, d) (a | (b << 2) | (c << 4) | (d << 6))
#define U (0) // undefined opcode
#define B (MP_OPCODE_BYTE) // single byte
#define Q (MP_OPCODE_QSTR) // single byte plus 2-byte qstr
#define V (MP_OPCODE_VAR_UINT) // single byte plus variable encoded unsigned int
#define O (MP_OPCODE_OFFSET) // single byte plus 2-byte bytecode offset
STATIC const byte opcode_format_table[64] = {
    OC4(U, U, U, U), // 0x00-0x03
    OC4(U, U, U, U), // 0x04-0x07
    OC4(U, U, U, U), // 0x08-0x0b
    OC4(U, U, U, U), // 0x0c-0x0f
    OC4(B, B, B, U), // 0x10-0x13
    OC4(V, U, Q, V), // 0x14-0x17
    OC4(B, V, V, Q), // 0x18-0x1b
    OC4(Q, Q, Q, Q), // 0x1c-0x1f
    OC4(B, B, V, V), // 0x20-0x23
    OC4(Q, Q, Q, B), // 0x24-0x27
    OC4(V, V, Q, Q), // 0x28-0x2b
    OC4(U, U, U, U), // 0x2c-0x2f
    OC4(B, B, B, B), // 0x30-0x33
    OC4(B, O, O, O), // 0x34-0x37
    OC4(O, O, U, U), // 0x38-0x3b
    OC4(U, O, B, O), // 0x3c-0x3f
    OC4(O, B, B, O), // 0x40-0x43
    OC4(B, B, O, U), // 0x44-0x47
    OC4(U, U, U, U), // 0x48-0x4b
    OC4(U, U, U, U), // 0x4c-0x4f
    OC4(V, V, U, V), // 0x50-0x53
    OC4(B, U, V, V), // 0x54-0x57
    OC4(V, V, V, B), // 0x58-0x5b
    OC4(B, B, B, U), // 0x5c-0x5f
    OC4(V, V, V, V), // 0x60-0x63
    OC4(V, V, V, V), // 0x64-0x67
    OC4(Q, Q, B, U), // 0x68-0x6b
    OC4(U, U, U, U), // 0x6c-0x6f

    OC4(B, B, B, B), // 0x70-0x73
    OC4(B, B, B, B), // 0x74-0x77
    OC4(B, B, B, B), // 0x78-0x7b
    OC4(B, B, B, B), // 0x7c-0x7f
    OC4(B, B, B, B), // 0x80-0x83
    OC4(B, B, B, B), // 0x84-0x87
    OC4(B, B, B, B), // 0x88-0x8b
    OC4(B, B, B, B), // 0x8c-0x8f
    OC4(B, B, B, B), // 0x90-0x93
    OC4(B, B, B, B), // 0x94-0x97
    OC4(B, B, B, B), // 0x98-0x9b
    OC4(B, B, B, B), // 0x9c-0x9f
    OC4(B, B, B, B), // 0xa0-0xa3
    OC4(B, B, B, B), // 0xa4-0xa7
    OC4(B, B, B, B), // 0xa8-0xab
    OC4(B, B, B, B), // 0xac-0xaf

    OC4(B, B, B, B), // 0xb0-0xb3
    OC4(B, B, B, B), // 0xb4-0xb7
    OC4(B, B, B, B), // 0xb8-0xbb
    OC4(B, B, B, B), // 0xbc-0xbf

    OC4(B, B, B, B), // 0xc0-0xc3
    OC4(B, B, B, B), // 0xc4-0xc7
    OC4(B, B, B, B), // 0xc8-0xcb
    OC4(B, B, B, B), // 0xcc-0xcf

    OC4(B, B, B, B), // 0xd0-0xd3
    OC4(B, B, B, B), // 0xd4-0xd7
    OC4(B, B, B, B), // 0xd8-0xdb
    OC4(B, B, B, B), // 0xdc-0xdf

    OC4(B, B, B, B), // 0xe0-0xe3
    OC4(B, B, B, B), // 0xe4-0xe7
    OC4(B, B, B, B), // 0xe8-0xeb
    OC4(B, B, B, B), // 0xec-0xef

    OC4(B, B, B, B), // 0xf0-0xf3
    OC4(B, B, B, B), // 0xf4-0xf7
    OC4(B, B, B, U), // 0xf8-0xfb
    OC4(U, U, U, U), // 0xfc-0xff
};
#undef OC4
#undef U
#undef B
#undef Q
#undef V
#undef O

uint mp_opcode_format(const byte *ip, size_t *opcode_size) {
    uint f = (opcode_format_table[*ip >> 2] >> (2 * (*ip & 3))) & 3;
    const byte *ip_start = ip;
    if (f == MP_OPCODE_QSTR) {
        ip += 3;
    } else {
        int extra_byte = (
            *ip == MP_BC_RAISE_VARARGS
            || *ip == MP_BC_MAKE_CLOSURE
            || *ip == MP_BC_MAKE_CLOSURE_DEFARGS
            #if MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE
            || *ip == MP_BC_LOAD_NAME
            || *ip == MP_BC_LOAD_GLOBAL
            || *ip == MP_BC_LOAD_ATTR
            || *ip == MP_BC_STORE_ATTR
            #endif
        );
        ip += 1;
        if (f == MP_OPCODE_VAR_UINT) {
            while ((*ip++ & 0x80) != 0) {
            }
        } else if (f == MP_OPCODE_OFFSET) {
            ip += 2;
        }
        ip += extra_byte;
    }
    *opcode_size = ip - ip_start;
    return f;
}

#endif // MICROPY_PERSISTENT_CODE_LOAD || MICROPY_PERSISTENT_CODE_SAVE
