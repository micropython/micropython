/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Mark Komus
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

#include "py/obj.h"
#include "lib/protomatter/src/core.h"
#include "shared-module/is31fl3741/IS31FL3741.h"

extern const mp_obj_type_t is31fl3741_FrameBuffer_type;
typedef struct {
    mp_obj_base_t base;
    is31fl3741_IS31FL3741_obj_t *is31fl3741;
    is31fl3741_IS31FL3741_obj_t inline_is31fl3741;
    mp_obj_t framebuffer;
    mp_buffer_info_t bufinfo;
    uint16_t bufsize, width, height, scale_width, scale_height;
    uint16_t *mapping;
    uint8_t bit_depth;
    bool paused;
    bool scale;
    bool auto_gamma;
} is31fl3741_FrameBuffer_obj_t;
