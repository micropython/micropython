/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017, 2018 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_FRAMEBUFFERDISPLAY_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_FRAMEBUFFERDISPLAY_H

#include "common-hal/microcontroller/Pin.h"

#include "shared-module/framebufferio/FramebufferDisplay.h"
#include "shared-module/displayio/Group.h"

extern const mp_obj_type_t framebufferio_framebufferdisplay_type;

#define DELAY 0x80

#define NO_BRIGHTNESS_COMMAND 0x100

void common_hal_framebufferio_framebufferdisplay_construct(framebufferio_framebufferdisplay_obj_t *self,
    mp_obj_t framebuffer,
    uint16_t rotation,
    bool auto_refresh);

bool common_hal_framebufferio_framebufferdisplay_show(framebufferio_framebufferdisplay_obj_t *self,
    displayio_group_t *root_group);

bool common_hal_framebufferio_framebufferdisplay_refresh(framebufferio_framebufferdisplay_obj_t *self, uint32_t target_ms_per_frame, uint32_t maximum_ms_per_real_frame);

bool common_hal_framebufferio_framebufferdisplay_get_auto_refresh(framebufferio_framebufferdisplay_obj_t *self);
void common_hal_framebufferio_framebufferdisplay_set_auto_refresh(framebufferio_framebufferdisplay_obj_t *self, bool auto_refresh);

uint16_t common_hal_framebufferio_framebufferdisplay_get_width(framebufferio_framebufferdisplay_obj_t *self);
uint16_t common_hal_framebufferio_framebufferdisplay_get_height(framebufferio_framebufferdisplay_obj_t *self);
uint16_t common_hal_framebufferio_framebufferdisplay_get_rotation(framebufferio_framebufferdisplay_obj_t *self);
void common_hal_framebufferio_framebufferdisplay_set_rotation(framebufferio_framebufferdisplay_obj_t *self, int rotation);

bool common_hal_framebufferio_framebufferdisplay_get_auto_brightness(framebufferio_framebufferdisplay_obj_t *self);
bool common_hal_framebufferio_framebufferdisplay_set_auto_brightness(framebufferio_framebufferdisplay_obj_t *self, bool auto_brightness);

mp_float_t common_hal_framebufferio_framebufferdisplay_get_brightness(framebufferio_framebufferdisplay_obj_t *self);
bool common_hal_framebufferio_framebufferdisplay_set_brightness(framebufferio_framebufferdisplay_obj_t *self, mp_float_t brightness);

mp_obj_t common_hal_framebufferio_framebufferdisplay_framebuffer(framebufferio_framebufferdisplay_obj_t *self);


#endif // MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_FRAMEBUFFERDISPLAY_H
