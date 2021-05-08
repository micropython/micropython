/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright 2020 Sony Semiconductor Solutions Corporation
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

#include "shared-bindings/camera/ImageFormat.h"

//| class ImageFormat:
//|     """Image format"""
//|
//|     def __init__(self) -> None:
//|         """Enum-like class to define the image format."""
//|
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

STATIC const mp_rom_map_elem_t camera_imageformat_locals_dict_table[] = {
    {MP_ROM_QSTR(MP_QSTR_JPG), MP_ROM_PTR(&camera_imageformat_jpg_obj)},
    {MP_ROM_QSTR(MP_QSTR_RGB565), MP_ROM_PTR(&camera_imageformat_rgb565_obj)},
};
STATIC MP_DEFINE_CONST_DICT(camera_imageformat_locals_dict, camera_imageformat_locals_dict_table);

STATIC void camera_imageformat_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    qstr format = MP_QSTR_None;
    if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&camera_imageformat_jpg_obj)) {
        format = MP_QSTR_JPG;
    } else if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&camera_imageformat_rgb565_obj)) {
        format = MP_QSTR_RGB565;
    }
    mp_printf(print, "%q.%q.%q", MP_QSTR_camera, MP_QSTR_ImageSize, format);
}

const mp_obj_type_t camera_imageformat_type = {
    { &mp_type_type },
    .name = MP_QSTR_ImageFormat,
    .print = camera_imageformat_print,
    .locals_dict = (mp_obj_t)&camera_imageformat_locals_dict,
};
