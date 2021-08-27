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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_CAMERA_IMAGEFORMAT_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_CAMERA_IMAGEFORMAT_H

#include "py/obj.h"

typedef enum {
    IMAGEFORMAT_NONE,
    IMAGEFORMAT_JPG,
    IMAGEFORMAT_RGB565,
} camera_imageformat_t;

extern const mp_obj_type_t camera_imageformat_type;

camera_imageformat_t camera_imageformat_obj_to_type(mp_obj_t obj);
mp_obj_t camera_imageformat_type_to_obj(camera_imageformat_t mode);

typedef struct {
    mp_obj_base_t base;
} camera_imageformat_obj_t;
extern const camera_imageformat_obj_t camera_imageformat_jpg_obj;
extern const camera_imageformat_obj_t camera_imageformat_rgb565_obj;

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_CAMERA_IMAGEFORMAT_H
