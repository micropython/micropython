// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_interface.h"

typedef struct dotclockframebuffer_framebuffer_obj {
    mp_obj_base_t base;
    mp_buffer_info_t bufinfo;
    mp_int_t width, row_stride;
    uint32_t frequency, refresh_rate;
    uint32_t first_pixel_offset;
    uint64_t used_pins_mask;
    volatile int32_t frame_count;
    esp_lcd_rgb_panel_config_t panel_config;
    esp_lcd_panel_handle_t panel_handle;
} dotclockframebuffer_framebuffer_obj_t;
