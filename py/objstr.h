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
#ifndef MICROPY_INCLUDED_PY_OBJSTR_H
#define MICROPY_INCLUDED_PY_OBJSTR_H

#include "py/obj.h"

typedef struct _mp_obj_str_t {
    mp_obj_base_t base;
    mp_uint_t hash;
    // len == number of bytes used in data, alloc = len + 1 because (at the moment) we also append a null byte
    size_t len;
    const byte *data;
} mp_obj_str_t;

#define MP_DEFINE_STR_OBJ(obj_name, str) mp_obj_str_t obj_name = {{&mp_type_str}, 0, sizeof(str) - 1, (const byte*)str}

// use this macro to extract the string hash
// warning: the hash can be 0, meaning invalid, and must then be explicitly computed from the data
#define GET_STR_HASH(str_obj_in, str_hash) \
    mp_uint_t str_hash; if (MP_OBJ_IS_QSTR(str_obj_in)) \
    { str_hash = qstr_hash(MP_OBJ_QSTR_VALUE(str_obj_in)); } else { str_hash = ((mp_obj_str_t*)MP_OBJ_TO_PTR(str_obj_in))->hash; }

// use this macro to extract the string length
#define GET_STR_LEN(str_obj_in, str_len) \
    size_t str_len; if (MP_OBJ_IS_QSTR(str_obj_in)) \
    { str_len = qstr_len(MP_OBJ_QSTR_VALUE(str_obj_in)); } else { str_len = ((mp_obj_str_t*)MP_OBJ_TO_PTR(str_obj_in))->len; }

// use this macro to extract the string data and length
#if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_C
const byte *mp_obj_str_get_data_no_check(mp_obj_t self_in, size_t *len);
#define GET_STR_DATA_LEN(str_obj_in, str_data, str_len) \
    size_t str_len; const byte *str_data = mp_obj_str_get_data_no_check(str_obj_in, &str_len);
#else
#define GET_STR_DATA_LEN(str_obj_in, str_data, str_len) \
    const byte *str_data; size_t str_len; if (MP_OBJ_IS_QSTR(str_obj_in)) \
    { str_data = qstr_data(MP_OBJ_QSTR_VALUE(str_obj_in), &str_len); } \
    else { str_len = ((mp_obj_str_t*)MP_OBJ_TO_PTR(str_obj_in))->len; str_data = ((mp_obj_str_t*)MP_OBJ_TO_PTR(str_obj_in))->data; }
#endif

mp_obj_t mp_obj_str_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args);
void mp_str_print_json(const mp_print_t *print, const byte *str_data, size_t str_len);
mp_obj_t mp_obj_str_format(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs);
mp_obj_t mp_obj_str_split(size_t n_args, const mp_obj_t *args);
mp_obj_t mp_obj_new_str_of_type(const mp_obj_type_t *type, const byte* data, size_t len);

mp_obj_t mp_obj_str_binary_op(mp_uint_t op, mp_obj_t lhs_in, mp_obj_t rhs_in);
mp_int_t mp_obj_str_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags);

const byte *str_index_to_ptr(const mp_obj_type_t *type, const byte *self_data, size_t self_len,
                             mp_obj_t index, bool is_slice);
const byte *find_subbytes(const byte *haystack, size_t hlen, const byte *needle, size_t nlen, int direction);

MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(str_encode_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(str_find_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(str_rfind_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(str_index_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(str_rindex_obj);
MP_DECLARE_CONST_FUN_OBJ_2(str_join_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(str_split_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(str_splitlines_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(str_rsplit_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(str_startswith_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(str_endswith_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(str_strip_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(str_lstrip_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(str_rstrip_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(str_format_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(str_replace_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(str_count_obj);
MP_DECLARE_CONST_FUN_OBJ_2(str_partition_obj);
MP_DECLARE_CONST_FUN_OBJ_2(str_rpartition_obj);
MP_DECLARE_CONST_FUN_OBJ_2(str_center_obj);
MP_DECLARE_CONST_FUN_OBJ_1(str_lower_obj);
MP_DECLARE_CONST_FUN_OBJ_1(str_upper_obj);
MP_DECLARE_CONST_FUN_OBJ_1(str_isspace_obj);
MP_DECLARE_CONST_FUN_OBJ_1(str_isalpha_obj);
MP_DECLARE_CONST_FUN_OBJ_1(str_isdigit_obj);
MP_DECLARE_CONST_FUN_OBJ_1(str_isupper_obj);
MP_DECLARE_CONST_FUN_OBJ_1(str_islower_obj);

#endif // MICROPY_INCLUDED_PY_OBJSTR_H
