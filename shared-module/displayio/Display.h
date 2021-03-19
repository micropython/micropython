/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef MICROPY_INCLUDED_SHARED_MODULE_DISPLAYIO_DISPLAY_H
#define MICROPY_INCLUDED_SHARED_MODULE_DISPLAYIO_DISPLAY_H

#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/displayio/Group.h"
#if CIRCUITPY_PWMIO
#include "shared-bindings/pwmio/PWMOut.h"
#endif

#include "shared-module/displayio/area.h"
#include "shared-module/displayio/display_core.h"

typedef struct {
    mp_obj_base_t base;
    displayio_display_core_t core;
    union {
        digitalio_digitalinout_obj_t backlight_inout;
        #if CIRCUITPY_PWMIO
        pwmio_pwmout_obj_t backlight_pwm;
        #endif
    };
    uint64_t last_backlight_refresh;
    uint64_t last_refresh_call;
    mp_float_t current_brightness;
    uint16_t brightness_command;
    uint16_t native_frames_per_second;
    uint16_t native_ms_per_frame;
    uint8_t set_column_command;
    uint8_t set_row_command;
    uint8_t write_ram_command;
    bool auto_refresh;
    bool first_manual_refresh;
    bool data_as_commands;
    bool auto_brightness;
    bool updating_backlight;
    bool backlight_on_high;
    // new quirk for sh1107
    bool SH1107_addressing;
} displayio_display_obj_t;

void displayio_display_background(displayio_display_obj_t *self);
void release_display(displayio_display_obj_t *self);
void reset_display(displayio_display_obj_t *self);

void displayio_display_collect_ptrs(displayio_display_obj_t *self);

#endif // MICROPY_INCLUDED_SHARED_MODULE_DISPLAYIO_DISPLAY_H
