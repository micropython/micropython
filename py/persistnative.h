/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Damien P. George
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
#ifndef __MICROPY_INCLUDED_PY_PERSISTNATIVE_H__
#define __MICROPY_INCLUDED_PY_PERSISTNATIVE_H__

#include "py/mpconfig.h"
#include "py/runtime0.h"
#include "py/runtime.h"
#include "py/emitglue.h"

// Macros to make it easy to write persistent native code.  The intention is
// that these macros can be redefined if such code is to be compiled statically
// into the executable.
#define CONTEXT_ALONE mp_persistent_native_data_t *pnd
#define CONTEXT mp_persistent_native_data_t *pnd,
#define QSTR(id) (pnd->qstr_table[MP_LOCAL_QSTR_ ## id])
#define RT(id) ((mp_fun_table_t*)pnd->fun_table)->id
#define CONST(id) ((mp_obj_t)((mp_fun_table_t*)pnd->fun_table)->mp_const ## id ## _obj)
#define MAKE_FUN_0(fun_name) RT(mp_obj_new_fun_extern)(false, 0, 0, fun_name, pnd)
#define MAKE_FUN_1(fun_name) RT(mp_obj_new_fun_extern)(false, 1, 1, fun_name, pnd)
#define MAKE_FUN_2(fun_name) RT(mp_obj_new_fun_extern)(false, 2, 2, fun_name, pnd)
#define MAKE_FUN_3(fun_name) RT(mp_obj_new_fun_extern)(false, 3, 3, fun_name, pnd)
#define MAKE_FUN_VAR(mn, fun_name) RT(mp_obj_new_fun_extern)(false, mn, MP_OBJ_FUN_ARGS_MAX, fun_name, pnd)
#define MAKE_FUN_VAR_BETWEEN(mn, mx, fun_name) RT(mp_obj_new_fun_extern)(false, mn, MP_OBJ_FUN_ARGS_MAX, fun_name, pnd)
#define MAKE_FUN_KW(mn, fun_name) RT(mp_obj_new_fun_extern)(true, mn, MP_OBJ_FUN_ARGS_MAX, fun_name, pnd)

// The linker likes to align the text following this header, so we make
// the header a nice multiple of 8 to prevent padding being inserted.
// TODO: consolidate the first 4 bytes here with stuff in persistentcode.c
#define MP_PERSISTENT_NATIVE_HEADER \
    __attribute__((section(".mpyheader"))) \
    const byte header[16] = { \
        'M', \
        2, \
        ((MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE) << 0) \
            | ((MICROPY_PY_BUILTINS_STR_UNICODE) << 1), \
        16, \
        MP_CODE_PERSISTENT_NATIVE, \
        MP_PERSISTENT_ARCH_CURRENT, \
        (MP_LOCAL_QSTR_number_of & 0xff), (MP_LOCAL_QSTR_number_of >> 8), \
        0, 0, /* size of text section, to be patched later */ \
        0, 0, 0, 0, 0, 0, /* padding */ \
    };

#define MP_PERSISTENT_NATIVE_INIT \
    __attribute__((section(".mpytext")))

struct _mp_raw_code_t;
struct _mp_code_state;

typedef struct _mp_fun_table_t {
    mp_uint_t (*mp_convert_obj_to_native)(mp_obj_t obj, mp_uint_t type);
    mp_obj_t (*mp_convert_native_to_obj)(mp_uint_t val, mp_uint_t type);
    mp_obj_t (*mp_load_name)(qstr qst);
    mp_obj_t (*mp_load_global)(qstr qst);
    mp_obj_t (*mp_load_build_class)(void);
    mp_obj_t (*mp_load_attr)(mp_obj_t base, qstr attr);
    void (*mp_load_method)(mp_obj_t base, qstr attr, mp_obj_t *dest);
    void (*mp_load_super_method)(qstr attr, mp_obj_t *dest);
    void (*mp_store_name)(qstr qst, mp_obj_t obj);
    void (*mp_store_global)(qstr qst, mp_obj_t obj);
    void (*mp_store_attr)(mp_obj_t base, qstr attr, mp_obj_t value);
    mp_obj_t (*mp_obj_subscr)(mp_obj_t base, mp_obj_t index, mp_obj_t val);
    bool (*mp_obj_is_true)(mp_obj_t arg);
    mp_obj_t (*mp_unary_op)(mp_uint_t op, mp_obj_t arg);
    mp_obj_t (*mp_binary_op)(mp_uint_t op, mp_obj_t lhs, mp_obj_t rhs);
    mp_obj_t (*mp_obj_new_tuple)(mp_uint_t n, const mp_obj_t *items);
    mp_obj_t (*mp_obj_new_list)(mp_uint_t n, mp_obj_t *items);
    mp_obj_t (*mp_obj_list_append)(mp_obj_t self_in, mp_obj_t arg);
    mp_obj_t (*mp_obj_new_dict)(mp_uint_t n_args);
    mp_obj_t (*mp_obj_dict_store)(mp_obj_t self_in, mp_obj_t key, mp_obj_t value);
#if MICROPY_PY_BUILTINS_SET
    mp_obj_t (*mp_obj_new_set)(mp_uint_t n_args, mp_obj_t *items);
    void (*mp_obj_set_store)(mp_obj_t self_in, mp_obj_t item);
#endif
    mp_obj_t (*mp_make_function_from_raw_code)(struct _mp_raw_code_t *rc, mp_obj_t def_args, mp_obj_t def_kw_args);
    mp_obj_t (*mp_native_call_function_n_kw)(mp_obj_t fun_in, mp_uint_t n_args_kw, const mp_obj_t *args);
    mp_obj_t (*mp_call_method_n_kw)(mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args);
    mp_obj_t (*mp_call_method_n_kw_var)(bool have_self, mp_uint_t n_args_n_kw, const mp_obj_t *args);
    mp_obj_t (*mp_getiter)(mp_obj_t o_in);
    mp_obj_t (*mp_iternext)(mp_obj_t o_in);
    unsigned int (*nlr_push)(nlr_buf_t *);
    void (*nlr_pop)(void);
    void (*mp_native_raise)(mp_obj_t o);
    mp_obj_t (*mp_import_name)(qstr name, mp_obj_t fromlist, mp_obj_t level);
    mp_obj_t (*mp_import_from)(mp_obj_t module, qstr name);
    void (*mp_import_all)(mp_obj_t module);
#if MICROPY_PY_BUILTINS_SLICE
    mp_obj_t (*mp_obj_new_slice)(mp_obj_t start, mp_obj_t stop, mp_obj_t step);
#endif
    void (*mp_unpack_sequence)(mp_obj_t seq_in, mp_uint_t num, mp_obj_t *items);
    void (*mp_unpack_ex)(mp_obj_t seq_in, mp_uint_t num_in, mp_obj_t *items);
    void (*mp_delete_name)(qstr qst);
    void (*mp_delete_global)(qstr qst);
    mp_obj_t (*mp_obj_new_cell)(mp_obj_t obj);
    mp_obj_t (*mp_make_closure_from_raw_code)(struct _mp_raw_code_t *rc, mp_uint_t n_closed_over, const mp_obj_t *args);
    void (*mp_setup_code_state)(struct _mp_code_state *code_state, mp_obj_t self_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args);
    qstr (*qstr_from_str)(const char *str);
    qstr (*qstr_from_strn)(const char *str, mp_uint_t len);
    mp_obj_t (*mp_obj_new_fun_extern)(bool is_kw, mp_uint_t n_args_min, mp_uint_t n_args_max, void *f, void *per_nat_data);
    mp_const_obj_t mp_const_none_obj;
    mp_const_obj_t mp_const_false_obj;
    mp_const_obj_t mp_const_true_obj;
    mp_const_obj_t mp_const_ellipsis_obj;
    // probably need to add: malloc, realloc, mp_get_buffer
    // could add port specific functions here
} mp_fun_table_t;

#endif // __MICROPY_INCLUDED_PY_PERSISTNATIVE_H__
