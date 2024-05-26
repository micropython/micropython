// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"

#include "bindings/picodvi/Framebuffer.h"
#include "shared-module/displayio/__init__.h"
#include "shared-bindings/framebufferio/FramebufferDisplay.h"

void board_init(void) {
    picodvi_framebuffer_obj_t *fb = &allocate_display_bus()->picodvi;
    fb->base.type = &picodvi_framebuffer_type;
    common_hal_picodvi_framebuffer_construct(fb, 320, 240,
        &pin_GPIO7, &pin_GPIO6,
        &pin_GPIO9, &pin_GPIO8,
        &pin_GPIO11, &pin_GPIO10,
        &pin_GPIO13, &pin_GPIO12,
        8);

    framebufferio_framebufferdisplay_obj_t *display = &displays[0].framebuffer_display;
    display->base.type = &framebufferio_framebufferdisplay_type;
    common_hal_framebufferio_framebufferdisplay_construct(
        display,
        MP_OBJ_FROM_PTR(fb),
        0,
        true);
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
