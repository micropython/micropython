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

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "py/runtime.h"
#include "py/smallint.h"
#include "py/nativeglue.h"
#include "py/gc.h"

#if MICROPY_DEBUG_VERBOSE // print debugging info
#define DEBUG_printf DEBUG_printf
#else // don't print debugging info
#define DEBUG_printf(...) (void)0
#endif

#if MICROPY_EMIT_NATIVE

int mp_native_type_from_qstr(qstr qst) {
    switch (qst) {
        case MP_QSTR_object:
            return MP_NATIVE_TYPE_OBJ;
        case MP_QSTR_bool:
            return MP_NATIVE_TYPE_BOOL;
        case MP_QSTR_int:
            return MP_NATIVE_TYPE_INT;
        case MP_QSTR_uint:
            return MP_NATIVE_TYPE_UINT;
        case MP_QSTR_ptr:
            return MP_NATIVE_TYPE_PTR;
        case MP_QSTR_ptr8:
            return MP_NATIVE_TYPE_PTR8;
        case MP_QSTR_ptr16:
            return MP_NATIVE_TYPE_PTR16;
        case MP_QSTR_ptr32:
            return MP_NATIVE_TYPE_PTR32;
        default:
            return -1;
    }
}

// convert a MicroPython object to a valid native value based on type
mp_uint_t mp_native_from_obj(mp_obj_t obj, mp_uint_t type) {
    DEBUG_printf("mp_native_from_obj(%p, " UINT_FMT ")\n", obj, type);
    switch (type & 0xf) {
        case MP_NATIVE_TYPE_OBJ:
            return (mp_uint_t)obj;
        case MP_NATIVE_TYPE_BOOL:
            return mp_obj_is_true(obj);
        case MP_NATIVE_TYPE_INT:
        case MP_NATIVE_TYPE_UINT:
            return mp_obj_get_int_truncated(obj);
        default: { // cast obj to a pointer
            mp_buffer_info_t bufinfo;
            if (mp_get_buffer(obj, &bufinfo, MP_BUFFER_READ)) {
                return (mp_uint_t)bufinfo.buf;
            } else {
                // assume obj is an integer that represents an address
                return mp_obj_get_int_truncated(obj);
            }
        }
    }
}

#endif

#if MICROPY_EMIT_MACHINE_CODE

// convert a native value to a MicroPython object based on type
mp_obj_t mp_native_to_obj(mp_uint_t val, mp_uint_t type) {
    DEBUG_printf("mp_native_to_obj(" UINT_FMT ", " UINT_FMT ")\n", val, type);
    switch (type & 0xf) {
        case MP_NATIVE_TYPE_OBJ:
            return (mp_obj_t)val;
        case MP_NATIVE_TYPE_BOOL:
            return mp_obj_new_bool(val);
        case MP_NATIVE_TYPE_INT:
            return mp_obj_new_int(val);
        case MP_NATIVE_TYPE_UINT:
            return mp_obj_new_int_from_uint(val);
        default: // a pointer
            // we return just the value of the pointer as an integer
            return mp_obj_new_int_from_uint(val);
    }
}

#endif

#if MICROPY_EMIT_NATIVE && !MICROPY_DYNAMIC_COMPILER

#if !MICROPY_PY_BUILTINS_SET
mp_obj_t mp_obj_new_set(size_t n_args, mp_obj_t *items) {
    (void)n_args;
    (void)items;
    mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("set unsupported"));
}

void mp_obj_set_store(mp_obj_t self_in, mp_obj_t item) {
    (void)self_in;
    (void)item;
    mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("set unsupported"));
}
#endif

#if !MICROPY_PY_BUILTINS_SLICE
mp_obj_t mp_obj_new_slice(mp_obj_t ostart, mp_obj_t ostop, mp_obj_t ostep) {
    (void)ostart;
    (void)ostop;
    (void)ostep;
    mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("slice unsupported"));
}
#endif

STATIC mp_obj_dict_t *mp_native_swap_globals(mp_obj_dict_t *new_globals) {
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
STATIC mp_obj_t mp_native_call_function_n_kw(mp_obj_t fun_in, size_t n_args_kw, const mp_obj_t *args) {
    return mp_call_function_n_kw(fun_in, n_args_kw & 0xff, (n_args_kw >> 8) & 0xff, args);
}

// wrapper that makes raise obj and raises it
// END_FINALLY opcode requires that we don't raise if o==None
STATIC void mp_native_raise(mp_obj_t o) {
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

#if !MICROPY_PY_BUILTINS_FLOAT

STATIC mp_obj_t mp_obj_new_float_from_f(float f) {
    (void)f;
    mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("float unsupported"));
}

STATIC mp_obj_t mp_obj_new_float_from_d(double d) {
    (void)d;
    mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("float unsupported"));
}

STATIC float mp_obj_get_float_to_f(mp_obj_t o) {
    (void)o;
    mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("float unsupported"));
}

STATIC double mp_obj_get_float_to_d(mp_obj_t o) {
    (void)o;
    mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("float unsupported"));
}

#endif

// these must correspond to the respective enum in nativeglue.h
const mp_fun_table_t mp_fun_table = {
    mp_const_none,
    mp_const_false,
    mp_const_true,
    mp_native_from_obj,
    mp_native_to_obj,
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
    mp_obj_new_dict,
    mp_obj_new_set,
    mp_obj_set_store,
    mp_obj_list_append,
    mp_obj_dict_store,
    mp_make_function_from_raw_code,
    mp_native_call_function_n_kw,
    mp_call_method_n_kw,
    mp_call_method_n_kw_var,
    mp_native_getiter,
    mp_native_iternext,
    #if MICROPY_NLR_SETJMP
    nlr_push_tail,
    #else
    nlr_push,
    #endif
    nlr_pop,
    mp_native_raise,
    mp_import_name,
    mp_import_from,
    mp_import_all,
    mp_obj_new_slice,
    mp_unpack_sequence,
    mp_unpack_ex,
    mp_delete_name,
    mp_delete_global,
    mp_obj_new_closure,
    mp_arg_check_num_sig,
    mp_setup_code_state,
    mp_small_int_floor_divide,
    mp_small_int_modulo,
    mp_native_yield_from,
    #if MICROPY_NLR_SETJMP
    setjmp,
    #else
    NULL,
    #endif
    // Additional entries for dynamic runtime, starts at index 50
    memset,
    memmove,
    gc_realloc,
    mp_printf,
    mp_vprintf,
    mp_raise_msg,
    mp_obj_get_type,
    mp_obj_new_str,
    mp_obj_new_bytes,
    mp_obj_new_bytearray_by_ref,
    mp_obj_new_float_from_f,
    mp_obj_new_float_from_d,
    mp_obj_get_float_to_f,
    mp_obj_get_float_to_d,
    mp_get_buffer_raise,
    mp_get_stream_raise,
    &mp_plat_print,
    &mp_type_type,
    &mp_type_str,
    &mp_type_list,
    &mp_type_dict,
    &mp_type_fun_builtin_0,
    &mp_type_fun_builtin_1,
    &mp_type_fun_builtin_2,
    &mp_type_fun_builtin_3,
    &mp_type_fun_builtin_var,
    &mp_stream_read_obj,
    &mp_stream_readinto_obj,
    &mp_stream_unbuffered_readline_obj,
    &mp_stream_write_obj,
};

#elif MICROPY_EMIT_NATIVE && MICROPY_DYNAMIC_COMPILER

const int mp_fun_table;

#endif // MICROPY_EMIT_NATIVE
