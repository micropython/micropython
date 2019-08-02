/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_EPAPERDISPLAY_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_EPAPERDISPLAY_H

#include "common-hal/microcontroller/Pin.h"

#include "shared-module/displayio/EPaperDisplay.h"
#include "shared-module/displayio/Group.h"

extern const mp_obj_type_t displayio_epaperdisplay_type;

#define DELAY 0x80

#define NO_COMMAND 0x100

void common_hal_displayio_epaperdisplay_construct(displayio_epaperdisplay_obj_t* self,
        mp_obj_t bus, uint8_t* start_sequence, uint16_t start_sequence_len, uint8_t* stop_sequence, uint16_t stop_sequence_len,
        uint16_t width, uint16_t height, uint16_t ram_width, uint16_t ram_height, int16_t colstart, int16_t rowstart, uint16_t rotation,
        uint16_t set_column_window_command, uint16_t set_row_window_command,
        uint16_t set_current_column_command, uint16_t set_current_row_command,
        uint16_t write_black_ram_command, bool black_bits_inverted, uint16_t write_color_ram_command, bool color_bits_inverted, uint32_t third_color, uint16_t refresh_display_command,
        const mcu_pin_obj_t* busy_pin, bool busy_state, mp_float_t seconds_per_frame, bool always_toggle_chip_select);

int32_t common_hal_displayio_epaperdisplay_wait_for_frame(displayio_epaperdisplay_obj_t* self);

bool common_hal_displayio_epaperdisplay_show(displayio_epaperdisplay_obj_t* self, displayio_group_t* root_group);

void common_hal_displayio_epaperdisplay_refresh_soon(displayio_epaperdisplay_obj_t* self);

bool displayio_epaperdisplay_begin_transaction(displayio_epaperdisplay_obj_t* self);
void displayio_epaperdisplay_end_transaction(displayio_epaperdisplay_obj_t* self);

// The second point of the region is exclusive.
void displayio_epaperdisplay_set_region_to_update(displayio_epaperdisplay_obj_t* self, displayio_area_t* area);
bool displayio_epaperdisplay_frame_queued(displayio_epaperdisplay_obj_t* self);

bool displayio_epaperdisplay_refresh_queued(displayio_epaperdisplay_obj_t* self);
void displayio_epaperdisplay_finish_refresh(displayio_epaperdisplay_obj_t* self);
void displayio_epaperdisplay_send_pixels(displayio_epaperdisplay_obj_t* self, uint8_t* pixels, uint32_t length);

bool common_hal_displayio_epaperdisplay_get_auto_brightness(displayio_epaperdisplay_obj_t* self);
void common_hal_displayio_epaperdisplay_set_auto_brightness(displayio_epaperdisplay_obj_t* self, bool auto_brightness);

uint16_t common_hal_displayio_epaperdisplay_get_width(displayio_epaperdisplay_obj_t* self);
uint16_t common_hal_displayio_epaperdisplay_get_height(displayio_epaperdisplay_obj_t* self);

mp_float_t common_hal_displayio_epaperdisplay_get_brightness(displayio_epaperdisplay_obj_t* self);
bool common_hal_displayio_epaperdisplay_set_brightness(displayio_epaperdisplay_obj_t* self, mp_float_t brightness);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_EPAPERDISPLAY_H
