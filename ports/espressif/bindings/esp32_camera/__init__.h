/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Jeff Epler for Adafruit Industries
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

#pragma once

#include "py/enum.h"
#include "py/obj.h"

#include "esp_camera.h"

extern const mp_obj_type_t esp32_camera_grab_mode_type;
extern const cp_enum_obj_t grab_mode_WHEN_EMPTY_obj;
extern const mp_obj_type_t esp32_camera_pixel_format_type;
extern const cp_enum_obj_t pixel_format_RGB565_obj;
extern const mp_obj_type_t esp32_camera_frame_size_type;
extern const cp_enum_obj_t frame_size_QQVGA_obj;
extern const mp_obj_type_t esp32_camera_gain_ceiling_type;

extern camera_grab_mode_t validate_grab_mode(mp_obj_t obj, qstr arg_name);
extern pixformat_t validate_pixel_format(mp_obj_t obj, qstr arg_name);
extern framesize_t validate_frame_size(mp_obj_t obj, qstr arg_name);
extern gainceiling_t validate_gain_ceiling(mp_obj_t obj, qstr arg_name);
