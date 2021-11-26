/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
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

#include "common-hal/videocore/Framebuffer.h"

extern const mp_obj_type_t videocore_framebuffer_type;

void common_hal_videocore_framebuffer_construct(videocore_framebuffer_obj_t *self, mp_uint_t width, mp_uint_t height);
void common_hal_videocore_framebuffer_deinit(videocore_framebuffer_obj_t *self);
bool common_hal_videocore_framebuffer_deinited(videocore_framebuffer_obj_t *self);
void common_hal_videocore_framebuffer_refresh(videocore_framebuffer_obj_t *self);
int common_hal_videocore_framebuffer_get_width(videocore_framebuffer_obj_t *self);
int common_hal_videocore_framebuffer_get_height(videocore_framebuffer_obj_t *self);
int common_hal_videocore_framebuffer_get_row_stride(videocore_framebuffer_obj_t *self);
mp_int_t common_hal_videocore_framebuffer_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags);
