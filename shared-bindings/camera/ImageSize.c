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

#include "shared-bindings/camera/ImageSize.h"

//| class ImageSize:
//|     """Image size"""
//|
//|     def __init__(self) -> None:
//|         """Enum-like class to define the image size."""
//|
//|     IMAGE_SIZE_320x240: ImageSize
//|     """Image size 320x240."""
//|
//|     IMAGE_SIZE_640x480: ImageSize
//|     """Image size 640x480."""
//|
//|     IMAGE_SIZE_1280x720: ImageSize
//|     """Image size 1280x720."""
//|
//|     IMAGE_SIZE_1280x960: ImageSize
//|     """Image size 1280x960."""
//|
//|     IMAGE_SIZE_1920x1080: ImageSize
//|     """Image size 1920x1080."""
//|
//|     IMAGE_SIZE_2048x1536: ImageSize
//|     """Image size 2048x1536."""
//|
//|     IMAGE_SIZE_2560x1920: ImageSize
//|     """Image size 2560x1920."""
//|
const mp_obj_type_t camera_imagesize_type;

const camera_imagesize_obj_t camera_imagesize_320x240_obj = {
    { &camera_imagesize_type },
};

const camera_imagesize_obj_t camera_imagesize_640x320_obj = {
    { &camera_imagesize_type },
};

const camera_imagesize_obj_t camera_imagesize_1280x720_obj = {
    { &camera_imagesize_type },
};

const camera_imagesize_obj_t camera_imagesize_1280x960_obj = {
    { &camera_imagesize_type },
};

const camera_imagesize_obj_t camera_imagesize_1920x1080_obj = {
    { &camera_imagesize_type },
};

const camera_imagesize_obj_t camera_imagesize_2048x1536_obj = {
    { &camera_imagesize_type },
};

const camera_imagesize_obj_t camera_imagesize_2560x1920_obj = {
    { &camera_imagesize_type },
};

camera_imagesize_t camera_imagesize_obj_to_type(mp_obj_t obj) {
    if (obj == MP_ROM_PTR(&camera_imagesize_320x240_obj)) {
        return IMAGESIZE_320x240;
    } else if (obj == MP_ROM_PTR(&camera_imagesize_640x320_obj)) {
        return IMAGESIZE_640x320;
    } else if (obj == MP_ROM_PTR(&camera_imagesize_1280x720_obj)) {
        return IMAGESIZE_1280x720;
    } else if (obj == MP_ROM_PTR(&camera_imagesize_1280x960_obj)) {
        return IMAGESIZE_1280x960;
    } else if (obj == MP_ROM_PTR(&camera_imagesize_1920x1080_obj)) {
        return IMAGESIZE_1920x1080;
    } else if (obj == MP_ROM_PTR(&camera_imagesize_2048x1536_obj)) {
        return IMAGESIZE_2048x1536;
    } else if (obj == MP_ROM_PTR(&camera_imagesize_2560x1920_obj)) {
        return IMAGESIZE_2560x1920;
    }
    return IMAGESIZE_NONE;
}

mp_obj_t camera_imagesize_type_to_obj(camera_imagesize_t size) {
    switch (size) {
        case IMAGESIZE_320x240:
            return (mp_obj_t)MP_ROM_PTR(&camera_imagesize_320x240_obj);
        case IMAGESIZE_640x320:
            return (mp_obj_t)MP_ROM_PTR(&camera_imagesize_640x320_obj);
        case IMAGESIZE_1280x720:
            return (mp_obj_t)MP_ROM_PTR(&camera_imagesize_1280x720_obj);
        case IMAGESIZE_1280x960:
            return (mp_obj_t)MP_ROM_PTR(&camera_imagesize_1280x960_obj);
        case IMAGESIZE_1920x1080:
            return (mp_obj_t)MP_ROM_PTR(&camera_imagesize_1920x1080_obj);
        case IMAGESIZE_2048x1536:
            return (mp_obj_t)MP_ROM_PTR(&camera_imagesize_2048x1536_obj);
        case IMAGESIZE_2560x1920:
            return (mp_obj_t)MP_ROM_PTR(&camera_imagesize_2560x1920_obj);
        case IMAGESIZE_NONE:
        default:
            return (mp_obj_t)MP_ROM_PTR(&mp_const_none_obj);
    }
}

STATIC const mp_rom_map_elem_t camera_imagesize_locals_dict_table[] = {
    {MP_ROM_QSTR(MP_QSTR_IMAGE_SIZE_320x240), MP_ROM_PTR(&camera_imagesize_320x240_obj)},
    {MP_ROM_QSTR(MP_QSTR_IMAGE_SIZE_640x320), MP_ROM_PTR(&camera_imagesize_640x320_obj)},
    {MP_ROM_QSTR(MP_QSTR_IMAGE_SIZE_1280x720), MP_ROM_PTR(&camera_imagesize_1280x720_obj)},
    {MP_ROM_QSTR(MP_QSTR_IMAGE_SIZE_1280x960), MP_ROM_PTR(&camera_imagesize_1280x960_obj)},
    {MP_ROM_QSTR(MP_QSTR_IMAGE_SIZE_1920x1080), MP_ROM_PTR(&camera_imagesize_1920x1080_obj)},
    {MP_ROM_QSTR(MP_QSTR_IMAGE_SIZE_2048x1536), MP_ROM_PTR(&camera_imagesize_2048x1536_obj)},
    {MP_ROM_QSTR(MP_QSTR_IMAGE_SIZE_2560x1920), MP_ROM_PTR(&camera_imagesize_2560x1920_obj)},
};
STATIC MP_DEFINE_CONST_DICT(camera_imagesize_locals_dict, camera_imagesize_locals_dict_table);

STATIC void camera_imagesize_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    qstr size = MP_QSTR_None;
    if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&camera_imagesize_320x240_obj)) {
        size = MP_QSTR_IMAGE_SIZE_320x240;
    } else if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&camera_imagesize_640x320_obj)) {
        size = MP_QSTR_IMAGE_SIZE_640x320;
    } else if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&camera_imagesize_1280x720_obj)) {
        size = MP_QSTR_IMAGE_SIZE_1280x720;
    } else if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&camera_imagesize_1280x960_obj)) {
        size = MP_QSTR_IMAGE_SIZE_1280x960;
    } else if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&camera_imagesize_1920x1080_obj)) {
        size = MP_QSTR_IMAGE_SIZE_1920x1080;
    } else if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&camera_imagesize_2048x1536_obj)) {
        size = MP_QSTR_IMAGE_SIZE_2048x1536;
    } else if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&camera_imagesize_2560x1920_obj)) {
        size = MP_QSTR_IMAGE_SIZE_2560x1920;
    }
    mp_printf(print, "%q.%q.%q", MP_QSTR_camera, MP_QSTR_ImageSize, size);
}

const mp_obj_type_t camera_imagesize_type = {
    { &mp_type_type },
    .name = MP_QSTR_ImageSize,
    .print = camera_imagesize_print,
    .locals_dict = (mp_obj_t)&camera_imagesize_locals_dict,
};
