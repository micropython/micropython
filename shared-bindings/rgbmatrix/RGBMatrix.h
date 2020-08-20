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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_RGBMATRIX_RGBMATRIX_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_RGBMATRIX_RGBMATRIX_H

#include "shared-module/rgbmatrix/RGBMatrix.h"
#include "lib/protomatter/core.h"

extern const mp_obj_type_t rgbmatrix_RGBMatrix_type;
typedef struct {
    mp_obj_base_t base;
    mp_obj_t framebuffer;
    mp_buffer_info_t bufinfo;
    Protomatter_core core;
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
} rgbmatrix_rgbmatrix_obj_t;

void common_hal_rgbmatrix_rgbmatrix_construct(rgbmatrix_rgbmatrix_obj_t* self, int width, int bit_depth, uint8_t rgb_count, uint8_t* rgb_pins, uint8_t addr_count, uint8_t* addr_pins, uint8_t clock_pin, uint8_t latch_pin, uint8_t oe_pin, bool doublebuffer, mp_obj_t framebuffer, void* timer);
void common_hal_rgbmatrix_rgbmatrix_deinit(rgbmatrix_rgbmatrix_obj_t*);
void rgbmatrix_rgbmatrix_collect_ptrs(rgbmatrix_rgbmatrix_obj_t*);
void common_hal_rgbmatrix_rgbmatrix_reconstruct(rgbmatrix_rgbmatrix_obj_t* self, mp_obj_t framebuffer);
void common_hal_rgbmatrix_rgbmatrix_set_paused(rgbmatrix_rgbmatrix_obj_t* self, bool paused);
bool common_hal_rgbmatrix_rgbmatrix_get_paused(rgbmatrix_rgbmatrix_obj_t* self);
void common_hal_rgbmatrix_rgbmatrix_refresh(rgbmatrix_rgbmatrix_obj_t* self);
int common_hal_rgbmatrix_rgbmatrix_get_width(rgbmatrix_rgbmatrix_obj_t* self);
int common_hal_rgbmatrix_rgbmatrix_get_height(rgbmatrix_rgbmatrix_obj_t* self);

#endif
