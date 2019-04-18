/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017, 2018 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYBUSIO_DISPLAY_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYBUSIO_DISPLAY_H

#include "common-hal/microcontroller/Pin.h"

#include "shared-module/displayio/Display.h"
#include "shared-module/displayio/Group.h"

extern const mp_obj_type_t displayio_display_type;

#define DELAY 0x80

void common_hal_displayio_display_construct(displayio_display_obj_t* self,
    mp_obj_t bus, uint16_t width, uint16_t height,
    int16_t colstart, int16_t rowstart, uint16_t rotation, uint16_t color_depth,
    uint8_t set_column_command, uint8_t set_row_command, uint8_t write_ram_command, uint8_t set_vertical_scroll,
    uint8_t* init_sequence, uint16_t init_sequence_len, const mcu_pin_obj_t* backlight_pin,
    mp_float_t brightness, bool auto_brightness,
    bool single_byte_bounds, bool data_as_commands);

int32_t common_hal_displayio_display_wait_for_frame(displayio_display_obj_t* self);

void common_hal_displayio_display_show(displayio_display_obj_t* self, displayio_group_t* root_group);

void common_hal_displayio_display_refresh_soon(displayio_display_obj_t* self);

bool displayio_display_begin_transaction(displayio_display_obj_t* self);
void displayio_display_end_transaction(displayio_display_obj_t* self);

void displayio_display_set_region_to_update(displayio_display_obj_t* self, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
bool displayio_display_frame_queued(displayio_display_obj_t* self);

bool displayio_display_refresh_queued(displayio_display_obj_t* self);
void displayio_display_finish_refresh(displayio_display_obj_t* self);
void displayio_display_send_pixels(displayio_display_obj_t* self, uint32_t* pixels, uint32_t length);

bool common_hal_displayio_display_get_auto_brightness(displayio_display_obj_t* self);
void common_hal_displayio_display_set_auto_brightness(displayio_display_obj_t* self, bool auto_brightness);

uint16_t common_hal_displayio_display_get_width(displayio_display_obj_t* self);
uint16_t common_hal_displayio_display_get_height(displayio_display_obj_t* self);

mp_float_t common_hal_displayio_display_get_brightness(displayio_display_obj_t* self);
bool common_hal_displayio_display_set_brightness(displayio_display_obj_t* self, mp_float_t brightness);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYBUSIO_DISPLAY_H
