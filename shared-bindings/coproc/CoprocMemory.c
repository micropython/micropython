/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 microDev
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

#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/runtime0.h"

#include "shared-bindings/coproc/CoprocMemory.h"
#include "supervisor/shared/translate/translate.h"

//| class CoprocMemory:
//|     def __init__(self, address: int, length: int) -> None:
//|         """Initialize coproc shared memory.
//|
//|         :param address: address of shared memory
//|         :param length: length of shared memory"""
STATIC mp_obj_t coproc_memory_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_address, ARG_length };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_address, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_length, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    coproc_memory_obj_t *self = m_new_obj(coproc_memory_obj_t);
    self->base.type = &coproc_memory_type;
    self->address = args[ARG_address].u_int;
    self->len = args[ARG_length].u_int;

    return MP_OBJ_FROM_PTR(self);
}

//|     def __bool__(self) -> bool:
//|         """``coproc_memory`` is ``True`` if its length is greater than zero.
//|         This is an easy way to check for its existence.
//|         """
//|         ...
//|     def __len__(self) -> int:
//|         """Return the length. This is used by (`len`)"""
//|         ...
STATIC mp_obj_t coproc_memory_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    coproc_memory_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint16_t len = common_hal_coproc_memory_get_length(self);
    switch (op) {
        case MP_UNARY_OP_BOOL:
            return mp_obj_new_bool(len != 0);
        case MP_UNARY_OP_LEN:
            return MP_OBJ_NEW_SMALL_INT(len);
        default:
            return MP_OBJ_NULL;      // op not supported
    }
}

STATIC const mp_rom_map_elem_t coproc_memory_locals_dict_table[] = {
};

STATIC MP_DEFINE_CONST_DICT(coproc_memory_locals_dict, coproc_memory_locals_dict_table);

//|     @overload
//|     def __getitem__(self, index: slice) -> bytearray: ...
//|     @overload
//|     def __getitem__(self, index: int) -> int:
//|         """Returns the value at the given index."""
//|         ...
//|     @overload
//|     def __setitem__(self, index: slice, value: ReadableBuffer) -> None: ...
//|     @overload
//|     def __setitem__(self, index: int, value: int) -> None:
//|         """Set the value at the given index."""
//|         ...
//|
STATIC mp_obj_t coproc_memory_subscr(mp_obj_t self_in, mp_obj_t index_in, mp_obj_t value) {
    if (value == MP_OBJ_NULL) {
        // delete item
        // slice deletion
        return MP_OBJ_NULL; // op not supported
    } else {
        coproc_memory_obj_t *self = MP_OBJ_TO_PTR(self_in);
        if (0) {
        #if MICROPY_PY_BUILTINS_SLICE
        } else if (mp_obj_is_type(index_in, &mp_type_slice)) {
            mp_bound_slice_t slice;
            if (!mp_seq_get_fast_slice_indexes(common_hal_coproc_memory_get_length(self), index_in, &slice)) {
                mp_raise_NotImplementedError(translate("only slices with step=1 (aka None) are supported"));
            }
            if (value != MP_OBJ_SENTINEL) {
                #if MICROPY_PY_ARRAY_SLICE_ASSIGN
                // Assign
                size_t src_len = slice.stop - slice.start;
                uint8_t *src_items;
                if (mp_obj_is_type(value, &mp_type_array) ||
                    mp_obj_is_type(value, &mp_type_bytearray) ||
                    mp_obj_is_type(value, &mp_type_memoryview) ||
                    mp_obj_is_type(value, &mp_type_bytes)) {
                    mp_buffer_info_t bufinfo;
                    mp_get_buffer_raise(value, &bufinfo, MP_BUFFER_READ);
                    if (bufinfo.len != src_len) {
                        mp_raise_ValueError(translate("Slice and value different lengths."));
                    }
                    src_len = bufinfo.len;
                    src_items = bufinfo.buf;
                    if (1 != mp_binary_get_size('@', bufinfo.typecode, NULL)) {
                        mp_raise_ValueError(translate("Array values should be single bytes."));
                    }
                } else {
                    mp_raise_NotImplementedError(translate("array/bytes required on right side"));
                }

                if (!common_hal_coproc_memory_set_bytes(self, slice.start, src_items, src_len)) {
                    mp_raise_RuntimeError(translate("Unable to write"));
                }
                return mp_const_none;
                #else
                return MP_OBJ_NULL; // op not supported
                #endif
            } else {
                // Read slice.
                size_t len = slice.stop - slice.start;
                uint8_t *items = m_new(uint8_t, len);
                common_hal_coproc_memory_get_bytes(self, slice.start, items, len);
                return mp_obj_new_bytearray_by_ref(len, items);
            }
        #endif
        } else {
            // Single index rather than slice.
            size_t index = mp_get_index(self->base.type, common_hal_coproc_memory_get_length(self),
                index_in, false);
            if (value == MP_OBJ_SENTINEL) {
                // load
                uint8_t value_out;
                common_hal_coproc_memory_get_bytes(self, index, &value_out, 1);
                return MP_OBJ_NEW_SMALL_INT(value_out);
            } else {
                // store
                mp_int_t byte_value = mp_obj_get_int(value);
                mp_arg_validate_int_range(byte_value, 0, 255, MP_QSTR_bytes);

                uint8_t short_value = byte_value;
                if (!common_hal_coproc_memory_set_bytes(self, index, &short_value, 1)) {
                    mp_raise_RuntimeError(translate("Unable to write"));
                }
                return mp_const_none;
            }
        }
    }
}

const mp_obj_type_t coproc_memory_type = {
    { &mp_type_type },
    .name = MP_QSTR_CoprocMemory,
    .flags = MP_TYPE_FLAG_EXTENDED,
    .make_new = coproc_memory_make_new,
    .locals_dict = (mp_obj_t)&coproc_memory_locals_dict,
    MP_TYPE_EXTENDED_FIELDS(
        .subscr = coproc_memory_subscr,
        .unary_op = coproc_memory_unary_op,
        ),
};
