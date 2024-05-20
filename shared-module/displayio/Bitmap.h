// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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
    uint32_t *data;
    uint16_t stride; // uint32_t's
    uint8_t bits_per_value;
    uint8_t x_shift;
    size_t x_mask;
    displayio_area_t dirty_area;
    uint16_t bitmask;
    bool read_only;
    bool data_alloc; // did bitmap allocate data or someone else
} displayio_bitmap_t;

void displayio_bitmap_finish_refresh(displayio_bitmap_t *self);
displayio_area_t *displayio_bitmap_get_refresh_areas(displayio_bitmap_t *self, displayio_area_t *tail);
void displayio_bitmap_set_dirty_area(displayio_bitmap_t *self, const displayio_area_t *area);
void displayio_bitmap_write_pixel(displayio_bitmap_t *self, int16_t x, int16_t y, uint32_t value);
