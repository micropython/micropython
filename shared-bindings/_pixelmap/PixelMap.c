// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Rose Hooper
// SPDX-FileCopyrightText: Copyright (c) 2022 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/objproperty.h"
#include "py/objtype.h"
#include "py/runtime.h"

#include "shared-bindings/_pixelmap/PixelMap.h"
#include "shared-bindings/adafruit_pixelbuf/PixelBuf.h"
#include "shared-module/_pixelmap/PixelMap.h"

//| from adafruit_pixelbuf import PixelBuf, PixelReturnType, PixelSequence, PixelType
//|
//| class PixelMap:
//|     def __init__(self, pixelbuf: PixelBuf, indices: Tuple[Union[int, Tuple[int]]]) -> None:
//|         """Construct a PixelMap object that uses the given indices of the underlying pixelbuf"""

static mp_obj_t pixelmap_pixelmap_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_pixelbuf, ARG_indices };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pixelbuf, MP_ARG_REQUIRED },
        { MP_QSTR_indices, MP_ARG_REQUIRED },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t pixelbuf = args[ARG_pixelbuf].u_obj;

    mp_obj_t native_pixelbuf = mp_obj_cast_to_native_base(pixelbuf, &pixelbuf_pixelbuf_type);
    if (!native_pixelbuf) {
        (void)mp_arg_validate_type(args[ARG_pixelbuf].u_obj, &pixelbuf_pixelbuf_type, MP_QSTR_pixelbuf);
    }
    mp_obj_assert_native_inited(native_pixelbuf);

    size_t buflen = common_hal_adafruit_pixelbuf_pixelbuf_get_len(pixelbuf);

    mp_obj_t indices = mp_arg_validate_type(args[ARG_indices].u_obj, &mp_type_tuple, MP_QSTR_indices);

    // validate indices
    size_t len;
    mp_obj_t *items;
    mp_obj_tuple_get(indices, &len, &items);
    mp_arg_validate_length_min(len, 1, MP_QSTR_items);

    for (size_t i = 0; i < len; i++) {
        mp_obj_t item = items[i];
        if (mp_obj_is_small_int(item)) {
            mp_arg_validate_index_range(MP_OBJ_SMALL_INT_VALUE(item), 0, buflen - 1, MP_QSTR_index);
        } else if (mp_obj_is_tuple_compatible(item)) {
            size_t len1;
            mp_obj_t *items1;
            mp_obj_tuple_get(item, &len1, &items1);
            for (size_t j = 0; j < len1; j++) {
                mp_obj_t item1 = items1[j];
                if (!mp_obj_is_small_int(item1)) {
                    mp_raise_TypeError(MP_ERROR_TEXT("nested index must be int"));
                }
                mp_arg_validate_index_range(MP_OBJ_SMALL_INT_VALUE(item1), 0, buflen - 1, MP_QSTR_index);
            }
        } else {
            mp_raise_TypeError(MP_ERROR_TEXT("index must be tuple or int"));
        }
    }

    pixelmap_pixelmap_obj_t *self = mp_obj_malloc(pixelmap_pixelmap_obj_t, &pixelmap_pixelmap_type);
    shared_module_pixelmap_pixelmap_construct(self, pixelbuf, indices);

    return MP_OBJ_FROM_PTR(self);
}

//|     auto_write: bool
//|     """True if updates should be automatically written"""
static mp_obj_t pixelmap_pixelmap_auto_write_get(const mp_obj_t self_in) {
    pixelmap_pixelmap_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(shared_module_pixelmap_pixelmap_auto_write_get(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(pixelmap_pixelmap_auto_write_get_obj, pixelmap_pixelmap_auto_write_get);

static mp_obj_t pixelmap_pixelmap_auto_write_set(const mp_obj_t self_in, const mp_obj_t arg) {
    pixelmap_pixelmap_obj_t *self = MP_OBJ_TO_PTR(self_in);
    shared_module_pixelmap_pixelmap_auto_write_set(self, mp_obj_is_true(arg));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(pixelmap_pixelmap_auto_write_set_obj, pixelmap_pixelmap_auto_write_set);

MP_PROPERTY_GETSET(pixelmap_pixelmap_auto_write_obj,
    (mp_obj_t)&pixelmap_pixelmap_auto_write_get_obj,
    (mp_obj_t)&pixelmap_pixelmap_auto_write_set_obj);

//|     bpp: int
//|     """The number of bytes per pixel in the buffer (read-only)"""
static mp_obj_t pixelmap_pixelmap_obj_get_bpp(mp_obj_t self_in) {
    pixelmap_pixelmap_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_adafruit_pixelbuf_pixelbuf_get_bpp(self->pixelbuf));
}
MP_DEFINE_CONST_FUN_OBJ_1(pixelmap_pixelmap_get_bpp_obj, pixelmap_pixelmap_obj_get_bpp);

MP_PROPERTY_GETTER(pixelmap_pixelmap_bpp_obj,
    (mp_obj_t)&pixelmap_pixelmap_get_bpp_obj);

//|     byteorder: str
//|     """byteorder string for the buffer (read-only)"""
static mp_obj_t pixelmap_pixelmap_obj_get_byteorder(mp_obj_t self_in) {
    pixelmap_pixelmap_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_adafruit_pixelbuf_pixelbuf_get_byteorder_string(self->pixelbuf);
}
MP_DEFINE_CONST_FUN_OBJ_1(pixelmap_pixelmap_get_byteorder, pixelmap_pixelmap_obj_get_byteorder);
MP_PROPERTY_GETTER(pixelmap_pixelmap_byteorder_obj,
    (mp_obj_t)&pixelmap_pixelmap_get_byteorder);

//|
//|     def fill(self, color: PixelType) -> None:
//|         """Fill all the pixels in the map with the given color"""
static mp_obj_t pixelmap_pixelmap_fill(const mp_obj_t self_in, const mp_obj_t color) {
    pixelmap_pixelmap_obj_t *self = MP_OBJ_TO_PTR(self_in);

    shared_module_pixelmap_pixelmap_fill(self, color);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(pixelmap_pixelmap_fill_obj, pixelmap_pixelmap_fill);

//|
//|     def indices(self, index: int) -> Tuple[int]:
//|         """Return the PixelBuf indices for a PixelMap index"""
static mp_obj_t pixelmap_pixelmap_indices(const mp_obj_t self_in, const mp_obj_t index) {
    pixelmap_pixelmap_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return shared_module_pixelmap_pixelmap_indices(self, mp_obj_get_int(index));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(pixelmap_pixelmap_indices_obj, pixelmap_pixelmap_indices);


//|     @overload
//|     def __getitem__(self, index: slice) -> PixelReturnSequence:
//|         """Retrieve the value of the underlying pixels."""
//|         ...
//|
//|     @overload
//|     def __getitem__(self, index: int) -> PixelReturnType:
//|         """Retrieve the value of one of the underlying pixels at 'index'."""
//|         ...
//|
//|     @overload
//|     def __setitem__(self, index: slice, value: PixelSequence) -> None: ...
//|     @overload
//|     def __setitem__(self, index: int, value: PixelType) -> None:
//|         """Sets the pixel value at the given index.  Value can either be a tuple or integer.  Tuples are
//|         The individual (Red, Green, Blue[, White]) values between 0 and 255.  If given an integer, the
//|         red, green and blue values are packed into the lower three bytes (0xRRGGBB).
//|         For RGBW byteorders, if given only RGB values either as an int or as a tuple, the white value
//|         is used instead when the red, green, and blue values are the same."""
//|         ...
static mp_obj_t pixelmap_pixelmap_subscr(mp_obj_t self_in, mp_obj_t index_in, mp_obj_t value) {
    pixelmap_pixelmap_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (value == MP_OBJ_NULL) {
        // delete
        return MP_OBJ_NULL; // op not supported
    }

    if (0) {
    #if MICROPY_PY_BUILTINS_SLICE
    } else if (mp_obj_is_type(index_in, &mp_type_slice)) {
        mp_bound_slice_t slice;
        mp_seq_get_fast_slice_indexes(self->len, index_in, &slice);
        size_t slice_len;
        if (slice.step > 0) {
            slice_len = slice.stop - slice.start;
        } else {
            slice_len = 1 + slice.start - slice.stop;
        }
        if (slice.step > 1 || slice.step < -1) {
            size_t step = slice.step > 0 ? slice.step : slice.step * -1;
            slice_len = (slice_len / step) + (slice_len % step ? 1 : 0);
        }

        if (value == MP_OBJ_SENTINEL) { // Get
            return shared_module_pixelmap_pixelmap_getslice(self, slice, slice_len);
        } else { // Set
            shared_module_pixelmap_pixelmap_setslice(self, value, slice, slice_len);
            return mp_const_none;
        }
    #endif
    } else { // single index
        int index = mp_obj_get_int(index_in);

        if (value == MP_OBJ_SENTINEL) { // Get
            return shared_module_pixelmap_pixelmap_getitem(self, index);
        } else {
            shared_module_pixelmap_pixelmap_setitem(self, mp_obj_get_int(index_in), value);
            return mp_const_none;
        }
    }
}

//|     def __len__(self) -> int:
//|         """Length of the map"""
static mp_obj_t pixelmap_pixelmap_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    pixelmap_pixelmap_obj_t *self = MP_OBJ_TO_PTR(self_in);
    switch (op) {
        case MP_UNARY_OP_BOOL:
            return mp_const_true;
        case MP_UNARY_OP_LEN:
            return MP_OBJ_NEW_SMALL_INT(self->len);
        default:
            return MP_OBJ_NULL;      // op not supported
    }
}

//|     def show(self) -> None:
//|         """Transmits the color data to the pixels so that they are shown. This is done automatically
//|         when `auto_write` is True."""
//|         ...
//|

static mp_obj_t pixelmap_pixelmap_show(mp_obj_t self_in) {
    pixelmap_pixelmap_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_adafruit_pixelbuf_pixelbuf_show(self->pixelbuf);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(pixelmap_pixelmap_show_obj, pixelmap_pixelmap_show);

static const mp_rom_map_elem_t pixelmap_pixelmap_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_auto_write), MP_ROM_PTR(&pixelmap_pixelmap_auto_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_bpp), MP_ROM_PTR(&pixelmap_pixelmap_bpp_obj) },
    { MP_ROM_QSTR(MP_QSTR_byteorder), MP_ROM_PTR(&pixelmap_pixelmap_byteorder_obj) },
    { MP_ROM_QSTR(MP_QSTR_fill), MP_ROM_PTR(&pixelmap_pixelmap_fill_obj) },
    { MP_ROM_QSTR(MP_QSTR_indices), MP_ROM_PTR(&pixelmap_pixelmap_indices_obj) },
    { MP_ROM_QSTR(MP_QSTR_show), MP_ROM_PTR(&pixelmap_pixelmap_show_obj) },
};

static MP_DEFINE_CONST_DICT(pixelmap_pixelmap_locals_dict, pixelmap_pixelmap_locals_dict_table);


MP_DEFINE_CONST_OBJ_TYPE(
    pixelmap_pixelmap_type,
    MP_QSTR_PixelMap,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    locals_dict, &pixelmap_pixelmap_locals_dict,
    make_new, pixelmap_pixelmap_make_new,
    subscr, pixelmap_pixelmap_subscr,
    unary_op, pixelmap_pixelmap_unary_op
    );
