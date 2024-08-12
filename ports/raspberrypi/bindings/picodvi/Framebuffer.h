// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/picodvi/Framebuffer.h"

#include "shared-bindings/microcontroller/Pin.h"

extern const mp_obj_type_t picodvi_framebuffer_type;

void common_hal_picodvi_framebuffer_construct(picodvi_framebuffer_obj_t *self,
    mp_uint_t width, mp_uint_t height,
    const mcu_pin_obj_t *clk_dp, const mcu_pin_obj_t *clk_dn,
    const mcu_pin_obj_t *red_dp, const mcu_pin_obj_t *red_dn,
    const mcu_pin_obj_t *green_dp, const mcu_pin_obj_t *green_dn,
    const mcu_pin_obj_t *blue_dp, const mcu_pin_obj_t *blue_dn,
    mp_uint_t color_depth);
void common_hal_picodvi_framebuffer_deinit(picodvi_framebuffer_obj_t *self);
bool common_hal_picodvi_framebuffer_deinited(picodvi_framebuffer_obj_t *self);
void common_hal_picodvi_framebuffer_refresh(picodvi_framebuffer_obj_t *self);
int common_hal_picodvi_framebuffer_get_width(picodvi_framebuffer_obj_t *self);
int common_hal_picodvi_framebuffer_get_height(picodvi_framebuffer_obj_t *self);
int common_hal_picodvi_framebuffer_get_row_stride(picodvi_framebuffer_obj_t *self);
int common_hal_picodvi_framebuffer_get_color_depth(picodvi_framebuffer_obj_t *self);
bool common_hal_picodvi_framebuffer_get_grayscale(picodvi_framebuffer_obj_t *self);
mp_int_t common_hal_picodvi_framebuffer_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags);
