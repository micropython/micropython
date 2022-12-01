/*
 * This file is part of the CircuitPython project, https://github.com/adafruit/circuitpython
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Rose Hooper
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
#include "py/obj.h"

extern const mp_obj_type_t pixelmap_pixelmap_type;

typedef struct _pixelmap_pixelmap_obj pixelmap_pixelmap_obj_t;

void shared_module_pixelmap_pixelmap_construct(pixelmap_pixelmap_obj_t *self, mp_obj_t pixelbuf, mp_obj_t indices);
bool shared_module_pixelmap_pixelmap_auto_write_get(pixelmap_pixelmap_obj_t *self);
void shared_module_pixelmap_pixelmap_auto_write_set(pixelmap_pixelmap_obj_t *self, bool auto_write);
void shared_module_pixelmap_pixelmap_fill(pixelmap_pixelmap_obj_t *self, const mp_obj_t color);
mp_obj_t shared_module_pixelmap_pixelmap_indices(pixelmap_pixelmap_obj_t *self, int index);
void shared_module_pixelmap_pixelmap_setslice(pixelmap_pixelmap_obj_t *self, const mp_obj_t value, mp_bound_slice_t slice, size_t slice_len);
mp_obj_t shared_module_pixelmap_pixelmap_getslice(pixelmap_pixelmap_obj_t *self, mp_bound_slice_t slice, size_t slice_len);
mp_obj_t shared_module_pixelmap_pixelmap_getitem(pixelmap_pixelmap_obj_t *self, mp_int_t index);
void shared_module_pixelmap_pixelmap_setitem(pixelmap_pixelmap_obj_t *self, mp_int_t index, const mp_obj_t value);
