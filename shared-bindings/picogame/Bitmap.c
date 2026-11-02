// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT
//
// picogame.Bitmap: pixel data (PAL8 or RGB565) shared by sprites, tilemaps and blits.

#include "py/runtime.h"
#include "shared-module/picogame/pg_compat.h"
#include "shared-bindings/busdisplay/BusDisplay.h"
#include "shared-bindings/picogame/__init__.h"
#include "shared-bindings/picogame/Bitmap.h"
#include "shared-bindings/picogame/Sprite.h"
#if CIRCUITPY_PICOGAME_FAST_DISPLAY
#include "shared-bindings/picogame/Display.h"   // fast DMA backend; absent on portable ports
#include "shared-module/picogame/Display.h"        // its struct (pg_get_display unwraps the wrapper)
#endif
#include "shared-bindings/picogame/Scene.h"
#include "shared-bindings/picogame/Tilemap.h"
#include "shared-bindings/picogame/Particles.h"
#include "shared-bindings/picogame/Canvas.h"
#include "shared-bindings/picogame/Framebuffer.h"
#include "shared-module/picogame/__init__.h"
#include "shared-module/picogame/Bitmap.h"
#include "shared-module/picogame/Sprite.h"

// ---------------------------------------------------------------------------
// Bitmap
// ---------------------------------------------------------------------------

//| class Bitmap:
//|     """An image atlas of one or more frames that all share one size. RGB565
//|     pixel data and palette entries must be in the display's transfer byte
//|     order (use :py:func:`rgb565` to build colors); PAL8 pixel data are
//|     palette indices.
//|
//|     The bitmap references the caller-provided data without copying or modifying
//|     it. The backing buffer may live in RAM or in read-only memory."""
//|
//|     def __init__(
//|         self,
//|         data: ReadableBuffer,
//|         width: int,
//|         height: int,
//|         *,
//|         format: int = RGB565,
//|         palette: Optional[ReadableBuffer] = None,
//|         frames: int = 1,
//|         stride: int = 0,
//|         transparent: Optional[int] = None,
//|     ) -> None:
//|         """:param ~circuitpython_typing.ReadableBuffer data: RGB565 pixel data in
//|             the display's transfer byte order, or palette indices for ``PAL8``
//|         :param int width: width of one frame in pixels
//|         :param int height: height of one frame in pixels
//|         :param int format: :py:data:`RGB565` (2 bytes per pixel) or :py:data:`PAL8`
//|             (1 byte per pixel, indexing ``palette``)
//|         :param ~circuitpython_typing.ReadableBuffer palette: for ``PAL8``, a buffer
//|             of transfer-order RGB565 colors, two bytes per entry. Every byte in
//|             ``data`` must be a valid palette index, that is, less than
//|             ``len(palette) // 2``.
//|         :param int frames: number of equal-size frames, laid out left to right in
//|             one horizontal atlas
//|         :param int stride: distance between two rows in pixels. ``0``, the
//|             default, means ``width * frames``; set it explicitly to reference a
//|             sub-region of a wider image.
//|         :param int transparent: color (``RGB565``) or index (``PAL8``) that is
//|             skipped when drawing. Defaults to `None`, fully opaque."""
//|         ...
//|
static mp_obj_t picogame_bitmap_make_new(const mp_obj_type_t *type, size_t n_args,
    size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_data, ARG_width, ARG_height, ARG_format, ARG_palette, ARG_frames, ARG_stride, ARG_transparent };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_data, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_width, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_height, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_format, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = PICOGAME_FMT_RGB565} },
        { MP_QSTR_palette, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_frames, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_stride, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_transparent, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // bound BEFORE any arithmetic: an unbounded width could overflow width*frames (int32) and wrap
    // small, slipping past the size guards below into an undersized buffer (OOB read in the blitter).
    mp_int_t width = mp_arg_validate_int_range(args[ARG_width].u_int, 1, 65535, MP_QSTR_width);
    mp_int_t height = mp_arg_validate_int_range(args[ARG_height].u_int, 1, 65535, MP_QSTR_height);
    mp_int_t frames = mp_arg_validate_int_range(args[ARG_frames].u_int, 1, 255, MP_QSTR_frames);
    mp_int_t format = args[ARG_format].u_int;
    if (format != PICOGAME_FMT_RGB565 && format != PICOGAME_FMT_PAL8) {
        mp_raise_ValueError(MP_ERROR_TEXT("Invalid format"));
    }
    mp_int_t stride = args[ARG_stride].u_int;
    if (stride <= 0) {
        stride = width * frames;
    }
    // stride must hold the whole horizontal atlas, and the dims must fit the uint16_t fields,
    // or blits index past a row / the stored stride truncates.
    mp_arg_validate_int_max(width * frames, 65535, MP_QSTR_width);
    mp_arg_validate_int_max(stride, 65535, MP_QSTR_stride);
    mp_arg_validate_int_min(stride, width * frames, MP_QSTR_stride);

    mp_buffer_info_t data_info;
    mp_get_buffer_raise(args[ARG_data].u_obj, &data_info, MP_BUFFER_READ);

    const uint16_t *palette = NULL;
    mp_obj_t palette_obj = MP_OBJ_NULL;
    size_t pal_len = 0;
    if (format == PICOGAME_FMT_PAL8) {
        if (args[ARG_palette].u_obj == mp_const_none) {
            mp_arg_error_invalid(MP_QSTR_palette);   // PAL8 needs one
        }
        mp_buffer_info_t pal_info;
        mp_get_buffer_raise(args[ARG_palette].u_obj, &pal_info, MP_BUFFER_READ);
        palette = pal_info.buf;
        palette_obj = args[ARG_palette].u_obj;
        pal_len = pal_info.len;
    }

    size_t bpp = (format == PICOGAME_FMT_PAL8) ? 1 : 2;
    // 64-bit: stride*height*bpp can exceed 32 bits (stride,height <= 65535) and wrap small in a 32-bit
    // size_t, letting a tiny buffer pass this check -> OOB read in the blitter. Compute + compare wide.
    uint64_t need = (uint64_t)stride * (uint64_t)height * (uint64_t)bpp;
    if ((uint64_t)data_info.len < need) {
        mp_raise_ValueError(MP_ERROR_TEXT("buffer too small"));
    }
    if (format == PICOGAME_FMT_PAL8 && pal_len < 2) {
        // Need >=1 entry so pal[0] is valid. Contract (see blitter): PAL8 indices MUST be < palette
        // length. The caller may write indices into the (mutable) `data` buffer directly, so we cannot
        // validate once here. An out-of-range index is undefined behaviour (reads past the palette:
        // usually a garbage colour, but it may fault on some platforms). Per-pixel clamping was dropped
        // for speed - see the "restore full bounds-safety" note in the PAL8 blit loop if it's needed.
        mp_arg_validate_length_min(pal_len, 1, MP_QSTR_palette);
    }

    picogame_bitmap_obj_t *self = mp_obj_malloc(picogame_bitmap_obj_t, type);
    self->data_obj = args[ARG_data].u_obj;
    self->palette_obj = palette_obj;
    self->data = data_info.buf;
    self->palette = palette;
    self->width = width;
    self->height = height;
    self->stride = stride;
    self->frames = frames;
    self->format = format;
    // palette length in entries (informational; blitter assumes indices < this - see blit contract).
    self->pal_entries = (uint16_t)((pal_len / 2) > 65535 ? 65535 : (pal_len / 2));
    if (args[ARG_transparent].u_obj != mp_const_none) {
        mp_int_t t = mp_obj_get_int(args[ARG_transparent].u_obj);
        // PAL8 keys are indexes: narrow to 8 bits so every blit path agrees.
        self->transparent = (format == PICOGAME_FMT_PAL8) ? (uint8_t)t : (uint16_t)t;
        self->has_transparent = true;
    } else {
        self->transparent = 0;
        self->has_transparent = false;
    }
    return MP_OBJ_FROM_PTR(self);
}

//|
//|     width: int
//|     """Width of one frame in pixels. (read-only)"""
//|     height: int
//|     """Height of one frame in pixels. (read-only)"""
//|     frames: int
//|     """Number of frames in the atlas. (read-only)"""
static mp_obj_t bitmap_get_width(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(((picogame_bitmap_obj_t *)MP_OBJ_TO_PTR(self_in))->width);
}
static MP_DEFINE_CONST_FUN_OBJ_1(bitmap_get_width_obj, bitmap_get_width);
MP_PROPERTY_GETTER(bitmap_width_obj, (mp_obj_t)&bitmap_get_width_obj);

static mp_obj_t bitmap_get_height(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(((picogame_bitmap_obj_t *)MP_OBJ_TO_PTR(self_in))->height);
}
static MP_DEFINE_CONST_FUN_OBJ_1(bitmap_get_height_obj, bitmap_get_height);
MP_PROPERTY_GETTER(bitmap_height_obj, (mp_obj_t)&bitmap_get_height_obj);

static mp_obj_t bitmap_get_frames(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(((picogame_bitmap_obj_t *)MP_OBJ_TO_PTR(self_in))->frames);
}
static MP_DEFINE_CONST_FUN_OBJ_1(bitmap_get_frames_obj, bitmap_get_frames);
MP_PROPERTY_GETTER(bitmap_frames_obj, (mp_obj_t)&bitmap_get_frames_obj);

//|     format: int
//|     """:py:data:`RGB565` or :py:data:`PAL8`. (read-only)"""
static mp_obj_t bitmap_get_format(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(((picogame_bitmap_obj_t *)MP_OBJ_TO_PTR(self_in))->format);
}
static MP_DEFINE_CONST_FUN_OBJ_1(bitmap_get_format_obj, bitmap_get_format);
MP_PROPERTY_GETTER(bitmap_format_obj, (mp_obj_t)&bitmap_get_format_obj);

//|     stride: int
//|     """Row stride in pixels. (read-only)"""
static mp_obj_t bitmap_get_stride(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(((picogame_bitmap_obj_t *)MP_OBJ_TO_PTR(self_in))->stride);
}
static MP_DEFINE_CONST_FUN_OBJ_1(bitmap_get_stride_obj, bitmap_get_stride);
MP_PROPERTY_GETTER(bitmap_stride_obj, (mp_obj_t)&bitmap_get_stride_obj);

//|     palette: Optional[ReadableBuffer]
//|     """The ``PAL8`` palette buffer this Bitmap was built with, or `None` for
//|     ``RGB565``. (read-only)"""
static mp_obj_t bitmap_get_palette(mp_obj_t self_in) {
    picogame_bitmap_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return (self->palette_obj == MP_OBJ_NULL) ? mp_const_none : self->palette_obj;
}
static MP_DEFINE_CONST_FUN_OBJ_1(bitmap_get_palette_obj, bitmap_get_palette);
MP_PROPERTY_GETTER(bitmap_palette_obj, (mp_obj_t)&bitmap_get_palette_obj);

//|     transparent: Optional[int]
//|     """The transparent color or index, or `None` if the Bitmap is fully
//|     opaque. (read-only)"""
//|
//|
static mp_obj_t bitmap_get_transparent(mp_obj_t self_in) {
    picogame_bitmap_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return self->has_transparent ? MP_OBJ_NEW_SMALL_INT(self->transparent) : mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(bitmap_get_transparent_obj, bitmap_get_transparent);
MP_PROPERTY_GETTER(bitmap_transparent_obj, (mp_obj_t)&bitmap_get_transparent_obj);

static const mp_rom_map_elem_t picogame_bitmap_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&bitmap_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_PTR(&bitmap_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_frames), MP_ROM_PTR(&bitmap_frames_obj) },
    { MP_ROM_QSTR(MP_QSTR_format), MP_ROM_PTR(&bitmap_format_obj) },
    { MP_ROM_QSTR(MP_QSTR_stride), MP_ROM_PTR(&bitmap_stride_obj) },
    { MP_ROM_QSTR(MP_QSTR_palette), MP_ROM_PTR(&bitmap_palette_obj) },
    { MP_ROM_QSTR(MP_QSTR_transparent), MP_ROM_PTR(&bitmap_transparent_obj) },
};
static MP_DEFINE_CONST_DICT(picogame_bitmap_locals_dict, picogame_bitmap_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    picogame_bitmap_type,
    MP_QSTR_Bitmap,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, picogame_bitmap_make_new,
    locals_dict, &picogame_bitmap_locals_dict
    );
