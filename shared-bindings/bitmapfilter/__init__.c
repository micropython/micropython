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
#include "shared-bindings/displayio/Palette.h"
#include "shared-bindings/bitmapfilter/__init__.h"

//|
//| def morph(
//|     bitmap: displayio.Bitmap,
//|     weights: Sequence[int],
//|     mul: float | None = None,
//|     add: float = 0,
//|     mask: displayio.Bitmap | None = None,
//|     threshold=False,
//|     offset: int = 0,
//|     invert: bool = False,
//| ) -> displayio.Bitmap:
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
//|     The mask should be an image the same size as the image being operated on.
//|     Only pixels set to a non-zero value in the mask are modified.
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


STATIC mp_float_t get_m(mp_obj_t mul_obj, mp_float_t sum) {
    return mul_obj != mp_const_none ? mp_obj_get_float(mul_obj) : sum ? 1 / (mp_float_t)sum : 1;
}

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

    mp_float_t b = mp_obj_get_float(args[ARG_add].u_obj);

    mp_obj_t weights = args[ARG_weights].u_obj;
    mp_obj_t obj_len = mp_obj_len(weights);
    if (obj_len == MP_OBJ_NULL || !mp_obj_is_small_int(obj_len)) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q must be of type %q, not %q"), MP_QSTR_weights, MP_QSTR_Sequence, mp_obj_get_type(weights)->name);
    }

    size_t n_weights = MP_OBJ_SMALL_INT_VALUE(obj_len);

    size_t sq_n_weights = (int)MICROPY_FLOAT_C_FUN(sqrt)(n_weights);
    if (sq_n_weights % 2 == 0 || sq_n_weights * sq_n_weights != n_weights) {
        mp_raise_ValueError(MP_ERROR_TEXT("weights must be a sequence with an odd square number of elements (usually 9 or 25)"));
    }

    int iweights[n_weights];
    int weight_sum = 0;
    for (size_t i = 0; i < n_weights; i++) {
        mp_int_t j = mp_obj_get_int(mp_obj_subscr(weights, MP_OBJ_NEW_SMALL_INT(i), MP_OBJ_SENTINEL));
        iweights[i] = j;
        weight_sum += j;
    }

    mp_float_t m = get_m(args[ARG_mul].u_obj, weight_sum);

    shared_module_bitmapfilter_morph(bitmap, mask, sq_n_weights / 2, iweights, m, b,
        args[ARG_threshold].u_bool, args[ARG_offset].u_bool, args[ARG_invert].u_bool);
    return args[ARG_bitmap].u_obj;
}
MP_DEFINE_CONST_FUN_OBJ_KW(bitmapfilter_morph_obj, 0, bitmapfilter_morph);

//|
//| def morph9(
//|     bitmap: displayio.Bitmap,
//|     weights: Sequence[int],
//|     mul: Sequence[float] | None,
//|     add: Sequence[float] | None,
//|     mask: displayio.Bitmap | None = None,
//|     threshold=False,
//|     offset: int = 0,
//|     invert: bool = False,
//| ) -> displayio.Bitmap:
//|     """Convolve an image with a kernel
//|
//|     This is like a combination of 9 calls to morph plus one call to mix. It's
//|     so complicated and hard to explain that it must be good for something.
//|
//|     The ``bitmap``, which must be in RGB565_SWAPPED format, is modified
//|     according to the ``weights``. Then a scaling factor ``m`` and an
//|     offset factor ``b`` are applied.
//|
//|     The ``weights`` must be a tuple of integers. The length of the tuple
//|     must be 9 times the square of an odd number, such as 81 or 225. The weights
//|     are taken in groups of 9, with the first 3 giving the proportion of each of
//|     R, G, and B that are mixed into the output R, the next three the
//|     proportions mixed into the output G, and the last 3 the proportions that
//|     are mixed into the output blue.
//|
//|     ``mul`` is a sequence of 3 numbers to multiply the convolution pixel
//|     results by. When not set it defaults to a value that will prevent scaling
//|     in the convolution output.
//|
//|     ``add`` is a sequence of 3 numbers giving a value to add to each
//|     convolution pixel result. If unspecified or None, 0 is used for all 3 channels.
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
//|     The mask should be an image the same size as the image being operated on.
//|     Only pixels set to a non-zero value in the mask are modified.
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

static mp_obj_t subscr(mp_obj_t o, int i) {
    return mp_obj_subscr(o, MP_OBJ_NEW_SMALL_INT(i), MP_OBJ_SENTINEL);
}

static mp_obj_t subscr_maybe(mp_obj_t o, int i) {
    return o == mp_const_none ? o : subscr(o, i);
}

static mp_float_t float_subscr(mp_obj_t o, int i) {
    return mp_obj_get_float(subscr(o, i));
}

STATIC mp_float_t float_subscr_maybe(mp_obj_t seq_maybe, int i, mp_float_t defval) {
    if (seq_maybe == mp_const_none) {
        return defval;
    }
    return float_subscr(seq_maybe, i);
}

STATIC mp_obj_t bitmapfilter_morph9(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_bitmap, ARG_weights, ARG_mul, ARG_add, ARG_threshold, ARG_offset, ARG_invert, ARG_mask };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bitmap, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        { MP_QSTR_weights, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        { MP_QSTR_mul, MP_ARG_OBJ, { .u_obj = MP_ROM_NONE } },
        { MP_QSTR_add, MP_ARG_OBJ, { .u_obj = MP_ROM_NONE } },
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

    mp_float_t b[3] = {
        float_subscr_maybe(args[ARG_add].u_obj, 0, 0),
        float_subscr_maybe(args[ARG_add].u_obj, 1, 0),
        float_subscr_maybe(args[ARG_add].u_obj, 2, 0),
    };

    mp_obj_t weights = args[ARG_weights].u_obj;
    mp_obj_t obj_len = mp_obj_len(weights);
    if (obj_len == MP_OBJ_NULL || !mp_obj_is_small_int(obj_len)) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q must be of type %q, not %q"), MP_QSTR_weights, MP_QSTR_Sequence, mp_obj_get_type(weights)->name);
    }

    size_t n_weights = MP_OBJ_SMALL_INT_VALUE(obj_len);

    size_t sq_n_weights = (int)MICROPY_FLOAT_C_FUN(sqrt)(n_weights / 9);
    if (sq_n_weights % 2 == 0 || sq_n_weights * sq_n_weights * 9 != n_weights) {
        mp_raise_ValueError(MP_ERROR_TEXT("weights must be a sequence with 9 times an odd square number of elements (usually 81 or 225)"));
    }

    int iweights[n_weights];
    int weight_sum[3] = {0, 0, 0};
    for (size_t i = 0; i < n_weights; i++) {
        mp_int_t j = mp_obj_get_int(mp_obj_subscr(weights, MP_OBJ_NEW_SMALL_INT(i), MP_OBJ_SENTINEL));
        iweights[i] = j;
        int target_channel = (i / 3) % 3;
        weight_sum[target_channel] += j;
    }

    mp_float_t m[3] = {
        get_m(subscr_maybe(args[ARG_mul].u_obj, 0), weight_sum[0]),
        get_m(subscr_maybe(args[ARG_mul].u_obj, 1), weight_sum[1]),
        get_m(subscr_maybe(args[ARG_mul].u_obj, 2), weight_sum[2])
    };

    shared_module_bitmapfilter_morph9(bitmap, mask, sq_n_weights / 2, iweights, m, b,
        args[ARG_threshold].u_bool, args[ARG_offset].u_bool, args[ARG_invert].u_bool);
    return args[ARG_bitmap].u_obj;
}
MP_DEFINE_CONST_FUN_OBJ_KW(bitmapfilter_morph9_obj, 0, bitmapfilter_morph9);

//| def mix(
//|     bitmap: displayio.Bitmap, weights: Sequence[int], mask: displayio.Bitmap | None = None
//| ) -> displayio.Bitmap:
//|     """Perform a channel mixing operation on the bitmap
//|
//|     The ``bitmap``, which must be in RGB565_SWAPPED format, is modified
//|     according to the ``weights``.
//|
//|     If ``weights`` is a list of length 3, then each channel is scaled independently:
//|     The numbers are the red, green, and blue channel scales.
//|
//|     If ``weights`` is a list of length 6, then each channel is scaled and
//|     offset independently: The first two numbers are applied to the red channel:
//|     scale and offset. The second two number are applied to the green
//|     channel, and the last two numbers to the blue channel.
//|
//|     If ``weights`` is a list of length 9, then channels are mixed. The first three
//|     numbers are the fraction of red, green and blue input channels mixed into the
//|     red output channel. The next 3 numbers are for green, and the final 3 are for blue.
//|
//|     If ``weights`` is a list of length 12, then channels are mixed with an offset.
//|     Every fourth value is the offset value.
//|
//|     ``mask`` is another image to use as a pixel level mask for the operation.
//|     The mask should be an image the same size as the image being operated on.
//|     Only pixels set to a non-zero value in the mask are modified.
//|
//|     For example, to perform a sepia conversion on an input image,
//|
//|     .. code-block:: python
//|
//|         sepia_weights = [
//|             .393,  .769,   .189,
//|             .349,  .686,   .168,
//|             .272,  .534,   .131]
//|
//|         def sepia(bitmap):
//|             \"""Convert the bitmap to sepia\"""
//|             bitmapfilter.mix(bitmap, sepia_weights)
//|     """
//|
STATIC mp_obj_t bitmapfilter_mix(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_bitmap, ARG_weights, ARG_mask };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bitmap, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        { MP_QSTR_weights, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        { MP_QSTR_mask, MP_ARG_OBJ, { .u_obj = MP_ROM_NONE } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_arg_validate_type(args[ARG_bitmap].u_obj, &displayio_bitmap_type, MP_QSTR_bitmap);
    displayio_bitmap_t *bitmap = MP_OBJ_TO_PTR(args[ARG_bitmap].u_obj);

    mp_float_t weights[12];
    memset(weights, 0, sizeof(weights));

    mp_obj_t weights_obj = args[ARG_weights].u_obj;
    mp_int_t len = mp_obj_get_int(mp_obj_len(weights_obj));

    switch (len) {
        case 3:
            for (int i = 0; i < 3; i++) {
                weights[5 * i] = float_subscr(weights_obj, i);
            }
            break;
        case 6:
            for (int i = 0; i < 3; i++) {
                weights[5 * i] = float_subscr(weights_obj, i * 2);
                weights[4 * i + 3] = float_subscr(weights_obj, i * 2 + 1);
            }
            break;
        case 9:
            for (int i = 0; i < 9; i++) {
                weights[i + i / 3] = float_subscr(weights_obj, i);
            }
            break;
        case 12:
            for (int i = 0; i < 12; i++) {
                weights[i] = float_subscr(weights_obj, i);
            }
            break;
        default:
            mp_raise_ValueError(
                MP_ERROR_TEXT("weights must be a sequence of length 3, 6, 9, or 12"));
    }


    displayio_bitmap_t *mask = NULL;
    if (args[ARG_mask].u_obj != mp_const_none) {
        mp_arg_validate_type(args[ARG_mask].u_obj, &displayio_bitmap_type, MP_QSTR_mask);
        mask = MP_OBJ_TO_PTR(args[ARG_mask].u_obj);
    }

    shared_module_bitmapfilter_mix(bitmap, mask, weights);
    return args[ARG_bitmap].u_obj;
}
MP_DEFINE_CONST_FUN_OBJ_KW(bitmapfilter_mix_obj, 0, bitmapfilter_mix);

STATIC mp_obj_t bitmapfilter_solarize(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_bitmap, ARG_threshold, ARG_mask };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bitmap, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        { MP_QSTR_threshold, MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        { MP_QSTR_mask, MP_ARG_OBJ, { .u_obj = MP_ROM_NONE } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_float_t threshold = (args[ARG_threshold].u_obj == NULL) ? MICROPY_FLOAT_CONST(0.5) : mp_obj_get_float(args[ARG_threshold].u_obj);
    mp_arg_validate_type(args[ARG_bitmap].u_obj, &displayio_bitmap_type, MP_QSTR_bitmap);
    displayio_bitmap_t *bitmap = MP_OBJ_TO_PTR(args[ARG_bitmap].u_obj);


    displayio_bitmap_t *mask = NULL;
    if (args[ARG_mask].u_obj != mp_const_none) {
        mp_arg_validate_type(args[ARG_mask].u_obj, &displayio_bitmap_type, MP_QSTR_mask);
        mask = MP_OBJ_TO_PTR(args[ARG_mask].u_obj);
    }

    shared_module_bitmapfilter_solarize(bitmap, mask, threshold);
    return args[ARG_bitmap].u_obj;
}

MP_DEFINE_CONST_FUN_OBJ_KW(bitmapfilter_solarize_obj, 0, bitmapfilter_solarize);


//| LookupFunction = Callable[[float], float]
//| ThreeLookupFunctions = Tuple[LookupFunction, LookupFunction, LookupFunction]
//|
//| def lookup(
//|     bitmap: displayio.Bitmap,
//|     lookup: LookupFunction | ThreeLookupFunctions,
//|     mask: displayio.Bitmap | None,
//| ) -> displayio.Bitmap:
//|     """Modify the channels of a bitmap according to a look-up table
//|
//|     This can be used to implement non-linear transformations of color values,
//|     such as gamma curves.
//|
//|     The ``bitmap``, which must be in RGB565_SWAPPED format, is modified
//|     according to the values of the ``lookup`` function or functions.
//|
//|     If one ``lookup`` function is supplied, the same function is used for all 3
//|     image channels. Otherwise, it must be a tuple of 3 functions. The first
//|     function is used for R, the second function for G, and the third for B.
//|
//|     Each lookup function is called for each possible channel value from 0 to 1
//|     inclusive (64 times for green, 32 times for red or blue), and the return
//|     value (also from 0 to 1) is used whenever that color value is returned.
//|
//|     ``mask`` is another image to use as a pixel level mask for the operation.
//|     The mask should be an image the same size as the image being operated on.
//|     Only pixels set to a non-zero value in the mask are modified.
//|     """
//|

STATIC int scaled_lut(int maxval, mp_obj_t func, int i) {
    mp_obj_t obj = mp_call_function_1(func, mp_obj_new_float(i / (mp_float_t)maxval));
    mp_float_t val = mp_obj_get_float(obj);
    return (int)MICROPY_FLOAT_C_FUN(round)(val * maxval);
}

STATIC mp_obj_t bitmapfilter_lookup(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_bitmap, ARG_lookup, ARG_mask };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bitmap, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        { MP_QSTR_lookup, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        { MP_QSTR_mask, MP_ARG_OBJ, { .u_obj = MP_ROM_NONE } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_arg_validate_type(args[ARG_bitmap].u_obj, &displayio_bitmap_type, MP_QSTR_bitmap);
    displayio_bitmap_t *bitmap = MP_OBJ_TO_PTR(args[ARG_bitmap].u_obj);

    mp_obj_t lookup_r, lookup_g, lookup_b;

    if (mp_obj_is_tuple_compatible(args[ARG_lookup].u_obj)) {
        mp_obj_tuple_t *lookup_tuple = MP_OBJ_TO_PTR(args[ARG_lookup].u_obj);
        mp_arg_validate_length(lookup_tuple->len, 3, MP_QSTR_lookup);
        lookup_r = lookup_tuple->items[0];
        lookup_g = lookup_tuple->items[1];
        lookup_b = lookup_tuple->items[2];
    } else {
        lookup_r = lookup_g = lookup_b = args[ARG_lookup].u_obj;
    }

    bitmapfilter_lookup_table_t table;

    for (int i = 0; i < 32; i++) {
        table.r[i] = scaled_lut(31, lookup_r, i);
        table.b[i] = lookup_r == lookup_b ? table.r[i] : scaled_lut(31, lookup_b, i);
    }
    for (int i = 0; i < 64; i++) {
        table.g[i] = scaled_lut(63, lookup_g, i);
    }

    displayio_bitmap_t *mask = NULL;
    if (args[ARG_mask].u_obj != mp_const_none) {
        mp_arg_validate_type(args[ARG_mask].u_obj, &displayio_bitmap_type, MP_QSTR_mask);
        mask = MP_OBJ_TO_PTR(args[ARG_mask].u_obj);
    }

    shared_module_bitmapfilter_lookup(bitmap, mask, &table);
    return args[ARG_bitmap].u_obj;
}

MP_DEFINE_CONST_FUN_OBJ_KW(bitmapfilter_lookup_obj, 0, bitmapfilter_lookup);

//| def false_color(
//|     bitmap: displayio.Bitmap,
//|     palette: displayio.Palette,
//|     mask: displayio.Bitmap | None,
//| ) -> displayio.Bitmap:
//|     """Convert the image to false color using the given palette
//|
//|     The ``bitmap``, which must be in RGB565_SWAPPED format, is converted into false color.
//|
//|     The ``palette``, which must be of length 256, is used as a look-up table.
//|
//|     Each pixel is converted to a luminance (brightness/greyscale) value
//|     in the range 0..255, then the corresponding palette entry is looked up and
//|     stored in the bitmap.
//|
//|     ``mask`` is another image to use as a pixel level mask for the operation.
//|     The mask should be an image the same size as the image being operated on.
//|     Only pixels set to a non-zero value in the mask are modified.
//|     """
//|
STATIC mp_obj_t bitmapfilter_false_color(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_bitmap, ARG_palette, ARG_mask };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bitmap, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        { MP_QSTR_palette, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        { MP_QSTR_mask, MP_ARG_OBJ, { .u_obj = MP_ROM_NONE } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_arg_validate_type(args[ARG_bitmap].u_obj, &displayio_bitmap_type, MP_QSTR_bitmap);
    displayio_bitmap_t *bitmap = MP_OBJ_TO_PTR(args[ARG_bitmap].u_obj);

    mp_arg_validate_type(args[ARG_palette].u_obj, &displayio_palette_type, MP_QSTR_palette);
    displayio_palette_t *palette = MP_OBJ_TO_PTR(args[ARG_palette].u_obj);
    mp_arg_validate_length(palette->color_count, 256, MP_QSTR_palette);

    displayio_bitmap_t *mask = NULL;
    if (args[ARG_mask].u_obj != mp_const_none) {
        mp_arg_validate_type(args[ARG_mask].u_obj, &displayio_bitmap_type, MP_QSTR_mask);
        mask = MP_OBJ_TO_PTR(args[ARG_mask].u_obj);
    }

    shared_module_bitmapfilter_false_color(bitmap, mask, palette->colors);
    return args[ARG_bitmap].u_obj;
}

MP_DEFINE_CONST_FUN_OBJ_KW(bitmapfilter_false_color_obj, 0, bitmapfilter_false_color);
STATIC const mp_rom_map_elem_t bitmapfilter_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_bitmapfilter) },
    { MP_ROM_QSTR(MP_QSTR_morph), MP_ROM_PTR(&bitmapfilter_morph_obj) },
    { MP_ROM_QSTR(MP_QSTR_morph9), MP_ROM_PTR(&bitmapfilter_morph9_obj) },
    { MP_ROM_QSTR(MP_QSTR_mix), MP_ROM_PTR(&bitmapfilter_mix_obj) },
    { MP_ROM_QSTR(MP_QSTR_solarize), MP_ROM_PTR(&bitmapfilter_solarize_obj) },
    { MP_ROM_QSTR(MP_QSTR_false_color), MP_ROM_PTR(&bitmapfilter_false_color_obj) },
    { MP_ROM_QSTR(MP_QSTR_lookup), MP_ROM_PTR(&bitmapfilter_lookup_obj) },
};
STATIC MP_DEFINE_CONST_DICT(bitmapfilter_module_globals, bitmapfilter_module_globals_table);

const mp_obj_module_t bitmapfilter_module = {
    .base = {&mp_type_module },
    .globals = (mp_obj_dict_t *)&bitmapfilter_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_bitmapfilter, bitmapfilter_module);
