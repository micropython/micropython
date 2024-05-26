// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Rose Hooper
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/objarray.h"

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
