// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/rgbmatrix/RGBMatrix.h"
#include "lib/protomatter/src/core.h"

extern const mp_obj_type_t rgbmatrix_RGBMatrix_type;

void common_hal_rgbmatrix_rgbmatrix_construct(rgbmatrix_rgbmatrix_obj_t *self, int width, int bit_depth, uint8_t rgb_count, uint8_t *rgb_pins, uint8_t addr_count, uint8_t *addr_pins, uint8_t clock_pin, uint8_t latch_pin, uint8_t oe_pin, bool doublebuffer, mp_obj_t framebuffer, int8_t tile, bool serpentine, void *timer);
void common_hal_rgbmatrix_rgbmatrix_deinit(rgbmatrix_rgbmatrix_obj_t *);
void rgbmatrix_rgbmatrix_collect_ptrs(rgbmatrix_rgbmatrix_obj_t *);
void common_hal_rgbmatrix_rgbmatrix_get_bufinfo(rgbmatrix_rgbmatrix_obj_t *self, mp_buffer_info_t *bufinfo);
void common_hal_rgbmatrix_rgbmatrix_reconstruct(rgbmatrix_rgbmatrix_obj_t *self);
void common_hal_rgbmatrix_rgbmatrix_set_paused(rgbmatrix_rgbmatrix_obj_t *self, bool paused);
bool common_hal_rgbmatrix_rgbmatrix_get_paused(rgbmatrix_rgbmatrix_obj_t *self);
void common_hal_rgbmatrix_rgbmatrix_refresh(rgbmatrix_rgbmatrix_obj_t *self);
int common_hal_rgbmatrix_rgbmatrix_get_width(rgbmatrix_rgbmatrix_obj_t *self);
int common_hal_rgbmatrix_rgbmatrix_get_height(rgbmatrix_rgbmatrix_obj_t *self);
