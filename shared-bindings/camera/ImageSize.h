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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_CAMERA_IMAGESIZE_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_CAMERA_IMAGESIZE_H

#include "py/obj.h"

typedef enum {
    IMAGESIZE_NONE,
    IMAGESIZE_320x240,
    IMAGESIZE_640x320,
    IMAGESIZE_1280x720,
    IMAGESIZE_1280x960,
    IMAGESIZE_1920x1080,
    IMAGESIZE_2048x1536,
    IMAGESIZE_2560x1920,
} camera_imagesize_t;

const mp_obj_type_t camera_imagesize_type;

camera_imagesize_t camera_imagesize_obj_to_type(mp_obj_t obj);
mp_obj_t camera_imagesize_type_to_obj(camera_imagesize_t mode);

typedef struct {
    mp_obj_base_t base;
} camera_imagesize_obj_t;
extern const camera_imagesize_obj_t camera_imagesize_320x240_obj;
extern const camera_imagesize_obj_t camera_imagesize_640x320_obj;
extern const camera_imagesize_obj_t camera_imagesize_1280x720_obj;
extern const camera_imagesize_obj_t camera_imagesize_1280x960_obj;
extern const camera_imagesize_obj_t camera_imagesize_1920x1080_obj;
extern const camera_imagesize_obj_t camera_imagesize_2048x1536_obj;
extern const camera_imagesize_obj_t camera_imagesize_2560x1920_obj;

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_CAMERA_IMAGESIZE_H
