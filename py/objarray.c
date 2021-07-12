/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
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

#include <string.h>
#include <assert.h>
#include <stdint.h>

#include "py/runtime.h"
#include "py/binary.h"
#include "py/objstr.h"
#include "py/objarray.h"

#include "supervisor/shared/translate.h"

#if MICROPY_PY_ARRAY || MICROPY_PY_BUILTINS_BYTEARRAY || MICROPY_PY_BUILTINS_MEMORYVIEW

// About memoryview object: We want to reuse as much code as possible from
// array, and keep the memoryview object 4 words in size so it fits in 1 GC
// block.  Also, memoryview must keep a pointer to the base of the buffer so
// that the buffer is not GC'd if the original parent object is no longer
// around (we are assuming that all memoryview'able objects return a pointer
// which points to the start of a GC chunk).  Given the above constraints we
// do the following:
//  - typecode high bit is set if the buffer is read-write (else read-only)
//  - free is the offset in elements to the first item in the memoryview
//  - len is the length in elements
//  - items points to the start of the original buffer
// Note that we don't handle the case where the original buffer might change
// size due to a resize of the original parent object.

#if MICROPY_PY_BUILTINS_MEMORYVIEW
#define TYPECODE_MASK (0x7f)
#define memview_offset free
#else
// make (& TYPECODE_MASK) a null operation if memorview not enabled
#define TYPECODE_MASK (~(size_t)0)
// memview_offset should not be accessed if memoryview is not enabled,
// so not defined to catch errors
#endif

STATIC mp_obj_t array_iterator_new(mp_obj_t array_in, mp_obj_iter_buf_t *iter_buf);
STATIC mp_obj_t array_append(mp_obj_t self_in, mp_obj_t arg);
STATIC mp_obj_t array_extend(mp_obj_t self_in, mp_obj_t arg_in);
STATIC mp_int_t array_get_buffer(mp_obj_t o_in, mp_buffer_info_t *bufinfo, mp_uint_t flags);
#if MICROPY_CPYTHON_COMPAT
STATIC mp_obj_t array_decode(size_t n_args, const mp_obj_t *args);
#endif


/******************************************************************************/
// array

#if MICROPY_PY_BUILTINS_BYTEARRAY || MICROPY_PY_ARRAY
STATIC void array_print(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_array_t *o = MP_OBJ_TO_PTR(o_in);
    if (o->typecode == BYTEARRAY_TYPECODE) {
        mp_print_str(print, "bytearray(b");
        mp_str_print_quoted(print, o->items, o->len, true);
    } else {
        mp_printf(print, "array('%c'", o->typecode);
        if (o->len > 0) {
            mp_print_str(print, ", [");
            for (size_t i = 0; i < o->len; i++) {
                if (i > 0) {
                    mp_print_str(print, ", ");
                }
                mp_obj_print_helper(print, mp_binary_get_val_array(o->typecode, o->items, i), PRINT_REPR);
            }
            mp_print_str(print, "]");
        }
    }
    mp_print_str(print, ")");
}
#endif

#if MICROPY_PY_BUILTINS_BYTEARRAY || MICROPY_PY_ARRAY
STATIC mp_obj_array_t *array_new(char typecode, size_t n) {
    if (typecode == 'x') {
        mp_raise_ValueError(MP_ERROR_TEXT("bad typecode"));
    }
    int typecode_size = mp_binary_get_size('@', typecode, NULL);
    mp_obj_array_t *o = m_new_obj(mp_obj_array_t);
    #if MICROPY_PY_BUILTINS_BYTEARRAY && MICROPY_PY_ARRAY
    o->base.type = (typecode == BYTEARRAY_TYPECODE) ? &mp_type_bytearray : &mp_type_array;
    #elif MICROPY_PY_BUILTINS_BYTEARRAY
    o->base.type = &mp_type_bytearray;
    #else
    o->base.type = &mp_type_array;
    #endif
    o->typecode = typecode;
    o->free = 0;
    o->len = n;
    o->items = m_new(byte, typecode_size * o->len);
    return o;
}
#endif

#if MICROPY_PY_BUILTINS_BYTEARRAY || MICROPY_PY_ARRAY
STATIC mp_obj_t array_construct(char typecode, mp_obj_t initializer) {
    // bytearrays can be raw-initialised from anything with the buffer protocol
    // other arrays can only be raw-initialised from bytes and bytearray objects
    mp_buffer_info_t bufinfo;
    if (((MICROPY_PY_BUILTINS_BYTEARRAY
          && typecode == BYTEARRAY_TYPECODE)
         || (MICROPY_PY_ARRAY
             && (mp_obj_is_type(initializer, &mp_type_bytes)
                 || (MICROPY_PY_BUILTINS_BYTEARRAY && mp_obj_is_type(initializer, &mp_type_bytearray)))))
        && mp_get_buffer(initializer, &bufinfo, MP_BUFFER_READ)) {
        // construct array from raw bytes
        size_t sz = mp_binary_get_size('@', typecode, NULL);
        if (bufinfo.len % sz) {
            mp_raise_ValueError(MP_ERROR_TEXT("bytes length not a multiple of item size"));
        }
        size_t len = bufinfo.len / sz;
        mp_obj_array_t *o = array_new(typecode, len);
        memcpy(o->items, bufinfo.buf, len * sz);
        return MP_OBJ_FROM_PTR(o);
    }

    size_t len;
    // Try to create array of exact len if initializer len is known
    mp_obj_t len_in = mp_obj_len_maybe(initializer);
    if (len_in == MP_OBJ_NULL) {
        len = 0;
    } else {
        len = MP_OBJ_SMALL_INT_VALUE(len_in);
    }

    mp_obj_array_t *array = array_new(typecode, len);

    mp_obj_t iterable = mp_getiter(initializer, NULL);
    mp_obj_t item;
    size_t i = 0;
    while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        if (len == 0) {
            array_append(MP_OBJ_FROM_PTR(array), item);
        } else {
            mp_binary_set_val_array(typecode, array->items, i++, item);
        }
    }

    return MP_OBJ_FROM_PTR(array);
}
#endif

#if MICROPY_PY_ARRAY
STATIC mp_obj_t array_make_new(const mp_obj_type_t *type_in, size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    (void)type_in;
    mp_arg_check_num(n_args, kw_args, 1, 2, false);

    // get typecode
    const char *typecode = mp_obj_str_get_str(args[0]);

    if (n_args == 1) {
        // 1 arg: make an empty array
        return MP_OBJ_FROM_PTR(array_new(*typecode, 0));
    } else {
        // 2 args: construct the array from the given object
        return array_construct(*typecode, args[1]);
    }
}
#endif

#if MICROPY_PY_BUILTINS_BYTEARRAY
STATIC mp_obj_t bytearray_make_new(const mp_obj_type_t *type_in, size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    (void)type_in;
    mp_arg_check_num(n_args, kw_args, 0, 3, false);

    if (n_args == 0) {
        // no args: construct an empty bytearray
        return MP_OBJ_FROM_PTR(array_new(BYTEARRAY_TYPECODE, 0));
    } else if (mp_obj_is_int(args[0])) {
        if (n_args > 1) {
            mp_raise_TypeError(MP_ERROR_TEXT("wrong number of arguments"));
        }
        // 1 arg, an integer: construct a blank bytearray of that length
        mp_uint_t len = mp_obj_get_int(args[0]);
        mp_obj_array_t *o = array_new(BYTEARRAY_TYPECODE, len);
        memset(o->items, 0, len);
        return MP_OBJ_FROM_PTR(o);
    } else {
        // 1 arg: construct the bytearray from that
        return array_construct(BYTEARRAY_TYPECODE, args[0]);
    }
}
#endif

#if MICROPY_PY_BUILTINS_MEMORYVIEW

mp_obj_t mp_obj_new_memoryview(byte typecode, size_t nitems, void *items) {
    mp_obj_array_t *self = m_new_obj(mp_obj_array_t);
    mp_obj_memoryview_init(self, typecode, 0, nitems, items);
    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t memoryview_make_new(const mp_obj_type_t *type_in, size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    (void)type_in;

    // TODO possibly allow memoryview constructor to take start/stop so that one
    // can do memoryview(b, 4, 8) instead of memoryview(b)[4:8] (uses less RAM)

    mp_arg_check_num(n_args, kw_args, 1, 1, false);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[0], &bufinfo, MP_BUFFER_READ);

    mp_obj_array_t *self = MP_OBJ_TO_PTR(mp_obj_new_memoryview(bufinfo.typecode,
        bufinfo.len / mp_binary_get_size('@', bufinfo.typecode, NULL),
        bufinfo.buf));

    // If the input object is a memoryview then need to point the items of the
    // new memoryview to the start of the buffer so the GC can trace it.
    if (mp_obj_get_type(args[0]) == &mp_type_memoryview) {
        mp_obj_array_t *other = MP_OBJ_TO_PTR(args[0]);
        self->memview_offset = other->memview_offset;
        self->items = other->items;
    }

    // test if the object can be written to
    if (mp_get_buffer(args[0], &bufinfo, MP_BUFFER_RW)) {
        self->typecode |= MP_OBJ_ARRAY_TYPECODE_FLAG_RW; // indicate writable buffer
    }

    return MP_OBJ_FROM_PTR(self);
}

#if MICROPY_CPYTHON_COMPAT
STATIC mp_obj_t memoryview_cast(const mp_obj_t self_in, const mp_obj_t typecode_in) {
    mp_obj_array_t *self = MP_OBJ_TO_PTR(self_in);
    const char *typecode = mp_obj_str_get_str(typecode_in);
    size_t element_size = mp_binary_get_size('@', typecode[0], NULL);
    size_t bytelen = self->len * mp_binary_get_size('@', self->typecode & ~MP_OBJ_ARRAY_TYPECODE_FLAG_RW, NULL);
    if (bytelen % element_size != 0) {
        mp_raise_TypeError(MP_ERROR_TEXT("memoryview: length is not a multiple of itemsize"));
    }
    mp_obj_array_t *result = MP_OBJ_TO_PTR(mp_obj_new_memoryview(*typecode, bytelen / element_size, self->items));

    // test if the object can be written to
    if (self->typecode & MP_OBJ_ARRAY_TYPECODE_FLAG_RW) {
        result->typecode |= MP_OBJ_ARRAY_TYPECODE_FLAG_RW; // indicate writable buffer
    }
    return MP_OBJ_FROM_PTR(result);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(memoryview_cast_obj, memoryview_cast);
#endif

#if MICROPY_PY_BUILTINS_MEMORYVIEW_ITEMSIZE
STATIC void memoryview_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    if (dest[0] != MP_OBJ_NULL) {
        return;
    }
    if (attr == MP_QSTR_itemsize) {
        mp_obj_array_t *self = MP_OBJ_TO_PTR(self_in);
        dest[0] = MP_OBJ_NEW_SMALL_INT(mp_binary_get_size('@', self->typecode & TYPECODE_MASK, NULL));
    }
}
#endif

#endif

STATIC mp_obj_t array_unary_op(mp_unary_op_t op, mp_obj_t o_in) {
    mp_obj_array_t *o = MP_OBJ_TO_PTR(o_in);
    switch (op) {
        case MP_UNARY_OP_BOOL:
            return mp_obj_new_bool(o->len != 0);
        case MP_UNARY_OP_LEN:
            return MP_OBJ_NEW_SMALL_INT(o->len);
        default:
            return MP_OBJ_NULL;      // op not supported
    }
}

STATIC int typecode_for_comparison(int typecode, bool *is_unsigned) {
    if (typecode == BYTEARRAY_TYPECODE) {
        typecode = 'B';
    }
    if (typecode <= 'Z') {
        typecode += 32; // to lowercase
        *is_unsigned = true;
    }
    return typecode;
}

STATIC mp_obj_t array_binary_op(mp_binary_op_t op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    mp_obj_array_t *lhs = MP_OBJ_TO_PTR(lhs_in);
    switch (op) {
        case MP_BINARY_OP_MULTIPLY:
        case MP_BINARY_OP_INPLACE_MULTIPLY: {
            if (!mp_obj_is_int(rhs_in)) {
                return MP_OBJ_NULL; // op not supported
            }
            mp_uint_t repeat = mp_obj_get_int(rhs_in);
            bool inplace = (op == MP_BINARY_OP_INPLACE_MULTIPLY);
            mp_buffer_info_t lhs_bufinfo;
            array_get_buffer(lhs_in, &lhs_bufinfo, MP_BUFFER_READ);
            mp_obj_array_t *res;
            byte *ptr;
            size_t orig_lhs_bufinfo_len = lhs_bufinfo.len;
            if (inplace) {
                res = lhs;
                size_t item_sz = mp_binary_get_size('@', lhs->typecode, NULL);
                lhs->items = m_renew(byte, lhs->items, (lhs->len + lhs->free) * item_sz, lhs->len * repeat * item_sz);
                lhs->len = lhs->len * repeat;
                lhs->free = 0;
                if (!repeat) {
                    return MP_OBJ_FROM_PTR(res);
                }
                repeat--;
                ptr = (byte *)res->items + orig_lhs_bufinfo_len;
            } else {
                res = array_new(lhs_bufinfo.typecode, lhs->len * repeat);
                ptr = (byte *)res->items;
            }
            if (orig_lhs_bufinfo_len) {
                for (; repeat--; ptr += orig_lhs_bufinfo_len) {
                    memcpy(ptr, lhs_bufinfo.buf, orig_lhs_bufinfo_len);
                }
            }
            return MP_OBJ_FROM_PTR(res);
        }
        case MP_BINARY_OP_ADD: {
            // allow to add anything that has the buffer protocol (extension to CPython)
            mp_buffer_info_t lhs_bufinfo;
            mp_buffer_info_t rhs_bufinfo;
            array_get_buffer(lhs_in, &lhs_bufinfo, MP_BUFFER_READ);
            mp_get_buffer_raise(rhs_in, &rhs_bufinfo, MP_BUFFER_READ);

            size_t sz = mp_binary_get_size('@', lhs_bufinfo.typecode, NULL);

            // convert byte count to element count (in case rhs is not multiple of sz)
            size_t rhs_len = rhs_bufinfo.len / sz;

            // note: lhs->len is element count of lhs, lhs_bufinfo.len is byte count
            mp_obj_array_t *res = array_new(lhs_bufinfo.typecode, lhs->len + rhs_len);
            mp_seq_cat((byte *)res->items, lhs_bufinfo.buf, lhs_bufinfo.len, rhs_bufinfo.buf, rhs_len * sz, byte);
            return MP_OBJ_FROM_PTR(res);
        }

        case MP_BINARY_OP_INPLACE_ADD: {
            #if MICROPY_PY_BUILTINS_MEMORYVIEW
            if (lhs->base.type == &mp_type_memoryview) {
                return MP_OBJ_NULL; // op not supported
            }
            #endif
            array_extend(lhs_in, rhs_in);
            return lhs_in;
        }

        case MP_BINARY_OP_CONTAINS: {
            #if MICROPY_PY_BUILTINS_BYTEARRAY
            // Can search string only in bytearray
            mp_buffer_info_t lhs_bufinfo;
            mp_buffer_info_t rhs_bufinfo;
            if (mp_get_buffer(rhs_in, &rhs_bufinfo, MP_BUFFER_READ)) {
                if (!mp_obj_is_type(lhs_in, &mp_type_bytearray)) {
                    return mp_const_false;
                }
                array_get_buffer(lhs_in, &lhs_bufinfo, MP_BUFFER_READ);
                return mp_obj_new_bool(
                    find_subbytes(lhs_bufinfo.buf, lhs_bufinfo.len, rhs_bufinfo.buf, rhs_bufinfo.len, 1) != NULL);
            }
            #endif

            // Otherwise, can only look for a scalar numeric value in an array
            if (mp_obj_is_int(rhs_in) || mp_obj_is_float(rhs_in)) {
                mp_raise_NotImplementedError(NULL);
            }

            return mp_const_false;
        }

        case MP_BINARY_OP_EQUAL:
        case MP_BINARY_OP_LESS:
        case MP_BINARY_OP_LESS_EQUAL:
        case MP_BINARY_OP_MORE:
        case MP_BINARY_OP_MORE_EQUAL: {
            mp_buffer_info_t lhs_bufinfo;
            mp_buffer_info_t rhs_bufinfo;
            array_get_buffer(lhs_in, &lhs_bufinfo, MP_BUFFER_READ);
            if (!mp_get_buffer(rhs_in, &rhs_bufinfo, MP_BUFFER_READ)) {
                return mp_const_false;
            }
            // mp_seq_cmp_bytes is used so only compatible representations can be correctly compared.
            // The type doesn't matter: array/bytearray/str/bytes all have the same buffer layout, so
            // just check if the typecodes are compatible; for testing equality the types should have the
            // same code except for signedness, and not be floating point because nan never equals nan.
            // For > and < the types should be the same and unsigned.
            // Note that typecode_for_comparison always returns lowercase letters to save code size.
            // No need for (& TYPECODE_MASK) here: xxx_get_buffer already takes care of that.
            bool is_unsigned = false;
            const int lhs_code = typecode_for_comparison(lhs_bufinfo.typecode, &is_unsigned);
            const int rhs_code = typecode_for_comparison(rhs_bufinfo.typecode, &is_unsigned);
            if (lhs_code == rhs_code && lhs_code != 'f' && lhs_code != 'd' && (op == MP_BINARY_OP_EQUAL || is_unsigned)) {
                return mp_obj_new_bool(mp_seq_cmp_bytes(op, lhs_bufinfo.buf, lhs_bufinfo.len, rhs_bufinfo.buf, rhs_bufinfo.len));
            }
            // mp_obj_equal_not_equal treats returning MP_OBJ_NULL as 'fall back to pointer comparison'
            // for MP_BINARY_OP_EQUAL but that is incompatible with CPython.
            mp_raise_NotImplementedError(NULL);
        }

        default:
            return MP_OBJ_NULL; // op not supported
    }
}

#if MICROPY_PY_BUILTINS_BYTEARRAY || MICROPY_PY_ARRAY
STATIC mp_obj_t array_append(mp_obj_t self_in, mp_obj_t arg) {
    // self is not a memoryview, so we don't need to use (& TYPECODE_MASK)
    assert((MICROPY_PY_BUILTINS_BYTEARRAY && mp_obj_is_type(self_in, &mp_type_bytearray))
        || (MICROPY_PY_ARRAY && mp_obj_is_type(self_in, &mp_type_array)));
    mp_obj_array_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->free == 0) {
        size_t item_sz = mp_binary_get_size('@', self->typecode, NULL);
        // TODO: alloc policy
        self->free = 8;
        self->items = m_renew(byte, self->items, item_sz * self->len, item_sz * (self->len + self->free));
        mp_seq_clear(self->items, self->len + 1, self->len + self->free, item_sz);
    }
    mp_binary_set_val_array(self->typecode, self->items, self->len, arg);
    // only update length/free if set succeeded
    self->len++;
    self->free--;
    return mp_const_none; // return None, as per CPython
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(array_append_obj, array_append);

STATIC mp_obj_t array_extend(mp_obj_t self_in, mp_obj_t arg_in) {
    // self is not a memoryview, so we don't need to use (& TYPECODE_MASK)
    assert((MICROPY_PY_BUILTINS_BYTEARRAY && mp_obj_is_type(self_in, &mp_type_bytearray))
        || (MICROPY_PY_ARRAY && mp_obj_is_type(self_in, &mp_type_array)));
    mp_obj_array_t *self = MP_OBJ_TO_PTR(self_in);

    // allow to extend by anything that has the buffer protocol (extension to CPython)
    mp_buffer_info_t arg_bufinfo;
    mp_get_buffer_raise(arg_in, &arg_bufinfo, MP_BUFFER_READ);

    size_t sz = mp_binary_get_size('@', self->typecode, NULL);

    // convert byte count to element count
    size_t len = arg_bufinfo.len / sz;

    // make sure we have enough room to extend
    // TODO: alloc policy; at the moment we go conservative
    if (self->free < len) {
        self->items = m_renew(byte, self->items, (self->len + self->free) * sz, (self->len + len) * sz);
        self->free = 0;
    } else {
        self->free -= len;
    }

    // extend
    mp_seq_copy((byte *)self->items + self->len * sz, arg_bufinfo.buf, len * sz, byte);
    self->len += len;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(array_extend_obj, array_extend);
#endif

#if MICROPY_PY_BUILTINS_BYTEARRAY && MICROPY_CPYTHON_COMPAT
STATIC mp_obj_t buffer_finder(size_t n_args, const mp_obj_t *args, int direction, bool is_index) {
    mp_check_self(mp_obj_is_type(args[0], &mp_type_bytearray));
    const mp_obj_type_t *self_type = mp_obj_get_type(args[0]);

    mp_buffer_info_t haystack_bufinfo;
    mp_get_buffer_raise(args[0], &haystack_bufinfo, MP_BUFFER_READ);

    mp_buffer_info_t needle_bufinfo;
    mp_get_buffer_raise(args[1], &needle_bufinfo, MP_BUFFER_READ);

    if (mp_binary_get_size('@', needle_bufinfo.typecode, NULL) != 1) {
        mp_raise_TypeError(MP_ERROR_TEXT("a bytes-like object is required"));
    }

    const byte *start = haystack_bufinfo.buf;
    const byte *end = ((const byte *)haystack_bufinfo.buf) + haystack_bufinfo.len;
    if (n_args >= 3 && args[2] != mp_const_none) {
        start += mp_get_index(self_type, haystack_bufinfo.len, args[2], true);
    }
    if (n_args >= 4 && args[3] != mp_const_none) {
        end = ((const byte *)haystack_bufinfo.buf) + mp_get_index(self_type, haystack_bufinfo.len, args[3], true);
    }

    const byte *p = NULL;
    if (end >= start) {
        p = find_subbytes(start, end - start, needle_bufinfo.buf, needle_bufinfo.len, direction);
    }

    if (p == NULL) {
        if (is_index) {
            mp_raise_ValueError(MP_ERROR_TEXT("substring not found"));
        } else {
            return MP_OBJ_NEW_SMALL_INT(-1);
        }
    }
    return MP_OBJ_NEW_SMALL_INT(p - (const byte *)haystack_bufinfo.buf);
}

STATIC mp_obj_t buffer_find(size_t n_args, const mp_obj_t *args) {
    return buffer_finder(n_args, args, 1, false);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(buffer_find_obj, 2, 4, buffer_find);

STATIC mp_obj_t buffer_rfind(size_t n_args, const mp_obj_t *args) {
    return buffer_finder(n_args, args, -1, false);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(buffer_rfind_obj, 2, 4, buffer_rfind);

STATIC mp_obj_t buffer_index(size_t n_args, const mp_obj_t *args) {
    return buffer_finder(n_args, args, 1, true);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(buffer_index_obj, 2, 4, buffer_index);

STATIC mp_obj_t buffer_rindex(size_t n_args, const mp_obj_t *args) {
    return buffer_finder(n_args, args, -1, true);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(buffer_rindex_obj, 2, 4, buffer_rindex);
#endif

STATIC mp_obj_t array_subscr(mp_obj_t self_in, mp_obj_t index_in, mp_obj_t value) {
    if (value == MP_OBJ_NULL) {
        // delete item
        // TODO implement
        // TODO: confirmed that both bytearray and array.array support
        // slice deletion
        return MP_OBJ_NULL; // op not supported
    } else {
        mp_obj_array_t *o = MP_OBJ_TO_PTR(self_in);
        #if MICROPY_PY_BUILTINS_SLICE
        if (mp_obj_is_type(index_in, &mp_type_slice)) {
            mp_bound_slice_t slice;
            if (!mp_seq_get_fast_slice_indexes(o->len, index_in, &slice)) {
                mp_raise_NotImplementedError(MP_ERROR_TEXT("only slices with step=1 (aka None) are supported"));
            }
            if (value != MP_OBJ_SENTINEL) {
                #if MICROPY_PY_ARRAY_SLICE_ASSIGN
                // Assign
                size_t src_len;
                void *src_items;
                size_t item_sz = mp_binary_get_size('@', o->typecode & TYPECODE_MASK, NULL);
                if (mp_obj_is_obj(value) && mp_type_get_subscr_slot(((mp_obj_base_t *)MP_OBJ_TO_PTR(value))->type) == array_subscr) {
                    // value is array, bytearray or memoryview
                    mp_obj_array_t *src_slice = MP_OBJ_TO_PTR(value);
                    if (item_sz != mp_binary_get_size('@', src_slice->typecode & TYPECODE_MASK, NULL)) {
                    compat_error:
                        mp_raise_ValueError(MP_ERROR_TEXT("lhs and rhs should be compatible"));
                    }
                    src_len = src_slice->len;
                    src_items = src_slice->items;
                    #if MICROPY_PY_BUILTINS_MEMORYVIEW
                    if (mp_obj_is_type(value, &mp_type_memoryview)) {
                        src_items = (uint8_t *)src_items + (src_slice->memview_offset * item_sz);
                    }
                    #endif
                } else if (mp_obj_is_type(value, &mp_type_bytes)) {
                    if (item_sz != 1) {
                        goto compat_error;
                    }
                    mp_buffer_info_t bufinfo;
                    mp_get_buffer_raise(value, &bufinfo, MP_BUFFER_READ);
                    src_len = bufinfo.len;
                    src_items = bufinfo.buf;
                } else {
                    mp_raise_NotImplementedError(MP_ERROR_TEXT("array/bytes required on right side"));
                }

                // TODO: check src/dst compat
                mp_int_t len_adj = src_len - (slice.stop - slice.start);
                uint8_t *dest_items = o->items;
                #if MICROPY_PY_BUILTINS_MEMORYVIEW
                if (o->base.type == &mp_type_memoryview) {
                    if (!(o->typecode & MP_OBJ_ARRAY_TYPECODE_FLAG_RW)) {
                        // store to read-only memoryview not allowed
                        return MP_OBJ_NULL;
                    }
                    if (len_adj != 0) {
                        goto compat_error;
                    }
                    dest_items += o->memview_offset * item_sz;
                }
                #endif
                if (len_adj > 0) {
                    if ((size_t)len_adj > o->free) {
                        // TODO: alloc policy; at the moment we go conservative
                        o->items = m_renew(byte, o->items, (o->len + o->free) * item_sz, (o->len + len_adj) * item_sz);
                        o->free = len_adj;
                        dest_items = o->items;
                    }
                    mp_seq_replace_slice_grow_inplace(dest_items, o->len,
                        slice.start, slice.stop, src_items, src_len, len_adj, item_sz);
                } else {
                    mp_seq_replace_slice_no_grow(dest_items, o->len,
                        slice.start, slice.stop, src_items, src_len, item_sz);
                    #if MICROPY_NONSTANDARD_TYPECODES
                    // Clear "freed" elements at the end of list
                    // TODO: This is actually only needed for typecode=='O'
                    mp_seq_clear(dest_items, o->len + len_adj, o->len, item_sz);
                    #endif
                    // TODO: alloc policy after shrinking
                }
                o->free -= len_adj;
                o->len += len_adj;
                return mp_const_none;
                #else
                return MP_OBJ_NULL; // op not supported
                #endif
            }

            mp_obj_array_t *res;
            size_t sz = mp_binary_get_size('@', o->typecode & TYPECODE_MASK, NULL);
            assert(sz > 0);
            #if MICROPY_PY_BUILTINS_MEMORYVIEW
            if (o->base.type == &mp_type_memoryview) {
                res = m_new_obj(mp_obj_array_t);
                *res = *o;
                res->memview_offset += slice.start;
                res->len = slice.stop - slice.start;
            } else
            #endif
            {
                res = array_new(o->typecode, slice.stop - slice.start);
                memcpy(res->items, (uint8_t *)o->items + slice.start * sz, (slice.stop - slice.start) * sz);
            }
            return MP_OBJ_FROM_PTR(res);
        } else
        #endif
        {
            size_t index = mp_get_index(o->base.type, o->len, index_in, false);
            #if MICROPY_PY_BUILTINS_MEMORYVIEW
            if (o->base.type == &mp_type_memoryview) {
                index += o->memview_offset;
                if (value != MP_OBJ_SENTINEL && !(o->typecode & MP_OBJ_ARRAY_TYPECODE_FLAG_RW)) {
                    // store to read-only memoryview
                    return MP_OBJ_NULL;
                }
            }
            #endif
            if (value == MP_OBJ_SENTINEL) {
                // load
                return mp_binary_get_val_array(o->typecode & TYPECODE_MASK, o->items, index);
            } else {
                // store
                mp_binary_set_val_array(o->typecode & TYPECODE_MASK, o->items, index, value);
                return mp_const_none;
            }
        }
    }
}

STATIC mp_int_t array_get_buffer(mp_obj_t o_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    mp_obj_array_t *o = MP_OBJ_TO_PTR(o_in);
    size_t sz = mp_binary_get_size('@', o->typecode & TYPECODE_MASK, NULL);
    bufinfo->buf = o->items;
    bufinfo->len = o->len * sz;
    bufinfo->typecode = o->typecode & TYPECODE_MASK;
    #if MICROPY_PY_BUILTINS_MEMORYVIEW
    if (o->base.type == &mp_type_memoryview) {
        if (!(o->typecode & MP_OBJ_ARRAY_TYPECODE_FLAG_RW) && (flags & MP_BUFFER_WRITE)) {
            // read-only memoryview
            return 1;
        }
        bufinfo->buf = (uint8_t *)bufinfo->buf + (size_t)o->memview_offset * sz;
    }
    #else
    (void)flags;
    #endif
    return 0;
}


#if MICROPY_CPYTHON_COMPAT && MICROPY_PY_BUILTINS_BYTEARRAY
// Directly lifted from objstr.c
STATIC mp_obj_t array_decode(size_t n_args, const mp_obj_t *args) {
    mp_obj_t new_args[2];
    if (n_args == 1) {
        new_args[0] = args[0];
        new_args[1] = MP_OBJ_NEW_QSTR(MP_QSTR_utf_hyphen_8);
        args = new_args;
        n_args++;
    }
    return mp_obj_str_make_new(&mp_type_str, n_args, args, NULL);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(array_decode_obj, 1, 3, array_decode);
#endif


#if MICROPY_PY_ARRAY
STATIC const mp_rom_map_elem_t array_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_append), MP_ROM_PTR(&array_append_obj) },
    { MP_ROM_QSTR(MP_QSTR_extend), MP_ROM_PTR(&array_extend_obj) },
    #if MICROPY_CPYTHON_COMPAT
    { MP_ROM_QSTR(MP_QSTR_decode), MP_ROM_PTR(&bytes_decode_obj) },
    #endif
};

STATIC MP_DEFINE_CONST_DICT(array_locals_dict, array_locals_dict_table);
#endif

#if MICROPY_PY_BUILTINS_BYTEARRAY
STATIC const mp_rom_map_elem_t bytearray_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_append), MP_ROM_PTR(&array_append_obj) },
    { MP_ROM_QSTR(MP_QSTR_extend), MP_ROM_PTR(&array_extend_obj) },

    #if MICROPY_CPYTHON_COMPAT
    { MP_ROM_QSTR(MP_QSTR_find), MP_ROM_PTR(&buffer_find_obj) },
    { MP_ROM_QSTR(MP_QSTR_rfind), MP_ROM_PTR(&buffer_rfind_obj) },
    { MP_ROM_QSTR(MP_QSTR_index), MP_ROM_PTR(&buffer_index_obj) },
    { MP_ROM_QSTR(MP_QSTR_rindex), MP_ROM_PTR(&buffer_rindex_obj) },

    { MP_ROM_QSTR(MP_QSTR_decode), MP_ROM_PTR(&array_decode_obj) },
    #endif
};

STATIC MP_DEFINE_CONST_DICT(bytearray_locals_dict, bytearray_locals_dict_table);
#endif


#if MICROPY_PY_ARRAY
const mp_obj_type_t mp_type_array = {
    { &mp_type_type },
    .flags = MP_TYPE_FLAG_EXTENDED,
    .name = MP_QSTR_array,
    .print = array_print,
    .make_new = array_make_new,
    .locals_dict = (mp_obj_dict_t *)&array_locals_dict,
    MP_TYPE_EXTENDED_FIELDS(
        .getiter = array_iterator_new,
        .unary_op = array_unary_op,
        .binary_op = array_binary_op,
        .subscr = array_subscr,
        .buffer_p = { .get_buffer = array_get_buffer },
        ),
};
#endif

#if MICROPY_PY_BUILTINS_BYTEARRAY
const mp_obj_type_t mp_type_bytearray = {
    { &mp_type_type },
    .flags = MP_TYPE_FLAG_EQ_CHECKS_OTHER_TYPE | MP_TYPE_FLAG_EXTENDED,
    .name = MP_QSTR_bytearray,
    .print = array_print,
    .make_new = bytearray_make_new,
    .locals_dict = (mp_obj_dict_t *)&bytearray_locals_dict,
    MP_TYPE_EXTENDED_FIELDS(
        .getiter = array_iterator_new,
        .unary_op = array_unary_op,
        .binary_op = array_binary_op,
        .subscr = array_subscr,
        .buffer_p = { .get_buffer = array_get_buffer },
        ),
};
#endif

#if MICROPY_PY_BUILTINS_MEMORYVIEW

#if MICROPY_CPYTHON_COMPAT
STATIC const mp_rom_map_elem_t memoryview_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_cast), MP_ROM_PTR(&memoryview_cast_obj) },
};

STATIC MP_DEFINE_CONST_DICT(memoryview_locals_dict, memoryview_locals_dict_table);
#endif

const mp_obj_type_t mp_type_memoryview = {
    { &mp_type_type },
    .flags = MP_TYPE_FLAG_EQ_CHECKS_OTHER_TYPE | MP_TYPE_FLAG_EXTENDED,
    .name = MP_QSTR_memoryview,
    .make_new = memoryview_make_new,
    #if MICROPY_CPYTHON_COMPAT
    .locals_dict = (mp_obj_dict_t *)&memoryview_locals_dict,
    #endif
    #if MICROPY_PY_BUILTINS_MEMORYVIEW_ITEMSIZE
    .attr = memoryview_attr,
    #endif
    MP_TYPE_EXTENDED_FIELDS(
        .getiter = array_iterator_new,
        .unary_op = array_unary_op,
        .binary_op = array_binary_op,
        .subscr = array_subscr,
        .buffer_p = { .get_buffer = array_get_buffer },
        ),
};
#endif

/* unused
size_t mp_obj_array_len(mp_obj_t self_in) {
    return ((mp_obj_array_t *)self_in)->len;
}
*/

#if MICROPY_PY_BUILTINS_BYTEARRAY
mp_obj_t mp_obj_new_bytearray(size_t n, void *items) {
    mp_obj_array_t *o = array_new(BYTEARRAY_TYPECODE, n);
    memcpy(o->items, items, n);
    return MP_OBJ_FROM_PTR(o);
}

mp_obj_t mp_obj_new_bytearray_of_zeros(size_t n) {
    mp_obj_array_t *o = array_new(BYTEARRAY_TYPECODE, n);
    memset(o->items, 0, n);
    return MP_OBJ_FROM_PTR(o);
}

// Create bytearray which references specified memory area
mp_obj_t mp_obj_new_bytearray_by_ref(size_t n, void *items) {
    mp_obj_array_t *o = m_new_obj(mp_obj_array_t);
    o->base.type = &mp_type_bytearray;
    o->typecode = BYTEARRAY_TYPECODE;
    o->free = 0;
    o->len = n;
    o->items = items;
    return MP_OBJ_FROM_PTR(o);
}
#endif

/******************************************************************************/
// array iterator

typedef struct _mp_obj_array_it_t {
    mp_obj_base_t base;
    mp_obj_array_t *array;
    size_t offset;
    size_t cur;
} mp_obj_array_it_t;

STATIC mp_obj_t array_it_iternext(mp_obj_t self_in) {
    mp_obj_array_it_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->cur < self->array->len) {
        return mp_binary_get_val_array(self->array->typecode & TYPECODE_MASK, self->array->items, self->offset + self->cur++);
    } else {
        return MP_OBJ_STOP_ITERATION;
    }
}

STATIC const mp_obj_type_t mp_type_array_it = {
    { &mp_type_type },
    .flags = MP_TYPE_FLAG_EXTENDED,
    .name = MP_QSTR_iterator,
    MP_TYPE_EXTENDED_FIELDS(
        .getiter = mp_identity_getiter,
        .iternext = array_it_iternext,
        ),
};

STATIC mp_obj_t array_iterator_new(mp_obj_t array_in, mp_obj_iter_buf_t *iter_buf) {
    assert(sizeof(mp_obj_array_t) <= sizeof(mp_obj_iter_buf_t));
    mp_obj_array_t *array = MP_OBJ_TO_PTR(array_in);
    mp_obj_array_it_t *o = (mp_obj_array_it_t *)iter_buf;
    o->base.type = &mp_type_array_it;
    o->array = array;
    o->offset = 0;
    o->cur = 0;
    #if MICROPY_PY_BUILTINS_MEMORYVIEW
    if (array->base.type == &mp_type_memoryview) {
        o->offset = array->memview_offset;
    }
    #endif
    return MP_OBJ_FROM_PTR(o);
}

#endif // MICROPY_PY_ARRAY || MICROPY_PY_BUILTINS_BYTEARRAY || MICROPY_PY_BUILTINS_MEMORYVIEW
