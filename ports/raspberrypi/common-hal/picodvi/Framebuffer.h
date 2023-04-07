#pragma once

/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Scott Shawcroft for Adafruit Industries
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

#include "supervisor/memory.h"

#include "lib/PicoDVI/software/libdvi/dvi.h"

typedef struct {
    mp_obj_base_t base;
    supervisor_allocation *allocation;
    uint32_t *framebuffer;
    size_t framebuffer_len; // in words
    size_t tmdsbuf_size; // in words
    struct dvi_inst dvi;
    mp_uint_t width;
    mp_uint_t height;
    uint tmds_lock;
    uint colour_lock;
    uint16_t next_scanline;
    uint16_t pitch; // Number of words between rows. (May be more than a width's worth.)
    uint8_t color_depth;
    uint8_t pwm_slice;
    int8_t pin_pair[4];
} picodvi_framebuffer_obj_t;
