/*
 * This file is part of the Circuit Python project, https://github.com/adafruit/circuitpython
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Roy Hooper
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
#include "../../shared-bindings/_pixelbuf/types.h"

#ifndef PIXELBUF_SHARED_MODULE_H
#define PIXELBUF_SHARED_MODULE_H

#define PIXEL_R 0
#define PIXEL_G 1
#define PIXEL_B 2
#define PIXEL_W 3

#define DOTSTAR_LED_START 0b11100000
#define DOTSTAR_BRIGHTNESS(brightness) ((32 - (uint8_t)(32 - brightness * 31)) & 0b00011111)
#define DOTSTAR_GET_BRIGHTNESS(value) ((value & 0b00011111) / 31.0)
#define DOTSTAR_LED_START_FULL_BRIGHT 0xFF

void pixelbuf_set_pixel(uint8_t *buf, uint8_t *rawbuf, float brightness, mp_obj_t *item, pixelbuf_byteorder_obj_t *byteorder, bool dotstar);
mp_obj_t *pixelbuf_get_pixel(uint8_t *buf, pixelbuf_byteorder_obj_t *byteorder, bool dotstar);
mp_obj_t *pixelbuf_get_pixel_array(uint8_t *buf, uint len, pixelbuf_byteorder_obj_t *byteorder, uint8_t step, bool dotstar);
void pixelbuf_set_pixel_int(uint8_t *buf, mp_int_t value, pixelbuf_byteorder_obj_t *byteorder);

#endif
