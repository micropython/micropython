// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Rose Hooper
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/objtuple.h"
#include "shared-module/adafruit_pixelbuf/PixelBuf.h"

extern const mp_obj_type_t pixelbuf_pixelbuf_type;

typedef union {
    struct {
        uint8_t r, g, b, w;
    };
    uint32_t rgbw;
} color_u;

void common_hal_adafruit_pixelbuf_pixelbuf_construct(pixelbuf_pixelbuf_obj_t *self, size_t n,
    pixelbuf_byteorder_details_t *byteorder, mp_float_t brightness, bool auto_write, uint8_t *header,
    size_t header_len, uint8_t *trailer, size_t trailer_len);

// These take mp_obj_t because they are called on subclasses of PixelBuf.
uint8_t common_hal_adafruit_pixelbuf_pixelbuf_get_bpp(mp_obj_t self);
mp_float_t common_hal_adafruit_pixelbuf_pixelbuf_get_brightness(mp_obj_t self);
void common_hal_adafruit_pixelbuf_pixelbuf_set_brightness(mp_obj_t self, mp_float_t brightness);
bool common_hal_adafruit_pixelbuf_pixelbuf_get_auto_write(mp_obj_t self);
void common_hal_adafruit_pixelbuf_pixelbuf_set_auto_write(mp_obj_t self, bool auto_write);
size_t common_hal_adafruit_pixelbuf_pixelbuf_get_len(mp_obj_t self_in);
mp_obj_t common_hal_adafruit_pixelbuf_pixelbuf_get_byteorder_string(mp_obj_t self);
void common_hal_adafruit_pixelbuf_pixelbuf_fill(mp_obj_t self, mp_obj_t item);
void common_hal_adafruit_pixelbuf_pixelbuf_show(mp_obj_t self);
mp_obj_t common_hal_adafruit_pixelbuf_pixelbuf_get_pixel(mp_obj_t self, size_t index);
void common_hal_adafruit_pixelbuf_pixelbuf_set_pixel(mp_obj_t self, size_t index, mp_obj_t item);
void common_hal_adafruit_pixelbuf_pixelbuf_set_pixels(mp_obj_t self_in, size_t start, mp_int_t step, size_t slice_len, mp_obj_t *values, mp_obj_tuple_t *flatten_to);
void common_hal_adafruit_pixelbuf_pixelbuf_parse_color(mp_obj_t self, mp_obj_t color, uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *w);
void common_hal_adafruit_pixelbuf_pixelbuf_set_pixel_color(mp_obj_t self, size_t index, uint8_t r, uint8_t g, uint8_t b, uint8_t w);
