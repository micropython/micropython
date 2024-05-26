// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Radomir Dopieralski
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t *chars;
    uint8_t *font;
    uint8_t *palette;
    int16_t x, y;
    uint8_t width, height;
} text_obj_t;

uint16_t get_text_pixel(text_obj_t *text, int16_t x, int16_t y);
