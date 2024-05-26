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
    uint8_t *map;
    uint8_t *graphic;
    uint8_t *palette;
    int16_t x, y;
    uint8_t width, height;
    uint8_t frame;
    uint8_t rotation;
} layer_obj_t;

uint16_t get_layer_pixel(layer_obj_t *layer, int16_t x, int16_t y);
