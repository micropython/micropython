/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
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

#ifndef LCD_MONO_FB_H__
#define LCD_MONO_FB_H__

extern const mp_obj_module_t mp_module_lcd_mono_fb;

#include <stdint.h>

#define LCD_BLACK 0
#define LCD_WHITE 1

typedef struct {
    uint8_t bit0 : 1;
    uint8_t bit1 : 1;
    uint8_t bit2 : 1;
    uint8_t bit3 : 1;
    uint8_t bit4 : 1;
    uint8_t bit5 : 1;
    uint8_t bit6 : 1;
    uint8_t bit7 : 1;
} bits_t;

typedef struct {
    union {
        uint8_t byte;
        bits_t  bits;
    };
} fb_byte_t;

typedef struct {
    mp_obj_base_t base;
    fb_byte_t * fb_bytes;
    fb_byte_t * fb_old;
    fb_byte_t * fb_dirty;
    uint16_t  height;
    uint16_t  width;
    mp_uint_t bytes_stride;
    mp_uint_t dirty_stride;
    mp_obj_t  line_update_cb;
    mp_uint_t bg_color;
    mp_uint_t fg_color;
    mp_uint_t font_size;
} mp_obj_framebuf_t;

// Functions for other drivers to use to create framebuffer instances using c.

mp_obj_t lcd_mono_fb_helper_make_new(mp_int_t width, mp_int_t height, mp_int_t direction);

#endif
