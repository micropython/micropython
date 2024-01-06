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

#include <math.h>

#include "py/runtime.h"
#include "shared-bindings/displayio/Bitmap.h"
#include "shared-bindings/bitmapfilter/__init__.h"

//|
//| def morph(
//|     bitmap: displayio.Bitmap,
//|     weights: tuple[int],
//|     mul: float = 1.0,
//|     add: int = 0,
//|     mask: displayio.Bitmap | None = None,
//|     threshold=False,
//|     offset: int = 0,
//|     invert: bool = False,
//| ):
//|     """Convolve an image with a kernel
//|
//|     The ``bitmap``, which must be in RGB565_SWAPPED format, is modified
//|     according to the ``weights``. Then a scaling factor ``m`` and an
//|     offset factor ``b`` are applied.
//|
//|     The ``weights`` must be a tuple of integers. The length of the tuple
//|     must be the square of an odd number, usually 9 and sometimes 25.
//|     Specific weights create different effects. For instance, these
//|     weights represent a 3x3 gaussian blur:
//|
//|     ``mul`` is number to multiply the convolution pixel results by. When
//|     not set it defaults to a value that will prevent scaling in the
//|     convolution output.
//|
//|     ``add`` is a value to add to each convolution pixel result.
//|
//|     ``mul`` basically allows you to do a global contrast adjustment and
//|     add allows you to do a global brightness adjustment. Pixels that go
//|     outside of the image mins and maxes for color channels will be
//|     clipped.
//|
//|     If you’d like to adaptive threshold the image on the output of the
//|     filter you can pass ``threshold=True`` which will enable adaptive
//|     thresholding of the image which sets pixels to one or zero based on a
//|     pixel’s brightness in relation to the brightness of the kernel of pixels
//|     around them. A negative ``offset`` value sets more pixels to 1 as you make
//|     it more negative while a positive value only sets the sharpest contrast
//|     changes to 1. Set ``invert`` to invert the binary image resulting output.
//|
//|     ``mask`` is another image to use as a pixel level mask for the operation.
//|     The mask should be an image with just black or white pixels and should
//|     be the same size as the image being operated on. Only pixels set in the
//|     mask are modified.
//|
//|     .. code-block:: python
//|
//|         kernel_gauss_3 = [
//|             1, 2, 1,
//|             2, 4, 2,
//|             1, 2, 1]
//|
//|         def blur(bitmap):
//|             \"""Blur the bitmap with a 3x3 gaussian kernel\"""
//|             bitmapfilter.morph(bitmap, kernel_gauss_3, 1/sum(kernel_gauss_3))
//|     """
//|

STATIC mp_obj_t bitmapfilter_morph(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_bitmap, ARG_weights, ARG_mul, ARG_add, ARG_threshold, ARG_offset, ARG_invert, ARG_mask };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bitmap, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        { MP_QSTR_weights, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        { MP_QSTR_mul, MP_ARG_OBJ, { .u_obj = MP_ROM_NONE } },
        { MP_QSTR_add, MP_ARG_OBJ, { .u_obj = MP_ROM_INT(0) } },
        { MP_QSTR_threshold, MP_ARG_BOOL, { .u_bool = false } },
        { MP_QSTR_offset, MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_invert, MP_ARG_BOOL, { .u_bool = false } },
        { MP_QSTR_mask, MP_ARG_OBJ, { .u_obj = MP_ROM_NONE } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_arg_validate_type(args[ARG_bitmap].u_obj, &displayio_bitmap_type, MP_QSTR_bitmap);
    displayio_bitmap_t *bitmap = args[ARG_bitmap].u_obj;

    displayio_bitmap_t *mask = NULL; // the mask bitmap
    if (args[ARG_mask].u_obj != mp_const_none) {
        mp_arg_validate_type(args[ARG_mask].u_obj, &displayio_bitmap_type, MP_QSTR_mask);
        mask = MP_OBJ_TO_PTR(args[ARG_mask].u_obj);
    }

    mp_int_t b = mp_obj_get_int(args[ARG_add].u_obj);

    size_t n_weights;
    mp_obj_t weights = mp_arg_validate_type(args[ARG_weights].u_obj, &mp_type_tuple, MP_QSTR_weights);
    mp_obj_t *items;
    mp_obj_tuple_get(weights, &n_weights, &items);
    mp_arg_validate_length_min(n_weights, 9, MP_QSTR_items);
    size_t sq_n_weights = (int)MICROPY_FLOAT_C_FUN(sqrt)(n_weights);
    if (sq_n_weights % 2 == 0 || sq_n_weights * sq_n_weights != n_weights) {
        mp_raise_ValueError(MP_ERROR_TEXT("Must be an odd square number of weights"));
    }

    int iweights[n_weights];
    int weight_sum = 0;
    for (size_t i = 0; i < n_weights; i++) {
        mp_int_t j = mp_obj_get_int(items[i]);
        iweights[i] = j;
        weight_sum += j;
    }

    mp_float_t m = args[ARG_mul].u_obj != mp_const_none ? mp_obj_get_float(args[ARG_mul].u_obj) : 1 / (mp_float_t)weight_sum;

    shared_module_bitmapfilter_morph(bitmap, mask, sq_n_weights / 2, iweights, (float)m, b,
        args[ARG_threshold].u_bool, args[ARG_offset].u_bool, args[ARG_invert].u_bool);
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_KW(bitmapfilter_morph_obj, 0, bitmapfilter_morph);

STATIC const mp_rom_map_elem_t bitmapfilter_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_bitmapfilter) },
    { MP_ROM_QSTR(MP_QSTR_morph), MP_ROM_PTR(&bitmapfilter_morph_obj) },
};
STATIC MP_DEFINE_CONST_DICT(bitmapfilter_module_globals, bitmapfilter_module_globals_table);

const mp_obj_module_t bitmapfilter_module = {
    .base = {&mp_type_module },
    .globals = (mp_obj_dict_t *)&bitmapfilter_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_bitmapfilter, bitmapfilter_module);
