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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_CAMERA_CAMERA_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_CAMERA_CAMERA_H

#include "common-hal/camera/Camera.h"
#include "shared-bindings/camera/ImageFormat.h"

extern const mp_obj_type_t camera_type;

void common_hal_camera_construct(camera_obj_t *self);
void common_hal_camera_deinit(camera_obj_t *self);
bool common_hal_camera_deinited(camera_obj_t *self);
size_t common_hal_camera_take_picture(camera_obj_t *self, uint8_t *buffer, size_t len, uint16_t width, uint16_t height, camera_imageformat_t format);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_CAMERA_CAMERA_H
