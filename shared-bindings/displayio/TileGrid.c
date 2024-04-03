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

#include "shared/runtime/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/objtype.h"
#include "py/runtime.h"
#include "shared-bindings/displayio/Bitmap.h"
#include "shared-bindings/displayio/ColorConverter.h"
#include "shared-bindings/displayio/OnDiskBitmap.h"
#include "shared-bindings/displayio/Palette.h"

//| class TileGrid:
//|     """A grid of tiles sourced out of one bitmap
//|
//|     Position a grid of tiles sourced from a bitmap and pixel_shader combination. Multiple grids
//|     can share bitmaps and pixel shaders.
//|
//|     A single tile grid is also known as a Sprite."""
//|
//|     def __init__(
//|         self,
//|         bitmap: Union[Bitmap, OnDiskBitmap],
//|         *,
//|         pixel_shader: Union[ColorConverter, Palette],
//|         width: int = 1,
//|         height: int = 1,
//|         tile_width: Optional[int] = None,
//|         tile_height: Optional[int] = None,
//|         default_tile: int = 0,
//|         x: int = 0,
//|         y: int = 0
//|     ) -> None:
//|         """Create a TileGrid object. The bitmap is source for 2d pixels. The pixel_shader is used to
//|         convert the value and its location to a display native pixel color. This may be a simple color
//|         palette lookup, a gradient, a pattern or a color transformer.
//|
//|         To save RAM usage, tile values are only allowed in the range from 0 to 255 inclusive (single byte values).
//|
//|         tile_width and tile_height match the height of the bitmap by default.
//|
//|         :param Bitmap,OnDiskBitmap bitmap: The bitmap storing one or more tiles.
//|         :param ColorConverter,Palette pixel_shader: The pixel shader that produces colors from values
//|         :param int width: Width of the grid in tiles.
//|         :param int height: Height of the grid in tiles.
//|         :param int tile_width: Width of a single tile in pixels. Defaults to the full Bitmap and must evenly divide into the Bitmap's dimensions.
//|         :param int tile_height: Height of a single tile in pixels. Defaults to the full Bitmap and must evenly divide into the Bitmap's dimensions.
//|         :param int default_tile: Default tile index to show.
//|         :param int x: Initial x position of the left edge within the parent.
//|         :param int y: Initial y position of the top edge within the parent."""
STATIC mp_obj_t displayio_tilegrid_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_bitmap, ARG_pixel_shader, ARG_width, ARG_height, ARG_tile_width, ARG_tile_height, ARG_default_tile, ARG_x, ARG_y };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bitmap, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_pixel_shader, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_width, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 1} },
        { MP_QSTR_height, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 1} },
        { MP_QSTR_tile_width, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
        { MP_QSTR_tile_height, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
        { MP_QSTR_default_tile, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
        { MP_QSTR_x, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
        { MP_QSTR_y, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t bitmap = args[ARG_bitmap].u_obj;

    uint16_t bitmap_width;
    uint16_t bitmap_height;
    if (mp_obj_is_type(bitmap, &displayio_bitmap_type)) {
        displayio_bitmap_t *bmp = MP_OBJ_TO_PTR(bitmap);
        bitmap_width = bmp->width;
        bitmap_height = bmp->height;
    } else if (mp_obj_is_type(bitmap, &displayio_ondiskbitmap_type)) {
        displayio_ondiskbitmap_t *bmp = MP_OBJ_TO_PTR(bitmap);
        bitmap_width = bmp->width;
        bitmap_height = bmp->height;
    } else {
        mp_raise_TypeError_varg(MP_ERROR_TEXT("unsupported %q type"), MP_QSTR_bitmap);
    }
    mp_obj_t pixel_shader = args[ARG_pixel_shader].u_obj;
    if (!mp_obj_is_type(pixel_shader, &displayio_colorconverter_type) &&
        !mp_obj_is_type(pixel_shader, &displayio_palette_type)) {
        mp_raise_TypeError_varg(MP_ERROR_TEXT("unsupported %q type"), MP_QSTR_pixel_shader);
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
        mp_raise_ValueError(MP_ERROR_TEXT("Tile width must exactly divide bitmap width"));
    }
    if (bitmap_height % tile_height != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("Tile height must exactly divide bitmap height"));
    }

    int16_t x = args[ARG_x].u_int;
    int16_t y = args[ARG_y].u_int;

    displayio_tilegrid_t *self = mp_obj_malloc(displayio_tilegrid_t, &displayio_tilegrid_type);
    common_hal_displayio_tilegrid_construct(self, bitmap,
        bitmap_width / tile_width, bitmap_height / tile_height,
        pixel_shader, args[ARG_width].u_int, args[ARG_height].u_int,
        tile_width, tile_height, x, y, args[ARG_default_tile].u_int);
    return MP_OBJ_FROM_PTR(self);
}

// Helper to ensure we have the native super class instead of a subclass.
static displayio_tilegrid_t *native_tilegrid(mp_obj_t tilegrid_obj) {
    mp_obj_t native_tilegrid = mp_obj_cast_to_native_base(tilegrid_obj, &displayio_tilegrid_type);
    mp_obj_assert_native_inited(native_tilegrid);
    return MP_OBJ_TO_PTR(native_tilegrid);
}

//|     hidden: bool
//|     """True when the TileGrid is hidden. This may be False even when a part of a hidden Group."""
STATIC mp_obj_t displayio_tilegrid_obj_get_hidden(mp_obj_t self_in) {
    displayio_tilegrid_t *self = native_tilegrid(self_in);
    return mp_obj_new_bool(common_hal_displayio_tilegrid_get_hidden(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_tilegrid_get_hidden_obj, displayio_tilegrid_obj_get_hidden);

STATIC mp_obj_t displayio_tilegrid_obj_set_hidden(mp_obj_t self_in, mp_obj_t hidden_obj) {
    displayio_tilegrid_t *self = native_tilegrid(self_in);

    common_hal_displayio_tilegrid_set_hidden(self, mp_obj_is_true(hidden_obj));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(displayio_tilegrid_set_hidden_obj, displayio_tilegrid_obj_set_hidden);

MP_PROPERTY_GETSET(displayio_tilegrid_hidden_obj,
    (mp_obj_t)&displayio_tilegrid_get_hidden_obj,
    (mp_obj_t)&displayio_tilegrid_set_hidden_obj);

//|     x: int
//|     """X position of the left edge in the parent."""
STATIC mp_obj_t displayio_tilegrid_obj_get_x(mp_obj_t self_in) {
    displayio_tilegrid_t *self = native_tilegrid(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_tilegrid_get_x(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_tilegrid_get_x_obj, displayio_tilegrid_obj_get_x);

STATIC mp_obj_t displayio_tilegrid_obj_set_x(mp_obj_t self_in, mp_obj_t x_obj) {
    displayio_tilegrid_t *self = native_tilegrid(self_in);

    mp_int_t x = mp_obj_get_int(x_obj);
    common_hal_displayio_tilegrid_set_x(self, x);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(displayio_tilegrid_set_x_obj, displayio_tilegrid_obj_set_x);

MP_PROPERTY_GETSET(displayio_tilegrid_x_obj,
    (mp_obj_t)&displayio_tilegrid_get_x_obj,
    (mp_obj_t)&displayio_tilegrid_set_x_obj);

//|     y: int
//|     """Y position of the top edge in the parent."""
STATIC mp_obj_t displayio_tilegrid_obj_get_y(mp_obj_t self_in) {
    displayio_tilegrid_t *self = native_tilegrid(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_tilegrid_get_y(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_tilegrid_get_y_obj, displayio_tilegrid_obj_get_y);

STATIC mp_obj_t displayio_tilegrid_obj_set_y(mp_obj_t self_in, mp_obj_t y_obj) {
    displayio_tilegrid_t *self = native_tilegrid(self_in);

    mp_int_t y = mp_obj_get_int(y_obj);
    common_hal_displayio_tilegrid_set_y(self, y);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(displayio_tilegrid_set_y_obj, displayio_tilegrid_obj_set_y);

MP_PROPERTY_GETSET(displayio_tilegrid_y_obj,
    (mp_obj_t)&displayio_tilegrid_get_y_obj,
    (mp_obj_t)&displayio_tilegrid_set_y_obj);

//|     width: int
//|     """Width of the tilegrid in tiles."""
STATIC mp_obj_t displayio_tilegrid_obj_get_width(mp_obj_t self_in) {
    displayio_tilegrid_t *self = native_tilegrid(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_tilegrid_get_width(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_tilegrid_get_width_obj, displayio_tilegrid_obj_get_width);

MP_PROPERTY_GETTER(displayio_tilegrid_width_obj,
    (mp_obj_t)&displayio_tilegrid_get_width_obj);

//|     height: int
//|     """Height of the tilegrid in tiles."""
STATIC mp_obj_t displayio_tilegrid_obj_get_height(mp_obj_t self_in) {
    displayio_tilegrid_t *self = native_tilegrid(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_tilegrid_get_height(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_tilegrid_get_height_obj, displayio_tilegrid_obj_get_height);

MP_PROPERTY_GETTER(displayio_tilegrid_height_obj,
    (mp_obj_t)&displayio_tilegrid_get_height_obj);

//|     tile_width: int
//|     """Width of a single tile in pixels."""
STATIC mp_obj_t displayio_tilegrid_obj_get_tile_width(mp_obj_t self_in) {
    displayio_tilegrid_t *self = native_tilegrid(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_tilegrid_get_tile_width(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_tilegrid_get_tile_width_obj, displayio_tilegrid_obj_get_tile_width);

MP_PROPERTY_GETTER(displayio_tilegrid_tile_width_obj,
    (mp_obj_t)&displayio_tilegrid_get_tile_width_obj);

//|     tile_height: int
//|     """Height of a single tile in pixels."""
STATIC mp_obj_t displayio_tilegrid_obj_get_tile_height(mp_obj_t self_in) {
    displayio_tilegrid_t *self = native_tilegrid(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_tilegrid_get_tile_height(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_tilegrid_get_tile_height_obj, displayio_tilegrid_obj_get_tile_height);

MP_PROPERTY_GETTER(displayio_tilegrid_tile_height_obj,
    (mp_obj_t)&displayio_tilegrid_get_tile_height_obj);

//|     flip_x: bool
//|     """If true, the left edge rendered will be the right edge of the right-most tile."""
STATIC mp_obj_t displayio_tilegrid_obj_get_flip_x(mp_obj_t self_in) {
    displayio_tilegrid_t *self = native_tilegrid(self_in);
    return mp_obj_new_bool(common_hal_displayio_tilegrid_get_flip_x(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_tilegrid_get_flip_x_obj, displayio_tilegrid_obj_get_flip_x);

STATIC mp_obj_t displayio_tilegrid_obj_set_flip_x(mp_obj_t self_in, mp_obj_t flip_x_obj) {
    displayio_tilegrid_t *self = native_tilegrid(self_in);

    common_hal_displayio_tilegrid_set_flip_x(self, mp_obj_is_true(flip_x_obj));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(displayio_tilegrid_set_flip_x_obj, displayio_tilegrid_obj_set_flip_x);

MP_PROPERTY_GETSET(displayio_tilegrid_flip_x_obj,
    (mp_obj_t)&displayio_tilegrid_get_flip_x_obj,
    (mp_obj_t)&displayio_tilegrid_set_flip_x_obj);

//|     flip_y: bool
//|     """If true, the top edge rendered will be the bottom edge of the bottom-most tile."""
STATIC mp_obj_t displayio_tilegrid_obj_get_flip_y(mp_obj_t self_in) {
    displayio_tilegrid_t *self = native_tilegrid(self_in);
    return mp_obj_new_bool(common_hal_displayio_tilegrid_get_flip_y(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_tilegrid_get_flip_y_obj, displayio_tilegrid_obj_get_flip_y);

STATIC mp_obj_t displayio_tilegrid_obj_set_flip_y(mp_obj_t self_in, mp_obj_t flip_y_obj) {
    displayio_tilegrid_t *self = native_tilegrid(self_in);

    common_hal_displayio_tilegrid_set_flip_y(self, mp_obj_is_true(flip_y_obj));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(displayio_tilegrid_set_flip_y_obj, displayio_tilegrid_obj_set_flip_y);

MP_PROPERTY_GETSET(displayio_tilegrid_flip_y_obj,
    (mp_obj_t)&displayio_tilegrid_get_flip_y_obj,
    (mp_obj_t)&displayio_tilegrid_set_flip_y_obj);


//|     transpose_xy: bool
//|     """If true, the TileGrid's axis will be swapped. When combined with mirroring, any 90 degree
//|     rotation can be achieved along with the corresponding mirrored version."""
STATIC mp_obj_t displayio_tilegrid_obj_get_transpose_xy(mp_obj_t self_in) {
    displayio_tilegrid_t *self = native_tilegrid(self_in);
    return mp_obj_new_bool(common_hal_displayio_tilegrid_get_transpose_xy(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_tilegrid_get_transpose_xy_obj, displayio_tilegrid_obj_get_transpose_xy);

STATIC mp_obj_t displayio_tilegrid_obj_set_transpose_xy(mp_obj_t self_in, mp_obj_t transpose_xy_obj) {
    displayio_tilegrid_t *self = native_tilegrid(self_in);

    common_hal_displayio_tilegrid_set_transpose_xy(self, mp_obj_is_true(transpose_xy_obj));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(displayio_tilegrid_set_transpose_xy_obj, displayio_tilegrid_obj_set_transpose_xy);

MP_PROPERTY_GETSET(displayio_tilegrid_transpose_xy_obj,
    (mp_obj_t)&displayio_tilegrid_get_transpose_xy_obj,
    (mp_obj_t)&displayio_tilegrid_set_transpose_xy_obj);

//|     def contains(self, touch_tuple: tuple) -> bool:
//|         """Returns True if the first two values in ``touch_tuple`` represent an x,y coordinate
//|         inside the tilegrid rectangle bounds."""
STATIC mp_obj_t displayio_tilegrid_obj_contains(mp_obj_t self_in, mp_obj_t touch_tuple) {
    displayio_tilegrid_t *self = MP_OBJ_TO_PTR(self_in);

    mp_obj_t *touch_tuple_items;
    mp_obj_get_array_fixed_n(touch_tuple, 3, &touch_tuple_items);
    uint16_t x = 0;
    uint16_t y = 0;
    x = mp_obj_get_int(touch_tuple_items[0]);
    y = mp_obj_get_int(touch_tuple_items[1]);

    return mp_obj_new_bool(common_hal_displayio_tilegrid_contains(self, x, y));
}
MP_DEFINE_CONST_FUN_OBJ_2(displayio_tilegrid_contains_obj, displayio_tilegrid_obj_contains);

//|     pixel_shader: Union[ColorConverter, Palette]
//|     """The pixel shader of the tilegrid."""
STATIC mp_obj_t displayio_tilegrid_obj_get_pixel_shader(mp_obj_t self_in) {
    displayio_tilegrid_t *self = native_tilegrid(self_in);
    return common_hal_displayio_tilegrid_get_pixel_shader(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_tilegrid_get_pixel_shader_obj, displayio_tilegrid_obj_get_pixel_shader);

STATIC mp_obj_t displayio_tilegrid_obj_set_pixel_shader(mp_obj_t self_in, mp_obj_t pixel_shader) {
    displayio_tilegrid_t *self = native_tilegrid(self_in);
    if (!mp_obj_is_type(pixel_shader, &displayio_palette_type) && !mp_obj_is_type(pixel_shader, &displayio_colorconverter_type)) {
        mp_raise_TypeError_varg(MP_ERROR_TEXT("unsupported %q type"), MP_QSTR_pixel_shader);
    }

    common_hal_displayio_tilegrid_set_pixel_shader(self, pixel_shader);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(displayio_tilegrid_set_pixel_shader_obj, displayio_tilegrid_obj_set_pixel_shader);

MP_PROPERTY_GETSET(displayio_tilegrid_pixel_shader_obj,
    (mp_obj_t)&displayio_tilegrid_get_pixel_shader_obj,
    (mp_obj_t)&displayio_tilegrid_set_pixel_shader_obj);

//|     bitmap: Union[Bitmap, OnDiskBitmap]
//|     """The bitmap of the tilegrid."""
STATIC mp_obj_t displayio_tilegrid_obj_get_bitmap(mp_obj_t self_in) {
    displayio_tilegrid_t *self = native_tilegrid(self_in);
    return common_hal_displayio_tilegrid_get_bitmap(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_tilegrid_get_bitmap_obj, displayio_tilegrid_obj_get_bitmap);

STATIC mp_obj_t displayio_tilegrid_obj_set_bitmap(mp_obj_t self_in, mp_obj_t bitmap) {
    displayio_tilegrid_t *self = native_tilegrid(self_in);

    uint16_t new_bitmap_width;
    uint16_t new_bitmap_height;
    if (mp_obj_is_type(bitmap, &displayio_bitmap_type)) {
        displayio_bitmap_t *bmp = MP_OBJ_TO_PTR(bitmap);
        new_bitmap_width = bmp->width;
        new_bitmap_height = bmp->height;
    } else if (mp_obj_is_type(bitmap, &displayio_ondiskbitmap_type)) {
        displayio_ondiskbitmap_t *bmp = MP_OBJ_TO_PTR(bitmap);
        new_bitmap_width = bmp->width;
        new_bitmap_height = bmp->height;
    } else {
        mp_raise_TypeError_varg(MP_ERROR_TEXT("unsupported %q type"), MP_QSTR_bitmap);
    }

    if (mp_obj_is_type(self->bitmap, &displayio_bitmap_type)) {
        displayio_bitmap_t *old_bmp = MP_OBJ_TO_PTR(self->bitmap);
        if (old_bmp->width != new_bitmap_width || old_bmp->height != new_bitmap_height) {
            mp_raise_ValueError(MP_ERROR_TEXT("New bitmap must be same size as old bitmap"));
        }
    } else if (mp_obj_is_type(self->bitmap, &displayio_ondiskbitmap_type)) {
        displayio_ondiskbitmap_t *old_bmp = MP_OBJ_TO_PTR(self->bitmap);
        if (old_bmp->width != new_bitmap_width || old_bmp->height != new_bitmap_height) {
            mp_raise_ValueError(MP_ERROR_TEXT("New bitmap must be same size as old bitmap"));
        }
    }

    common_hal_displayio_tilegrid_set_bitmap(self, bitmap);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(displayio_tilegrid_set_bitmap_obj, displayio_tilegrid_obj_set_bitmap);

MP_PROPERTY_GETSET(displayio_tilegrid_bitmap_obj,
    (mp_obj_t)&displayio_tilegrid_get_bitmap_obj,
    (mp_obj_t)&displayio_tilegrid_set_bitmap_obj);

//|     def __getitem__(self, index: Union[Tuple[int, int], int]) -> int:
//|         """Returns the tile index at the given index. The index can either be an x,y tuple or an int equal
//|         to ``y * width + x``.
//|
//|         This allows you to::
//|
//|           print(grid[0])"""
//|         ...
//|
//|     def __setitem__(self, index: Union[Tuple[int, int], int], value: int) -> None:
//|         """Sets the tile index at the given index. The index can either be an x,y tuple or an int equal
//|         to ``y * width + x``.
//|
//|         This allows you to::
//|
//|           grid[0] = 10
//|
//|         or::
//|
//|           grid[0,0] = 10"""
//|         ...
//|
STATIC mp_obj_t tilegrid_subscr(mp_obj_t self_in, mp_obj_t index_obj, mp_obj_t value_obj) {
    displayio_tilegrid_t *self = native_tilegrid(self_in);


    if (mp_obj_is_type(index_obj, &mp_type_slice)) {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("Slices not supported"));
    } else {
        uint16_t x = 0;
        uint16_t y = 0;
        if (mp_obj_is_small_int(index_obj)) {
            mp_int_t i = MP_OBJ_SMALL_INT_VALUE(index_obj);
            uint16_t width = common_hal_displayio_tilegrid_get_width(self);
            x = i % width;
            y = i / width;
        } else {
            mp_obj_t *items;
            mp_obj_get_array_fixed_n(index_obj, 2, &items);
            x = mp_obj_get_int(items[0]);
            y = mp_obj_get_int(items[1]);
        }
        if (x >= common_hal_displayio_tilegrid_get_width(self) ||
            y >= common_hal_displayio_tilegrid_get_height(self)) {
            mp_raise_IndexError(MP_ERROR_TEXT("Tile index out of bounds"));
        }

        if (value_obj == MP_OBJ_SENTINEL) {
            // load
            return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_tilegrid_get_tile(self, x, y));
        } else if (value_obj == mp_const_none) {
            return MP_OBJ_NULL; // op not supported
        } else {
            mp_int_t value = mp_obj_get_int(value_obj);
            mp_arg_validate_int_range(value, 0, 255, MP_QSTR_tile);

            common_hal_displayio_tilegrid_set_tile(self, x, y, value);
        }
    }
    return mp_const_none;
}

STATIC const mp_rom_map_elem_t displayio_tilegrid_locals_dict_table[] = {
    // Properties
    { MP_ROM_QSTR(MP_QSTR_hidden), MP_ROM_PTR(&displayio_tilegrid_hidden_obj) },
    { MP_ROM_QSTR(MP_QSTR_x), MP_ROM_PTR(&displayio_tilegrid_x_obj) },
    { MP_ROM_QSTR(MP_QSTR_y), MP_ROM_PTR(&displayio_tilegrid_y_obj) },
    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&displayio_tilegrid_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_PTR(&displayio_tilegrid_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_tile_width), MP_ROM_PTR(&displayio_tilegrid_tile_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_tile_height), MP_ROM_PTR(&displayio_tilegrid_tile_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_flip_x), MP_ROM_PTR(&displayio_tilegrid_flip_x_obj) },
    { MP_ROM_QSTR(MP_QSTR_flip_y), MP_ROM_PTR(&displayio_tilegrid_flip_y_obj) },
    { MP_ROM_QSTR(MP_QSTR_transpose_xy), MP_ROM_PTR(&displayio_tilegrid_transpose_xy_obj) },
    { MP_ROM_QSTR(MP_QSTR_contains), MP_ROM_PTR(&displayio_tilegrid_contains_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel_shader), MP_ROM_PTR(&displayio_tilegrid_pixel_shader_obj) },
    { MP_ROM_QSTR(MP_QSTR_bitmap), MP_ROM_PTR(&displayio_tilegrid_bitmap_obj) },
};
STATIC MP_DEFINE_CONST_DICT(displayio_tilegrid_locals_dict, displayio_tilegrid_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    displayio_tilegrid_type,
    MP_QSTR_TileGrid,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, displayio_tilegrid_make_new,
    locals_dict, &displayio_tilegrid_locals_dict,
    subscr, tilegrid_subscr
    );
