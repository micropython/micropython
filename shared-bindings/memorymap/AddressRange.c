/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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
#include "py/runtime.h"
#include "py/runtime0.h"
#include "shared-bindings/memorymap/AddressRange.h"

//| class AddressRange:
//|     r"""Presents a range of addresses as a bytearray.
//|
//|     The addresses may access memory or memory mapped peripherals.
//|
//|     Some address ranges may be protected by CircuitPython to prevent errors.
//|     An exception will be raised when constructing an AddressRange for an
//|     invalid or protected address.
//|
//|     Multiple AddressRanges may overlap. There is no "claiming" of addresses.
//|
//|     Example usage on ESP32-S2::
//|
//|        import memorymap
//|        rtc_slow_mem = memorymap.AddressRange(start=0x50000000, length=0x2000)
//|        rtc_slow_mem[0:3] = b"\xcc\x10\x00"
//|
//|     Example I/O register usage on RP2040::
//|
//|        import binascii
//|        import board
//|        import digitalio
//|        import memorymap
//|
//|        def rp2040_set_pad_drive(p, d):
//|            pads_bank0 = memorymap.AddressRange(start=0x4001C000, length=0x4000)
//|            pad_ctrl = int.from_bytes(pads_bank0[p*4+4:p*4+8], "little")
//|            # Pad control register is updated using an MP-safe atomic XOR
//|            pad_ctrl ^= (d << 4)
//|            pad_ctrl &= 0x00000030
//|            pads_bank0[p*4+0x3004:p*4+0x3008] = pad_ctrl.to_bytes(4, "little")
//|
//|        def rp2040_get_pad_drive(p):
//|            pads_bank0 = memorymap.AddressRange(start=0x4001C000, length=0x4000)
//|            pad_ctrl = int.from_bytes(pads_bank0[p*4+4:p*4+8], "little")
//|            return (pad_ctrl >> 4) & 0x3
//|
//|        # set GPIO16 pad drive strength to 12 mA
//|        rp2040_set_pad_drive(16, 3)
//|
//|        # print GPIO16 pad drive strength
//|        print(rp2040_get_pad_drive(16))
//|     """
//|

//|     def __init__(self, *, start, length) -> None:
//|         """Constructs an address range starting at ``start`` and ending at
//|         ``start + length``. An exception will be raised if any of the
//|         addresses are invalid or protected."""
//|         ...
STATIC mp_obj_t memorymap_addressrange_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_start, ARG_length };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_start, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_length, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Argument start is a pointer into the address map, so we validate it here because a
    // signed int argument will overflow if it is in the upper half of the map.
    size_t start;
    if (mp_obj_is_small_int(args[ARG_start].u_obj)) {
        start = MP_OBJ_SMALL_INT_VALUE(args[ARG_start].u_obj);
    } else if (mp_obj_is_exact_type(args[ARG_start].u_obj, &mp_type_int)) {
        start = mp_obj_int_get_uint_checked(args[ARG_start].u_obj);
    } else {
        mp_obj_t arg = mp_unary_op(MP_UNARY_OP_INT_MAYBE, args[ARG_start].u_obj);
        start = mp_obj_int_get_uint_checked(arg);
    }
    size_t length =
        mp_arg_validate_int_min(args[ARG_length].u_int, 1, MP_QSTR_length);
    // Check for address range wrap here as this can break port-specific code due to size_t overflow.
    if (start + length - 1 < start) {
        mp_raise_ValueError(MP_ERROR_TEXT("Address range wraps around"));
    }

    memorymap_addressrange_obj_t *self = mp_obj_malloc(memorymap_addressrange_obj_t, &memorymap_addressrange_type);

    common_hal_memorymap_addressrange_construct(self, (uint8_t *)start, length);

    return MP_OBJ_FROM_PTR(self);
}

//|     def __bool__(self) -> bool: ...
//|     def __len__(self) -> int:
//|         """Return the length. This is used by (`len`)"""
//|         ...
STATIC mp_obj_t memorymap_addressrange_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    memorymap_addressrange_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint16_t len = common_hal_memorymap_addressrange_get_length(self);
    switch (op) {
        case MP_UNARY_OP_BOOL:
            return mp_obj_new_bool(len != 0);
        case MP_UNARY_OP_LEN:
            return MP_OBJ_NEW_SMALL_INT(len);
        default:
            return MP_OBJ_NULL;      // op not supported
    }
}

STATIC const mp_rom_map_elem_t memorymap_addressrange_locals_dict_table[] = {
};

STATIC MP_DEFINE_CONST_DICT(memorymap_addressrange_locals_dict, memorymap_addressrange_locals_dict_table);

//|     @overload
//|     def __getitem__(self, index: slice) -> bytearray: ...
//|     @overload
//|     def __getitem__(self, index: int) -> int:
//|         """Returns the value(s) at the given index.
//|
//|         1, 2, 4 and 8 byte aligned reads will be done in one transaction
//|         when possible.
//|         All others may use multiple transactions."""
//|         ...
//|
//|     @overload
//|     def __setitem__(self, index: slice, value: ReadableBuffer) -> None: ...
//|     @overload
//|     def __setitem__(self, index: int, value: int) -> None:
//|         """Set the value(s) at the given index.
//|
//|         1, 2, 4 and 8 byte aligned writes will be done in one transaction
//|         when possible.
//|         All others may use multiple transactions."""
//|         ...
//|
STATIC mp_obj_t memorymap_addressrange_subscr(mp_obj_t self_in, mp_obj_t index_in, mp_obj_t value) {
    if (value == MP_OBJ_NULL) {
        // delete item
        // slice deletion
        return MP_OBJ_NULL; // op not supported
    } else {
        memorymap_addressrange_obj_t *self = MP_OBJ_TO_PTR(self_in);
        if (0) {
        #if MICROPY_PY_BUILTINS_SLICE
        } else if (mp_obj_is_type(index_in, &mp_type_slice)) {
            mp_bound_slice_t slice;
            if (!mp_seq_get_fast_slice_indexes(common_hal_memorymap_addressrange_get_length(self), index_in, &slice)) {
                mp_raise_NotImplementedError(MP_ERROR_TEXT("only slices with step=1 (aka None) are supported"));
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
                        mp_raise_ValueError(MP_ERROR_TEXT("Slice and value different lengths."));
                    }
                    src_len = bufinfo.len;
                    src_items = bufinfo.buf;
                    if (1 != mp_binary_get_size('@', bufinfo.typecode, NULL)) {
                        mp_raise_ValueError(MP_ERROR_TEXT("Array values should be single bytes."));
                    }
                } else {
                    mp_raise_NotImplementedError(MP_ERROR_TEXT("array/bytes required on right side"));
                }

                common_hal_memorymap_addressrange_set_bytes(self, slice.start, src_items, src_len);
                return mp_const_none;
                #else
                return MP_OBJ_NULL; // op not supported
                #endif
            } else {
                // Read slice.
                size_t len = slice.stop - slice.start;
                uint8_t *items = m_new(uint8_t, len);
                common_hal_memorymap_addressrange_get_bytes(self, slice.start, len, items);
                return mp_obj_new_bytearray_by_ref(len, items);
            }
        #endif
        } else {
            // Single index rather than slice.
            size_t index = mp_get_index(self->base.type, common_hal_memorymap_addressrange_get_length(self),
                index_in, false);
            if (value == MP_OBJ_SENTINEL) {
                // load
                uint8_t value_out;
                common_hal_memorymap_addressrange_get_bytes(self, index, 1, &value_out);
                return MP_OBJ_NEW_SMALL_INT(value_out);
            } else {
                // store
                mp_int_t byte_value = mp_obj_get_int(value);
                mp_arg_validate_int_range(byte_value, 0, 255, MP_QSTR_bytes);

                uint8_t short_value = byte_value;
                common_hal_memorymap_addressrange_set_bytes(self, index, &short_value, 1);
                return mp_const_none;
            }
        }
    }
}

MP_DEFINE_CONST_OBJ_TYPE(
    memorymap_addressrange_type,
    MP_QSTR_AddressRange,
    MP_TYPE_FLAG_NONE,
    make_new, memorymap_addressrange_make_new,
    locals_dict, (mp_obj_t)&memorymap_addressrange_locals_dict,
    subscr, memorymap_addressrange_subscr,
    unary_op, memorymap_addressrange_unary_op
    );
