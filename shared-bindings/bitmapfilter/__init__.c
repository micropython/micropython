// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <math.h>

#include "py/runtime.h"
#include "py/objnamedtuple.h"
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
//|     The name of the function comes from
//|     `OpenMV <https://docs.openmv.io/library/omv.image.html#image.Image.morph>`_.
//|     ImageMagick calls this "-morphology" ("-morph" is an unrelated image blending
//|     algorithm). PIL calls this "kernel".
//|
//|     For background on how this kind of image processing, including some
//|     useful ``weights`` values, see `wikipedia's article on the
//|     subject <https://en.wikipedia.org/wiki/Kernel_(image_processing)>`_.
//|
//|     The ``bitmap``, which must be in RGB565_SWAPPED format, is modified
//|     according to the ``weights``. Then a scaling factor ``mul`` and an
//|     offset factor ``add`` are applied.
//|
//|     The ``weights`` must be a sequence of integers. The length of the tuple
//|     must be the square of an odd number, usually 9 and sometimes 25.
//|     Specific weights create different effects. For instance, these
//|     weights represent a 3x3 gaussian blur: ``[1, 2, 1, 2, 4, 2, 1, 2, 1]``
//|
//|     ``mul`` is number to multiply the convolution pixel results by.
//|     If `None` (the default) is passed, the value of ``1/sum(weights)``
//|     is used (or ``1`` if ``sum(weights)`` is ``0``). For most weights, his
//|     default value will preserve the overall image brightness.
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


static mp_float_t get_m(mp_obj_t mul_obj, int sum) {
    return mul_obj != mp_const_none ? mp_obj_get_float(mul_obj) : sum ? 1 / (mp_float_t)sum : 1;
}

static mp_obj_t bitmapfilter_morph(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q must be of type %q, not %q"), MP_QSTR_weights, MP_QSTR_Sequence, mp_obj_get_type_qstr(weights));
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
static mp_obj_t subscr(mp_obj_t o, int i) {
    return mp_obj_subscr(o, MP_OBJ_NEW_SMALL_INT(i), MP_OBJ_SENTINEL);
}

static mp_float_t float_subscr(mp_obj_t o, int i) {
    return mp_obj_get_float(subscr(o, i));
}

//| class ChannelScale:
//|     """A weight object to use with mix() that scales each channel independently
//|
//|     This is useful for global contrast and brightness adjustment on a
//|     per-component basis. For instance, to cut red contrast in half (while keeping the minimum value
//|     as black or 0.0),
//|
//|     .. code-block:: python
//|
//|         reduce_red_contrast = bitmapfilter.ChannelScale(0.5, 1, 1)
//|     """
//|
//|     def __init__(self, r: float, g: float, b: float) -> None:
//|         """Construct a ChannelScale object
//|
//|         The ``r`` parameter gives the scale factor for the red channel of
//|         pixels, and so forth."""
//|
static const mp_obj_namedtuple_type_t bitmapfilter_channel_scale_type = {
    NAMEDTUPLE_TYPE_BASE_AND_SLOTS(MP_QSTR_ChannelScale),
    .n_fields = 3,
    .fields = {
        MP_QSTR_r,
        MP_QSTR_g,
        MP_QSTR_b,
    },
};
//| class ChannelScaleOffset:
//|     """A weight object to use with mix() that scales and offsets each channel independently
//|
//|     The ``r``, ``g``, and ``b`` parameters give a scale factor for each color
//|     component, while the ``r_add`, ``g_add`` and ``b_add`` give offset values
//|     added to each component.
//|
//|     This is useful for global contrast and brightness adjustment on a
//|     per-component basis. For instance, to cut red contrast in half while adjusting the
//|     brightness so that the middle value is still 0.5:
//|
//|     .. code-block:: python
//|
//|         reduce_red_contrast = bitmapfilter.ChannelScaleOffset(
//|                 0.5, 0.25,
//|                 1, 0,
//|                 1, 0)
//|     """
//|
//|     def __init__(
//|         self, r: float, r_add: float, g: float, g_add: float, b: float, b_add: float
//|     ) -> None:
//|         """Construct a ChannelScaleOffset object"""
//|
static const mp_obj_namedtuple_type_t bitmapfilter_channel_scale_offset_type = {
    NAMEDTUPLE_TYPE_BASE_AND_SLOTS(MP_QSTR_ChannelScaleOffset),
    .n_fields = 6,
    .fields = {
        MP_QSTR_r,
        MP_QSTR_g,
        MP_QSTR_b,
        MP_QSTR_r_add,
        MP_QSTR_g_add,
        MP_QSTR_b_add,
    },
};

//| class ChannelMixer:
//|     """A weight object to use with mix() that mixes different channels together
//|
//|     The parameters with names like ``rb`` give the fraction of
//|     each channel to mix into every other channel. For instance,
//|     ``rb`` gives the fraction of blue to mix into red, and ``gg``
//|     gives the fraction of green to mix into green.
//|
//|     Conversion to sepia is an example where a ChannelMixer is appropriate,
//|     because the sepia conversion is defined as mixing a certain fraction of R,
//|     G, and B input values into each output value:
//|
//|     .. code-block:: python
//|
//|         sepia_weights = bitmapfilter.ChannelMixer(
//|             .393,  .769,   .189,
//|             .349,  .686,   .168,
//|             .272,  .534,   .131)
//|
//|         def sepia(bitmap):
//|             \"""Convert the bitmap to sepia\"""
//|             bitmapfilter.mix(bitmap, sepia_weights)
//|         mix_into_red = ChannelMixer(
//|                 0.5, 0.25, 0.25,
//|                 0,   1,    0,
//|                 0,   1,    0)
//|     """
//|
//|     def __init__(
//|         self,
//|         rr: float,
//|         rg: float,
//|         rb: float,
//|         gr: float,
//|         gg: float,
//|         gb: float,
//|         br: float,
//|         bg: float,
//|         bb: float,
//|     ) -> None:
//|         """Construct a ChannelMixer object"""
//|
static const mp_obj_namedtuple_type_t bitmapfilter_channel_mixer_type = {
    NAMEDTUPLE_TYPE_BASE_AND_SLOTS(MP_QSTR_ChannelMixer),
    .n_fields = 9,
    .fields = {
        MP_QSTR_rr,
        MP_QSTR_rg,
        MP_QSTR_rb,
        MP_QSTR_gr,
        MP_QSTR_gg,
        MP_QSTR_gb,
        MP_QSTR_br,
        MP_QSTR_bg,
        MP_QSTR_bb,
    },
};

//| class ChannelMixerOffset:
//|     """A weight object to use with mix() that mixes different channels together, plus an offset value
//|
//|     The parameters with names like ``rb`` give the fraction of
//|     each channel to mix into every other channel. For instance,
//|     ``rb`` gives the fraction of blue to mix into red, and ``gg``
//|     gives the fraction of green to mix into green.  The ``r_add``, ``g_add``
//|     and ``b_add`` parameters give offsets applied to each component.
//|
//|     For instance, to perform sepia conversion but also increase the overall brightness by 10%:
//|
//|     .. code-block:: python
//|
//|         sepia_weights_brighten = bitmapfilter.ChannelMixerOffset(
//|             .393,  .769,   .189, .1
//|             .349,  .686,   .168, .1
//|             .272,  .534,   .131, .1)
//|     """
//|
//|     def __init__(
//|         self,
//|         rr: float,
//|         rg: float,
//|         rb: float,
//|         r_add: float,
//|         gr: float,
//|         gg: float,
//|         gb: float,
//|         g_add: float,
//|         br: float,
//|         bg: float,
//|         bb: float,
//|         b_add: float,
//|     ) -> None:
//|         """Construct a ChannelMixerOffset object"""
//|
static const mp_obj_namedtuple_type_t bitmapfilter_channel_mixer_offset_type = {
    NAMEDTUPLE_TYPE_BASE_AND_SLOTS(MP_QSTR_ChannelMixerOffset),
    .n_fields = 12,
    .fields = {
        MP_QSTR_rr,
        MP_QSTR_rg,
        MP_QSTR_rb,
        MP_QSTR_r_add,
        MP_QSTR_gr,
        MP_QSTR_gg,
        MP_QSTR_gb,
        MP_QSTR_g_add,
        MP_QSTR_br,
        MP_QSTR_bg,
        MP_QSTR_bb,
        MP_QSTR_b_add,
    },
};

//| def mix(
//|     bitmap: displayio.Bitmap,
//|     weights: ChannelScale | ChannelScaleOffset | ChannelMixer | ChannelMixerOffset,
//|     mask: displayio.Bitmap | None = None,
//| ) -> displayio.Bitmap:
//|     """Perform a channel mixing operation on the bitmap
//|
//|     This is similar to the "channel mixer" tool in popular photo editing software.
//|     Imagemagick calls this "-color-matrix". In PIL, this is accomplished with the
//|     ``convert`` method's ``matrix`` argument.
//|
//|     The ``bitmap``, which must be in RGB565_SWAPPED format, is modified
//|     according to the ``weights``.
//|
//|     The ``weights`` must be one of the above types: `ChannelScale`,
//|     `ChannelScaleOffset`, `ChannelMixer`, or `ChannelMixerOffset`. For the
//|     effect of each different kind of weights object, see the type
//|     documentation.
//|
//|     After computation, any out of range values are clamped to the greatest or
//|     smallest valid value.
//|
//|     ``mask`` is another image to use as a pixel level mask for the operation.
//|     The mask should be an image the same size as the image being operated on.
//|     Only pixels set to a non-zero value in the mask are modified.
//|     """
//|
static mp_obj_t bitmapfilter_mix(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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
    if (mp_obj_is_type(weights_obj, (const mp_obj_type_t *)&bitmapfilter_channel_scale_type)) {
        for (int i = 0; i < 3; i++) {
            weights[5 * i] = float_subscr(weights_obj, i);
        }
    } else if (mp_obj_is_type(weights_obj, (const mp_obj_type_t *)&bitmapfilter_channel_scale_offset_type)) {
        for (int i = 0; i < 3; i++) {
            weights[5 * i] = float_subscr(weights_obj, i * 2);
            weights[4 * i + 3] = float_subscr(weights_obj, i * 2 + 1);
        }
    } else if (mp_obj_is_type(weights_obj, (const mp_obj_type_t *)&bitmapfilter_channel_mixer_type)) {
        for (int i = 0; i < 9; i++) {
            weights[i + i / 3] = float_subscr(weights_obj, i);
        }
    } else if (mp_obj_is_type(weights_obj, (const mp_obj_type_t *)&bitmapfilter_channel_mixer_offset_type)) {
        for (int i = 0; i < 12; i++) {
            weights[i] = float_subscr(weights_obj, i);
        }
    } else {
        mp_raise_ValueError_varg(
            MP_ERROR_TEXT("weights must be an object of type %q, %q, %q, or %q, not %q "),
            MP_QSTR_ScaleMixer, MP_QSTR_ScaleMixerOffset,
            MP_QSTR_ChannelMixer, MP_QSTR_ChannelMixerOffset,
            mp_obj_get_type_qstr(weights_obj)
            );
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

//| def solarize(bitmap, threshold: float = 0.5, mask: displayio.Bitmap | None = None):
//|     """Create a "solarization" effect on an image
//|
//|     This filter inverts pixels with brightness values above ``threshold``, while leaving
//|     lower brightness pixels alone.
//|
//|     This effect is similar to `an effect observed in real life film
//|     <https://en.wikipedia.org/wiki/Solarization_(photography)>`_ which can also be
//|     `produced during the printmaking process
//|     <https://en.wikipedia.org/wiki/Sabattier_effect>`_
//|
//|     PIL and ImageMagic both call this "solarize".
//|     """
//|
static mp_obj_t bitmapfilter_solarize(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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
//| """Any function which takes a number and returns a number. The input
//| and output values should be in the range from 0 to 1 inclusive."""
//| ThreeLookupFunctions = Tuple[LookupFunction, LookupFunction, LookupFunction]
//| """Any sequenceof three `LookupFunction` objects"""
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
//|     This is similar to, but more limiting than, PIL's "LUT3D" facility. It is not
//|     directly available in OpenMV or ImageMagic.
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

static int scaled_lut(int maxval, mp_obj_t func, int i) {
    mp_obj_t obj = mp_call_function_1(func, mp_obj_new_float(i / (mp_float_t)maxval));
    mp_float_t val = mp_obj_get_float(obj);
    return (int)MICROPY_FLOAT_C_FUN(round)(val * maxval);
}

static mp_obj_t bitmapfilter_lookup(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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
//|     In OpenMV this is accomplished via the ``ironbow`` function, which uses a default
//|     palette known as "ironbow". Imagemagic produces a similar effect with ``-clut``.
//|     PIL can accomplish this by converting an image to "L" format, then applying a
//|     palette to convert it into "P" mode.
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
static mp_obj_t bitmapfilter_false_color(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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

#define BLEND_TABLE_SIZE (4096)
static uint8_t *get_blend_table(mp_obj_t lookup, int mode) {
    mp_buffer_info_t lookup_buf;
    if (!mp_get_buffer(lookup, &lookup_buf, mode) || lookup_buf.len != BLEND_TABLE_SIZE) {
        return NULL;
    }
    return lookup_buf.buf;
}
//|
//| BlendFunction = Callable[[float, float], float]
//| """A function used to blend two images"""
//|
//| BlendTable = bytearray
//| """A precomputed blend table
//|
//| There is not actually a BlendTable type. The real type is actually any
//| buffer 4096 bytes in length."""
//|
//| def blend_precompute(lookup: BlendFunction, table: BlendTable | None = None) -> BlendTable:
//|     """Precompute a BlendTable from a BlendFunction
//|
//|     If the optional ``table`` argument is provided, an existing `BlendTable` is updated
//|     with the new function values.
//|
//|     The function's two arguments will range from 0 to 1. The returned value should also range from 0 to 1.
//|
//|     A function to do a 33% blend of each source image could look like this:
//|
//|     .. code-block:: python
//|
//|         def blend_one_third(a, b):
//|             return a * .33 + b * .67
//|     """
//|
static mp_obj_t blend_precompute(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_lookup, ARG_table };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_lookup, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        { MP_QSTR_table, MP_ARG_OBJ, { .u_obj = MP_ROM_NONE } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t table = args[ARG_table].u_obj;
    if (table == mp_const_none) {
        table = mp_obj_new_bytearray_of_zeros(BLEND_TABLE_SIZE);
    }
    uint8_t *buf = get_blend_table(table, MP_BUFFER_WRITE);
    if (!buf) {
        mp_raise_TypeError_varg(MP_ERROR_TEXT("%q must be of type %q or %q, not %q"),
            MP_QSTR_table, MP_QSTR_NoneType, MP_QSTR_WritableBuffer,
            mp_obj_get_type_qstr(table));
    }
    shared_module_bitmapfilter_blend_precompute(args[ARG_lookup].u_obj, buf);
    return table;
}
MP_DEFINE_CONST_FUN_OBJ_KW(bitmapfilter_blend_precompute_obj, 0, blend_precompute);

//|
//| def blend(
//|     dest: displayio.Bitmap,
//|     src1: displayio.Bitmap,
//|     src2: displayio.Bitmap,
//|     lookup: BlendFunction | BlendTable,
//|     mask: displayio.Bitmap | None = None,
//| ) -> displayio.Bitmap:
//|     """Blend the 'src1' and 'src2' images according to lookup function or table 'lookup'
//|
//|     If ``lookup`` is a function, it is converted to a `BlendTable` by
//|     internally calling blend_precompute. If a blend function is used repeatedly
//|     it can be more efficient to compute it once with `blend_precompute`.
//|
//|     If the mask is supplied, pixels from ``src1`` are taken unchanged in masked areas.
//|
//|     The source and destination bitmaps may be the same bitmap.
//|
//|     The destination bitmap is returned.
//|     """
//|

static mp_obj_t bitmapfilter_blend(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_dest, ARG_src1, ARG_src2, ARG_lookup, ARG_mask };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_dest, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        { MP_QSTR_src1, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        { MP_QSTR_src2, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        { MP_QSTR_lookup, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        { MP_QSTR_mask, MP_ARG_OBJ, { .u_obj = MP_ROM_NONE } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_arg_validate_type(args[ARG_dest].u_obj, &displayio_bitmap_type, MP_QSTR_dest);
    displayio_bitmap_t *dest = MP_OBJ_TO_PTR(args[ARG_dest].u_obj);

    mp_arg_validate_type(args[ARG_src1].u_obj, &displayio_bitmap_type, MP_QSTR_src1);
    displayio_bitmap_t *src1 = MP_OBJ_TO_PTR(args[ARG_src1].u_obj);

    mp_arg_validate_type(args[ARG_src2].u_obj, &displayio_bitmap_type, MP_QSTR_src2);
    displayio_bitmap_t *src2 = MP_OBJ_TO_PTR(args[ARG_src2].u_obj);

    mp_obj_t lookup = args[ARG_lookup].u_obj;
    if (mp_obj_is_callable(lookup)) {
        lookup = mp_call_function_1(MP_OBJ_FROM_PTR(&bitmapfilter_blend_precompute_obj), lookup);
    }
    uint8_t *lookup_buf = get_blend_table(lookup, MP_BUFFER_READ);
    if (!lookup_buf) {
        mp_raise_TypeError_varg(MP_ERROR_TEXT("%q must be of type %q or %q, not %q"),
            MP_QSTR_lookup, MP_QSTR_callable, MP_QSTR_ReadableBuffer,
            mp_obj_get_type_qstr(lookup));
    }

    displayio_bitmap_t *mask = NULL;
    if (args[ARG_mask].u_obj != mp_const_none) {
        mp_arg_validate_type(args[ARG_mask].u_obj, &displayio_bitmap_type, MP_QSTR_mask);
        mask = MP_OBJ_TO_PTR(args[ARG_mask].u_obj);
    }

    shared_module_bitmapfilter_blend(dest, src1, src2, mask, lookup_buf);
    return args[ARG_dest].u_obj;
}
MP_DEFINE_CONST_FUN_OBJ_KW(bitmapfilter_blend_obj, 0, bitmapfilter_blend);

static const mp_rom_map_elem_t bitmapfilter_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_bitmapfilter) },
    { MP_ROM_QSTR(MP_QSTR_morph), MP_ROM_PTR(&bitmapfilter_morph_obj) },
    { MP_ROM_QSTR(MP_QSTR_mix), MP_ROM_PTR(&bitmapfilter_mix_obj) },
    { MP_ROM_QSTR(MP_QSTR_solarize), MP_ROM_PTR(&bitmapfilter_solarize_obj) },
    { MP_ROM_QSTR(MP_QSTR_false_color), MP_ROM_PTR(&bitmapfilter_false_color_obj) },
    { MP_ROM_QSTR(MP_QSTR_lookup), MP_ROM_PTR(&bitmapfilter_lookup_obj) },
    { MP_ROM_QSTR(MP_QSTR_ChannelScale), MP_ROM_PTR(&bitmapfilter_channel_scale_type) },
    { MP_ROM_QSTR(MP_QSTR_ChannelScaleOffset), MP_ROM_PTR(&bitmapfilter_channel_scale_offset_type) },
    { MP_ROM_QSTR(MP_QSTR_ChannelMixer), MP_ROM_PTR(&bitmapfilter_channel_mixer_type) },
    { MP_ROM_QSTR(MP_QSTR_ChannelMixerOffset), MP_ROM_PTR(&bitmapfilter_channel_mixer_offset_type) },
    { MP_ROM_QSTR(MP_QSTR_blend), MP_ROM_PTR(&bitmapfilter_blend_obj) },
    { MP_ROM_QSTR(MP_QSTR_blend_precompute), MP_ROM_PTR(&bitmapfilter_blend_precompute_obj) },
};
static MP_DEFINE_CONST_DICT(bitmapfilter_module_globals, bitmapfilter_module_globals_table);

const mp_obj_module_t bitmapfilter_module = {
    .base = {&mp_type_module },
    .globals = (mp_obj_dict_t *)&bitmapfilter_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_bitmapfilter, bitmapfilter_module);
