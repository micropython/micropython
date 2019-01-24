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

#include "shared-bindings/displayio/TileGrid.h"

#include <stdint.h>

#include "lib/utils/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/displayio/Bitmap.h"
#include "shared-bindings/displayio/ColorConverter.h"
#include "shared-bindings/displayio/OnDiskBitmap.h"
#include "shared-bindings/displayio/Palette.h"
#include "shared-bindings/displayio/Shape.h"
#include "supervisor/shared/translate.h"

static void unpack_position(mp_obj_t position_obj, int16_t* x, int16_t* y) {
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
//| :class:`TileGrid` -- A grid of tiles sourced out of one bitmap
//| ==========================================================================
//|
//| Position a grid of tiles sourced from a bitmap and pixel_shader combination. Multiple grids
//| can share bitmaps and pixel shaders.
//|
//| A single tile grid is also known as a Sprite.
//|
//| .. warning:: This will be changed before 4.0.0. Consider it very experimental.
//|
//| .. class:: TileGrid(bitmap, *, pixel_shader, position, width=1, height=1, tile_width=None, tile_height=None, default_tile=0)
//|
//|   Create a TileGrid object. The bitmap is source for 2d pixels. The pixel_shader is used to
//|   convert the value and its location to a display native pixel color. This may be a simple color
//|   palette lookup, a gradient, a pattern or a color transformer.
//|
//|   tile_width and tile_height match the height of the bitmap by default.
//|
//|   :param displayio.Bitmap bitmap: The bitmap storing one or more tiles.
//|   :param displayio.Palette pixel_shader: The pixel shader that produces colors from values
//|   :param tuple position: Upper left corner of the grid
//|   :param int width: Width of the grid in tiles.
//|   :param int height: Height of the grid in tiles.
//|   :param int tile_width: Width of a single tile in pixels. Defaults to the full Bitmap and must evenly divide into the Bitmap's dimensions.
//|   :param int tile_height: Height of a single tile in pixels. Defaults to the full Bitmap and must evenly divide into the Bitmap's dimensions.
//|   :param in default_tile: Default tile index to show.
//|
STATIC mp_obj_t displayio_tilegrid_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_bitmap, ARG_pixel_shader, ARG_position, ARG_width, ARG_height, ARG_tile_width, ARG_tile_height, ARG_default_tile };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bitmap, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_pixel_shader, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_position, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_width, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 1} },
        { MP_QSTR_height, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 1} },
        { MP_QSTR_tile_width, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
        { MP_QSTR_tile_height, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
        { MP_QSTR_default_tile, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t bitmap = args[ARG_bitmap].u_obj;

    uint16_t bitmap_width;
    uint16_t bitmap_height;
    mp_obj_t native = mp_instance_cast_to_native_base(bitmap, &displayio_shape_type);
    if (native != MP_OBJ_NULL) {
        displayio_shape_t* bmp = MP_OBJ_TO_PTR(native);
        bitmap_width = bmp->width;
        bitmap_height = bmp->height;
    } else if (MP_OBJ_IS_TYPE(bitmap, &displayio_bitmap_type)) {
        displayio_bitmap_t* bmp = MP_OBJ_TO_PTR(bitmap);
        native = bitmap;
        bitmap_width = bmp->width;
        bitmap_height = bmp->height;
    } else if (MP_OBJ_IS_TYPE(bitmap, &displayio_ondiskbitmap_type)) {
        displayio_ondiskbitmap_t* bmp = MP_OBJ_TO_PTR(bitmap);
        native = bitmap;
        bitmap_width = bmp->width;
        bitmap_height = bmp->height;
    } else {
        mp_raise_TypeError(translate("unsupported bitmap type"));
    }
    uint16_t tile_width = args[ARG_tile_width].u_int;
    if (tile_width == 0) {
        tile_width = bitmap_width;
    }
    uint16_t tile_height = args[ARG_tile_height].u_int;
    if (tile_height == 0) {
        tile_height = bitmap_height;
    }
    if (bitmap_width % tile_width != 0) {
        mp_raise_ValueError(translate("Tile width must exactly divide bitmap width"));
    }
    if (bitmap_height % tile_height != 0) {
        mp_raise_ValueError(translate("Tile height must exactly divide bitmap height"));
    }

    int16_t x = 0;
    int16_t y = 0;
    mp_obj_t position_obj = args[ARG_position].u_obj;
    unpack_position(position_obj, &x, &y);

    displayio_tilegrid_t *self = m_new_obj(displayio_tilegrid_t);
    self->base.type = &displayio_tilegrid_type;
    common_hal_displayio_tilegrid_construct(self, native, args[ARG_pixel_shader].u_obj,
        args[ARG_width].u_int, args[ARG_height].u_int, tile_width, tile_height, x, y,
        args[ARG_default_tile].u_int);
    return MP_OBJ_FROM_PTR(self);
}

//|   .. attribute:: position
//|
//|     The position of the top-left corner of the tilegrid.
//|
STATIC mp_obj_t displayio_tilegrid_obj_get_position(mp_obj_t self_in) {
    displayio_tilegrid_t *self = MP_OBJ_TO_PTR(self_in);
    int16_t x;
    int16_t y;
    common_hal_displayio_tilegrid_get_position(self, &x, &y);

    mp_obj_t coords[2];
    coords[0] = mp_obj_new_int(x);
    coords[1] = mp_obj_new_int(y);

    return mp_obj_new_tuple(2, coords);
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_tilegrid_get_position_obj, displayio_tilegrid_obj_get_position);

STATIC mp_obj_t displayio_tilegrid_obj_set_position(mp_obj_t self_in, mp_obj_t value) {
    displayio_tilegrid_t *self = MP_OBJ_TO_PTR(self_in);

    int16_t x = 0;
    int16_t y = 0;
    unpack_position(value, &x, &y);

    common_hal_displayio_tilegrid_set_position(self, x, y);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(displayio_tilegrid_set_position_obj, displayio_tilegrid_obj_set_position);

const mp_obj_property_t displayio_tilegrid_position_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&displayio_tilegrid_get_position_obj,
              (mp_obj_t)&displayio_tilegrid_set_position_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|   .. attribute:: pixel_shader
//|
//|     The pixel shader of the tilegrid.
//|
STATIC mp_obj_t displayio_tilegrid_obj_get_pixel_shader(mp_obj_t self_in) {
    displayio_tilegrid_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_displayio_tilegrid_get_pixel_shader(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_tilegrid_get_pixel_shader_obj, displayio_tilegrid_obj_get_pixel_shader);

STATIC mp_obj_t displayio_tilegrid_obj_set_pixel_shader(mp_obj_t self_in, mp_obj_t pixel_shader) {
    displayio_tilegrid_t *self = MP_OBJ_TO_PTR(self_in);
    if (!MP_OBJ_IS_TYPE(pixel_shader, &displayio_palette_type) && !MP_OBJ_IS_TYPE(pixel_shader, &displayio_colorconverter_type)) {
        mp_raise_TypeError(translate("pixel_shader must be displayio.Palette or displayio.ColorConverter"));
    }

    common_hal_displayio_tilegrid_set_pixel_shader(self, pixel_shader);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(displayio_tilegrid_set_pixel_shader_obj, displayio_tilegrid_obj_set_pixel_shader);

const mp_obj_property_t displayio_tilegrid_pixel_shader_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&displayio_tilegrid_get_pixel_shader_obj,
              (mp_obj_t)&displayio_tilegrid_set_pixel_shader_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC const mp_rom_map_elem_t displayio_tilegrid_locals_dict_table[] = {
    // Properties
    { MP_ROM_QSTR(MP_QSTR_position),          MP_ROM_PTR(&displayio_tilegrid_position_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel_shader),          MP_ROM_PTR(&displayio_tilegrid_pixel_shader_obj) },
};
STATIC MP_DEFINE_CONST_DICT(displayio_tilegrid_locals_dict, displayio_tilegrid_locals_dict_table);

const mp_obj_type_t displayio_tilegrid_type = {
    { &mp_type_type },
    .name = MP_QSTR_Sprite,
    .make_new = displayio_tilegrid_make_new,
    .locals_dict = (mp_obj_dict_t*)&displayio_tilegrid_locals_dict,
};
