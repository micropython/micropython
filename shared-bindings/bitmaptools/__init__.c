/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Kevin Matocha
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
#include "shared-bindings/bitmaptools/__init__.h"

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

//| """Collection of bitmap manipulation tools"""
//|

STATIC int16_t validate_point(mp_obj_t point, int16_t default_value) {
    // Checks if point is None and returns default_value, otherwise decodes integer value
    if ( point == mp_const_none ) {
        return default_value;
    }
    return mp_obj_get_int(point);
}

STATIC void extract_tuple(mp_obj_t xy_tuple, int16_t *x, int16_t *y, int16_t x_default, int16_t y_default) {
    // Helper function for rotozoom
    // Extract x,y values from a tuple or default if None
    if ( xy_tuple == mp_const_none ) {
        *x = x_default;
        *y = y_default;
    } else if ( !MP_OBJ_IS_OBJ(xy_tuple) ) {
        mp_raise_ValueError(translate("clip point must be (x,y) tuple"));
    } else {
        mp_obj_t* items;
        mp_obj_get_array_fixed_n(xy_tuple, 2, &items);
        *x = mp_obj_get_int(items[0]);
        *y = mp_obj_get_int(items[1]);
    }
}

STATIC void validate_clip_region(displayio_bitmap_t *bitmap, mp_obj_t clip0_tuple, int16_t *clip0_x, int16_t *clip0_y,
                                                   mp_obj_t clip1_tuple, int16_t *clip1_x, int16_t *clip1_y) {
    // Helper function for rotozoom
    // 1. Extract the clip x,y points from the two clip tuples
    // 2. Rearrange values such that clip0_ < clip1_
    // 3. Constrain the clip points to within the bitmap

    extract_tuple(clip0_tuple, clip0_x, clip0_y, 0, 0);
    extract_tuple(clip1_tuple, clip1_x, clip1_y, bitmap->width, bitmap->height);

    // Ensure the value for clip0 is less than clip1 (for both x and y)
    if ( *clip0_x > *clip1_x ) {
        int16_t temp_value = *clip0_x; // swap values
        *clip0_x = *clip1_x;
        *clip1_x = temp_value;
    }
    if ( *clip0_y > *clip1_y ) {
        int16_t temp_value = *clip0_y; // swap values
        *clip0_y = *clip1_y;
        *clip1_y = temp_value;
    }

    // Constrain the clip window to within the bitmap boundaries
    if (*clip0_x < 0) {
        *clip0_x = 0;
    }
    if (*clip0_y < 0) {
        *clip0_y = 0;
    }
    if (*clip0_x > bitmap->width) {
        *clip0_x = bitmap->width;
    }
    if (*clip0_y > bitmap->height) {
        *clip0_y = bitmap->height;
    }
    if (*clip1_x < 0) {
        *clip1_x = 0;
    }
    if (*clip1_y < 0) {
        *clip1_y = 0;
    }
    if (*clip1_x > bitmap->width) {
        *clip1_x = bitmap->width;
    }
    if (*clip1_y > bitmap->height) {
        *clip1_y = bitmap->height;
    }

}

//|
//| def rotozoom(
//|        dest_bitmap: displayio.Bitmap, source_bitmap: displayio.Bitmap,
//|        *,
//|        ox: int, oy: int, dest_clip0: Tuple[int, int], dest_clip1: Tuple[int, int],
//|        px: int, py: int, source_clip0: Tuple[int, int], source_clip1: Tuple[int, int],
//|        angle: float, scale: float, skip_index: int) -> None:
//|      """Inserts the source bitmap region into the destination bitmap with rotation
//|      (angle), scale and clipping (both on source and destination bitmaps).
//|
//|      :param bitmap dest_bitmap: Destination bitmap that will be copied into
//|      :param bitmap source_bitmap: Source bitmap that contains the graphical region to be copied
//|      :param int ox: Horizontal pixel location in destination bitmap where source bitmap
//|             point (px,py) is placed
//|      :param int oy: Vertical pixel location in destination bitmap where source bitmap
//|             point (px,py) is placed
//|      :param Tuple[int,int] dest_clip0: First corner of rectangular destination clipping
//|             region that constrains region of writing into destination bitmap
//|      :param Tuple[int,int] dest_clip1: Second corner of rectangular destination clipping
//|             region that constrains region of writing into destination bitmap
//|      :param int px: Horizontal pixel location in source bitmap that is placed into the
//|             destination bitmap at (ox,oy)
//|      :param int py: Vertical pixel location in source bitmap that is placed into the
//|             destination bitmap at (ox,oy)
//|      :param Tuple[int,int] source_clip0: First corner of rectangular source clipping
//|             region that constrains region of reading from the source bitmap
//|      :param Tuple[int,int] source_clip1: Second corner of rectangular source clipping
//|             region that constrains region of reading from the source bitmap
//|      :param float angle: Angle of rotation, in radians (positive is clockwise direction)
//|      :param float scale: Scaling factor
//|      :param int skip_index: Bitmap palette index in the source that will not be copied,
//|             set to None to copy all pixels"""
//|      ...
//|
STATIC mp_obj_t bitmaptools_obj_rotozoom(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args){
    enum {ARG_dest_bitmap, ARG_source_bitmap,
        ARG_ox, ARG_oy, ARG_dest_clip0, ARG_dest_clip1,
        ARG_px, ARG_py, ARG_source_clip0, ARG_source_clip1,
        ARG_angle, ARG_scale, ARG_skip_index};

    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_dest_bitmap, MP_ARG_REQUIRED | MP_ARG_OBJ},
        {MP_QSTR_source_bitmap, MP_ARG_REQUIRED | MP_ARG_OBJ},

        {MP_QSTR_ox, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} }, // None convert to destination->width  / 2
        {MP_QSTR_oy, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} }, // None convert to destination->height / 2
        {MP_QSTR_dest_clip0, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        {MP_QSTR_dest_clip1, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },

        {MP_QSTR_px, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} }, // None convert to source->width  / 2
        {MP_QSTR_py, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} }, // None convert to source->height / 2
        {MP_QSTR_source_clip0, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        {MP_QSTR_source_clip1, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },

        {MP_QSTR_angle, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} }, // None convert to 0.0
        {MP_QSTR_scale, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} }, // None convert to 1.0
        {MP_QSTR_skip_index, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj=mp_const_none} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    displayio_bitmap_t *destination = MP_OBJ_TO_PTR(args[ARG_dest_bitmap].u_obj); // the destination bitmap

    displayio_bitmap_t *source = MP_OBJ_TO_PTR(args[ARG_source_bitmap].u_obj); // the source bitmap

    // ensure that the destination bitmap has at least as many `bits_per_value` as the source
    if (destination->bits_per_value < source->bits_per_value) {
        mp_raise_ValueError(translate("source palette too large"));
    }

    // Confirm the destination location target (ox,oy); if None, default to bitmap midpoint
    int16_t ox, oy;
    ox = validate_point(args[ARG_ox].u_obj, destination->width  / 2);
    oy = validate_point(args[ARG_oy].u_obj, destination->height / 2);

    // Confirm the source location target (px,py); if None, default to bitmap midpoint
    int16_t px, py;
    px = validate_point(args[ARG_px].u_obj, source->width  / 2);
    py = validate_point(args[ARG_py].u_obj, source->height / 2);

    // Validate the clipping regions for the destination bitmap
    int16_t dest_clip0_x, dest_clip0_y, dest_clip1_x, dest_clip1_y;

    validate_clip_region(destination, args[ARG_dest_clip0].u_obj, &dest_clip0_x, &dest_clip0_y,
                               args[ARG_dest_clip1].u_obj, &dest_clip1_x, &dest_clip1_y);

    // Validate the clipping regions for the source bitmap
    int16_t source_clip0_x, source_clip0_y, source_clip1_x, source_clip1_y;

    validate_clip_region(source, args[ARG_source_clip0].u_obj, &source_clip0_x, &source_clip0_y,
                                 args[ARG_source_clip1].u_obj, &source_clip1_x, &source_clip1_y);

    // Confirm the angle value
    float angle=0.0;
    if ( args[ARG_angle].u_obj != mp_const_none ) {
        angle = mp_obj_get_float(args[ARG_angle].u_obj);
    }

    // Confirm the scale value
    float scale=1.0;
    if ( args[ARG_scale].u_obj != mp_const_none ) {
        scale = mp_obj_get_float(args[ARG_scale].u_obj);
    }
    if (scale < 0) { // ensure scale >= 0
        scale = 1.0;
    }

    uint32_t skip_index;
    bool skip_index_none; // Flag whether input skip_value was None
    if (args[ARG_skip_index].u_obj == mp_const_none ) {
        skip_index = 0;
        skip_index_none = true;
    } else {
        skip_index = mp_obj_get_int(args[ARG_skip_index].u_obj);
        skip_index_none = false;
    }

    common_hal_bitmaptools_rotozoom(destination, ox, oy,
                                            dest_clip0_x, dest_clip0_y,
                                            dest_clip1_x, dest_clip1_y,
                                            source, px, py,
                                            source_clip0_x, source_clip0_y,
                                            source_clip1_x, source_clip1_y,
                                            angle,
                                            scale,
                                            skip_index, skip_index_none);

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_KW(bitmaptools_rotozoom_obj, 0, bitmaptools_obj_rotozoom);
// requires at least 2 arguments (destination bitmap and source bitmap)


STATIC const mp_rom_map_elem_t bitmaptools_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_rotozoom), MP_ROM_PTR(&bitmaptools_rotozoom_obj) },
};
STATIC MP_DEFINE_CONST_DICT(bitmaptools_module_globals, bitmaptools_module_globals_table);


const mp_obj_module_t bitmaptools_module = {
    .base = {&mp_type_module },
    .globals = (mp_obj_dict_t*)&bitmaptools_module_globals,
};
