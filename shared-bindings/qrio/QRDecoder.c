// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Jeff Epler
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/qrio/__init__.h"
#include "shared-bindings/qrio/QRDecoder.h"
#include "shared-module/qrio/QRDecoder.h"

#include "py/obj.h"
#include "py/objproperty.h"
#include "py/enum.h"

//| class QRDecoder:
//|     def __init__(self, width: int, height: int) -> None:
//|         """Construct a QRDecoder object
//|
//|         :param int width: The pixel width of the image to decode
//|         :param int height: The pixel height of the image to decode
//|         """
//|         ...

static mp_obj_t qrio_qrdecoder_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args_in) {
    enum { ARG_width, ARG_height };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_width, MP_ARG_INT | MP_ARG_REQUIRED, {.u_int = 0} },
        { MP_QSTR_height, MP_ARG_INT | MP_ARG_REQUIRED, {.u_int = 0} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, args_in, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    qrio_qrdecoder_obj_t *self = mp_obj_malloc(qrio_qrdecoder_obj_t, &qrio_qrdecoder_type_obj);
    shared_module_qrio_qrdecoder_construct(self, args[ARG_width].u_int, args[ARG_height].u_int);

    return self;
}


static void verify_buffer_size(qrio_qrdecoder_obj_t *self, mp_obj_t *buffer, size_t len, qrio_pixel_policy_t policy) {
    int width = shared_module_qrio_qrdecoder_get_width(self);
    int height = shared_module_qrio_qrdecoder_get_height(self);

    // verify that the buffer is big enough
    int sz = width * height;
    if (policy != QRIO_EVERY_BYTE) {
        sz *= 2;
    }
    mp_get_index(mp_obj_get_type(*buffer), len, MP_OBJ_NEW_SMALL_INT(sz - 1), false);
}

//|     def decode(
//|         self, buffer: ReadableBuffer, pixel_policy: PixelPolicy = PixelPolicy.EVERY_BYTE
//|     ) -> List[QRInfo]:
//|         """Decode zero or more QR codes from the given image.  The size of the buffer must be at least ``length``×``width`` bytes for `EVERY_BYTE`, and 2×``length``×``width`` bytes for `EVEN_BYTES` or `ODD_BYTES`."""
static mp_obj_t qrio_qrdecoder_decode(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    qrio_qrdecoder_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    enum { ARG_buffer, ARG_pixel_policy };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_int = 0} },
        { MP_QSTR_pixel_policy, MP_ARG_OBJ, {.u_obj = MP_ROM_PTR((mp_obj_t *)&qrio_pixel_policy_EVERY_BYTE_obj)} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_READ);
    qrio_pixel_policy_t policy = cp_enum_value(&qrio_pixel_policy_type, args[ARG_pixel_policy].u_obj, MP_QSTR_pixel_policy);
    verify_buffer_size(self, &args[ARG_buffer].u_obj, bufinfo.len, policy);

    return shared_module_qrio_qrdecoder_decode(self, &bufinfo, policy);
}
MP_DEFINE_CONST_FUN_OBJ_KW(qrio_qrdecoder_decode_obj, 1, qrio_qrdecoder_decode);


//|     def find(
//|         self, buffer: ReadableBuffer, pixel_policy: PixelPolicy = PixelPolicy.EVERY_BYTE
//|     ) -> List[QRPosition]:
//|         """Find all visible QR codes from the given image.  The size of the buffer must be at least ``length``×``width`` bytes for `EVERY_BYTE`, and 2×``length``×``width`` bytes for `EVEN_BYTES` or `ODD_BYTES`."""
static mp_obj_t qrio_qrdecoder_find(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    qrio_qrdecoder_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    enum { ARG_buffer, ARG_pixel_policy };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_int = 0} },
        { MP_QSTR_pixel_policy, MP_ARG_OBJ, {.u_obj = MP_ROM_PTR((mp_obj_t *)&qrio_pixel_policy_EVERY_BYTE_obj)} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_READ);
    qrio_pixel_policy_t policy = cp_enum_value(&qrio_pixel_policy_type, args[ARG_pixel_policy].u_obj, MP_QSTR_pixel_policy);
    verify_buffer_size(self, &args[ARG_buffer].u_obj, bufinfo.len, policy);

    return shared_module_qrio_qrdecoder_find(self, &bufinfo, policy);
}
MP_DEFINE_CONST_FUN_OBJ_KW(qrio_qrdecoder_find_obj, 1, qrio_qrdecoder_find);


//|     width: int
//|     """The width of image the decoder expects"""
static mp_obj_t qrio_qrdecoder_get_width(mp_obj_t self_in) {
    qrio_qrdecoder_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(shared_module_qrio_qrdecoder_get_width(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(qrio_qrdecoder_get_width_obj, qrio_qrdecoder_get_width);

static mp_obj_t qrio_qrdecoder_set_width(mp_obj_t self_in, mp_obj_t width_in) {
    qrio_qrdecoder_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int width = mp_obj_get_int(width_in);
    shared_module_qrio_qrdecoder_set_width(self, width);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(qrio_qrdecoder_set_width_obj, qrio_qrdecoder_set_width);

MP_PROPERTY_GETSET(qrio_qrdecoder_width_obj,
    (mp_obj_t)&qrio_qrdecoder_get_width_obj,
    (mp_obj_t)&qrio_qrdecoder_set_width_obj);

//|     height: int
//|     """The height of image the decoder expects"""
//|
static mp_obj_t qrio_qrdecoder_get_height(mp_obj_t self_in) {
    qrio_qrdecoder_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(shared_module_qrio_qrdecoder_get_height(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(qrio_qrdecoder_get_height_obj, qrio_qrdecoder_get_height);

static mp_obj_t qrio_qrdecoder_set_height(mp_obj_t self_in, mp_obj_t height_in) {
    qrio_qrdecoder_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int height = mp_obj_get_int(height_in);
    shared_module_qrio_qrdecoder_set_height(self, height);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(qrio_qrdecoder_set_height_obj, qrio_qrdecoder_set_height);

MP_PROPERTY_GETSET(qrio_qrdecoder_height_obj,
    (mp_obj_t)&qrio_qrdecoder_get_height_obj,
    (mp_obj_t)&qrio_qrdecoder_set_height_obj);

static const mp_rom_map_elem_t qrio_qrdecoder_locals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_QRDecoder) },
    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&qrio_qrdecoder_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_PTR(&qrio_qrdecoder_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_decode), MP_ROM_PTR(&qrio_qrdecoder_decode_obj) },
    { MP_ROM_QSTR(MP_QSTR_find), MP_ROM_PTR(&qrio_qrdecoder_find_obj) },
};

static MP_DEFINE_CONST_DICT(qrio_qrdecoder_locals, qrio_qrdecoder_locals_table);

MP_DEFINE_CONST_OBJ_TYPE(
    qrio_qrdecoder_type_obj,
    MP_QSTR_QRDecoder,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, qrio_qrdecoder_make_new,
    locals_dict, &qrio_qrdecoder_locals
    );
