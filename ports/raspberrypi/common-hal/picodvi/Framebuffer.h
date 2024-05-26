// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "lib/PicoDVI/software/libdvi/dvi.h"

typedef struct {
    mp_obj_base_t base;
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
