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

void common_hal_displayio_epaperdisplay_construct(displayio_epaperdisplay_obj_t *self,
    mp_obj_t bus, const uint8_t *start_sequence, uint16_t start_sequence_len, const uint8_t *stop_sequence, uint16_t stop_sequence_len,
    uint16_t width, uint16_t height, uint16_t ram_width, uint16_t ram_height, int16_t colstart, int16_t rowstart, uint16_t rotation,
    uint16_t set_column_window_command, uint16_t set_row_window_command,
    uint16_t set_current_column_command, uint16_t set_current_row_command,
    uint16_t write_black_ram_command, bool black_bits_inverted, uint16_t write_color_ram_command, bool color_bits_inverted, uint32_t highlight_color, uint16_t refresh_display_command, mp_float_t refresh_time,
    const mcu_pin_obj_t *busy_pin, bool busy_state, mp_float_t seconds_per_frame, bool always_toggle_chip_select, bool grayscale);

bool common_hal_displayio_epaperdisplay_refresh(displayio_epaperdisplay_obj_t *self);

bool common_hal_displayio_epaperdisplay_show(displayio_epaperdisplay_obj_t *self, displayio_group_t *root_group);

// Returns time in milliseconds.
uint32_t common_hal_displayio_epaperdisplay_get_time_to_refresh(displayio_epaperdisplay_obj_t *self);
bool common_hal_displayio_epaperdisplay_get_busy(displayio_epaperdisplay_obj_t *self);

uint16_t common_hal_displayio_epaperdisplay_get_width(displayio_epaperdisplay_obj_t *self);
uint16_t common_hal_displayio_epaperdisplay_get_height(displayio_epaperdisplay_obj_t *self);
uint16_t common_hal_displayio_epaperdisplay_get_rotation(displayio_epaperdisplay_obj_t *self);
void common_hal_displayio_epaperdisplay_set_rotation(displayio_epaperdisplay_obj_t *self, int rotation);

mp_obj_t common_hal_displayio_epaperdisplay_get_bus(displayio_epaperdisplay_obj_t *self);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_EPAPERDISPLAY_H
