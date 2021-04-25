/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
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

extern const mp_obj_type_t rgbmatrix_RGBMatrix_type;
typedef struct {
    mp_obj_base_t base;
    mp_obj_t framebuffer;
    mp_buffer_info_t bufinfo;
    Protomatter_core protomatter;
    void *timer;
    uint16_t bufsize, width;
    uint8_t rgb_pins[30];
    uint8_t addr_pins[10];
    uint8_t clock_pin, latch_pin, oe_pin;
    uint8_t rgb_count, addr_count;
    uint8_t bit_depth;
    bool core_is_initialized;
    bool paused;
    bool doublebuffer;
    bool serpentine;
    int8_t tile;
} rgbmatrix_rgbmatrix_obj_t;
