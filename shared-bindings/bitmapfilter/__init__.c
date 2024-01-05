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

STATIC mp_obj_t bitmapfilter_morph(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_dest_bitmap, ARG_source_bitmap, ARG_weights, ARG_m, ARG_b };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_dest_bitmap, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        { MP_QSTR_source_bitmap, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        { MP_QSTR_weights, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        { MP_QSTR_m, MP_ARG_OBJ, { .u_obj = MP_ROM_INT(1) } },
        { MP_QSTR_b, MP_ARG_OBJ, { .u_obj = MP_ROM_INT(0) } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_arg_validate_type(args[ARG_dest_bitmap].u_obj, &displayio_bitmap_type, MP_QSTR_dest_bitmap);
    mp_arg_validate_type(args[ARG_source_bitmap].u_obj, &displayio_bitmap_type, MP_QSTR_source_bitmap);
    displayio_bitmap_t *destination = MP_OBJ_TO_PTR(args[ARG_dest_bitmap].u_obj);     // the destination bitmap
    displayio_bitmap_t *source = MP_OBJ_TO_PTR(args[ARG_source_bitmap].u_obj);     // the source bitmap

    mp_float_t m = mp_obj_get_float(args[ARG_m].u_obj);
    mp_int_t b = mp_obj_get_int(args[ARG_b].u_obj);

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
    for (size_t i = 0; i < n_weights; i++) {
        iweights[i] = mp_obj_get_int(items[i]);
    }

    shared_module_bitmapfilter_morph(source, destination, sq_n_weights / 2, iweights, (float)m, b, false, 0, false);
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
