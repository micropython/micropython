/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#include "py/runtime.h"
#include "py/bc0.h"
#include "py/bc.h"

#if MICROPY_DEBUG_VERBOSE // print debugging info
#define DEBUG_PRINT (1)
#else // don't print debugging info
#define DEBUG_PRINT (0)
#define DEBUG_printf(...) (void)0
#endif

#if !MICROPY_PERSISTENT_CODE

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

#endif

STATIC NORETURN void fun_pos_args_mismatch(mp_obj_fun_bc_t *f, size_t expected, size_t given) {
    #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
    // generic message, used also for other argument issues
    (void)f;
    (void)expected;
    (void)given;
    mp_arg_error_terse_mismatch();
    #elif MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_NORMAL
    (void)f;
    mp_raise_msg_varg(&mp_type_TypeError,
        MP_ERROR_TEXT("function takes %d positional arguments but %d were given"), expected, given);
    #elif MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_DETAILED
    mp_raise_msg_varg(&mp_type_TypeError,
        MP_ERROR_TEXT("%q() takes %d positional arguments but %d were given"),
        mp_obj_fun_get_name(MP_OBJ_FROM_PTR(f)), expected, given);
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

    #if MICROPY_PY_SYS_SETTRACE
    code_state->prev_state = NULL;
    code_state->frame = NULL;
    #endif

    // Get cached n_state (rather than decode it again)
    size_t n_state = code_state->n_state;

    // Decode prelude
    size_t n_state_unused, n_exc_stack_unused, scope_flags, n_pos_args, n_kwonly_args, n_def_pos_args;
    MP_BC_PRELUDE_SIG_DECODE_INTO(code_state->ip, n_state_unused, n_exc_stack_unused, scope_flags, n_pos_args, n_kwonly_args, n_def_pos_args);
    (void)n_state_unused;
    (void)n_exc_stack_unused;

    code_state->sp = &code_state->state[0] - 1;
    code_state->exc_sp_idx = 0;

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
        const mp_obj_t *arg_names = (const mp_obj_t *)self->const_table;

        for (size_t i = 0; i < n_kw; i++) {
            // the keys in kwargs are expected to be qstr objects
            mp_obj_t wanted_arg_name = kwargs[2 * i];
            for (size_t j = 0; j < n_pos_args + n_kwonly_args; j++) {
                if (wanted_arg_name == arg_names[j]) {
                    if (code_state->state[n_state - 1 - j] != MP_OBJ_NULL) {
                        mp_raise_msg_varg(&mp_type_TypeError,
                            MP_ERROR_TEXT("function got multiple values for argument '%q'"), MP_OBJ_QSTR_VALUE(wanted_arg_name));
                    }
                    code_state->state[n_state - 1 - j] = kwargs[2 * i + 1];
                    goto continue2;
                }
            }
            // Didn't find name match with positional args
            if ((scope_flags & MP_SCOPE_FLAG_VARKEYWORDS) == 0) {
                #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
                mp_raise_TypeError(MP_ERROR_TEXT("unexpected keyword argument"));
                #else
                mp_raise_msg_varg(&mp_type_TypeError,
                    MP_ERROR_TEXT("unexpected keyword argument '%q'"), MP_OBJ_QSTR_VALUE(wanted_arg_name));
                #endif
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
                mp_raise_msg_varg(&mp_type_TypeError,
                    MP_ERROR_TEXT("function missing required positional argument #%d"), &code_state->state[n_state] - d);
            }
        }

        // Check that all mandatory keyword args are specified
        // Fill in default kw args if we have them
        for (size_t i = 0; i < n_kwonly_args; i++) {
            if (code_state->state[n_state - 1 - n_pos_args - i] == MP_OBJ_NULL) {
                mp_map_elem_t *elem = NULL;
                if ((scope_flags & MP_SCOPE_FLAG_DEFKWARGS) != 0) {
                    elem = mp_map_lookup(&((mp_obj_dict_t *)MP_OBJ_TO_PTR(self->extra_args[n_def_pos_args]))->map, arg_names[n_pos_args + i], MP_MAP_LOOKUP);
                }
                if (elem != NULL) {
                    code_state->state[n_state - 1 - n_pos_args - i] = elem->value;
                } else {
                    mp_raise_msg_varg(&mp_type_TypeError,
                        MP_ERROR_TEXT("function missing required keyword argument '%q'"), MP_OBJ_QSTR_VALUE(arg_names[n_pos_args + i]));
                }
            }
        }

    } else {
        // no keyword arguments given
        if (n_kwonly_args != 0) {
            mp_raise_TypeError(MP_ERROR_TEXT("function missing keyword-only argument"));
        }
        if ((scope_flags & MP_SCOPE_FLAG_VARKEYWORDS) != 0) {
            *var_pos_kw_args = mp_obj_new_dict(0);
        }
    }

    // read the size part of the prelude
    const byte *ip = code_state->ip;
    MP_BC_PRELUDE_SIZE_DECODE(ip);

    // jump over code info (source file and line-number mapping)
    ip += n_info;

    // bytecode prelude: initialise closed over variables
    for (; n_cell; --n_cell) {
        size_t local_num = *ip++;
        code_state->state[n_state - 1 - local_num] =
            mp_obj_new_cell(code_state->state[n_state - 1 - local_num]);
    }

    #if !MICROPY_PERSISTENT_CODE
    // so bytecode is aligned
    ip = MP_ALIGN(ip, sizeof(mp_uint_t));
    #endif

    // now that we skipped over the prelude, set the ip for the VM
    code_state->ip = ip;

    DEBUG_printf("Calling: n_pos_args=%d, n_kwonly_args=%d\n", n_pos_args, n_kwonly_args);
    dump_args(code_state->state + n_state - n_pos_args - n_kwonly_args, n_pos_args + n_kwonly_args);
    dump_args(code_state->state, n_state);
}

#if MICROPY_PERSISTENT_CODE_LOAD || MICROPY_PERSISTENT_CODE_SAVE

// The following table encodes the number of bytes that a specific opcode
// takes up.  Some opcodes have an extra byte, defined by MP_BC_MASK_EXTRA_BYTE.
// There are 4 special opcodes that have an extra byte only when
// MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE is enabled (and they take a qstr):
//     MP_BC_LOAD_NAME
//     MP_BC_LOAD_GLOBAL
//     MP_BC_LOAD_ATTR
//     MP_BC_STORE_ATTR
uint mp_opcode_format(const byte *ip, size_t *opcode_size, bool count_var_uint) {
    uint f = MP_BC_FORMAT(*ip);
    const byte *ip_start = ip;
    if (f == MP_BC_FORMAT_QSTR) {
        if (MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE_DYNAMIC) {
            if (*ip == MP_BC_LOAD_NAME
                || *ip == MP_BC_LOAD_GLOBAL
                || *ip == MP_BC_LOAD_ATTR
                || *ip == MP_BC_STORE_ATTR) {
                ip += 1;
            }
        }
        ip += 3;
    } else {
        int extra_byte = (*ip & MP_BC_MASK_EXTRA_BYTE) == 0;
        ip += 1;
        if (f == MP_BC_FORMAT_VAR_UINT) {
            if (count_var_uint) {
                while ((*ip++ & 0x80) != 0) {
                }
            }
        } else if (f == MP_BC_FORMAT_OFFSET) {
            ip += 2;
        }
        ip += extra_byte;
    }
    *opcode_size = ip - ip_start;
    return f;
}

#endif // MICROPY_PERSISTENT_CODE_LOAD || MICROPY_PERSISTENT_CODE_SAVE
