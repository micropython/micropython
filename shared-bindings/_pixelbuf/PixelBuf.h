/*
 * This file is part of the CircuitPython project, https://github.com/adafruit/circuitpython
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Rose Hooper
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

#ifndef CP_SHARED_BINDINGS_PIXELBUF_PIXELBUF_H
#define CP_SHARED_BINDINGS_PIXELBUF_PIXELBUF_H

#include "shared-module/_pixelbuf/PixelBuf.h"

extern const mp_obj_type_t pixelbuf_pixelbuf_type;

void common_hal__pixelbuf_pixelbuf_construct(pixelbuf_pixelbuf_obj_t *self, size_t n,
    pixelbuf_byteorder_details_t *byteorder, mp_float_t brightness, bool auto_write, uint8_t *header,
    size_t header_len, uint8_t *trailer, size_t trailer_len);

// These take mp_obj_t because they are called on subclasses of PixelBuf.
uint8_t common_hal__pixelbuf_pixelbuf_get_bpp(mp_obj_t self);
mp_float_t common_hal__pixelbuf_pixelbuf_get_brightness(mp_obj_t self);
void common_hal__pixelbuf_pixelbuf_set_brightness(mp_obj_t self, mp_float_t brightness);
bool common_hal__pixelbuf_pixelbuf_get_auto_write(mp_obj_t self);
void common_hal__pixelbuf_pixelbuf_set_auto_write(mp_obj_t self, bool auto_write);
size_t common_hal__pixelbuf_pixelbuf_get_len(mp_obj_t self_in);
mp_obj_t common_hal__pixelbuf_pixelbuf_get_byteorder_string(mp_obj_t self);
void common_hal__pixelbuf_pixelbuf_fill(mp_obj_t self, mp_obj_t item);
void common_hal__pixelbuf_pixelbuf_show(mp_obj_t self);
mp_obj_t common_hal__pixelbuf_pixelbuf_get_pixel(mp_obj_t self, size_t index);
void common_hal__pixelbuf_pixelbuf_set_pixel(mp_obj_t self, size_t index, mp_obj_t item);
void common_hal__pixelbuf_pixelbuf_set_pixels(mp_obj_t self_in, size_t start, mp_int_t step, size_t slice_len, mp_obj_t *values, mp_obj_tuple_t *flatten_to);

#endif  // CP_SHARED_BINDINGS_PIXELBUF_PIXELBUF_H
