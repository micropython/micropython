/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Jeff Epler for Adafruit Industries
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

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/displayio/Bitmap.h"
#include "shared-bindings/jpegio/JpegDecoder.h"
#include "shared-module/jpegio/JpegDecoder.h"
#include "shared-module/displayio/Bitmap.h"

//| class JpegDecoder:
//|     """A JPEG decoder
//|
//|     A JPEG decoder allocates a few thousand bytes of memory. To reduce memory fragmentation,
//|     create a single JpegDecoder object and use it anytime a JPEG image needs to be decoded.
//|     """
//|
//|     def __init__(self) -> None: ...
//|
STATIC mp_obj_t jpegio_jpegdecoder_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    static const mp_arg_t allowed_args[] = {
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    jpegio_jpegdecoder_obj_t *self = mp_obj_malloc(jpegio_jpegdecoder_obj_t, &jpegio_jpegdecoder_type);
    self->base.type = &jpegio_jpegdecoder_type;
    common_hal_jpegio_jpegdecoder_construct(self);

    return MP_OBJ_FROM_PTR(self);
}

//|     def decode(
//|         self, data: ReadableBuffer, bitmap: displayio.Bitmap | None = None, scale=0
//|     ) -> tuple[int, int]:
//|         """Decode JPEG data
//|
//|         If ``bitmap`` is None, only the header is decoded.
//|         Otherwise, the bitmap must be large enough to contain the decoded image.
//|         The pixel data is stored in the `displayio.Colorspace.RGB565_SWAPPED` colorspace.
//|
//|         The image is optionally downscaled by a factor of ``2**scale``.
//|         Scaling by a factor of 8 (scale=3) is particularly efficient in terms of decoding time.
//|
//|         :param ReadableBuffer data: Data in JPEG format
//|         :param Bitmap bitmap: Optional output buffer
//|         :param int scale: Scale factor from 0 to 3.
//|         :returns: The size of the (possibly scaled) image as ``width, height``
//|         """
//|
STATIC mp_obj_t jpegio_jpegdecoder_decode(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    jpegio_jpegdecoder_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    enum { ARG_data, ARG_bitmap, ARG_scale };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_data, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = mp_const_none } },
        { MP_QSTR_bitmap, MP_ARG_OBJ, {.u_obj = mp_const_none } },
        { MP_QSTR_scale, MP_ARG_INT, {.u_int = 0 } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_data].u_obj, &bufinfo, MP_BUFFER_READ);

    mp_obj_t bitmap_in = args[ARG_bitmap].u_obj;
    mp_arg_validate_type_or_none(bitmap_in, &displayio_bitmap_type, MP_QSTR_bitmap);
    displayio_bitmap_t *bitmap = (args[ARG_bitmap].u_obj != mp_const_none) ? MP_OBJ_TO_PTR(args[ARG_bitmap].u_obj) : NULL;

    int scale = args[ARG_scale].u_int;
    mp_arg_validate_int_range(scale, 0, 3, MP_QSTR_scale);

    return common_hal_jpegio_jpegdecoder_decode(self, bitmap, &bufinfo, scale);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(jpegio_jpegdecoder_decode_obj, 2, jpegio_jpegdecoder_decode);

STATIC const mp_rom_map_elem_t jpegio_jpegdecoder_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_decode), MP_ROM_PTR(&jpegio_jpegdecoder_decode_obj) },
};
STATIC MP_DEFINE_CONST_DICT(jpegio_jpegdecoder_locals_dict, jpegio_jpegdecoder_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    jpegio_jpegdecoder_type,
    MP_QSTR_JpegDecoder,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, jpegio_jpegdecoder_make_new,
    locals_dict, &jpegio_jpegdecoder_locals_dict
    );
