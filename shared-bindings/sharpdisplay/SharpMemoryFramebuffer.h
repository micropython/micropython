// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/objtype.h"

#include "shared-module/sharpdisplay/SharpMemoryFramebuffer.h"

extern const mp_obj_type_t sharpdisplay_framebuffer_type;

void common_hal_sharpdisplay_framebuffer_construct(sharpdisplay_framebuffer_obj_t *self, busio_spi_obj_t *spi, const mcu_pin_obj_t *chip_select, int baudrate, int width, int height, bool jdi_display);
void common_hal_sharpdisplay_framebuffer_swap_buffers(sharpdisplay_framebuffer_obj_t *self, uint8_t *dirty_row_bitmask);
void common_hal_sharpdisplay_framebuffer_deinit(sharpdisplay_framebuffer_obj_t *self);
void common_hal_sharpdisplay_framebuffer_get_bufinfo(sharpdisplay_framebuffer_obj_t *self, mp_buffer_info_t *bufinfo);
int common_hal_sharpdisplay_framebuffer_get_height(sharpdisplay_framebuffer_obj_t *self);
int common_hal_sharpdisplay_framebuffer_get_width(sharpdisplay_framebuffer_obj_t *self);
void common_hal_sharpdisplay_framebuffer_swap_buffers(sharpdisplay_framebuffer_obj_t *self, uint8_t *dirty_row_bitmask);
void common_hal_sharpdisplay_framebuffer_reset(sharpdisplay_framebuffer_obj_t *self);
void common_hal_sharpdisplay_framebuffer_reconstruct(sharpdisplay_framebuffer_obj_t *self);
