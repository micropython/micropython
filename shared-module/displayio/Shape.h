// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"
#include "shared-module/displayio/area.h"

typedef struct {
    mp_obj_base_t base;
    uint16_t width;
    uint16_t height;
    uint16_t half_width;
    uint16_t half_height;
    uint16_t *data;
    bool mirror_x;
    bool mirror_y;
    displayio_area_t dirty_area;
} displayio_shape_t;

void displayio_shape_finish_refresh(displayio_shape_t *self);
displayio_area_t *displayio_shape_get_refresh_areas(displayio_shape_t *self, displayio_area_t *tail);
