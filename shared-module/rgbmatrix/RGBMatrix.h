// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

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
    uint32_t bufsize;
    uint16_t width;
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
