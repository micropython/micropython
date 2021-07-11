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


#include "py/obj.h"
#include "py/objarray.h"

#ifndef PIXELBUF_SHARED_MODULE_H
#define PIXELBUF_SHARED_MODULE_H

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t w;
} pixelbuf_rgbw_t;

typedef struct {
    uint8_t bpp;
    pixelbuf_rgbw_t byteorder;
    bool has_white;
    bool is_dotstar;
    mp_obj_t order_string;
} pixelbuf_byteorder_details_t;

typedef struct {
    mp_obj_base_t base;
    size_t pixel_count;
    uint16_t bytes_per_pixel;
    uint16_t scaled_brightness;
    pixelbuf_byteorder_details_t byteorder;
    mp_float_t brightness;
    mp_obj_t transmit_buffer_obj;
    // The post_brightness_buffer is offset into the buffer allocated in transmit_buffer_obj to
    // account for any header.
    uint8_t *post_brightness_buffer;
    uint8_t *pre_brightness_buffer;
    bool auto_write;
} pixelbuf_pixelbuf_obj_t;

#define PIXEL_R 0
#define PIXEL_G 1
#define PIXEL_B 2
#define PIXEL_W 3

#define DOTSTAR_LED_START 0b11100000
#define DOTSTAR_LED_START_FULL_BRIGHT 0xFF

#endif
