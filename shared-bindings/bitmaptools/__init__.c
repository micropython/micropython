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
#include "shared-bindings/displayio/Palette.h"
#include "shared-bindings/displayio/ColorConverter.h"
#include "shared-bindings/bitmaptools/__init__.h"

#include <stdint.h>

#include "py/binary.h"
#include "py/enum.h"
#include "py/obj.h"
#include "py/runtime.h"

#if MICROPY_VFS
#include "extmod/vfs.h"
#endif
#if defined(MICROPY_VFS_POSIX) && MICROPY_VFS_POSIX
#include "extmod/vfs_posix.h"
#endif

//| """Collection of bitmap manipulation tools
//|
//| .. note:: If you're looking for information about displaying bitmaps on
//|     screens in CircuitPython, see `this Learn guide
//|     <https://learn.adafruit.com/circuitpython-display-support-using-displayio>`_
//|     for information about using the :py:mod:`displayio` module.
//| """
//|

STATIC int16_t validate_point(mp_obj_t point, int16_t default_value) {
    // Checks if point is None and returns default_value, otherwise decodes integer value
    if (point == mp_const_none) {
        return default_value;
    }
    return mp_obj_get_int(point);
}

STATIC void extract_tuple(mp_obj_t xy_tuple, int16_t *x, int16_t *y, int16_t x_default, int16_t y_default) {
    // Helper function for rotozoom
    // Extract x,y values from a tuple or default if None
    if (xy_tuple == mp_const_none) {
        *x = x_default;
        *y = y_default;
    } else if (!mp_obj_is_obj(xy_tuple)) {
        mp_raise_ValueError(translate("clip point must be (x,y) tuple"));
    } else {
        mp_obj_t *items;
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
    if (*clip0_x > *clip1_x) {
        int16_t temp_value = *clip0_x; // swap values
        *clip0_x = *clip1_x;
        *clip1_x = temp_value;
    }
    if (*clip0_y > *clip1_y) {
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

//| def rotozoom(
//|     dest_bitmap: displayio.Bitmap,
//|     source_bitmap: displayio.Bitmap,
//|     *,
//|     ox: int,
//|     oy: int,
//|     dest_clip0: Tuple[int, int],
//|     dest_clip1: Tuple[int, int],
//|     px: int,
//|     py: int,
//|     source_clip0: Tuple[int, int],
//|     source_clip1: Tuple[int, int],
//|     angle: float,
//|     scale: float,
//|     skip_index: int
//| ) -> None:
//|     """Inserts the source bitmap region into the destination bitmap with rotation
//|     (angle), scale and clipping (both on source and destination bitmaps).
//|
//|     :param bitmap dest_bitmap: Destination bitmap that will be copied into
//|     :param bitmap source_bitmap: Source bitmap that contains the graphical region to be copied
//|     :param int ox: Horizontal pixel location in destination bitmap where source bitmap
//|            point (px,py) is placed. Defaults to None which causes it to use the horizontal
//|            midway point of the destination bitmap.
//|     :param int oy: Vertical pixel location in destination bitmap where source bitmap
//|            point (px,py) is placed. Defaults to None which causes it to use the vertical
//|            midway point of the destination bitmap.
//|     :param Tuple[int,int] dest_clip0: First corner of rectangular destination clipping
//|            region that constrains region of writing into destination bitmap
//|     :param Tuple[int,int] dest_clip1: Second corner of rectangular destination clipping
//|            region that constrains region of writing into destination bitmap
//|     :param int px: Horizontal pixel location in source bitmap that is placed into the
//|            destination bitmap at (ox,oy). Defaults to None which causes it to use the
//|            horizontal midway point in the source bitmap.
//|     :param int py: Vertical pixel location in source bitmap that is placed into the
//|            destination bitmap at (ox,oy). Defaults to None which causes it to use the
//|            vertical midway point in the source bitmap.
//|     :param Tuple[int,int] source_clip0: First corner of rectangular source clipping
//|            region that constrains region of reading from the source bitmap
//|     :param Tuple[int,int] source_clip1: Second corner of rectangular source clipping
//|            region that constrains region of reading from the source bitmap
//|     :param float angle: Angle of rotation, in radians (positive is clockwise direction).
//|            Defaults to None which gets treated as 0.0 radians or no rotation.
//|     :param float scale: Scaling factor. Defaults to None which gets treated as 1.0 or same
//|            as original source size.
//|     :param int skip_index: Bitmap palette index in the source that will not be copied,
//|            set to None to copy all pixels"""
//|     ...
//|
STATIC mp_obj_t bitmaptools_obj_rotozoom(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {ARG_dest_bitmap, ARG_source_bitmap,
          ARG_ox, ARG_oy, ARG_dest_clip0, ARG_dest_clip1,
          ARG_px, ARG_py, ARG_source_clip0, ARG_source_clip1,
          ARG_angle, ARG_scale, ARG_skip_index};

    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_dest_bitmap, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_source_bitmap, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},

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
        {MP_QSTR_skip_index, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_none} },
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
    ox = validate_point(args[ARG_ox].u_obj, destination->width / 2);
    oy = validate_point(args[ARG_oy].u_obj, destination->height / 2);

    // Confirm the source location target (px,py); if None, default to bitmap midpoint
    int16_t px, py;
    px = validate_point(args[ARG_px].u_obj, source->width / 2);
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
    mp_float_t angle = 0.0;
    if (args[ARG_angle].u_obj != mp_const_none) {
        angle = mp_obj_get_float(args[ARG_angle].u_obj);
    }

    // Confirm the scale value
    mp_float_t scale = 1.0;
    if (args[ARG_scale].u_obj != mp_const_none) {
        scale = mp_obj_get_float(args[ARG_scale].u_obj);
    }
    if (scale < 0) { // ensure scale >= 0
        scale = 1.0;
    }

    uint32_t skip_index;
    bool skip_index_none; // Flag whether input skip_value was None
    if (args[ARG_skip_index].u_obj == mp_const_none) {
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

//| def alphablend(
//|     dest_bitmap: displayio.Bitmap,
//|     source_bitmap_1: displayio.Bitmap,
//|     source_bitmap_2: displayio.Bitmap,
//|     colorspace: displayio.Colorspace,
//|     factor1: float = 0.5,
//|     factor2: Optional[float] = None,
//| ) -> None:
//|     """Alpha blend the two source bitmaps into the destination.
//|
//|     It is permitted for the destination bitmap to be one of the two
//|     source bitmaps.
//|
//|     :param bitmap dest_bitmap: Destination bitmap that will be written into
//|     :param bitmap source_bitmap_1: The first source bitmap
//|     :param bitmap source_bitmap_2: The second source bitmap
//|     :param float factor1: The proportion of bitmap 1 to mix in
//|     :param float factor2: The proportion of bitmap 2 to mix in.  If specified as `None`, ``1-factor1`` is used.  Usually the proportions should sum to 1.
//|     :param displayio.Colorspace colorspace: The colorspace of the bitmaps. They must all have the same colorspace.  Only the following colorspaces are permitted:  ``L8``, ``RGB565``, ``RGB565_SWAPPED``, ``BGR565`` and ``BGR565_SWAPPED``.
//|
//|     For the L8 colorspace, the bitmaps must have a bits-per-value of 8.
//|     For the RGB colorspaces, they must have a bits-per-value of 16."""
//|

STATIC mp_obj_t bitmaptools_alphablend(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {ARG_dest_bitmap, ARG_source_bitmap_1, ARG_source_bitmap_2, ARG_colorspace, ARG_factor_1, ARG_factor_2};

    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_dest_bitmap, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = NULL}},
        {MP_QSTR_source_bitmap_1, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = NULL}},
        {MP_QSTR_source_bitmap_2, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = NULL}},
        {MP_QSTR_colorspace, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = NULL}},
        {MP_QSTR_factor_1, MP_ARG_OBJ, {.u_obj = MP_ROM_NONE}},
        {MP_QSTR_factor_2, MP_ARG_OBJ, {.u_obj = MP_ROM_NONE}},
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    displayio_bitmap_t *destination = MP_OBJ_TO_PTR(mp_arg_validate_type(args[ARG_dest_bitmap].u_obj, &displayio_bitmap_type, MP_QSTR_dest_bitmap)); // the destination bitmap
    displayio_bitmap_t *source1 = MP_OBJ_TO_PTR(mp_arg_validate_type(args[ARG_source_bitmap_1].u_obj, &displayio_bitmap_type, MP_QSTR_source_bitmap_1)); // the first source bitmap
    displayio_bitmap_t *source2 = MP_OBJ_TO_PTR(mp_arg_validate_type(args[ARG_source_bitmap_2].u_obj, &displayio_bitmap_type, MP_QSTR_source_bitmap_2)); // the second source bitmap

    mp_float_t factor1 = (args[ARG_factor_1].u_obj == mp_const_none) ? MICROPY_FLOAT_CONST(.5) : mp_obj_get_float(args[ARG_factor_1].u_obj);
    mp_float_t factor2 = (args[ARG_factor_2].u_obj == mp_const_none) ? 1 - factor1 : mp_obj_get_float(args[ARG_factor_2].u_obj);

    displayio_colorspace_t colorspace = (displayio_colorspace_t)cp_enum_value(&displayio_colorspace_type, args[ARG_colorspace].u_obj, MP_QSTR_colorspace);

    if (destination->width != source1->width
        || destination->height != source1->height
        || destination->bits_per_value != source1->bits_per_value
        || destination->width != source2->width
        || destination->height != source2->height
        || destination->bits_per_value != source2->bits_per_value
        ) {
        mp_raise_ValueError(translate("Bitmap size and bits per value must match"));
    }

    switch (colorspace) {
        case DISPLAYIO_COLORSPACE_L8:
            if (destination->bits_per_value != 8) {
                mp_raise_ValueError(translate("For L8 colorspace, input bitmap must have 8 bits per pixel"));
            }
            break;

        case DISPLAYIO_COLORSPACE_RGB565:
        case DISPLAYIO_COLORSPACE_RGB565_SWAPPED:
        case DISPLAYIO_COLORSPACE_BGR565:
        case DISPLAYIO_COLORSPACE_BGR565_SWAPPED:
            if (destination->bits_per_value != 16) {
                mp_raise_ValueError(translate("For RGB colorspaces, input bitmap must have 16 bits per pixel"));
            }
            break;

        default:
            mp_raise_ValueError(translate("Unsupported colorspace"));
    }

    common_hal_bitmaptools_alphablend(destination, source1, source2, colorspace, factor1, factor2);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(bitmaptools_alphablend_obj, 0, bitmaptools_alphablend);

//| def fill_region(
//|     dest_bitmap: displayio.Bitmap, x1: int, y1: int, x2: int, y2: int, value: int
//| ) -> None:
//|     """Draws the color value into the destination bitmap within the
//|     rectangular region bounded by (x1,y1) and (x2,y2), exclusive.
//|
//|     :param bitmap dest_bitmap: Destination bitmap that will be written into
//|     :param int x1: x-pixel position of the first corner of the rectangular fill region
//|     :param int y1: y-pixel position of the first corner of the rectangular fill region
//|     :param int x2: x-pixel position of the second corner of the rectangular fill region (exclusive)
//|     :param int y2: y-pixel position of the second corner of the rectangular fill region (exclusive)
//|     :param int value: Bitmap palette index that will be written into the rectangular
//|            fill region in the destination bitmap"""
//|     ...
//|
STATIC mp_obj_t bitmaptools_obj_fill_region(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {ARG_dest_bitmap, ARG_x1, ARG_y1, ARG_x2, ARG_y2, ARG_value};

    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_dest_bitmap, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_x1, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_y1, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_x2, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_y2, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_value, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    displayio_bitmap_t *destination = MP_OBJ_TO_PTR(args[ARG_dest_bitmap].u_obj);     // the destination bitmap

    uint32_t value, color_depth;
    value = args[ARG_value].u_int;
    color_depth = (1 << destination->bits_per_value);
    if (color_depth <= value) {
        mp_raise_ValueError(translate("out of range of target"));
    }

    int16_t x1 = args[ARG_x1].u_int;
    int16_t y1 = args[ARG_y1].u_int;
    int16_t x2 = args[ARG_x2].u_int;
    int16_t y2 = args[ARG_y2].u_int;

    common_hal_bitmaptools_fill_region(destination, x1, y1, x2, y2, value);

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_KW(bitmaptools_fill_region_obj, 0, bitmaptools_obj_fill_region);
//| def boundary_fill(
//|     dest_bitmap: displayio.Bitmap,
//|     x: int,
//|     y: int,
//|     fill_color_value: int,
//|     replaced_color_value: int,
//| ) -> None:
//|     """Draws the color value into the destination bitmap enclosed
//|     area of pixels of the background_value color. Like "Paint Bucket"
//|     fill tool.
//|
//|     :param bitmap dest_bitmap: Destination bitmap that will be written into
//|     :param int x: x-pixel position of the first pixel to check and fill if needed
//|     :param int y: y-pixel position of the first pixel to check and fill if needed
//|     :param int fill_color_value: Bitmap palette index that will be written into the
//|            enclosed area in the destination bitmap
//|     :param int replaced_color_value: Bitmap palette index that will filled with the
//|            value color in the enclosed area in the destination bitmap"""
//|     ...
//|
STATIC mp_obj_t bitmaptools_obj_boundary_fill(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {ARG_dest_bitmap, ARG_x, ARG_y, ARG_fill_color_value, ARG_replaced_color_value};

    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_dest_bitmap, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_fill_color_value, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_replaced_color_value, MP_ARG_INT, {.u_int = INT_MAX} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    displayio_bitmap_t *destination = MP_OBJ_TO_PTR(mp_arg_validate_type(args[ARG_dest_bitmap].u_obj, &displayio_bitmap_type, MP_QSTR_dest_bitmap));     // the destination bitmap

    uint32_t fill_color_value, color_depth;
    fill_color_value = args[ARG_fill_color_value].u_int;
    color_depth = (1 << destination->bits_per_value);
    if (color_depth <= fill_color_value) {
        mp_raise_ValueError(translate("value out of range of target"));
    }

    uint32_t replaced_color_value;
    replaced_color_value = args[ARG_replaced_color_value].u_int;
    if (replaced_color_value != INT_MAX && color_depth <= replaced_color_value) {
        mp_raise_ValueError(translate("background value out of range of target"));
    }

    int16_t x = args[ARG_x].u_int;
    int16_t y = args[ARG_y].u_int;

    if (x < 0 || x >= destination->width) {
        mp_raise_ValueError(translate("out of range of target"));
    }
    if (y < 0 || y >= destination->height) {
        mp_raise_ValueError(translate("out of range of target"));
    }

    common_hal_bitmaptools_boundary_fill(destination, x, y, fill_color_value, replaced_color_value);

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_KW(bitmaptools_boundary_fill_obj, 0, bitmaptools_obj_boundary_fill);
// requires all 6 arguments

//| def draw_line(
//|     dest_bitmap: displayio.Bitmap, x1: int, y1: int, x2: int, y2: int, value: int
//| ) -> None:
//|     """Draws a line into a bitmap specified two endpoints (x1,y1) and (x2,y2).
//|
//|     :param bitmap dest_bitmap: Destination bitmap that will be written into
//|     :param int x1: x-pixel position of the line's first endpoint
//|     :param int y1: y-pixel position of the line's first endpoint
//|     :param int x2: x-pixel position of the line's second endpoint
//|     :param int y2: y-pixel position of the line's second endpoint
//|     :param int value: Bitmap palette index that will be written into the
//|            line in the destination bitmap"""
//|     ...
//|
STATIC mp_obj_t bitmaptools_obj_draw_line(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {ARG_dest_bitmap, ARG_x1, ARG_y1, ARG_x2, ARG_y2, ARG_value};

    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_dest_bitmap, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_x1, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_y1, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_x2, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_y2, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_value, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    displayio_bitmap_t *destination = MP_OBJ_TO_PTR(args[ARG_dest_bitmap].u_obj);     // the destination bitmap

    uint32_t value, color_depth;
    value = args[ARG_value].u_int;
    color_depth = (1 << destination->bits_per_value);
    if (color_depth <= value) {
        mp_raise_ValueError(translate("out of range of target"));
    }

    int16_t x1 = args[ARG_x1].u_int;
    int16_t y1 = args[ARG_y1].u_int;
    int16_t x2 = args[ARG_x2].u_int;
    int16_t y2 = args[ARG_y2].u_int;

    common_hal_bitmaptools_draw_line(destination, x1, y1, x2, y2, value);

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_KW(bitmaptools_draw_line_obj, 0, bitmaptools_obj_draw_line);
// requires all 6 arguments

//| def draw_polygon(
//|     dest_bitmap: displayio.Bitmap,
//|     xs: ReadableBuffer,
//|     ys: ReadableBuffer,
//|     value: int,
//|     close: Optional[bool] = True,
//| ) -> None:
//|     """Draw a polygon connecting points on provided bitmap with provided value
//|
//|     :param bitmap dest_bitmap: Destination bitmap that will be written into
//|     :param ReadableBuffer xs: x-pixel position of the polygon's vertices
//|     :param ReadableBuffer ys: y-pixel position of the polygon's vertices
//|     :param int value: Bitmap palette index that will be written into the
//|            line in the destination bitmap
//|     :param bool close: (Optional) Whether to connect first and last point. (True)
//|
//|     .. code-block:: Python
//|
//|        import board
//|        import displayio
//|        import bitmaptools
//|
//|        display = board.DISPLAY
//|        main_group = displayio.Group()
//|        display.root_group = main_group
//|
//|        palette = displayio.Palette(3)
//|        palette[0] = 0xffffff
//|        palette[1] = 0x0000ff
//|        palette[2] = 0xff0000
//|
//|        bmp = displayio.Bitmap(128,128, 3)
//|        bmp.fill(0)
//|
//|        xs = bytes([4, 101, 101, 19])
//|        ys = bytes([4, 19,  121, 101])
//|        bitmaptools.draw_polygon(bmp, xs, ys, 1)
//|
//|        xs = bytes([14, 60, 110])
//|        ys = bytes([14, 24,  90])
//|        bitmaptools.draw_polygon(bmp, xs, ys, 2)
//|
//|        tilegrid = displayio.TileGrid(bitmap=bmp, pixel_shader=palette)
//|        main_group.append(tilegrid)
//|
//|        while True:
//|            pass
//|     """
//|     ...
//|
STATIC mp_obj_t bitmaptools_obj_draw_polygon(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {ARG_dest_bitmap, ARG_xs, ARG_ys, ARG_value, ARG_close};

    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_dest_bitmap, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_xs, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_ys, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_value, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_close, MP_ARG_BOOL, {.u_bool = true}},
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    displayio_bitmap_t *destination = MP_OBJ_TO_PTR(args[ARG_dest_bitmap].u_obj);     // the destination bitmap

    mp_buffer_info_t xs_buf, ys_buf;
    mp_get_buffer_raise(args[ARG_xs].u_obj, &xs_buf, MP_BUFFER_READ);
    mp_get_buffer_raise(args[ARG_ys].u_obj, &ys_buf, MP_BUFFER_READ);
    size_t xs_size = mp_binary_get_size('@', xs_buf.typecode, NULL);
    size_t ys_size = mp_binary_get_size('@', ys_buf.typecode, NULL);
    size_t xs_len = xs_buf.len / xs_size;
    size_t ys_len = ys_buf.len / ys_size;
    if (xs_size != ys_size) {
        mp_raise_ValueError(translate("Coordinate arrays types have different sizes"));
    }
    if (xs_len != ys_len) {
        mp_raise_ValueError(translate("Coordinate arrays have different lengths"));
    }

    uint32_t value, color_depth;
    value = args[ARG_value].u_int;
    color_depth = (1 << destination->bits_per_value);
    if (color_depth <= value) {
        mp_raise_ValueError(translate("out of range of target"));
    }

    bool close = args[ARG_close].u_bool;

    common_hal_bitmaptools_draw_polygon(destination, xs_buf.buf, ys_buf.buf, xs_len, xs_size, value, close);

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_KW(bitmaptools_draw_polygon_obj, 0, bitmaptools_obj_draw_polygon);

//| def arrayblit(
//|     bitmap: displayio.Bitmap,
//|     data: ReadableBuffer,
//|     x1: int = 0,
//|     y1: int = 0,
//|     x2: Optional[int] = None,
//|     y2: Optional[int] = None,
//|     skip_index: Optional[int] = None,
//| ) -> None:
//|     """Inserts pixels from ``data`` into the rectangle of width×height pixels with the upper left corner at ``(x,y)``
//|
//|     The values from ``data`` are taken modulo the number of color values
//|     available in the destination bitmap.
//|
//|     If x1 or y1 are not specified, they are taken as 0.  If x2 or y2
//|     are not specified, or are given as -1, they are taken as the width
//|     and height of the image.
//|
//|     The coordinates affected by the blit are ``x1 <= x < x2`` and ``y1 <= y < y2``.
//|
//|     ``data`` must contain at least as many elements as required.  If it
//|     contains excess elements, they are ignored.
//|
//|     The blit takes place by rows, so the first elements of ``data`` go
//|     to the first row, the next elements to the next row, and so on.
//|
//|     :param displayio.Bitmap bitmap: A writable bitmap
//|     :param ReadableBuffer data: Buffer containing the source pixel values
//|     :param int x1: The left corner of the area to blit into (inclusive)
//|     :param int y1: The top corner of the area to blit into (inclusive)
//|     :param int x2: The right of the area to blit into (exclusive)
//|     :param int y2: The bottom corner of the area to blit into (exclusive)
//|     :param int skip_index: Bitmap palette index in the source that will not be copied,
//|             set to None to copy all pixels
//|     """
//|     ...
//|
STATIC mp_obj_t bitmaptools_arrayblit(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_bitmap, ARG_data, ARG_x1, ARG_y1, ARG_x2, ARG_y2, ARG_skip_index };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bitmap, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_data, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_x1, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_y1, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_x2, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_y2, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_skip_index, MP_ARG_OBJ, {.u_obj = mp_const_none } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    displayio_bitmap_t *bitmap = mp_arg_validate_type(args[ARG_bitmap].u_obj, &displayio_bitmap_type, MP_QSTR_bitmap);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_data].u_obj, &bufinfo, MP_BUFFER_READ);

    int x1 = args[ARG_x1].u_int;
    int y1 = args[ARG_y1].u_int;
    int x2 = args[ARG_x2].u_int == -1 ? bitmap->width : args[ARG_x2].u_int;
    int y2 = args[ARG_y2].u_int == -1 ? bitmap->height : args[ARG_y2].u_int;

    if ((x1 < 0) || (y1 < 0) || (x1 > x2) || (y1 > y2) || (x2 > bitmap->width) || (y2 > bitmap->height)) {
        mp_raise_IndexError(translate("pixel coordinates out of bounds"));
    }

    size_t output_element_count = (x2 - x1) * (y2 - y1);
    size_t element_size = mp_binary_get_size('@', bufinfo.typecode, NULL);
    size_t input_element_count = bufinfo.len / element_size;

    bool skip_specified = args[ARG_skip_index].u_obj != mp_const_none;
    uint32_t skip_index = skip_specified ? mp_obj_get_int(args[ARG_skip_index].u_obj) : 0;
    if (input_element_count < output_element_count) {
        mp_raise_IndexError(translate("index out of range"));
    }

    common_hal_bitmaptools_arrayblit(bitmap, bufinfo.buf, element_size, x1, y1, x2, y2, skip_specified, skip_index);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(bitmaptools_arrayblit_obj, 0, bitmaptools_arrayblit);


//| def readinto(
//|     bitmap: displayio.Bitmap,
//|     file: typing.BinaryIO,
//|     bits_per_pixel: int,
//|     element_size: int = 1,
//|     reverse_pixels_in_element: bool = False,
//|     swap_bytes_in_element: bool = False,
//|     reverse_rows: bool = False,
//| ) -> None:
//|     """Reads from a binary file into a bitmap.
//|
//|     The file must be positioned so that it consists of ``bitmap.height`` rows of pixel data, where each row is the smallest multiple of ``element_size`` bytes that can hold ``bitmap.width`` pixels.
//|
//|     The bytes in an element can be optionally swapped, and the pixels in an element can be reversed.  Also, the
//|     row loading direction can be reversed, which may be requires for loading certain bitmap files.
//|
//|     This function doesn't parse image headers, but is useful to speed up loading of uncompressed image formats such as PCF glyph data.
//|
//|     :param displayio.Bitmap bitmap: A writable bitmap
//|     :param typing.BinaryIO file: A file opened in binary mode
//|     :param int bits_per_pixel: Number of bits per pixel.  Values 1, 2, 4, 8, 16, 24, and 32 are supported;
//|     :param int element_size: Number of bytes per element.  Values of 1, 2, and 4 are supported, except that 24 ``bits_per_pixel`` requires 1 byte per element.
//|     :param bool reverse_pixels_in_element: If set, the first pixel in a word is taken from the Most Significant Bits; otherwise, it is taken from the Least Significant Bits.
//|     :param bool swap_bytes_in_element: If the ``element_size`` is not 1, then reverse the byte order of each element read.
//|     :param bool reverse_rows: Reverse the direction of the row loading (required for some bitmap images).
//|     """
//|     ...
//|

STATIC mp_obj_t bitmaptools_readinto(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_bitmap, ARG_file, ARG_bits_per_pixel, ARG_element_size, ARG_reverse_pixels_in_element, ARG_swap_bytes_in_element, ARG_reverse_rows };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bitmap, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_file, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_bits_per_pixel, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_element_size, MP_ARG_INT, { .u_int = 1 } },
        { MP_QSTR_reverse_pixels_in_element, MP_ARG_BOOL, { .u_bool = false } },
        { MP_QSTR_swap_bytes_in_element,  MP_ARG_BOOL, { .u_bool = false } },
        { MP_QSTR_reverse_rows,  MP_ARG_BOOL, { .u_bool = false } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    displayio_bitmap_t *bitmap = mp_arg_validate_type(args[ARG_bitmap].u_obj, &displayio_bitmap_type, MP_QSTR_bitmap);

    mp_obj_t *file = args[ARG_file].u_obj;

    int element_size = args[ARG_element_size].u_int;
    if (element_size != 1 && element_size != 2 && element_size != 4) {
        mp_raise_ValueError_varg(translate("invalid element_size %d, must be, 1, 2, or 4"), element_size);
    }

    int bits_per_pixel = args[ARG_bits_per_pixel].u_int;
    switch (bits_per_pixel) {
        case 24:
            if (element_size != 1) {
                mp_raise_ValueError_varg(translate("invalid element size %d for bits_per_pixel %d\n"), element_size, bits_per_pixel);
            }
            break;
        case 1:
        case 2:
        case 4:
        case 8:
        case 16:
        case 32:
            break;
        default:
            mp_raise_ValueError_varg(translate("invalid bits_per_pixel %d, must be, 1, 2, 4, 8, 16, 24, or 32"), bits_per_pixel);
    }

    bool reverse_pixels_in_element = args[ARG_reverse_pixels_in_element].u_bool;
    bool swap_bytes_in_element = args[ARG_swap_bytes_in_element].u_bool;
    bool reverse_rows = args[ARG_reverse_rows].u_bool;

    common_hal_bitmaptools_readinto(bitmap, file, element_size, bits_per_pixel, reverse_pixels_in_element, swap_bytes_in_element, reverse_rows);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(bitmaptools_readinto_obj, 0, bitmaptools_readinto);

//| class DitherAlgorithm:
//|     """Identifies the algorithm for dither to use"""
//|
//|     Atkinson: "DitherAlgorithm"
//|     """The classic Atkinson dither, often associated with the Hypercard esthetic"""
//|
//|     FloydStenberg: "DitherAlgorithm"
//|     """The Floyd-Stenberg dither"""
//|
MAKE_ENUM_VALUE(bitmaptools_dither_algorithm_type, dither_algorithm, Atkinson, DITHER_ALGORITHM_ATKINSON);
MAKE_ENUM_VALUE(bitmaptools_dither_algorithm_type, dither_algorithm, FloydStenberg, DITHER_ALGORITHM_FLOYD_STENBERG);

MAKE_ENUM_MAP(bitmaptools_dither_algorithm) {
    MAKE_ENUM_MAP_ENTRY(dither_algorithm, Atkinson),
    MAKE_ENUM_MAP_ENTRY(dither_algorithm, FloydStenberg),
};
STATIC MP_DEFINE_CONST_DICT(bitmaptools_dither_algorithm_locals_dict, bitmaptools_dither_algorithm_locals_table);

MAKE_PRINTER(bitmaptools, bitmaptools_dither_algorithm);

MAKE_ENUM_TYPE(bitmaptools, DitherAlgorithm, bitmaptools_dither_algorithm);

//| def dither(
//|     dest_bitmap: displayio.Bitmap,
//|     source_bitmapp: displayio.Bitmap,
//|     source_colorspace: displayio.Colorspace,
//|     algorithm: DitherAlgorithm = DitherAlgorithm.Atkinson,
//| ) -> None:
//|     """Convert the input image into a 2-level output image using the given dither algorithm.
//|
//|     :param bitmap dest_bitmap: Destination bitmap.  It must have a value_count of 2 or 65536.  The stored values are 0 and the maximum pixel value.
//|     :param bitmap source_bitmap: Source bitmap that contains the graphical region to be dithered.  It must have a value_count of 65536.
//|     :param colorspace: The colorspace of the image.  The supported colorspaces are ``RGB565``, ``BGR565``, ``RGB565_SWAPPED``, and ``BGR565_SWAPPED``
//|     :param algorithm: The dither algorithm to use, one of the `DitherAlgorithm` values.
//|     """
//|     ...
//|
STATIC mp_obj_t bitmaptools_dither(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_dest_bitmap, ARG_source_bitmap, ARG_source_colorspace, ARG_algorithm };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_dest_bitmap, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_source_bitmap, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_source_colorspace, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_algorithm, MP_ARG_OBJ, { .u_obj = MP_ROM_PTR((void *)&dither_algorithm_Atkinson_obj) } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    displayio_bitmap_t *source_bitmap = mp_arg_validate_type(args[ARG_source_bitmap].u_obj, &displayio_bitmap_type, MP_QSTR_source_bitmap);
    displayio_bitmap_t *dest_bitmap = mp_arg_validate_type(args[ARG_dest_bitmap].u_obj, &displayio_bitmap_type, MP_QSTR_dest_bitmap);
    bitmaptools_dither_algorithm_t algorithm = cp_enum_value(&bitmaptools_dither_algorithm_type, args[ARG_algorithm].u_obj, MP_QSTR_algorithm);
    displayio_colorspace_t colorspace = cp_enum_value(&displayio_colorspace_type, args[ARG_source_colorspace].u_obj, MP_QSTR_source_colorspace);

    if (source_bitmap->width != dest_bitmap->width || source_bitmap->height != dest_bitmap->height) {
        mp_raise_TypeError(translate("bitmap sizes must match"));
    }

    if (dest_bitmap->bits_per_value != 16 && dest_bitmap->bits_per_value != 1) {
        mp_raise_TypeError(translate("source_bitmap must have value_count of 2 or 65536"));
    }


    switch (colorspace) {
        case DISPLAYIO_COLORSPACE_RGB565:
        case DISPLAYIO_COLORSPACE_RGB565_SWAPPED:
        case DISPLAYIO_COLORSPACE_BGR565:
        case DISPLAYIO_COLORSPACE_BGR565_SWAPPED:
            if (source_bitmap->bits_per_value != 16) {
                mp_raise_TypeError(translate("source_bitmap must have value_count of 65536"));
            }
            break;

        case DISPLAYIO_COLORSPACE_L8:
            if (source_bitmap->bits_per_value != 8) {
                mp_raise_TypeError(translate("source_bitmap must have value_count of 8"));
            }
            break;

        default:
            mp_raise_TypeError(translate("unsupported colorspace for dither"));
    }


    common_hal_bitmaptools_dither(dest_bitmap, source_bitmap, colorspace, algorithm);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(bitmaptools_dither_obj, 0, bitmaptools_dither);
// requires all 5 arguments

//| def draw_circle(
//|     dest_bitmap: displayio.Bitmap, x: int, y: int, radius: int, value: int
//| ) -> None:
//|     """Draws a circle into a bitmap specified using a center (x0,y0) and radius r.
//|
//|     :param bitmap dest_bitmap: Destination bitmap that will be written into
//|     :param int x: x-pixel position of the circle's center
//|     :param int y: y-pixel position of the circle's center
//|     :param int radius: circle's radius
//|     :param int value: Bitmap palette index that will be written into the
//|            circle in the destination bitmap
//|
//|     .. code-block:: Python
//|
//|        import board
//|        import displayio
//|        import bitmaptools
//|
//|        display = board.DISPLAY
//|        main_group = displayio.Group()
//|        display.root_group = main_group
//|
//|        palette = displayio.Palette(2)
//|        palette[0] = 0xffffff
//|        palette[1] = 0x440044
//|
//|        bmp = displayio.Bitmap(128,128, 2)
//|        bmp.fill(0)
//|
//|        bitmaptools.circle(64,64, 32, 1)
//|
//|        tilegrid = displayio.TileGrid(bitmap=bmp, pixel_shader=palette)
//|        main_group.append(tilegrid)
//|
//|        while True:
//|            pass
//|
//|     """
//|
//|     ...
//|
STATIC mp_obj_t bitmaptools_obj_draw_circle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {ARG_dest_bitmap, ARG_x, ARG_y, ARG_radius, ARG_value};

    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_dest_bitmap, MP_ARG_REQUIRED | MP_ARG_OBJ},
        {MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_INT},
        {MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_INT},
        {MP_QSTR_radius, MP_ARG_REQUIRED | MP_ARG_INT},
        {MP_QSTR_value, MP_ARG_REQUIRED | MP_ARG_INT},
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    displayio_bitmap_t *destination = MP_OBJ_TO_PTR(args[ARG_dest_bitmap].u_obj);     // the destination bitmap

    uint32_t value, color_depth;
    value = args[ARG_value].u_int;
    color_depth = (1 << destination->bits_per_value);
    if (color_depth <= value) {
        mp_raise_ValueError(translate("out of range of target"));
    }


    int16_t x = args[ARG_x].u_int;
    int16_t y = args[ARG_y].u_int;
    int16_t radius = args[ARG_radius].u_int;

    mp_arg_validate_int_range(x, 0, destination->width, MP_QSTR_x);
    mp_arg_validate_int_range(y, 0, destination->height, MP_QSTR_y);
    mp_arg_validate_int_min(radius, 0, MP_QSTR_radius);

    common_hal_bitmaptools_draw_circle(destination, x, y, radius, value);

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_KW(bitmaptools_draw_circle_obj, 0, bitmaptools_obj_draw_circle);

//|     def blit(
//|         dest_bitmap: displayio.Bitmap,
//|         source_bitmap: displayio.Bitmap,
//|         x: int,
//|         y: int,
//|         *,
//|         x1: int,
//|         y1: int,
//|         x2: int,
//|         y2: int,
//|         skip_source_index: int,
//|         skip_dest_index: int
//|     ) -> None:
//|         """Inserts the source_bitmap region defined by rectangular boundaries
//|                     (x1,y1) and (x2,y2) into the bitmap at the specified (x,y) location.
//|
//|         :param bitmap dest_bitmap: Destination bitmap that the area will be copied into.
//|         :param bitmap source_bitmap: Source bitmap that contains the graphical region to be copied
//|         :param int x: Horizontal pixel location in bitmap where source_bitmap upper-left
//|                       corner will be placed
//|         :param int y: Vertical pixel location in bitmap where source_bitmap upper-left
//|                       corner will be placed
//|         :param int x1: Minimum x-value for rectangular bounding box to be copied from the source bitmap
//|         :param int y1: Minimum y-value for rectangular bounding box to be copied from the source bitmap
//|         :param int x2: Maximum x-value (exclusive) for rectangular bounding box to be copied from the source bitmap
//|         :param int y2: Maximum y-value (exclusive) for rectangular bounding box to be copied from the source bitmap
//|         :param int skip_source_index: bitmap palette index in the source that will not be copied,
//|                                set to None to copy all pixels
//|         :param int skip_dest_index: bitmap palette index in the destination bitmap that will not get overwritten
//|                                 by the pixels from the source"""
//|         ...
//|
STATIC mp_obj_t bitmaptools_obj_blit(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {ARG_destination, ARG_source, ARG_x, ARG_y, ARG_x1, ARG_y1, ARG_x2, ARG_y2, ARG_skip_source_index, ARG_skip_dest_index};
    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_dest_bitmap, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        {MP_QSTR_source_bitmap, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        {MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL} },
        {MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL} },
        {MP_QSTR_x1, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        {MP_QSTR_y1, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        {MP_QSTR_x2, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} }, // None convert to source->width
        {MP_QSTR_y2, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} }, // None convert to source->height
        {MP_QSTR_skip_source_index, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        {MP_QSTR_skip_dest_index, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    // mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // displayio_bitmap_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    displayio_bitmap_t *destination = mp_arg_validate_type(args[ARG_destination].u_obj, &displayio_bitmap_type, MP_QSTR_dest_bitmap);
    // check_for_deinit(destination);

    // Check x,y are within self (target) bitmap boundary
    int16_t x = mp_arg_validate_int_range(args[ARG_x].u_int, 0, MAX(0, destination->width - 1), MP_QSTR_x);
    int16_t y = mp_arg_validate_int_range(args[ARG_y].u_int, 0, MAX(0, destination->height - 1), MP_QSTR_y);


    displayio_bitmap_t *source = mp_arg_validate_type(args[ARG_source].u_obj, &displayio_bitmap_type, MP_QSTR_source_bitmap);


    // ensure that the target bitmap (self) has at least as many `bits_per_value` as the source
    if (destination->bits_per_value < source->bits_per_value) {
        mp_raise_ValueError(translate("source palette too large"));
    }

    // Check x1,y1,x2,y2 are within source bitmap boundary
    int16_t x1 = mp_arg_validate_int_range(args[ARG_x1].u_int, 0, MAX(0, source->width - 1), MP_QSTR_x1);
    int16_t y1 = mp_arg_validate_int_range(args[ARG_y1].u_int, 0, MAX(0, source->height - 1), MP_QSTR_y1);
    int16_t x2, y2;
    // if x2 or y2 is None, then set as the maximum size of the source bitmap
    if (args[ARG_x2].u_obj == mp_const_none) {
        x2 = source->width;
    } else {
        x2 = mp_arg_validate_int_range(mp_obj_get_int(args[ARG_x2].u_obj), 0, source->width, MP_QSTR_x2);
    }
    // int16_t y2;
    if (args[ARG_y2].u_obj == mp_const_none) {
        y2 = source->height;
    } else {
        y2 = mp_arg_validate_int_range(mp_obj_get_int(args[ARG_y2].u_obj), 0, source->height, MP_QSTR_y2);
    }

    // Ensure x1 < x2 and y1 < y2
    if (x1 > x2) {
        int16_t temp = x2;
        x2 = x1;
        x1 = temp;
    }
    if (y1 > y2) {
        int16_t temp = y2;
        y2 = y1;
        y1 = temp;
    }

    uint32_t skip_source_index;
    bool skip_source_index_none; // flag whether skip_value was None

    if (args[ARG_skip_source_index].u_obj == mp_const_none) {
        skip_source_index = 0;
        skip_source_index_none = true;
    } else {
        skip_source_index = mp_obj_get_int(args[ARG_skip_source_index].u_obj);
        skip_source_index_none = false;
    }

    uint32_t skip_dest_index;
    bool skip_dest_index_none; // flag whether skip_self_value was None

    if (args[ARG_skip_dest_index].u_obj == mp_const_none) {
        skip_dest_index = 0;
        skip_dest_index_none = true;
    } else {
        skip_dest_index = mp_obj_get_int(args[ARG_skip_dest_index].u_obj);
        skip_dest_index_none = false;
    }

    common_hal_bitmaptools_blit(destination, source, x, y, x1, y1, x2, y2, skip_source_index, skip_source_index_none, skip_dest_index,
        skip_dest_index_none);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(bitmaptools_blit_obj, 1, bitmaptools_obj_blit);


STATIC const mp_rom_map_elem_t bitmaptools_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_bitmaptools) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&bitmaptools_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_rotozoom), MP_ROM_PTR(&bitmaptools_rotozoom_obj) },
    { MP_ROM_QSTR(MP_QSTR_arrayblit), MP_ROM_PTR(&bitmaptools_arrayblit_obj) },
    { MP_ROM_QSTR(MP_QSTR_alphablend), MP_ROM_PTR(&bitmaptools_alphablend_obj) },
    { MP_ROM_QSTR(MP_QSTR_fill_region), MP_ROM_PTR(&bitmaptools_fill_region_obj) },
    { MP_ROM_QSTR(MP_QSTR_boundary_fill), MP_ROM_PTR(&bitmaptools_boundary_fill_obj) },
    { MP_ROM_QSTR(MP_QSTR_draw_line), MP_ROM_PTR(&bitmaptools_draw_line_obj) },
    { MP_ROM_QSTR(MP_QSTR_draw_polygon), MP_ROM_PTR(&bitmaptools_draw_polygon_obj) },
    { MP_ROM_QSTR(MP_QSTR_draw_circle), MP_ROM_PTR(&bitmaptools_draw_circle_obj) },
    { MP_ROM_QSTR(MP_QSTR_blit), MP_ROM_PTR(&bitmaptools_blit_obj) },
    { MP_ROM_QSTR(MP_QSTR_dither), MP_ROM_PTR(&bitmaptools_dither_obj) },
    { MP_ROM_QSTR(MP_QSTR_DitherAlgorithm), MP_ROM_PTR(&bitmaptools_dither_algorithm_type) },
};
STATIC MP_DEFINE_CONST_DICT(bitmaptools_module_globals, bitmaptools_module_globals_table);

const mp_obj_module_t bitmaptools_module = {
    .base = {&mp_type_module },
    .globals = (mp_obj_dict_t *)&bitmaptools_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_bitmaptools, bitmaptools_module, CIRCUITPY_BITMAPTOOLS);
