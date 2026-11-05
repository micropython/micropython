// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "shared-bindings/mipidsi/Bus.h"
#include "common-hal/microcontroller/Pin.h"
#include "common-hal/digitalio/DigitalInOut.h"
#include "common-hal/pwmio/PWMOut.h"
#include <esp_lcd_panel_io.h>
#include <esp_lcd_mipi_dsi.h>

typedef struct {
    mp_obj_base_t base;
    mipidsi_bus_obj_t *bus;
    mp_uint_t virtual_channel;
    mp_uint_t width;
    mp_uint_t height;
    mp_int_t rotation;
    mp_uint_t color_depth;
    mp_uint_t native_frames_per_second;
    mp_uint_t pixel_clock_frequency;
    uint8_t *framebuffer;
    esp_lcd_panel_io_handle_t dbi_io_handle;
    esp_lcd_panel_handle_t dpi_panel_handle;
    size_t framebuffer_size;
    union {
        digitalio_digitalinout_obj_t backlight_inout;
        #if CIRCUITPY_PWMIO
        pwmio_pwmout_obj_t backlight_pwm;
        #endif
    };
    bool backlight_on_high;
    mp_float_t current_brightness;
} mipidsi_display_obj_t;
