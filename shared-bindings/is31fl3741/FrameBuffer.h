// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Mark Komus
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/is31fl3741/FrameBuffer.h"
#include "shared-module/is31fl3741/IS31FL3741.h"

extern const mp_obj_type_t is31fl3741_framebuffer_type;

void common_hal_is31fl3741_framebuffer_construct(is31fl3741_framebuffer_obj_t *self, int width, int height, mp_obj_t framebuffer, is31fl3741_IS31FL3741_obj_t *is31, mp_obj_t mapping);

void common_hal_is31fl3741_framebuffer_deinit(is31fl3741_framebuffer_obj_t *);

int common_hal_is31fl3741_framebuffer_get_width(is31fl3741_framebuffer_obj_t *self);
int common_hal_is31fl3741_framebuffer_get_height(is31fl3741_framebuffer_obj_t *self);

void common_hal_is31fl3741_framebuffer_set_global_current(is31fl3741_framebuffer_obj_t *self, uint8_t current);
uint8_t common_hal_is31fl3741_framebuffer_get_global_current(is31fl3741_framebuffer_obj_t *self);

void common_hal_is31fl3741_framebuffer_set_paused(is31fl3741_framebuffer_obj_t *self, bool paused);
bool common_hal_is31fl3741_framebuffer_get_paused(is31fl3741_framebuffer_obj_t *self);
void common_hal_is31fl3741_framebuffer_refresh(is31fl3741_framebuffer_obj_t *self, uint8_t *dirtyrows);

void common_hal_is31fl3741_framebuffer_reconstruct(is31fl3741_framebuffer_obj_t *self, mp_obj_t framebuffer);

void is31fl3741_framebuffer_collect_ptrs(is31fl3741_framebuffer_obj_t *self);
