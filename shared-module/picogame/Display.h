// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

// Fast display backend on a port with async busio SPI: wraps an existing busdisplay and sends
// each strip with busio's write_start() so the CPU blits the next strip while the current one
// goes out. Reuses the busdisplay's bus, window opcodes and dimensions.

#pragma once

#include "py/obj.h"
#include "shared-bindings/busdisplay/BusDisplay.h"
#include "shared-module/picogame/Sprite.h"

typedef struct {
    mp_obj_base_t base;
    busdisplay_busdisplay_obj_t *display;
    bool async_send;  // the display bus sends a strip asynchronously, so a blit overlaps a send
    bool rgb444;      // pack strips to 12-bit RGB444 before sending (~25% less SPI traffic)
} picogame_display_obj_t;

void common_hal_picogame_display_construct(picogame_display_obj_t *self,
    busdisplay_busdisplay_obj_t *display, bool rgb444);

void common_hal_picogame_display_render(picogame_display_obj_t *self,
    mp_obj_t *items, uint8_t *kinds, size_t n,
    uint16_t *buf_a, uint16_t *buf_b, size_t buf_pixels,
    int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t background,
    int ox, int oy);
