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

STATIC NORETURN void fun_pos_args_mismatch(mp_obj_fun_bc_t *f, mp_uint_t expected, mp_uint_t given) {
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
        mp_obj_fun_get_name(f), expected, given));
#endif
}

#if DEBUG_PRINT
STATIC void dump_args(const mp_obj_t *a, mp_uint_t sz) {
    DEBUG_printf("%p: ", a);
    for (mp_uint_t i = 0; i < sz; i++) {
        DEBUG_printf("%p ", a[i]);
    }
    DEBUG_printf("\n");
}
#else
#define dump_args(...) (void)0
#endif

// On entry code_state should be allocated somewhere (stack/heap) and
// contain the following valid entries:
//    - code_state->code_info should be the offset in bytes from the start of
//      the bytecode chunk to the start of the code-info within the bytecode
//    - code_state->ip should contain the offset in bytes from the start of
//      the bytecode chunk to the start of the prelude within the bytecode
//    - code_state->n_state should be set to the state size (locals plus stack)
void mp_setup_code_state(mp_code_state *code_state, mp_obj_t self_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // This function is pretty complicated.  It's main aim is to be efficient in speed and RAM
    // usage for the common case of positional only args.
    mp_obj_fun_bc_t *self = self_in;
    mp_uint_t n_state = code_state->n_state;

    #if MICROPY_STACKLESS
    code_state->prev = NULL;
    #endif
    code_state->code_info = self->bytecode + (mp_uint_t)code_state->code_info;
    code_state->sp = &code_state->state[0] - 1;
    code_state->exc_sp = (mp_exc_stack_t*)(code_state->state + n_state) - 1;

    // zero out the local stack to begin with
    memset(code_state->state, 0, n_state * sizeof(*code_state->state));

    const mp_obj_t *kwargs = args + n_args;

    // var_pos_kw_args points to the stack where the var-args tuple, and var-kw dict, should go (if they are needed)
    mp_obj_t *var_pos_kw_args = &code_state->state[n_state - 1 - self->n_pos_args - self->n_kwonly_args];

    // check positional arguments

    if (n_args > self->n_pos_args) {
        // given more than enough arguments
        if (!self->takes_var_args) {
            fun_pos_args_mismatch(self, self->n_pos_args, n_args);
        }
        // put extra arguments in varargs tuple
        *var_pos_kw_args-- = mp_obj_new_tuple(n_args - self->n_pos_args, args + self->n_pos_args);
        n_args = self->n_pos_args;
    } else {
        if (self->takes_var_args) {
            DEBUG_printf("passing empty tuple as *args\n");
            *var_pos_kw_args-- = mp_const_empty_tuple;
        }
        // Apply processing and check below only if we don't have kwargs,
        // otherwise, kw handling code below has own extensive checks.
        if (n_kw == 0 && !self->has_def_kw_args) {
            if (n_args >= (mp_uint_t)(self->n_pos_args - self->n_def_args)) {
                // given enough arguments, but may need to use some default arguments
                for (mp_uint_t i = n_args; i < self->n_pos_args; i++) {
                    code_state->state[n_state - 1 - i] = self->extra_args[i - (self->n_pos_args - self->n_def_args)];
                }
            } else {
                fun_pos_args_mismatch(self, self->n_pos_args - self->n_def_args, n_args);
            }
        }
    }

    // copy positional args into state
    for (mp_uint_t i = 0; i < n_args; i++) {
        code_state->state[n_state - 1 - i] = args[i];
    }

    // check keyword arguments

    if (n_kw != 0 || self->has_def_kw_args) {
        DEBUG_printf("Initial args: ");
        dump_args(code_state->state + n_state - self->n_pos_args - self->n_kwonly_args, self->n_pos_args + self->n_kwonly_args);

        mp_obj_t dict = MP_OBJ_NULL;
        if (self->takes_kw_args) {
            dict = mp_obj_new_dict(n_kw); // TODO: better go conservative with 0?
            *var_pos_kw_args = dict;
        }

        // get pointer to arg_names array at start of bytecode prelude
        const mp_obj_t *arg_names;
        {
            const byte *code_info = code_state->code_info;
            mp_uint_t code_info_size = mp_decode_uint(&code_info);
            arg_names = (const mp_obj_t*)(code_state->code_info + code_info_size);
        }

        for (mp_uint_t i = 0; i < n_kw; i++) {
            mp_obj_t wanted_arg_name = kwargs[2 * i];
            for (mp_uint_t j = 0; j < self->n_pos_args + self->n_kwonly_args; j++) {
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
            if (!self->takes_kw_args) {
                nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "function does not take keyword arguments"));
            }
            mp_obj_dict_store(dict, kwargs[2 * i], kwargs[2 * i + 1]);
continue2:;
        }

        DEBUG_printf("Args with kws flattened: ");
        dump_args(code_state->state + n_state - self->n_pos_args - self->n_kwonly_args, self->n_pos_args + self->n_kwonly_args);

        // fill in defaults for positional args
        mp_obj_t *d = &code_state->state[n_state - self->n_pos_args];
        mp_obj_t *s = &self->extra_args[self->n_def_args - 1];
        for (mp_uint_t i = self->n_def_args; i > 0; i--, d++, s--) {
            if (*d == MP_OBJ_NULL) {
                *d = *s;
            }
        }

        DEBUG_printf("Args after filling default positional: ");
        dump_args(code_state->state + n_state - self->n_pos_args - self->n_kwonly_args, self->n_pos_args + self->n_kwonly_args);

        // Check that all mandatory positional args are specified
        while (d < &code_state->state[n_state]) {
            if (*d++ == MP_OBJ_NULL) {
                nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError,
                    "function missing required positional argument #%d", &code_state->state[n_state] - d));
            }
        }

        // Check that all mandatory keyword args are specified
        // Fill in default kw args if we have them
        for (mp_uint_t i = 0; i < self->n_kwonly_args; i++) {
            if (code_state->state[n_state - 1 - self->n_pos_args - i] == MP_OBJ_NULL) {
                mp_map_elem_t *elem = NULL;
                if (self->has_def_kw_args) {
                    elem = mp_map_lookup(&((mp_obj_dict_t*)self->extra_args[self->n_def_args])->map, arg_names[self->n_pos_args + i], MP_MAP_LOOKUP);
                }
                if (elem != NULL) {
                    code_state->state[n_state - 1 - self->n_pos_args - i] = elem->value;
                } else {
                    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError,
                        "function missing required keyword argument '%q'", MP_OBJ_QSTR_VALUE(arg_names[self->n_pos_args + i])));
                }
            }
        }

    } else {
        // no keyword arguments given
        if (self->n_kwonly_args != 0) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError,
                "function missing keyword-only argument"));
        }
        if (self->takes_kw_args) {
            *var_pos_kw_args = mp_obj_new_dict(0);
        }
    }

    // bytecode prelude: initialise closed over variables
    const byte *ip = self->bytecode + (mp_uint_t)code_state->ip;
    mp_uint_t local_num;
    while ((local_num = *ip++) != 255) {
        code_state->state[n_state - 1 - local_num] =
            mp_obj_new_cell(code_state->state[n_state - 1 - local_num]);
    }

    // now that we skipped over the prelude, set the ip for the VM
    code_state->ip = ip;

    DEBUG_printf("Calling: n_pos_args=%d, n_kwonly_args=%d\n", self->n_pos_args, self->n_kwonly_args);
    dump_args(code_state->state + n_state - self->n_pos_args - self->n_kwonly_args, self->n_pos_args + self->n_kwonly_args);
    dump_args(code_state->state, n_state);
}
