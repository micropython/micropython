// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/videocore/Framebuffer.h"

extern const mp_obj_type_t videocore_framebuffer_type;

void common_hal_videocore_framebuffer_construct(videocore_framebuffer_obj_t *self, mp_uint_t width, mp_uint_t height);
void common_hal_videocore_framebuffer_deinit(videocore_framebuffer_obj_t *self);
bool common_hal_videocore_framebuffer_deinited(videocore_framebuffer_obj_t *self);
void common_hal_videocore_framebuffer_refresh(videocore_framebuffer_obj_t *self);
int common_hal_videocore_framebuffer_get_width(videocore_framebuffer_obj_t *self);
int common_hal_videocore_framebuffer_get_height(videocore_framebuffer_obj_t *self);
int common_hal_videocore_framebuffer_get_row_stride(videocore_framebuffer_obj_t *self);
mp_int_t common_hal_videocore_framebuffer_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags);
