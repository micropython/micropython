// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/board/__init__.h"
#include "shared-bindings/dotclockframebuffer/DotClockFramebuffer.h"
#include "shared-bindings/dotclockframebuffer/__init__.h"
#include "shared-bindings/framebufferio/FramebufferDisplay.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"

static const mcu_pin_obj_t *blue_pins[] = {
    &pin_GPIO3,
    &pin_GPIO4,
    &pin_GPIO5,
    &pin_GPIO6,
    &pin_GPIO7
};
static const mcu_pin_obj_t *green_pins[] = {
    &pin_GPIO8,
    &pin_GPIO9,
    &pin_GPIO10,
    &pin_GPIO11,
    &pin_GPIO12,
    &pin_GPIO13
};
static const mcu_pin_obj_t *red_pins[] = {
    &pin_GPIO14,
    &pin_GPIO15,
    &pin_GPIO16,
    &pin_GPIO17,
    &pin_GPIO18
};

static void display_init(void) {

    // Turn on backlight
    // gpio_set_direction(2, GPIO_MODE_DEF_OUTPUT);
    // gpio_set_level(2, true);
    common_hal_never_reset_pin(&pin_GPIO39);

    dotclockframebuffer_framebuffer_obj_t *framebuffer = &allocate_display_bus_or_raise()->dotclock;
    framebuffer->base.type = &dotclockframebuffer_framebuffer_type;

    common_hal_dotclockframebuffer_framebuffer_construct(
        framebuffer,
        &pin_GPIO45,    // de
        &pin_GPIO48,    // vsync
        &pin_GPIO47,    // hsync
        &pin_GPIO21,    // pclk
        red_pins, MP_ARRAY_SIZE(red_pins),
        green_pins, MP_ARRAY_SIZE(green_pins),
        blue_pins, MP_ARRAY_SIZE(blue_pins),
        6500000,        // Frequency
        800,            // width
        480,            // height
        30, 16, 210, false, // horiz: pulse, back porch, front porch, idle low
        13, 10, 22, false,  // vert: pulse, back porch, front porch, idle low
        false,          // DE idle high
        false,          // pclk active high
        false,          // pclk idle high
        0               // overscan left
        );

    framebufferio_framebufferdisplay_obj_t *display = &allocate_display_or_raise()->framebuffer_display;
    display->base.type = &framebufferio_framebufferdisplay_type;
    common_hal_framebufferio_framebufferdisplay_construct(
        display,
        framebuffer,
        0,    // rotation
        true  // auto-refresh
        );
}

void board_init(void) {
    // Debug UART
    #ifdef DEBUG
    common_hal_never_reset_pin(&pin_GPIO43);
    common_hal_never_reset_pin(&pin_GPIO44);
    #endif
    display_init();
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
