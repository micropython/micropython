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

#include <stdio.h>

#include <string.h>
#include "py/obj.h"
#include "framebuffer.h"

#if MICROPY_PY_DISPLAY_FRAMEBUFFER

void framebuffer_init(framebuffer_t * p_fb, framebuffer_init_t * p_init_conf) {
    uint16_t width = p_init_conf->width;
    uint16_t height = p_init_conf->height;

    if (p_init_conf->line_orientation == FRAMEBUFFER_LINE_DIR_HORIZONTAL) {
        uint16_t dirty_row_stride = height / 8;
        p_fb->fb_dirty = m_new(framebuffer_byte_t, dirty_row_stride);
        p_fb->fb_dirty_stride = dirty_row_stride;
        p_fb->fb_stride = width / 8;
    } else { // FRAMEBUFFER_LINE_DIR_VERTICAL
        uint16_t dirty_column_stride = width / 8;
        p_fb->fb_dirty = m_new(framebuffer_byte_t, dirty_column_stride);
        p_fb->fb_dirty_stride = dirty_column_stride;
        p_fb->fb_stride = height / 8;
    }

    p_fb->fb_new = m_new(framebuffer_byte_t, (width * height / 8));

    if (p_init_conf->double_buffer) {
        p_fb->fb_old = m_new(framebuffer_byte_t, (width * height / 8));
    }
    p_fb->line_orientation = p_init_conf->line_orientation;
    p_fb->screen_width = width;
    p_fb->screen_height = height;
}


void framebuffer_flip(framebuffer_t * p_fb) {
    if (p_fb->fb_double) {
        framebuffer_byte_t * old = p_fb->fb_old;
        p_fb->fb_old = p_fb->fb_new;
        p_fb->fb_new = old;
    }
}

void framebuffer_pixel_set(framebuffer_t * p_fb, uint16_t x, uint16_t y) {
    if (p_fb->line_orientation == FRAMEBUFFER_LINE_DIR_HORIZONTAL) {
        uint16_t col     = (x / 8);
        uint16_t row     = y;
        uint8_t  bit_pos = x % 8;

        p_fb->fb_new[row * (p_fb->fb_stride) + col].byte |= (1 << bit_pos);
        p_fb->fb_dirty[y / 8].byte |= (uint8_t)(0x1 << y % 8);
    } else {
        uint16_t col     = x;
        uint16_t row     = (y / 8);
        uint8_t  bit_pos = y % 8;

        p_fb->fb_new[col * (p_fb->fb_stride) + row].byte |= (1 << bit_pos);
        p_fb->fb_dirty[x / 8].byte |= (uint8_t)(0x1 << x % 8);
    }
}

void framebuffer_pixel_clear(framebuffer_t * p_fb, uint16_t x, uint16_t y) {
    if (p_fb->line_orientation == FRAMEBUFFER_LINE_DIR_HORIZONTAL) {
        uint16_t col     = (x / 8);
        uint16_t row     = y;
        uint8_t  bit_pos = x % 8;

        p_fb->fb_new[row * (p_fb->fb_stride) + col].byte &= ~(1 << bit_pos);
        p_fb->fb_dirty[y / 8].byte |= (uint8_t)(0x1 << y % 8);
    } else {
        uint16_t col  = x;
        uint16_t row    = (y / 8);
        uint8_t  bit_pos = y % 8;

        p_fb->fb_new[col * (p_fb->fb_stride) + row].byte &= ~(1 << bit_pos);
        p_fb->fb_dirty[x / 8].byte |= (uint8_t)(0x1 << x % 8);
    }
}

void framebuffer_clear(framebuffer_t * p_fb) {
    memset(p_fb->fb_new, 0x00, p_fb->screen_width * p_fb->screen_height / 8);
    memset(p_fb->fb_dirty, 0xFF, p_fb->fb_dirty_stride);
}

void framebuffer_fill(framebuffer_t * p_fb) {
    memset(p_fb->fb_new, 0xFF, p_fb->screen_width * p_fb->screen_height / 8);
    memset(p_fb->fb_dirty, 0xFF, p_fb->fb_dirty_stride);
}

#endif // MICROPY_PY_DISPLAY_FRAMEBUFFER
