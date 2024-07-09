// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright 2020 Sony Semiconductor Solutions Corporation
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/camera/ImageFormat.h"

//| class ImageFormat:
//|     """Image format"""
//|
//|     def __init__(self) -> None:
//|         """Enum-like class to define the image format."""
//|     JPG: ImageFormat
//|     """JPG format."""
//|
//|     RGB565: ImageFormat
//|     """RGB565 format."""
//|

const camera_imageformat_obj_t camera_imageformat_jpg_obj = {
    { &camera_imageformat_type },
};

const camera_imageformat_obj_t camera_imageformat_rgb565_obj = {
    { &camera_imageformat_type },
};

camera_imageformat_t camera_imageformat_obj_to_type(mp_obj_t obj) {
    if (obj == MP_ROM_PTR(&camera_imageformat_jpg_obj)) {
        return IMAGEFORMAT_JPG;
    } else if (obj == MP_ROM_PTR(&camera_imageformat_rgb565_obj)) {
        return IMAGEFORMAT_RGB565;
    }
    return IMAGEFORMAT_NONE;
}

mp_obj_t camera_imageformat_type_to_obj(camera_imageformat_t format) {
    switch (format) {
        case IMAGEFORMAT_JPG:
            return (mp_obj_t)MP_ROM_PTR(&camera_imageformat_jpg_obj);
        case IMAGEFORMAT_RGB565:
            return (mp_obj_t)MP_ROM_PTR(&camera_imageformat_rgb565_obj);
        case IMAGEFORMAT_NONE:
        default:
            return MP_ROM_NONE;
    }
}

static const mp_rom_map_elem_t camera_imageformat_locals_dict_table[] = {
    {MP_ROM_QSTR(MP_QSTR_JPG), MP_ROM_PTR(&camera_imageformat_jpg_obj)},
    {MP_ROM_QSTR(MP_QSTR_RGB565), MP_ROM_PTR(&camera_imageformat_rgb565_obj)},
};
static MP_DEFINE_CONST_DICT(camera_imageformat_locals_dict, camera_imageformat_locals_dict_table);

static void camera_imageformat_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    qstr format = MP_QSTR_None;
    if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&camera_imageformat_jpg_obj)) {
        format = MP_QSTR_JPG;
    } else if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&camera_imageformat_rgb565_obj)) {
        format = MP_QSTR_RGB565;
    }
    mp_printf(print, "%q.%q.%q", MP_QSTR_camera, MP_QSTR_ImageSize, format);
}

MP_DEFINE_CONST_OBJ_TYPE(
    camera_imageformat_type,
    MP_QSTR_ImageFormat,
    MP_TYPE_FLAG_NONE,
    print, camera_imageformat_print,
    locals_dict, &camera_imageformat_locals_dict
    );
