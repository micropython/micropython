/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Glenn Ruben Bakke
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

#ifndef DISPLAY_FRAMEBUFFER_H__
#define DISPLAY_FRAMEBUFFER_H__

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t bit0 : 1;
    uint8_t bit1 : 1;
    uint8_t bit2 : 1;
    uint8_t bit3 : 1;
    uint8_t bit4 : 1;
    uint8_t bit5 : 1;
    uint8_t bit6 : 1;
    uint8_t bit7 : 1;
} bits_le_t;

typedef struct {
    uint8_t bit7 : 1;
    uint8_t bit6 : 1;
    uint8_t bit5 : 1;
    uint8_t bit4 : 1;
    uint8_t bit3 : 1;
    uint8_t bit2 : 1;
    uint8_t bit1 : 1;
    uint8_t bit0 : 1;
} bits_be_t;

typedef struct {
    union {
        uint8_t byte;
        bits_le_t  bits_le;
        bits_be_t  bits_be;
    };
} framebuffer_byte_t;

typedef enum {
    FRAMEBUFFER_LINE_DIR_HORIZONTAL,
    FRAMEBUFFER_LINE_DIR_VERTICAL
} framebuffer_line_orientation_t;

typedef struct {
    framebuffer_byte_t * fb_new;
    framebuffer_byte_t * fb_old;
    uint16_t    fb_stride;
    bool        fb_double;
    framebuffer_byte_t * fb_dirty;
    uint16_t   fb_dirty_stride;
    uint16_t   fb_orientation;
    uint16_t   screen_height;
    uint16_t   screen_width;
    framebuffer_line_orientation_t line_orientation;
} framebuffer_t;

typedef struct {
    uint16_t                       width;
    uint16_t                       height;
    framebuffer_line_orientation_t line_orientation;
    bool                           double_buffer;
} framebuffer_init_t;

void framebuffer_init(framebuffer_t * p_fb, framebuffer_init_t * p_init_conf);
void framebuffer_deinit(framebuffer_t * p_fb);

void framebuffer_flip(framebuffer_t * p_fb);
void framebuffer_pixel_set(framebuffer_t * p_fb, uint16_t x, uint16_t y);
void framebuffer_pixel_clear(framebuffer_t * p_fb, uint16_t x, uint16_t y);
void framebuffer_clear(framebuffer_t * p_fb);
void framebuffer_fill(framebuffer_t * p_fb);

#endif // DISPLAY_FRAMEBUFFER_H__
