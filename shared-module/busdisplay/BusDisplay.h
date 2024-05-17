// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/displayio/Group.h"
#if CIRCUITPY_PWMIO
#include "shared-bindings/pwmio/PWMOut.h"
#endif

#include "shared-module/displayio/area.h"
#include "shared-module/displayio/bus_core.h"
#include "shared-module/displayio/display_core.h"

typedef struct {
    mp_obj_base_t base;
    displayio_display_core_t core;
    displayio_display_bus_t bus;
    union {
        digitalio_digitalinout_obj_t backlight_inout;
        #if CIRCUITPY_PWMIO
        pwmio_pwmout_obj_t backlight_pwm;
        #endif
    };
    uint64_t last_refresh_call;
    mp_float_t current_brightness;
    uint16_t brightness_command;
    uint16_t native_frames_per_second;
    uint16_t native_ms_per_frame;
    uint8_t write_ram_command;
    bool auto_refresh;
    bool first_manual_refresh;
    bool backlight_on_high;
} busdisplay_busdisplay_obj_t;

void busdisplay_busdisplay_background(busdisplay_busdisplay_obj_t *self);
void release_busdisplay(busdisplay_busdisplay_obj_t *self);
void reset_busdisplay(busdisplay_busdisplay_obj_t *self);
void busdisplay_busdisplay_collect_ptrs(busdisplay_busdisplay_obj_t *self);
