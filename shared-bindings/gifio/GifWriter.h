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

#include "py/obj.h"

#pragma once

typedef struct gifio_gifwriter gifio_gifwriter_t;
typedef enum displayio_colorspace displayio_colorspace_t;

extern const mp_obj_type_t gifio_gifwriter_type;

void shared_module_gifio_gifwriter_construct(gifio_gifwriter_t *self, mp_obj_t *file, int width, int height, displayio_colorspace_t colorspace, bool loop, bool dither, bool own_file);
void shared_module_gifio_gifwriter_check_for_deinit(gifio_gifwriter_t *self);
bool shared_module_gifio_gifwriter_deinited(gifio_gifwriter_t *self);
void shared_module_gifio_gifwriter_deinit(gifio_gifwriter_t *self);
void shared_module_gifio_gifwriter_add_frame(gifio_gifwriter_t *self, const mp_buffer_info_t *buf, int16_t delay);
void shared_module_gifio_gifwriter_close(gifio_gifwriter_t *self);
