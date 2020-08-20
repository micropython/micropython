/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#include "shared-bindings/displayio/Bitmap.h"

#include <stdint.h>

#include "lib/utils/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"
#include "supervisor/shared/translate.h"

//| class Bitmap:
//|     """Stores values of a certain size in a 2D array"""
//|
//|     def __init__(self, width: int, height: int, value_count: int):
//|         """Create a Bitmap object with the given fixed size. Each pixel stores a value that is used to
//|         index into a corresponding palette. This enables differently colored sprites to share the
//|         underlying Bitmap. value_count is used to minimize the memory used to store the Bitmap.
//|
//|         :param int width: The number of values wide
//|         :param int height: The number of values high
//|         :param int value_count: The number of possible pixel values."""
//|         ...
//|
STATIC mp_obj_t displayio_bitmap_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_arg_check_num(n_args, kw_args, 3, 3, false);
    uint32_t width = mp_obj_get_int(pos_args[0]);
    uint32_t height = mp_obj_get_int(pos_args[1]);
    uint32_t value_count = mp_obj_get_int(pos_args[2]);
    uint32_t bits = 1;

    if (value_count == 0) {
        mp_raise_ValueError(translate("value_count must be > 0"));
    }
    while ((value_count - 1) >> bits) {
        if (bits < 8) {
            bits <<= 1;
        } else {
            bits += 8;
        }
    }

    displayio_bitmap_t *self = m_new_obj(displayio_bitmap_t);
    self->base.type = &displayio_bitmap_type;
    common_hal_displayio_bitmap_construct(self, width, height, bits);

    return MP_OBJ_FROM_PTR(self);
}
//|     width: Any = ...
//|     """Width of the bitmap. (read only)"""
//|
STATIC mp_obj_t displayio_bitmap_obj_get_width(mp_obj_t self_in) {
    displayio_bitmap_t *self = MP_OBJ_TO_PTR(self_in);

    return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_bitmap_get_width(self));
}

MP_DEFINE_CONST_FUN_OBJ_1(displayio_bitmap_get_width_obj, displayio_bitmap_obj_get_width);

const mp_obj_property_t displayio_bitmap_width_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&displayio_bitmap_get_width_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|     height: Any = ...
//|     """Height of the bitmap. (read only)"""
//|
STATIC mp_obj_t displayio_bitmap_obj_get_height(mp_obj_t self_in) {
    displayio_bitmap_t *self = MP_OBJ_TO_PTR(self_in);

    return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_bitmap_get_height(self));
}

MP_DEFINE_CONST_FUN_OBJ_1(displayio_bitmap_get_height_obj, displayio_bitmap_obj_get_height);

const mp_obj_property_t displayio_bitmap_height_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&displayio_bitmap_get_height_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|     def __getitem__(self, index: Any) -> Any:
//|         """Returns the value at the given index. The index can either be an x,y tuple or an int equal
//|         to ``y * width + x``.
//|
//|         This allows you to::
//|
//|           print(bitmap[0,1])"""
//|         ...
//|
//|     def __setitem__(self, index: Any, value: Any) -> Any:
//|         """Sets the value at the given index. The index can either be an x,y tuple or an int equal
//|         to ``y * width + x``.
//|
//|         This allows you to::
//|
//|           bitmap[0,1] = 3"""
//|         ...
//|
STATIC mp_obj_t bitmap_subscr(mp_obj_t self_in, mp_obj_t index_obj, mp_obj_t value_obj) {
    if (value_obj == mp_const_none) {
        // delete item
        mp_raise_AttributeError(translate("Cannot delete values"));
        return mp_const_none;
    }

    displayio_bitmap_t *self = MP_OBJ_TO_PTR(self_in);

    if (MP_OBJ_IS_TYPE(index_obj, &mp_type_slice)) {
        // TODO(tannewt): Implement subscr after slices support start, stop and step tuples.
        mp_raise_NotImplementedError(translate("Slices not supported"));
        return mp_const_none;
    }

    uint16_t x = 0;
    uint16_t y = 0;
    if (MP_OBJ_IS_SMALL_INT(index_obj)) {
        mp_int_t i = MP_OBJ_SMALL_INT_VALUE(index_obj);
        uint16_t width = common_hal_displayio_bitmap_get_width(self);
        x = i % width;
        y = i / width;
    } else {
        mp_obj_t* items;
        mp_obj_get_array_fixed_n(index_obj, 2, &items);
        x = mp_obj_get_int(items[0]);
        y = mp_obj_get_int(items[1]);
        if (x >= common_hal_displayio_bitmap_get_width(self) || y >= common_hal_displayio_bitmap_get_height(self)) {
            mp_raise_IndexError(translate("pixel coordinates out of bounds"));
        }
    }

    if (value_obj == MP_OBJ_SENTINEL) {
        // load
        return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_bitmap_get_pixel(self, x, y));
    } else {
        mp_int_t value = mp_obj_get_int(value_obj);
        if (value >= 1 << common_hal_displayio_bitmap_get_bits_per_value(self)) {
            mp_raise_ValueError(translate("pixel value requires too many bits"));
        }
        common_hal_displayio_bitmap_set_pixel(self, x, y, value);
    }
    return mp_const_none;
}

//|     def fill(self, value: Any) -> Any:
//|         """Fills the bitmap with the supplied palette index value."""
//|         ...
//|
STATIC mp_obj_t displayio_bitmap_obj_fill(mp_obj_t self_in, mp_obj_t value_obj) {
    displayio_bitmap_t *self = MP_OBJ_TO_PTR(self_in);

    mp_int_t value = mp_obj_get_int(value_obj);
    if (value >= 1 << common_hal_displayio_bitmap_get_bits_per_value(self)) {
            mp_raise_ValueError(translate("pixel value requires too many bits"));
    }
    common_hal_displayio_bitmap_fill(self, value);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(displayio_bitmap_fill_obj, displayio_bitmap_obj_fill);

STATIC const mp_rom_map_elem_t displayio_bitmap_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_PTR(&displayio_bitmap_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&displayio_bitmap_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_fill), MP_ROM_PTR(&displayio_bitmap_fill_obj) },

};
STATIC MP_DEFINE_CONST_DICT(displayio_bitmap_locals_dict, displayio_bitmap_locals_dict_table);

const mp_obj_type_t displayio_bitmap_type = {
    { &mp_type_type },
    .name = MP_QSTR_Bitmap,
    .make_new = displayio_bitmap_make_new,
    .subscr = bitmap_subscr,
    .locals_dict = (mp_obj_dict_t*)&displayio_bitmap_locals_dict,
};
