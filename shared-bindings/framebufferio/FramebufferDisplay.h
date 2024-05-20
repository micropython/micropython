// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017, 2018 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include "shared-module/framebufferio/FramebufferDisplay.h"
#include "shared-module/displayio/Group.h"

extern const mp_obj_type_t framebufferio_framebufferdisplay_type;

#define NO_BRIGHTNESS_COMMAND 0x100
#define NO_FPS_LIMIT 0xffffffff

void common_hal_framebufferio_framebufferdisplay_construct(framebufferio_framebufferdisplay_obj_t *self,
    mp_obj_t framebuffer,
    uint16_t rotation,
    bool auto_refresh);

bool common_hal_framebufferio_framebufferdisplay_refresh(framebufferio_framebufferdisplay_obj_t *self, uint32_t target_ms_per_frame, uint32_t maximum_ms_per_real_frame);

bool common_hal_framebufferio_framebufferdisplay_get_auto_refresh(framebufferio_framebufferdisplay_obj_t *self);
void common_hal_framebufferio_framebufferdisplay_set_auto_refresh(framebufferio_framebufferdisplay_obj_t *self, bool auto_refresh);

uint16_t common_hal_framebufferio_framebufferdisplay_get_width(framebufferio_framebufferdisplay_obj_t *self);
uint16_t common_hal_framebufferio_framebufferdisplay_get_height(framebufferio_framebufferdisplay_obj_t *self);
uint16_t common_hal_framebufferio_framebufferdisplay_get_rotation(framebufferio_framebufferdisplay_obj_t *self);
void common_hal_framebufferio_framebufferdisplay_set_rotation(framebufferio_framebufferdisplay_obj_t *self, int rotation);

mp_float_t common_hal_framebufferio_framebufferdisplay_get_brightness(framebufferio_framebufferdisplay_obj_t *self);
bool common_hal_framebufferio_framebufferdisplay_set_brightness(framebufferio_framebufferdisplay_obj_t *self, mp_float_t brightness);

mp_obj_t common_hal_framebufferio_framebufferdisplay_framebuffer(framebufferio_framebufferdisplay_obj_t *self);

mp_obj_t common_hal_framebufferio_framebufferdisplay_get_root_group(framebufferio_framebufferdisplay_obj_t *self);
mp_obj_t common_hal_framebufferio_framebufferdisplay_set_root_group(framebufferio_framebufferdisplay_obj_t *self, displayio_group_t *root_group);
