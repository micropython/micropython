// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/mipidsi/Display.h"
#include "shared-bindings/mipidsi/Bus.h"
#include "common-hal/microcontroller/Pin.h"

extern const mp_obj_type_t mipidsi_display_type;

void common_hal_mipidsi_display_construct(mipidsi_display_obj_t *self,
    mipidsi_bus_obj_t *bus,
    const uint8_t *init_sequence,
    size_t init_sequence_len,
    mp_uint_t virtual_channel,
    mp_uint_t width,
    mp_uint_t height,
    mp_int_t rotation,
    mp_uint_t color_depth,
    const mcu_pin_obj_t *backlight_pin,
    mp_float_t brightness,
    mp_uint_t native_frames_per_second,
    bool backlight_on_high,
    mp_uint_t hsync_pulse_width,
    mp_uint_t hsync_back_porch,
    mp_uint_t hsync_front_porch,
    mp_uint_t vsync_pulse_width,
    mp_uint_t vsync_back_porch,
    mp_uint_t vsync_front_porch,
    mp_uint_t pixel_clock_frequency,
    bool low_power_blanking);
void common_hal_mipidsi_display_deinit(mipidsi_display_obj_t *self);
bool common_hal_mipidsi_display_deinited(mipidsi_display_obj_t *self);
void common_hal_mipidsi_display_refresh(mipidsi_display_obj_t *self);
// Make the rows marked in dirty_row_bitmap (one bit per row, LSB first) visible to the
// display. NULL refreshes every row. This backs the framebuffer protocol's swapbuffers
// callback (mipidsi_display_swapbuffers in Display.c), which FramebufferDisplay calls
// during refresh with the rows it just redrew.
void common_hal_mipidsi_display_refresh_rows(mipidsi_display_obj_t *self, const uint8_t *dirty_row_bitmap);
mp_float_t common_hal_mipidsi_display_get_brightness(mipidsi_display_obj_t *self);
bool common_hal_mipidsi_display_set_brightness(mipidsi_display_obj_t *self, mp_float_t brightness);
int common_hal_mipidsi_display_get_width(mipidsi_display_obj_t *self);
int common_hal_mipidsi_display_get_height(mipidsi_display_obj_t *self);
int common_hal_mipidsi_display_get_row_stride(mipidsi_display_obj_t *self);
int common_hal_mipidsi_display_get_color_depth(mipidsi_display_obj_t *self);
mp_uint_t common_hal_mipidsi_display_get_pixel_clock_frequency(mipidsi_display_obj_t *self);
int common_hal_mipidsi_display_get_native_frames_per_second(mipidsi_display_obj_t *self);
bool common_hal_mipidsi_display_get_grayscale(mipidsi_display_obj_t *self);
mp_int_t common_hal_mipidsi_display_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags);
