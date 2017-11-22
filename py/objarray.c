/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

// make (& TYPECODE_MASK) a null operation if memorview not enabled
#if MICROPY_PY_BUILTINS_MEMORYVIEW
#define TYPECODE_MASK (0x7f)
#else
#define TYPECODE_MASK (~(size_t)0)
#endif

STATIC mp_obj_t array_iterator_new(mp_obj_t array_in, mp_obj_iter_buf_t *iter_buf);
STATIC mp_obj_t array_append(mp_obj_t self_in, mp_obj_t arg);
STATIC mp_obj_t array_extend(mp_obj_t self_in, mp_obj_t arg_in);
STATIC mp_int_t array_get_buffer(mp_obj_t o_in, mp_buffer_info_t *bufinfo, mp_uint_t flags);

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
            && (MP_OBJ_IS_TYPE(initializer, &mp_type_bytes)
                || (MICROPY_PY_BUILTINS_BYTEARRAY && MP_OBJ_IS_TYPE(initializer, &mp_type_bytearray)))))
        && mp_get_buffer(initializer, &bufinfo, MP_BUFFER_READ)) {
        // construct array from raw bytes
        // we round-down the len to make it a multiple of sz (CPython raises error)
        size_t sz = mp_binary_get_size('@', typecode, NULL);
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
STATIC mp_obj_t array_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)type_in;
    mp_arg_check_num(n_args, n_kw, 1, 2, false);

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
STATIC mp_obj_t bytearray_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)type_in;
    mp_arg_check_num(n_args, n_kw, 0, 1, false);

    if (n_args == 0) {
        // no args: construct an empty bytearray
        return MP_OBJ_FROM_PTR(array_new(BYTEARRAY_TYPECODE, 0));
    } else if (MP_OBJ_IS_INT(args[0])) {
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
    self->base.type = &mp_type_memoryview;
    self->typecode = typecode;
    self->free = 0;
    self->len = nitems;
    self->items = items;
    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t memoryview_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)type_in;

    // TODO possibly allow memoryview constructor to take start/stop so that one
    // can do memoryview(b, 4, 8) instead of memoryview(b)[4:8] (uses less RAM)

    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[0], &bufinfo, MP_BUFFER_READ);

    mp_obj_array_t *self = MP_OBJ_TO_PTR(mp_obj_new_memoryview(bufinfo.typecode,
        bufinfo.len / mp_binary_get_size('@', bufinfo.typecode, NULL),
        bufinfo.buf));

    // test if the object can be written to
    if (mp_get_buffer(args[0], &bufinfo, MP_BUFFER_RW)) {
        self->typecode |= 0x80; // used to indicate writable buffer
    }

    return MP_OBJ_FROM_PTR(self);
}
#endif

STATIC mp_obj_t array_unary_op(mp_unary_op_t op, mp_obj_t o_in) {
    mp_obj_array_t *o = MP_OBJ_TO_PTR(o_in);
    switch (op) {
        case MP_UNARY_OP_BOOL: return mp_obj_new_bool(o->len != 0);
        case MP_UNARY_OP_LEN: return MP_OBJ_NEW_SMALL_INT(o->len);
        default: return MP_OBJ_NULL; // op not supported
    }
}

STATIC mp_obj_t array_binary_op(mp_binary_op_t op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    mp_obj_array_t *lhs = MP_OBJ_TO_PTR(lhs_in);
    switch (op) {
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
            mp_seq_cat((byte*)res->items, lhs_bufinfo.buf, lhs_bufinfo.len, rhs_bufinfo.buf, rhs_len * sz, byte);
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

        case MP_BINARY_OP_IN: {
            /* NOTE `a in b` is `b.__contains__(a)` */
            mp_buffer_info_t lhs_bufinfo;
            mp_buffer_info_t rhs_bufinfo;

            // Can search string only in bytearray
            if (mp_get_buffer(rhs_in, &rhs_bufinfo, MP_BUFFER_READ)) {
                if (!MP_OBJ_IS_TYPE(lhs_in, &mp_type_bytearray)) {
                    return mp_const_false;
                }
                array_get_buffer(lhs_in, &lhs_bufinfo, MP_BUFFER_READ);
                return mp_obj_new_bool(
                    find_subbytes(lhs_bufinfo.buf, lhs_bufinfo.len, rhs_bufinfo.buf, rhs_bufinfo.len, 1) != NULL);
            }

            // Otherwise, can only look for a scalar numeric value in an array
            if (MP_OBJ_IS_INT(rhs_in) || mp_obj_is_float(rhs_in)) {
                mp_raise_NotImplementedError(NULL);
            }

            return mp_const_false;
        }

        case MP_BINARY_OP_EQUAL: {
            mp_buffer_info_t lhs_bufinfo;
            mp_buffer_info_t rhs_bufinfo;
            array_get_buffer(lhs_in, &lhs_bufinfo, MP_BUFFER_READ);
            if (!mp_get_buffer(rhs_in, &rhs_bufinfo, MP_BUFFER_READ)) {
                return mp_const_false;
            }
            return mp_obj_new_bool(mp_seq_cmp_bytes(op, lhs_bufinfo.buf, lhs_bufinfo.len, rhs_bufinfo.buf, rhs_bufinfo.len));
        }

        default:
            return MP_OBJ_NULL; // op not supported
    }
}

#if MICROPY_PY_BUILTINS_BYTEARRAY || MICROPY_PY_ARRAY
STATIC mp_obj_t array_append(mp_obj_t self_in, mp_obj_t arg) {
    // self is not a memoryview, so we don't need to use (& TYPECODE_MASK)
    assert((MICROPY_PY_BUILTINS_BYTEARRAY && MP_OBJ_IS_TYPE(self_in, &mp_type_bytearray))
        || (MICROPY_PY_ARRAY && MP_OBJ_IS_TYPE(self_in, &mp_type_array)));
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
    assert((MICROPY_PY_BUILTINS_BYTEARRAY && MP_OBJ_IS_TYPE(self_in, &mp_type_bytearray))
        || (MICROPY_PY_ARRAY && MP_OBJ_IS_TYPE(self_in, &mp_type_array)));
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
    mp_seq_copy((byte*)self->items + self->len * sz, arg_bufinfo.buf, len * sz, byte);
    self->len += len;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(array_extend_obj, array_extend);
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
        if (0) {
#if MICROPY_PY_BUILTINS_SLICE
        } else if (MP_OBJ_IS_TYPE(index_in, &mp_type_slice)) {
            mp_bound_slice_t slice;
            if (!mp_seq_get_fast_slice_indexes(o->len, index_in, &slice)) {
                mp_raise_NotImplementedError("only slices with step=1 (aka None) are supported");
            }
            if (value != MP_OBJ_SENTINEL) {
                #if MICROPY_PY_ARRAY_SLICE_ASSIGN
                // Assign
                size_t src_len;
                void *src_items;
                size_t item_sz = mp_binary_get_size('@', o->typecode & TYPECODE_MASK, NULL);
                if (MP_OBJ_IS_OBJ(value) && ((mp_obj_base_t*)MP_OBJ_TO_PTR(value))->type->subscr == array_subscr) {
                    // value is array, bytearray or memoryview
                    mp_obj_array_t *src_slice = MP_OBJ_TO_PTR(value);
                    if (item_sz != mp_binary_get_size('@', src_slice->typecode & TYPECODE_MASK, NULL)) {
                    compat_error:
                        mp_raise_ValueError("lhs and rhs should be compatible");
                    }
                    src_len = src_slice->len;
                    src_items = src_slice->items;
                    #if MICROPY_PY_BUILTINS_MEMORYVIEW
                    if (MP_OBJ_IS_TYPE(value, &mp_type_memoryview)) {
                        src_items = (uint8_t*)src_items + (src_slice->free * item_sz);
                    }
                    #endif
                } else if (MP_OBJ_IS_TYPE(value, &mp_type_bytes)) {
                    if (item_sz != 1) {
                        goto compat_error;
                    }
                    mp_buffer_info_t bufinfo;
                    mp_get_buffer_raise(value, &bufinfo, MP_BUFFER_READ);
                    src_len = bufinfo.len;
                    src_items = bufinfo.buf;
                } else {
                    mp_raise_NotImplementedError("array/bytes required on right side");
                }

                // TODO: check src/dst compat
                mp_int_t len_adj = src_len - (slice.stop - slice.start);
                uint8_t* dest_items = o->items;
                #if MICROPY_PY_BUILTINS_MEMORYVIEW
                if (o->base.type == &mp_type_memoryview) {
                    if ((o->typecode & 0x80) == 0) {
                        // store to read-only memoryview not allowed
                        return MP_OBJ_NULL;
                    }
                    if (len_adj != 0) {
                        goto compat_error;
                    }
                    dest_items += o->free * item_sz;
                }
                #endif
                if (len_adj > 0) {
                    if (len_adj > o->free) {
                        // TODO: alloc policy; at the moment we go conservative
                        o->items = m_renew(byte, o->items, (o->len + o->free) * item_sz, (o->len + len_adj) * item_sz);
                        o->free = 0;
                        dest_items = o->items;
                    }
                    mp_seq_replace_slice_grow_inplace(dest_items, o->len,
                        slice.start, slice.stop, src_items, src_len, len_adj, item_sz);
                } else {
                    mp_seq_replace_slice_no_grow(dest_items, o->len,
                        slice.start, slice.stop, src_items, src_len, item_sz);
                    // Clear "freed" elements at the end of list
                    // TODO: This is actually only needed for typecode=='O'
                    mp_seq_clear(dest_items, o->len + len_adj, o->len, item_sz);
                    // TODO: alloc policy after shrinking
                }
                o->len += len_adj;
                return mp_const_none;
                #else
                return MP_OBJ_NULL; // op not supported
                #endif
            }

            mp_obj_array_t *res;
            size_t sz = mp_binary_get_size('@', o->typecode & TYPECODE_MASK, NULL);
            assert(sz > 0);
            if (0) {
                // dummy
            #if MICROPY_PY_BUILTINS_MEMORYVIEW
            } else if (o->base.type == &mp_type_memoryview) {
                res = m_new_obj(mp_obj_array_t);
                *res = *o;
                res->free += slice.start;
                res->len = slice.stop - slice.start;
            #endif
            } else {
                res = array_new(o->typecode, slice.stop - slice.start);
                memcpy(res->items, (uint8_t*)o->items + slice.start * sz, (slice.stop - slice.start) * sz);
            }
            return MP_OBJ_FROM_PTR(res);
#endif
        } else {
            size_t index = mp_get_index(o->base.type, o->len, index_in, false);
            #if MICROPY_PY_BUILTINS_MEMORYVIEW
            if (o->base.type == &mp_type_memoryview) {
                index += o->free;
                if (value != MP_OBJ_SENTINEL && (o->typecode & 0x80) == 0) {
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
        if ((o->typecode & 0x80) == 0 && (flags & MP_BUFFER_WRITE)) {
            // read-only memoryview
            return 1;
        }
        bufinfo->buf = (uint8_t*)bufinfo->buf + (size_t)o->free * sz;
    }
    #else
    (void)flags;
    #endif
    return 0;
}

#if MICROPY_PY_BUILTINS_BYTEARRAY || MICROPY_PY_ARRAY
STATIC const mp_rom_map_elem_t array_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_append), MP_ROM_PTR(&array_append_obj) },
    { MP_ROM_QSTR(MP_QSTR_extend), MP_ROM_PTR(&array_extend_obj) },
};

STATIC MP_DEFINE_CONST_DICT(array_locals_dict, array_locals_dict_table);
#endif

#if MICROPY_PY_ARRAY
const mp_obj_type_t mp_type_array = {
    { &mp_type_type },
    .name = MP_QSTR_array,
    .print = array_print,
    .make_new = array_make_new,
    .getiter = array_iterator_new,
    .unary_op = array_unary_op,
    .binary_op = array_binary_op,
    .subscr = array_subscr,
    .buffer_p = { .get_buffer = array_get_buffer },
    .locals_dict = (mp_obj_dict_t*)&array_locals_dict,
};
#endif

#if MICROPY_PY_BUILTINS_BYTEARRAY
const mp_obj_type_t mp_type_bytearray = {
    { &mp_type_type },
    .name = MP_QSTR_bytearray,
    .print = array_print,
    .make_new = bytearray_make_new,
    .getiter = array_iterator_new,
    .unary_op = array_unary_op,
    .binary_op = array_binary_op,
    .subscr = array_subscr,
    .buffer_p = { .get_buffer = array_get_buffer },
    .locals_dict = (mp_obj_dict_t*)&array_locals_dict,
};
#endif

#if MICROPY_PY_BUILTINS_MEMORYVIEW
const mp_obj_type_t mp_type_memoryview = {
    { &mp_type_type },
    .name = MP_QSTR_memoryview,
    .make_new = memoryview_make_new,
    .getiter = array_iterator_new,
    .unary_op = array_unary_op,
    .binary_op = array_binary_op,
    .subscr = array_subscr,
    .buffer_p = { .get_buffer = array_get_buffer },
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

STATIC const mp_obj_type_t array_it_type = {
    { &mp_type_type },
    .name = MP_QSTR_iterator,
    .getiter = mp_identity_getiter,
    .iternext = array_it_iternext,
};

STATIC mp_obj_t array_iterator_new(mp_obj_t array_in, mp_obj_iter_buf_t *iter_buf) {
    assert(sizeof(mp_obj_array_t) <= sizeof(mp_obj_iter_buf_t));
    mp_obj_array_t *array = MP_OBJ_TO_PTR(array_in);
    mp_obj_array_it_t *o = (mp_obj_array_it_t*)iter_buf;
    o->base.type = &array_it_type;
    o->array = array;
    o->offset = 0;
    o->cur = 0;
    #if MICROPY_PY_BUILTINS_MEMORYVIEW
    if (array->base.type == &mp_type_memoryview) {
        o->offset = array->free;
    }
    #endif
    return MP_OBJ_FROM_PTR(o);
}

#endif // MICROPY_PY_ARRAY || MICROPY_PY_BUILTINS_BYTEARRAY || MICROPY_PY_BUILTINS_MEMORYVIEW
