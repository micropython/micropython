/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2019 Damien P. George
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
#ifndef MICROPY_INCLUDED_PY_NATIVEGLUE_H
#define MICROPY_INCLUDED_PY_NATIVEGLUE_H

#include <stdarg.h>
#include "py/obj.h"
#include "py/persistentcode.h"
#include "py/stream.h"

typedef enum {
    MP_F_CONST_NONE_OBJ = 0,
    MP_F_CONST_FALSE_OBJ,
    MP_F_CONST_TRUE_OBJ,
    MP_F_CONVERT_OBJ_TO_NATIVE,
    MP_F_CONVERT_NATIVE_TO_OBJ,
    MP_F_NATIVE_SWAP_GLOBALS,
    MP_F_LOAD_NAME,
    MP_F_LOAD_GLOBAL,
    MP_F_LOAD_BUILD_CLASS,
    MP_F_LOAD_ATTR,
    MP_F_LOAD_METHOD,
    MP_F_LOAD_SUPER_METHOD,
    MP_F_STORE_NAME,
    MP_F_STORE_GLOBAL,
    MP_F_STORE_ATTR,
    MP_F_OBJ_SUBSCR,
    MP_F_OBJ_IS_TRUE,
    MP_F_UNARY_OP,
    MP_F_BINARY_OP,
    MP_F_BUILD_TUPLE,
    MP_F_BUILD_LIST,
    MP_F_BUILD_MAP,
    MP_F_BUILD_SET,
    MP_F_STORE_SET,
    MP_F_LIST_APPEND,
    MP_F_STORE_MAP,
    MP_F_MAKE_FUNCTION_FROM_RAW_CODE,
    MP_F_NATIVE_CALL_FUNCTION_N_KW,
    MP_F_CALL_METHOD_N_KW,
    MP_F_CALL_METHOD_N_KW_VAR,
    MP_F_NATIVE_GETITER,
    MP_F_NATIVE_ITERNEXT,
    MP_F_NLR_PUSH,
    MP_F_NLR_POP,
    MP_F_NATIVE_RAISE,
    MP_F_IMPORT_NAME,
    MP_F_IMPORT_FROM,
    MP_F_IMPORT_ALL,
    MP_F_NEW_SLICE,
    MP_F_UNPACK_SEQUENCE,
    MP_F_UNPACK_EX,
    MP_F_DELETE_NAME,
    MP_F_DELETE_GLOBAL,
    MP_F_MAKE_CLOSURE_FROM_RAW_CODE,
    MP_F_ARG_CHECK_NUM_SIG,
    MP_F_SETUP_CODE_STATE,
    MP_F_SMALL_INT_FLOOR_DIVIDE,
    MP_F_SMALL_INT_MODULO,
    MP_F_NATIVE_YIELD_FROM,
    MP_F_SETJMP,
    MP_F_NUMBER_OF,
} mp_fun_kind_t;

typedef struct _mp_fun_table_t {
    mp_const_obj_t const_none;
    mp_const_obj_t const_false;
    mp_const_obj_t const_true;
    mp_uint_t (*native_from_obj)(mp_obj_t obj, mp_uint_t type);
    mp_obj_t (*native_to_obj)(mp_uint_t val, mp_uint_t type);
    mp_obj_dict_t *(*swap_globals)(mp_obj_dict_t * new_globals);
    mp_obj_t (*load_name)(qstr qst);
    mp_obj_t (*load_global)(qstr qst);
    mp_obj_t (*load_build_class)(void);
    mp_obj_t (*load_attr)(mp_obj_t base, qstr attr);
    void (*load_method)(mp_obj_t base, qstr attr, mp_obj_t *dest);
    void (*load_super_method)(qstr attr, mp_obj_t *dest);
    void (*store_name)(qstr qst, mp_obj_t obj);
    void (*store_global)(qstr qst, mp_obj_t obj);
    void (*store_attr)(mp_obj_t base, qstr attr, mp_obj_t val);
    mp_obj_t (*obj_subscr)(mp_obj_t base, mp_obj_t index, mp_obj_t val);
    bool (*obj_is_true)(mp_obj_t arg);
    mp_obj_t (*unary_op)(mp_unary_op_t op, mp_obj_t arg);
    mp_obj_t (*binary_op)(mp_binary_op_t op, mp_obj_t lhs, mp_obj_t rhs);
    mp_obj_t (*new_tuple)(size_t n, const mp_obj_t *items);
    mp_obj_t (*new_list)(size_t n, mp_obj_t *items);
    mp_obj_t (*new_dict)(size_t n_args);
    mp_obj_t (*new_set)(size_t n_args, mp_obj_t *items);
    void (*set_store)(mp_obj_t self_in, mp_obj_t item);
    mp_obj_t (*list_append)(mp_obj_t self_in, mp_obj_t arg);
    mp_obj_t (*dict_store)(mp_obj_t self_in, mp_obj_t key, mp_obj_t value);
    mp_obj_t (*make_function_from_raw_code)(const mp_raw_code_t *rc, mp_obj_t def_args, mp_obj_t def_kw_args);
    mp_obj_t (*call_function_n_kw)(mp_obj_t fun_in, size_t n_args_kw, const mp_obj_t *args);
    mp_obj_t (*call_method_n_kw)(size_t n_args, size_t n_kw, const mp_obj_t *args);
    mp_obj_t (*call_method_n_kw_var)(bool have_self, size_t n_args_n_kw, const mp_obj_t *args);
    mp_obj_t (*getiter)(mp_obj_t obj, mp_obj_iter_buf_t *iter);
    mp_obj_t (*iternext)(mp_obj_iter_buf_t *iter);
    unsigned int (*nlr_push)(nlr_buf_t *);
    void (*nlr_pop)(void);
    void (*raise)(mp_obj_t o);
    mp_obj_t (*import_name)(qstr name, mp_obj_t fromlist, mp_obj_t level);
    mp_obj_t (*import_from)(mp_obj_t module, qstr name);
    void (*import_all)(mp_obj_t module);
    mp_obj_t (*new_slice)(mp_obj_t start, mp_obj_t stop, mp_obj_t step);
    void (*unpack_sequence)(mp_obj_t seq, size_t num, mp_obj_t *items);
    void (*unpack_ex)(mp_obj_t seq, size_t num, mp_obj_t *items);
    void (*delete_name)(qstr qst);
    void (*delete_global)(qstr qst);
    mp_obj_t (*make_closure_from_raw_code)(const mp_raw_code_t *rc, mp_uint_t n_closed_over, const mp_obj_t *args);
    void (*arg_check_num_sig)(size_t n_args, size_t n_kw, uint32_t sig);
    void (*setup_code_state)(mp_code_state_t *code_state, size_t n_args, size_t n_kw, const mp_obj_t *args);
    mp_int_t (*small_int_floor_divide)(mp_int_t num, mp_int_t denom);
    mp_int_t (*small_int_modulo)(mp_int_t dividend, mp_int_t divisor);
    bool (*yield_from)(mp_obj_t gen, mp_obj_t send_value, mp_obj_t *ret_value);
    void *setjmp_;
    // Additional entries for dynamic runtime, starts at index 50
    void *(*memset_)(void *s, int c, size_t n);
    void *(*memmove_)(void *dest, const void *src, size_t n);
    void *(*realloc_)(void *ptr, size_t n_bytes, bool allow_move);
    int (*printf_)(const mp_print_t *print, const char *fmt, ...);
    int (*vprintf_)(const mp_print_t *print, const char *fmt, va_list args);
    #if defined(__GNUC__)
    NORETURN // Only certain compilers support no-return attributes in function pointer declarations
    #endif
    void (*raise_msg)(const mp_obj_type_t *exc_type, mp_rom_error_text_t msg);
    const mp_obj_type_t *(*obj_get_type)(mp_const_obj_t o_in);
    mp_obj_t (*obj_new_str)(const char *data, size_t len);
    mp_obj_t (*obj_new_bytes)(const byte *data, size_t len);
    mp_obj_t (*obj_new_bytearray_by_ref)(size_t n, void *items);
    mp_obj_t (*obj_new_float_from_f)(float f);
    mp_obj_t (*obj_new_float_from_d)(double d);
    float (*obj_get_float_to_f)(mp_obj_t o);
    double (*obj_get_float_to_d)(mp_obj_t o);
    void (*get_buffer_raise)(mp_obj_t obj, mp_buffer_info_t *bufinfo, mp_uint_t flags);
    const mp_stream_p_t *(*get_stream_raise)(mp_obj_t self_in, int flags);
    const mp_print_t *plat_print;
    const mp_obj_type_t *type_type;
    const mp_obj_type_t *type_str;
    const mp_obj_type_t *type_list;
    const mp_obj_type_t *type_dict;
    const mp_obj_type_t *type_fun_builtin_0;
    const mp_obj_type_t *type_fun_builtin_1;
    const mp_obj_type_t *type_fun_builtin_2;
    const mp_obj_type_t *type_fun_builtin_3;
    const mp_obj_type_t *type_fun_builtin_var;
    const mp_obj_fun_builtin_var_t *stream_read_obj;
    const mp_obj_fun_builtin_var_t *stream_readinto_obj;
    const mp_obj_fun_builtin_var_t *stream_unbuffered_readline_obj;
    const mp_obj_fun_builtin_var_t *stream_write_obj;
} mp_fun_table_t;

extern const mp_fun_table_t mp_fun_table;

#endif // MICROPY_INCLUDED_PY_NATIVEGLUE_H
