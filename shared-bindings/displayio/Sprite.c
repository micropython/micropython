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

#include "shared-bindings/displayio/Sprite.h"

#include <stdint.h>

#include "lib/utils/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/displayio/Bitmap.h"
#include "supervisor/shared/translate.h"

void unpack_position(mp_obj_t position_obj, int16_t* x, int16_t* y) {
    // TODO(tannewt): Support any value sequence such as bytearray or bytes.
    mp_obj_tuple_t *position = MP_OBJ_TO_PTR(position_obj);
    if (MP_OBJ_IS_TYPE(position_obj, &mp_type_tuple) && position->len == 2) {
        *x = mp_obj_get_int(position->items[0]);
        *y = mp_obj_get_int(position->items[1]);
    } else if (position != mp_const_none) {
        mp_raise_TypeError(translate("position must be 2-tuple"));
    }
}

//| .. currentmodule:: displayio
//|
//| :class:`Sprite` -- A particular copy of an image to display
//| ==========================================================================
//|
//| Position a particular image and palette combination.
//|
//| .. warning:: This will be changed before 4.0.0. Consider it very experimental.
//|
//| .. class:: Sprite(bitmap, *, palette, position, width, height)
//|
//|   Create a Sprite object
//|
//|
STATIC mp_obj_t displayio_sprite_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *pos_args) {
    mp_arg_check_num(n_args, n_kw, 1, 4, true);
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, pos_args + n_args);
    enum { ARG_bitmap, ARG_palette, ARG_position, ARG_width, ARG_height };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bitmap, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_palette, MP_ARG_OBJ | MP_ARG_KW_ONLY },
        { MP_QSTR_position, MP_ARG_OBJ | MP_ARG_KW_ONLY },
        { MP_QSTR_width, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = -1} },
        { MP_QSTR_height, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = -1} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, &kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t bitmap = args[ARG_bitmap].u_obj;

    uint16_t width;
    uint16_t height;
    if (MP_OBJ_IS_TYPE(bitmap, &displayio_bitmap_type)) {
        displayio_bitmap_t* bmp = MP_OBJ_TO_PTR(bitmap);
        width = bmp->width;
        height = bmp->height;
    } else {
        mp_raise_TypeError(translate("unsupported bitmap type"));
    }
    int16_t x = 0;
    int16_t y = 0;
    mp_obj_t position_obj = args[ARG_position].u_obj;
    unpack_position(position_obj, &x, &y);

    displayio_sprite_t *self = m_new_obj(displayio_sprite_t);
    self->base.type = &displayio_sprite_type;
    common_hal_displayio_sprite_construct(self, bitmap, args[ARG_palette].u_obj,
            width, height, x, y);
    return MP_OBJ_FROM_PTR(self);
}

//|   .. attribute:: position
//|
//|     The position of the top-left corner of the sprite.
//|
STATIC mp_obj_t displayio_sprite_obj_get_position(mp_obj_t self_in) {
    displayio_sprite_t *self = MP_OBJ_TO_PTR(self_in);
    int16_t x;
    int16_t y;
    common_hal_displayio_sprite_get_position(self, &x, &y);

    mp_obj_t coords[2];
    coords[0] = mp_obj_new_int(x);
    coords[1] = mp_obj_new_int(y);

    return mp_obj_new_tuple(2, coords);
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_sprite_get_position_obj, displayio_sprite_obj_get_position);

STATIC mp_obj_t displayio_sprite_obj_set_position(mp_obj_t self_in, mp_obj_t value) {
    displayio_sprite_t *self = MP_OBJ_TO_PTR(self_in);

    int16_t x = 0;
    int16_t y = 0;
    unpack_position(value, &x, &y);

    common_hal_displayio_sprite_set_position(self, x, y);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(displayio_sprite_set_position_obj, displayio_sprite_obj_set_position);

const mp_obj_property_t displayio_sprite_position_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&displayio_sprite_get_position_obj,
              (mp_obj_t)&displayio_sprite_set_position_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|   .. attribute:: palette
//|
//|     The color palette of the sprite.
//|
STATIC mp_obj_t displayio_sprite_obj_get_palette(mp_obj_t self_in) {
    displayio_sprite_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_displayio_sprite_get_palette(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_sprite_get_palette_obj, displayio_sprite_obj_get_palette);

STATIC mp_obj_t displayio_sprite_obj_set_palette(mp_obj_t self_in, mp_obj_t palette_in) {
    displayio_sprite_t *self = MP_OBJ_TO_PTR(self_in);
    if (!MP_OBJ_IS_TYPE(palette_in, &displayio_palette_type)) {
        mp_raise_TypeError(translate("palette must be displayio.Palette"));
    }
    displayio_palette_t *palette = MP_OBJ_TO_PTR(palette_in);

    common_hal_displayio_sprite_set_palette(self, palette);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(displayio_sprite_set_palette_obj, displayio_sprite_obj_set_palette);

const mp_obj_property_t displayio_sprite_palette_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&displayio_sprite_get_palette_obj,
              (mp_obj_t)&displayio_sprite_set_palette_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC const mp_rom_map_elem_t displayio_sprite_locals_dict_table[] = {
    // Properties
    { MP_ROM_QSTR(MP_QSTR_position),          MP_ROM_PTR(&displayio_sprite_position_obj) },
    { MP_ROM_QSTR(MP_QSTR_palette),          MP_ROM_PTR(&displayio_sprite_palette_obj) },
};
STATIC MP_DEFINE_CONST_DICT(displayio_sprite_locals_dict, displayio_sprite_locals_dict_table);

const mp_obj_type_t displayio_sprite_type = {
    { &mp_type_type },
    .name = MP_QSTR_Sprite,
    .make_new = displayio_sprite_make_new,
    .locals_dict = (mp_obj_dict_t*)&displayio_sprite_locals_dict,
};
