/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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
#ifndef MICROPY_INCLUDED_PY_DYNRUNTIME_H
#define MICROPY_INCLUDED_PY_DYNRUNTIME_H

// This header file contains definitions to dynamically implement the static
// MicroPython runtime API defined in py/obj.h and py/runtime.h.

#include "py/nativeglue.h"
#include "py/objfun.h"
#include "py/objstr.h"
#include "py/objtype.h"

#if !MICROPY_ENABLE_DYNRUNTIME
#error "dynruntime.h included in non-dynamic-module build."
#endif

#undef MP_ROM_QSTR
#undef MP_OBJ_QSTR_VALUE
#undef MP_OBJ_NEW_QSTR
#undef mp_const_none
#undef mp_const_false
#undef mp_const_true
#undef mp_const_empty_bytes
#undef mp_const_empty_tuple
#undef nlr_raise

/******************************************************************************/
// Memory allocation

#define m_malloc(n)                     (m_malloc_dyn((n)))
#define m_free(ptr)                     (m_free_dyn((ptr)))
#define m_realloc(ptr, new_num_bytes)   (m_realloc_dyn((ptr), (new_num_bytes)))

static inline void *m_malloc_dyn(size_t n) {
    // TODO won't raise on OOM
    return mp_fun_table.realloc_(NULL, n, false);
}

static inline void m_free_dyn(void *ptr) {
    mp_fun_table.realloc_(ptr, 0, false);
}

static inline void *m_realloc_dyn(void *ptr, size_t new_num_bytes) {
    // TODO won't raise on OOM
    return mp_fun_table.realloc_(ptr, new_num_bytes, true);
}

/******************************************************************************/
// Printing

#define mp_plat_print               (*mp_fun_table.plat_print)
#define mp_printf(p, ...)           (mp_fun_table.printf_((p), __VA_ARGS__))
#define mp_vprintf(p, fmt, args)    (mp_fun_table.vprintf_((p), (fmt), (args)))

/******************************************************************************/
// Types and objects

#define MP_OBJ_NEW_QSTR(x) MP_OBJ_NEW_QSTR_##x

#define mp_type_type                        (*mp_fun_table.type_type)
#define mp_type_NoneType                    (*mp_obj_get_type(mp_const_none))
#define mp_type_bool                        (*mp_obj_get_type(mp_const_false))
#define mp_type_int                         (*(mp_obj_type_t *)(mp_load_global(MP_QSTR_int)))
#define mp_type_str                         (*mp_fun_table.type_str)
#define mp_type_bytes                       (*(mp_obj_type_t *)(mp_load_global(MP_QSTR_bytes)))
#define mp_type_tuple                       (*((mp_obj_base_t *)mp_const_empty_tuple)->type)
#define mp_type_list                        (*mp_fun_table.type_list)
#define mp_type_EOFError                    (*(mp_obj_type_t *)(mp_load_global(MP_QSTR_EOFError)))
#define mp_type_IndexError                  (*(mp_obj_type_t *)(mp_load_global(MP_QSTR_IndexError)))
#define mp_type_KeyError                    (*(mp_obj_type_t *)(mp_load_global(MP_QSTR_KeyError)))
#define mp_type_NotImplementedError         (*(mp_obj_type_t *)(mp_load_global(MP_QSTR_NotImplementedError)))
#define mp_type_RuntimeError                (*(mp_obj_type_t *)(mp_load_global(MP_QSTR_RuntimeError)))
#define mp_type_TypeError                   (*(mp_obj_type_t *)(mp_load_global(MP_QSTR_TypeError)))
#define mp_type_ValueError                  (*(mp_obj_type_t *)(mp_load_global(MP_QSTR_ValueError)))

#define mp_stream_read_obj                  (*mp_fun_table.stream_read_obj)
#define mp_stream_readinto_obj              (*mp_fun_table.stream_readinto_obj)
#define mp_stream_unbuffered_readline_obj   (*mp_fun_table.stream_unbuffered_readline_obj)
#define mp_stream_write_obj                 (*mp_fun_table.stream_write_obj)

#define mp_const_none                       ((mp_obj_t)mp_fun_table.const_none)
#define mp_const_false                      ((mp_obj_t)mp_fun_table.const_false)
#define mp_const_true                       ((mp_obj_t)mp_fun_table.const_true)
#define mp_const_empty_bytes                (MP_OBJ_TYPE_GET_SLOT(&mp_type_bytes, make_new)(NULL, 0, 0, NULL))
#define mp_const_empty_tuple                (mp_fun_table.new_tuple(0, NULL))

#define mp_obj_new_bool(b)                  ((b) ? (mp_obj_t)mp_fun_table.const_true : (mp_obj_t)mp_fun_table.const_false)
#define mp_obj_new_int(i)                   (mp_fun_table.native_to_obj(i, MP_NATIVE_TYPE_INT))
#define mp_obj_new_int_from_uint(i)         (mp_fun_table.native_to_obj(i, MP_NATIVE_TYPE_UINT))
#define mp_obj_new_str(data, len)           (mp_fun_table.obj_new_str((data), (len)))
#define mp_obj_new_str_of_type(t, d, l)     (mp_obj_new_str_of_type_dyn((t), (d), (l)))
#define mp_obj_new_bytes(data, len)         (mp_fun_table.obj_new_bytes((data), (len)))
#define mp_obj_new_bytearray_by_ref(n, i)   (mp_fun_table.obj_new_bytearray_by_ref((n), (i)))
#define mp_obj_new_tuple(n, items)          (mp_fun_table.new_tuple((n), (items)))
#define mp_obj_new_list(n, items)           (mp_fun_table.new_list((n), (items)))
#define mp_obj_new_dict(n)                  (mp_fun_table.new_dict((n)))

#define mp_obj_get_type(o)                  (mp_fun_table.obj_get_type((o)))
#define mp_obj_cast_to_native_base(o, t)    (mp_obj_cast_to_native_base_dyn((o), (t)))
#define mp_obj_get_int(o)                   (mp_fun_table.native_from_obj(o, MP_NATIVE_TYPE_INT))
#define mp_obj_get_int_truncated(o)         (mp_fun_table.native_from_obj(o, MP_NATIVE_TYPE_UINT))
#define mp_obj_str_get_str(s)               (mp_obj_str_get_data_dyn((s), NULL))
#define mp_obj_str_get_data(o, len)         (mp_obj_str_get_data_dyn((o), (len)))
#define mp_get_buffer_raise(o, bufinfo, fl) (mp_fun_table.get_buffer_raise((o), (bufinfo), (fl)))
#define mp_get_stream_raise(s, flags)       (mp_fun_table.get_stream_raise((s), (flags)))
#define mp_obj_is_true(o)                   (mp_fun_table.native_from_obj(o, MP_NATIVE_TYPE_BOOL))

#define mp_obj_len(o)                       (mp_obj_len_dyn(o))
#define mp_obj_subscr(base, index, val)     (mp_fun_table.obj_subscr((base), (index), (val)))
#define mp_obj_get_array(o, len, items)     (mp_obj_get_array_dyn((o), (len), (items)))
#define mp_obj_list_append(list, item)      (mp_fun_table.list_append((list), (item)))
#define mp_obj_dict_store(dict, key, val)   (mp_fun_table.dict_store((dict), (key), (val)))

#define mp_obj_malloc_helper(n, t)          (mp_obj_malloc_helper_dyn(n, t))

static inline mp_obj_t mp_obj_new_str_of_type_dyn(const mp_obj_type_t *type, const byte *data, size_t len) {
    if (type == &mp_type_str) {
        return mp_obj_new_str((const char *)data, len);
    } else {
        return mp_obj_new_bytes(data, len);
    }
}

static inline mp_obj_t mp_obj_cast_to_native_base_dyn(mp_obj_t self_in, mp_const_obj_t native_type) {
    const mp_obj_type_t *self_type = mp_obj_get_type(self_in);

    if (MP_OBJ_FROM_PTR(self_type) == native_type) {
        return self_in;
    } else if (MP_OBJ_TYPE_GET_SLOT_OR_NULL(self_type, parent) != native_type) {
        // The self_in object is not a direct descendant of native_type, so fail the cast.
        // This is a very simple version of mp_obj_is_subclass_fast that could be improved.
        return MP_OBJ_NULL;
    } else {
        mp_obj_instance_t *self = (mp_obj_instance_t *)MP_OBJ_TO_PTR(self_in);
        return self->subobj[0];
    }
}

static inline void *mp_obj_str_get_data_dyn(mp_obj_t o, size_t *l) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(o, &bufinfo, MP_BUFFER_READ);
    if (l != NULL) {
        *l = bufinfo.len;
    }
    return bufinfo.buf;
}

static inline mp_obj_t mp_obj_len_dyn(mp_obj_t o) {
    // If bytes implemented MP_UNARY_OP_LEN could use: mp_unary_op(MP_UNARY_OP_LEN, o)
    return mp_fun_table.call_function_n_kw(mp_fun_table.load_name(MP_QSTR_len), 1, &o);
}

static inline void *mp_obj_malloc_helper_dyn(size_t num_bytes, const mp_obj_type_t *type) {
    mp_obj_base_t *base = (mp_obj_base_t *)m_malloc(num_bytes);
    base->type = type;
    return base;
}

/******************************************************************************/
// General runtime functions

#define mp_load_name(qst)                 (mp_fun_table.load_name((qst)))
#define mp_load_global(qst)               (mp_fun_table.load_global((qst)))
#define mp_load_attr(base, attr)          (mp_fun_table.load_attr((base), (attr)))
#define mp_load_method(base, attr, dest)  (mp_fun_table.load_method((base), (attr), (dest)))
#define mp_load_super_method(attr, dest)  (mp_fun_table.load_super_method((attr), (dest)))
#define mp_store_name(qst, obj)           (mp_fun_table.store_name((qst), (obj)))
#define mp_store_global(qst, obj)         (mp_fun_table.store_global((qst), (obj)))
#define mp_store_attr(base, attr, val)    (mp_fun_table.store_attr((base), (attr), (val)))

#define mp_unary_op(op, obj)        (mp_fun_table.unary_op((op), (obj)))
#define mp_binary_op(op, lhs, rhs)  (mp_fun_table.binary_op((op), (lhs), (rhs)))

#define mp_make_function_from_raw_code(rc, context, def_args) \
    (mp_fun_table.make_function_from_raw_code((rc), (context), (def_args)))

#define mp_call_function_n_kw(fun, n_args, n_kw, args) \
    (mp_fun_table.call_function_n_kw((fun), (n_args) | ((n_kw) << 8), args))

#define mp_arg_check_num(n_args, n_kw, n_args_min, n_args_max, takes_kw) \
    (mp_fun_table.arg_check_num_sig((n_args), (n_kw), MP_OBJ_FUN_MAKE_SIG((n_args_min), (n_args_max), (takes_kw))))

#define MP_DYNRUNTIME_INIT_ENTRY \
    mp_obj_t old_globals = mp_fun_table.swap_globals(self->context->module.globals); \
    mp_raw_code_t rc; \
    rc.kind = MP_CODE_NATIVE_VIPER; \
    rc.scope_flags = 0; \
    (void)rc;

#define MP_DYNRUNTIME_INIT_EXIT \
    mp_fun_table.swap_globals(old_globals); \
    return mp_const_none;

#define MP_DYNRUNTIME_MAKE_FUNCTION(f) \
    (mp_make_function_from_raw_code((rc.fun_data = (f), &rc), self->context, NULL))

#define mp_import_name(name, fromlist, level) \
    (mp_fun_table.import_name((name), (fromlist), (level)))
#define mp_import_from(module, name) \
    (mp_fun_table.import_from((module), (name)))
#define mp_import_all(module) \
    (mp_fun_table.import_all((module))

/******************************************************************************/
// Exceptions

#define mp_obj_new_exception(o)                 ((mp_obj_t)(o)) // Assumes returned object will be raised, will create instance then
#define mp_obj_new_exception_arg1(e_type, arg)  (mp_obj_new_exception_arg1_dyn((e_type), (arg)))

#define nlr_raise(o)                            (mp_raise_dyn(o))
#define mp_raise_type_arg(type, arg)            (mp_raise_dyn(mp_obj_new_exception_arg1_dyn((type), (arg))))
#define mp_raise_msg(type, msg)                 (mp_fun_table.raise_msg((type), (msg)))
#define mp_raise_OSError(er)                    (mp_raise_OSError_dyn(er))
#define mp_raise_NotImplementedError(msg)       (mp_raise_msg(&mp_type_NotImplementedError, (msg)))
#define mp_raise_TypeError(msg)                 (mp_raise_msg(&mp_type_TypeError, (msg)))
#define mp_raise_ValueError(msg)                (mp_raise_msg(&mp_type_ValueError, (msg)))

static inline mp_obj_t mp_obj_new_exception_arg1_dyn(const mp_obj_type_t *exc_type, mp_obj_t arg) {
    mp_obj_t args[1] = { arg };
    return mp_call_function_n_kw(MP_OBJ_FROM_PTR(exc_type), 1, 0, &args[0]);
}

static NORETURN inline void mp_raise_dyn(mp_obj_t o) {
    mp_fun_table.raise(o);
    for (;;) {
    }
}

static inline void mp_raise_OSError_dyn(int er) {
    mp_obj_t args[1] = { MP_OBJ_NEW_SMALL_INT(er) };
    nlr_raise(mp_call_function_n_kw(mp_load_global(MP_QSTR_OSError), 1, 0, &args[0]));
}

/******************************************************************************/
// Floating point

#define mp_obj_new_float_from_f(f)  (mp_fun_table.obj_new_float_from_f((f)))
#define mp_obj_new_float_from_d(d)  (mp_fun_table.obj_new_float_from_d((d)))
#define mp_obj_get_float_to_f(o)    (mp_fun_table.obj_get_float_to_f((o)))
#define mp_obj_get_float_to_d(o)    (mp_fun_table.obj_get_float_to_d((o)))

#if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
#define mp_obj_new_float(f)         (mp_obj_new_float_from_f((f)))
#define mp_obj_get_float(o)         (mp_obj_get_float_to_f((o)))
#elif MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE
#define mp_obj_new_float(f)         (mp_obj_new_float_from_d((f)))
#define mp_obj_get_float(o)         (mp_obj_get_float_to_d((o)))
#endif

/******************************************************************************/
// Inline function definitions.

// *items may point inside a GC block
static inline void mp_obj_get_array_dyn(mp_obj_t o, size_t *len, mp_obj_t **items) {
    const mp_obj_type_t *type = mp_obj_get_type(o);
    if (type == &mp_type_tuple) {
        mp_obj_tuple_t *t = MP_OBJ_TO_PTR(o);
        *len = t->len;
        *items = &t->items[0];
    } else if (type == &mp_type_list) {
        mp_obj_list_t *l = MP_OBJ_TO_PTR(o);
        *len = l->len;
        *items = l->items;
    } else {
        mp_raise_TypeError("expected tuple/list");
    }
}

#endif // MICROPY_INCLUDED_PY_DYNRUNTIME_H
