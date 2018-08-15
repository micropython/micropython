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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYBUSIO_FOURWIRE_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYBUSIO_FOURWIRE_H

#include "common-hal/displayio/FourWire.h"
#include "common-hal/microcontroller/Pin.h"

#include "shared-module/displayio/Group.h"

extern const mp_obj_type_t displayio_fourwire_type;

// TODO(tannewt): Split this apart into FourWire and a Display object because the dimensions and
// commands are also used for the parallel buses.
void common_hal_displayio_fourwire_construct(displayio_fourwire_obj_t* self,
    const mcu_pin_obj_t* clock, const mcu_pin_obj_t* data, const mcu_pin_obj_t* command,
    const mcu_pin_obj_t* chip_select, const mcu_pin_obj_t* reset, uint16_t width, uint16_t height,
    int16_t colstart, int16_t rowstart, uint16_t color_depth,
    uint8_t set_column_command, uint8_t set_row_command, uint8_t write_ram_command);

int32_t common_hal_displayio_fourwire_wait_for_frame(displayio_fourwire_obj_t* self);

bool common_hal_displayio_fourwire_begin_transaction(displayio_fourwire_obj_t* self);

void common_hal_displayio_fourwire_send(displayio_fourwire_obj_t* self, bool command, uint8_t *data, uint32_t data_length);

void common_hal_displayio_fourwire_end_transaction(displayio_fourwire_obj_t* self);

void common_hal_displayio_fourwire_show(displayio_fourwire_obj_t* self, displayio_group_t* root_group);

void common_hal_displayio_fourwire_refresh_soon(displayio_fourwire_obj_t* self);

void displayio_fourwire_start_region_update(displayio_fourwire_obj_t* self, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void displayio_fourwire_finish_region_update(displayio_fourwire_obj_t* self);
bool displayio_fourwire_frame_queued(displayio_fourwire_obj_t* self);

bool displayio_fourwire_refresh_queued(displayio_fourwire_obj_t* self);
void displayio_fourwire_finish_refresh(displayio_fourwire_obj_t* self);
bool displayio_fourwire_send_pixels(displayio_fourwire_obj_t* self, uint32_t* pixels, uint32_t length);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYBUSIO_FOURWIRE_H
