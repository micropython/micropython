/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#ifndef MICROPY_INCLUDED_SHARED_MODULE_DISPLAYIO_DISPLAY_H
#define MICROPY_INCLUDED_SHARED_MODULE_DISPLAYIO_DISPLAY_H

#include "shared-module/displayio/Group.h"

typedef bool (*display_bus_begin_transaction)(mp_obj_t bus);
typedef void (*display_bus_send)(mp_obj_t bus, bool command, uint8_t *data, uint32_t data_length);
typedef void (*display_bus_end_transaction)(mp_obj_t bus);

typedef struct {
    mp_obj_base_t base;
    mp_obj_t bus;
    uint16_t width;
    uint16_t height;
    uint16_t color_depth;
    uint8_t set_column_command;
    uint8_t set_row_command;
    uint8_t write_ram_command;
    displayio_group_t *current_group;
    bool refresh;
    uint64_t last_refresh;
    int16_t colstart;
    int16_t rowstart;
    display_bus_begin_transaction begin_transaction;
    display_bus_send send;
    display_bus_end_transaction end_transaction;
} displayio_display_obj_t;

#endif // MICROPY_INCLUDED_SHARED_MODULE_DISPLAYIO_DISPLAY_H
