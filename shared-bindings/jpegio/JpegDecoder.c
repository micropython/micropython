// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/builtin.h"
#include "py/runtime.h"

#include "shared-bindings/bitmaptools/__init__.h"
#include "shared-bindings/displayio/Bitmap.h"
#include "shared-bindings/jpegio/JpegDecoder.h"
#include "shared-module/jpegio/JpegDecoder.h"
#include "shared-module/displayio/Bitmap.h"

//| class JpegDecoder:
//|     """A JPEG decoder
//|
//|     A JpegDecoder allocates a few thousand bytes of memory. To reduce memory fragmentation,
//|     create a single JpegDecoder object and use it anytime a JPEG image needs to be decoded.
//|
//|     Example::
//|
//|         from jpegio import JpegDecoder
//|         from displayio import Bitmap
//|
//|         decoder = JpegDecoder()
//|         width, height = decoder.open("/sd/example.jpg")
//|         bitmap = Bitmap(width, height, 65535)
//|         decoder.decode(bitmap)
//|         # .. do something with bitmap
//|     """
//|
//|     def __init__(self) -> None: ...
//|
static mp_obj_t jpegio_jpegdecoder_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    static const mp_arg_t allowed_args[] = {
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    jpegio_jpegdecoder_obj_t *self = mp_obj_malloc(jpegio_jpegdecoder_obj_t, &jpegio_jpegdecoder_type);
    self->base.type = &jpegio_jpegdecoder_type;
    common_hal_jpegio_jpegdecoder_construct(self);

    return MP_OBJ_FROM_PTR(self);
}

//|     @overload
//|     def open(self, filename: str) -> Tuple[int, int]: ...
//|     @overload
//|     def open(self, buffer: ReadableBuffer) -> Tuple[int, int]: ...
//|     @overload
//|     def open(self, bytesio: io.BytesIO) -> Tuple[int, int]:
//|         """Use the specified object  as the JPEG data source.
//|
//|         The source may be a filename, a binary buffer in memory, or an opened binary stream.
//|
//|         The single parameter is positional-only (write ``open(f)``, not
//|         ``open(filename=f)`` but due to technical limitations this is
//|         not shown in the function signature in the documentation.
//|
//|         Returns the image size as the tuple ``(width, height)``."""
static mp_obj_t jpegio_jpegdecoder_open(mp_obj_t self_in, mp_obj_t arg) {
    jpegio_jpegdecoder_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (mp_obj_is_str(arg)) {
        arg = mp_call_function_2(
            MP_OBJ_FROM_PTR(&mp_builtin_open_obj),
            arg,
            MP_OBJ_NEW_QSTR(MP_QSTR_rb));
    }

    mp_buffer_info_t bufinfo;
    const mp_stream_p_t *proto = mp_get_stream(arg);

    if (proto && proto->read && !proto->is_text) {
        return common_hal_jpegio_jpegdecoder_set_source_file(self, arg);
    } else if (mp_get_buffer(arg, &bufinfo, MP_BUFFER_READ)) {
        return common_hal_jpegio_jpegdecoder_set_source_buffer(self, arg);
    }
    mp_raise_TypeError_varg(MP_ERROR_TEXT("%q must be of type %q, %q, or %q, not %q"), MP_QSTR_data_source, MP_QSTR_str, MP_QSTR_BytesIO, MP_QSTR_ReadableBuffer);
}
MP_DEFINE_CONST_FUN_OBJ_2(jpegio_jpegdecoder_open_obj, jpegio_jpegdecoder_open);

//|     def decode(
//|         self,
//|         bitmap: displayio.Bitmap,
//|         scale: int = 0,
//|         x: int = 0,
//|         y: int = 0,
//|         *,
//|         x1: int,
//|         y1: int,
//|         x2: int,
//|         y2: int,
//|         skip_source_index: int,
//|         skip_dest_index: int,
//|     ) -> None:
//|         """Decode JPEG data
//|
//|         The bitmap must be large enough to contain the decoded image.
//|         The pixel data is stored in the `displayio.Colorspace.RGB565_SWAPPED` colorspace.
//|
//|         The image is optionally downscaled by a factor of ``2**scale``.
//|         Scaling by a factor of 8 (scale=3) is particularly efficient in terms of decoding time.
//|
//|         The remaining parameters are as for `bitmaptools.blit`.
//|         Because JPEG is a lossy data format, chroma keying based on the "source
//|         index" is not reliable, because the same original RGB value might end
//|         up being decompressed as a similar but not equal color value. Using a
//|         higher JPEG encoding quality can help, but ultimately it will not be
//|         perfect.
//|
//|         After a call to ``decode``, you must ``open`` a new JPEG. It is not
//|         possible to repeatedly ``decode`` the same jpeg data, even if it is to
//|         select different scales or crop regions from it.
//|
//|         :param Bitmap bitmap: Optional output buffer
//|         :param int scale: Scale factor from 0 to 3, inclusive.
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
//|                                 by the pixels from the source
//|         """
//|
static mp_obj_t jpegio_jpegdecoder_decode(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    jpegio_jpegdecoder_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    enum { ARG_bitmap, ARG_scale, ARG_x, ARG_y, ARGS_X1_Y1_X2_Y2, ARG_skip_source_index, ARG_skip_dest_index };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bitmap, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = mp_const_none } },
        { MP_QSTR_scale, MP_ARG_INT, {.u_int = 0 } },
        { MP_QSTR_x, MP_ARG_INT, {.u_int = 0 } },
        { MP_QSTR_y, MP_ARG_INT, {.u_int = 0 } },
        ALLOWED_ARGS_X1_Y1_X2_Y2(0, 0),
        {MP_QSTR_skip_source_index, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        {MP_QSTR_skip_dest_index, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t bitmap_in = args[ARG_bitmap].u_obj;
    mp_arg_validate_type(bitmap_in, &displayio_bitmap_type, MP_QSTR_bitmap);
    displayio_bitmap_t *bitmap = MP_OBJ_TO_PTR(args[ARG_bitmap].u_obj);

    int scale = args[ARG_scale].u_int;
    mp_arg_validate_int_range(scale, 0, 3, MP_QSTR_scale);

    int x = mp_arg_validate_int_range(args[ARG_x].u_int, 0, bitmap->width, MP_QSTR_x);
    int y = mp_arg_validate_int_range(args[ARG_y].u_int, 0, bitmap->height, MP_QSTR_y);
    bitmaptools_rect_t lim = bitmaptools_validate_coord_range_pair(&args[ARG_x1], bitmap->width, bitmap->height);

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
    common_hal_jpegio_jpegdecoder_decode_into(self, bitmap, scale, x, y, &lim, skip_source_index, skip_source_index_none, skip_dest_index, skip_dest_index_none);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(jpegio_jpegdecoder_decode_obj, 1, jpegio_jpegdecoder_decode);

static const mp_rom_map_elem_t jpegio_jpegdecoder_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&jpegio_jpegdecoder_open_obj) },
    { MP_ROM_QSTR(MP_QSTR_decode), MP_ROM_PTR(&jpegio_jpegdecoder_decode_obj) },
};
static MP_DEFINE_CONST_DICT(jpegio_jpegdecoder_locals_dict, jpegio_jpegdecoder_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    jpegio_jpegdecoder_type,
    MP_QSTR_JpegDecoder,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, jpegio_jpegdecoder_make_new,
    locals_dict, &jpegio_jpegdecoder_locals_dict
    );
