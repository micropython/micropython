/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Jeff Epler for Adafruit Industries
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
#include "lib/quirc/lib/quirc.h"
#include "shared-bindings/qrio/PixelPolicy.h"

typedef struct qrio_qrdecoder_obj {
    mp_obj_base_t base;
    struct quirc *quirc;
    struct quirc_code code;
    struct quirc_data data;
} qrdecoder_qrdecoder_obj_t;

void shared_module_qrio_qrdecoder_construct(qrdecoder_qrdecoder_obj_t *, int width, int height);
int shared_module_qrio_qrdecoder_get_height(qrdecoder_qrdecoder_obj_t *);
int shared_module_qrio_qrdecoder_get_width(qrdecoder_qrdecoder_obj_t *);
void shared_module_qrio_qrdecoder_set_height(qrdecoder_qrdecoder_obj_t *, int height);
void shared_module_qrio_qrdecoder_set_width(qrdecoder_qrdecoder_obj_t *, int width);
mp_obj_t shared_module_qrio_qrdecoder_decode(qrdecoder_qrdecoder_obj_t *, const mp_buffer_info_t *bufinfo, qrio_pixel_policy_t policy);
mp_obj_t shared_module_qrio_qrdecoder_find(qrdecoder_qrdecoder_obj_t *, const mp_buffer_info_t *bufinfo, qrio_pixel_policy_t policy);
