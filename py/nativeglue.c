/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
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
#include <string.h>
#include <assert.h>

#include "py/runtime.h"
#include "py/smallint.h"
#include "py/emitglue.h"
#include "py/bc.h"

#if MICROPY_DEBUG_VERBOSE // print debugging info
#define DEBUG_printf DEBUG_printf
#else // don't print debugging info
#define DEBUG_printf(...) (void)0
#endif

#if MICROPY_EMIT_NATIVE

// convert a MicroPython object to a valid native value based on type
mp_uint_t mp_convert_obj_to_native(mp_obj_t obj, mp_uint_t type) {
    DEBUG_printf("mp_convert_obj_to_native(%p, " UINT_FMT ")\n", obj, type);
    switch (type & 0xf) {
        case MP_NATIVE_TYPE_OBJ: return (mp_uint_t)obj;
        case MP_NATIVE_TYPE_BOOL:
        case MP_NATIVE_TYPE_INT:
        case MP_NATIVE_TYPE_UINT: return mp_obj_get_int_truncated(obj);
        default: { // cast obj to a pointer
            mp_buffer_info_t bufinfo;
            if (mp_get_buffer(obj, &bufinfo, MP_BUFFER_RW)) {
                return (mp_uint_t)bufinfo.buf;
            } else {
                // assume obj is an integer that represents an address
                return mp_obj_get_int_truncated(obj);
            }
        }
    }
}

#endif

#if MICROPY_EMIT_NATIVE || MICROPY_EMIT_INLINE_ASM

// convert a native value to a MicroPython object based on type
mp_obj_t mp_convert_native_to_obj(mp_uint_t val, mp_uint_t type) {
    DEBUG_printf("mp_convert_native_to_obj(" UINT_FMT ", " UINT_FMT ")\n", val, type);
    switch (type & 0xf) {
        case MP_NATIVE_TYPE_OBJ: return (mp_obj_t)val;
        case MP_NATIVE_TYPE_BOOL: return mp_obj_new_bool(val);
        case MP_NATIVE_TYPE_INT: return mp_obj_new_int(val);
        case MP_NATIVE_TYPE_UINT: return mp_obj_new_int_from_uint(val);
        default: // a pointer
            // we return just the value of the pointer as an integer
            return mp_obj_new_int_from_uint(val);
    }
}

#endif

#if MICROPY_EMIT_NATIVE

mp_obj_dict_t *mp_native_swap_globals(mp_obj_dict_t *new_globals) {
    if (new_globals == NULL) {
        // Globals were the originally the same so don't restore them
        return NULL;
    }
    mp_obj_dict_t *old_globals = mp_globals_get();
    if (old_globals == new_globals) {
        // Don't set globals if they are the same, and return NULL to indicate this
        return NULL;
    }
    mp_globals_set(new_globals);
    return old_globals;
}

// wrapper that accepts n_args and n_kw in one argument
// (native emitter can only pass at most 3 arguments to a function)
mp_obj_t mp_native_call_function_n_kw(mp_obj_t fun_in, size_t n_args_kw, const mp_obj_t *args) {
    return mp_call_function_n_kw(fun_in, n_args_kw & 0xff, (n_args_kw >> 8) & 0xff, args);
}

// wrapper that makes raise obj and raises it
// END_FINALLY opcode requires that we don't raise if o==None
void mp_native_raise(mp_obj_t o) {
    if (o != MP_OBJ_NULL && o != mp_const_none) {
        nlr_raise(mp_make_raise_obj(o));
    }
}

// wrapper that handles iterator buffer
STATIC mp_obj_t mp_native_getiter(mp_obj_t obj, mp_obj_iter_buf_t *iter) {
    if (iter == NULL) {
        return mp_getiter(obj, NULL);
    } else {
        obj = mp_getiter(obj, iter);
        if (obj != MP_OBJ_FROM_PTR(iter)) {
            // Iterator didn't use the stack so indicate that with MP_OBJ_NULL.
            iter->base.type = MP_OBJ_NULL;
            iter->buf[0] = obj;
        }
        return NULL;
    }
}

// wrapper that handles iterator buffer
STATIC mp_obj_t mp_native_iternext(mp_obj_iter_buf_t *iter) {
    mp_obj_t obj;
    if (iter->base.type == MP_OBJ_NULL) {
        obj = iter->buf[0];
    } else {
        obj = MP_OBJ_FROM_PTR(iter);
    }
    return mp_iternext(obj);
}

STATIC bool mp_native_yield_from(mp_obj_t gen, mp_obj_t send_value, mp_obj_t *ret_value) {
    mp_vm_return_kind_t ret_kind;
    nlr_buf_t nlr_buf;
    mp_obj_t throw_value = *ret_value;
    if (nlr_push(&nlr_buf) == 0) {
        if (throw_value != MP_OBJ_NULL) {
            send_value = MP_OBJ_NULL;
        }
        ret_kind = mp_resume(gen, send_value, throw_value, ret_value);
        nlr_pop();
    } else {
        ret_kind = MP_VM_RETURN_EXCEPTION;
        *ret_value = nlr_buf.ret_val;
    }

    if (ret_kind == MP_VM_RETURN_YIELD) {
        return true;
    } else if (ret_kind == MP_VM_RETURN_NORMAL) {
        if (*ret_value == MP_OBJ_STOP_ITERATION) {
            *ret_value = mp_const_none;
        }
    } else {
        assert(ret_kind == MP_VM_RETURN_EXCEPTION);
        if (!mp_obj_exception_match(*ret_value, MP_OBJ_FROM_PTR(&mp_type_StopIteration))) {
            nlr_raise(*ret_value);
        }
        *ret_value = mp_obj_exception_get_value(*ret_value);
    }

    if (throw_value != MP_OBJ_NULL && mp_obj_exception_match(throw_value, MP_OBJ_FROM_PTR(&mp_type_GeneratorExit))) {
        nlr_raise(mp_make_raise_obj(throw_value));
    }

    return false;
}

// these must correspond to the respective enum in runtime0.h
const void *const mp_fun_table[MP_F_NUMBER_OF] = {
    &mp_const_none_obj,
    &mp_const_false_obj,
    &mp_const_true_obj,
    mp_convert_obj_to_native,
    mp_convert_native_to_obj,
    mp_native_swap_globals,
    mp_load_name,
    mp_load_global,
    mp_load_build_class,
    mp_load_attr,
    mp_load_method,
    mp_load_super_method,
    mp_store_name,
    mp_store_global,
    mp_store_attr,
    mp_obj_subscr,
    mp_obj_is_true,
    mp_unary_op,
    mp_binary_op,
    mp_obj_new_tuple,
    mp_obj_new_list,
    mp_obj_list_append,
    mp_obj_new_dict,
    mp_obj_dict_store,
#if MICROPY_PY_BUILTINS_SET
    mp_obj_set_store,
    mp_obj_new_set,
#endif
    mp_make_function_from_raw_code,
    mp_native_call_function_n_kw,
    mp_call_method_n_kw,
    mp_call_method_n_kw_var,
    mp_native_getiter,
    mp_native_iternext,
    nlr_push,
    nlr_pop,
    mp_native_raise,
    mp_import_name,
    mp_import_from,
    mp_import_all,
#if MICROPY_PY_BUILTINS_SLICE
    mp_obj_new_slice,
#endif
    mp_unpack_sequence,
    mp_unpack_ex,
    mp_delete_name,
    mp_delete_global,
    mp_obj_new_cell,
    mp_make_closure_from_raw_code,
    mp_arg_check_num_sig,
    mp_setup_code_state,
    mp_small_int_floor_divide,
    mp_small_int_modulo,
    mp_native_yield_from,
};

/*
void mp_f_vector(mp_fun_kind_t fun_kind) {
    (mp_f_table[fun_kind])();
}
*/

#endif // MICROPY_EMIT_NATIVE
