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

#include "shared-bindings/displayio/Palette.h"

#include <stdint.h>

#include "lib/utils/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"
#include "supervisor/shared/translate.h"

//| class Palette:
//|     """Map a pixel palette_index to a full color. Colors are transformed to the display's format internally to
//|     save memory."""
//|
//|     def __init__(self, color_count: int) -> None:
//|         """Create a Palette object to store a set number of colors.
//|
//|         :param int color_count: The number of colors in the Palette"""
//|         ...
//|
// TODO(tannewt): Add support for other color formats.
// TODO(tannewt): Add support for 8-bit alpha blending.
//|
STATIC mp_obj_t displayio_palette_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_color_count };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_color_count, MP_ARG_REQUIRED | MP_ARG_INT },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    displayio_palette_t *self = m_new_obj(displayio_palette_t);
    self->base.type = &displayio_palette_type;
    common_hal_displayio_palette_construct(self, args[ARG_color_count].u_int);

    return MP_OBJ_FROM_PTR(self);
}

//|     def __bool__(self) -> bool:
//|         ...
//|
//|     def __len__(self) -> int:
//|         """Returns the number of colors in a Palette"""
//|         ...
//|
STATIC mp_obj_t group_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    displayio_palette_t *self = MP_OBJ_TO_PTR(self_in);
    switch (op) {
        case MP_UNARY_OP_BOOL:
            return mp_const_true;
        case MP_UNARY_OP_LEN:
            return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_palette_get_len(self));
        default:
            return MP_OBJ_NULL;      // op not supported
    }
}

//|     def __getitem__(self, index: int) -> Optional[int]:
//|         r"""Return the pixel color at the given index as an integer."""
//|         ...
//|
//|     def __setitem__(self, index: int, value: Union[int, ReadableBuffer, Tuple[int, int, int]]) -> None:
//|         r"""Sets the pixel color at the given index. The index should be an integer in the range 0 to color_count-1.
//|
//|         The value argument represents a color, and can be from 0x000000 to 0xFFFFFF (to represent an RGB value).
//|         Value can be an int, bytes (3 bytes (RGB) or 4 bytes (RGB + pad byte)), bytearray,
//|         or a tuple or list of 3 integers.
//|
//|         This allows you to::
//|
//|           palette[0] = 0xFFFFFF                     # set using an integer
//|           palette[1] = b'\xff\xff\x00'              # set using 3 bytes
//|           palette[2] = b'\xff\xff\x00\x00'          # set using 4 bytes
//|           palette[3] = bytearray(b'\x00\x00\xFF')   # set using a bytearay of 3 or 4 bytes
//|           palette[4] = (10, 20, 30)                 # set using a tuple of 3 integers"""
//|         ...
//|
STATIC mp_obj_t palette_subscr(mp_obj_t self_in, mp_obj_t index_in, mp_obj_t value) {
    if (value == MP_OBJ_NULL) {
        // delete item
        return MP_OBJ_NULL; // op not supported
    }
    // Slicing not supported. Use a duplicate Palette to swap multiple colors atomically.
    if (mp_obj_is_type(index_in, &mp_type_slice)) {
        return MP_OBJ_NULL;
    }
    displayio_palette_t *self = MP_OBJ_TO_PTR(self_in);
    size_t index = mp_get_index(&displayio_palette_type, self->color_count, index_in, false);
    // index read
    if (value == MP_OBJ_SENTINEL) {
        return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_palette_get_color(self, index));
    }

    // Convert a tuple or list to a bytearray.
    if (mp_obj_is_type(value, &mp_type_tuple) ||
        mp_obj_is_type(value, &mp_type_list)) {
        value = mp_type_bytes.make_new(&mp_type_bytes, 1, &value, NULL);
    }

    uint32_t color;
    mp_int_t int_value;
    mp_buffer_info_t bufinfo;
    if (mp_get_buffer(value, &bufinfo, MP_BUFFER_READ)) {
        if (bufinfo.typecode != 'b' && bufinfo.typecode != 'B' && bufinfo.typecode != BYTEARRAY_TYPECODE) {
            mp_raise_ValueError(translate("color buffer must be a bytearray or array of type 'b' or 'B'"));
        }
        uint8_t *buf = bufinfo.buf;
        if (bufinfo.len == 3 || bufinfo.len == 4) {
            color = buf[0] << 16 | buf[1] << 8 | buf[2];
        } else {
            mp_raise_ValueError(translate("color buffer must be 3 bytes (RGB) or 4 bytes (RGB + pad byte)"));
        }
    } else if (mp_obj_get_int_maybe(value, &int_value)) {
        if (int_value < 0 || int_value > 0xffffff) {
            mp_raise_TypeError(translate("color must be between 0x000000 and 0xffffff"));
        }
        color = int_value;
    } else {
        mp_raise_TypeError(translate("color buffer must be a buffer, tuple, list, or int"));
    }
    common_hal_displayio_palette_set_color(self, index, color);
    return mp_const_none;
}

//|     def make_transparent(self, palette_index: int) -> None:
//|         ...
//|
STATIC mp_obj_t displayio_palette_obj_make_transparent(mp_obj_t self_in, mp_obj_t palette_index_obj) {
    displayio_palette_t *self = MP_OBJ_TO_PTR(self_in);

    mp_int_t palette_index;
    if (!mp_obj_get_int_maybe(palette_index_obj, &palette_index)) {
        mp_raise_ValueError(translate("palette_index should be an int"));
    }
    common_hal_displayio_palette_make_transparent(self, palette_index);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(displayio_palette_make_transparent_obj, displayio_palette_obj_make_transparent);

//|     def make_opaque(self, palette_index: int) -> None:
//|         ...
//|
STATIC mp_obj_t displayio_palette_obj_make_opaque(mp_obj_t self_in, mp_obj_t palette_index_obj) {
    displayio_palette_t *self = MP_OBJ_TO_PTR(self_in);

    mp_int_t palette_index;
    if (!mp_obj_get_int_maybe(palette_index_obj, &palette_index)) {
        mp_raise_ValueError(translate("palette_index should be an int"));
    }
    common_hal_displayio_palette_make_opaque(self, palette_index);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(displayio_palette_make_opaque_obj, displayio_palette_obj_make_opaque);

//|     def is_transparent(self, palette_index: int) -> bool:
//|         """Returns `True` if the palette index is transparent.  Returns `False` if opaque."""
//|         ...
//|
STATIC mp_obj_t displayio_palette_obj_is_transparent(mp_obj_t self_in, mp_obj_t palette_index_obj) {
    displayio_palette_t *self = MP_OBJ_TO_PTR(self_in);

    mp_int_t palette_index;
    if (!mp_obj_get_int_maybe(palette_index_obj, &palette_index)) {
        mp_raise_ValueError(translate("palette_index should be an int"));
    }
    return mp_obj_new_bool(common_hal_displayio_palette_is_transparent(self, palette_index));
}
MP_DEFINE_CONST_FUN_OBJ_2(displayio_palette_is_transparent_obj, displayio_palette_obj_is_transparent);

STATIC const mp_rom_map_elem_t displayio_palette_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_make_transparent), MP_ROM_PTR(&displayio_palette_make_transparent_obj) },
    { MP_ROM_QSTR(MP_QSTR_make_opaque), MP_ROM_PTR(&displayio_palette_make_opaque_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_transparent), MP_ROM_PTR(&displayio_palette_is_transparent_obj) },
};
STATIC MP_DEFINE_CONST_DICT(displayio_palette_locals_dict, displayio_palette_locals_dict_table);

const mp_obj_type_t displayio_palette_type = {
    { &mp_type_type },
    .flags = MP_TYPE_FLAG_EXTENDED,
    .name = MP_QSTR_Palette,
    .make_new = displayio_palette_make_new,
    .locals_dict = (mp_obj_dict_t *)&displayio_palette_locals_dict,
    MP_TYPE_EXTENDED_FIELDS(
        .subscr = palette_subscr,
        .unary_op = group_unary_op,
        .getiter = mp_obj_new_generic_iterator,
        ),
};
