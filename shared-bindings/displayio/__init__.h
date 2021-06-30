/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO___INIT___H
#define MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO___INIT___H

#include "py/enum.h"
#include "py/obj.h"

typedef enum {
    DISPLAY_COMMAND,
    DISPLAY_DATA
} display_byte_type_t;

typedef enum {
    CHIP_SELECT_UNTOUCHED,
    CHIP_SELECT_TOGGLE_EVERY_BYTE
} display_chip_select_behavior_t;

typedef enum {
    DISPLAYIO_COLORSPACE_RGB888,
    DISPLAYIO_COLORSPACE_RGB565,
    DISPLAYIO_COLORSPACE_RGB555,
    DISPLAYIO_COLORSPACE_RGB565_SWAPPED,
    DISPLAYIO_COLORSPACE_RGB555_SWAPPED,
    DISPLAYIO_COLORSPACE_BGR565,
    DISPLAYIO_COLORSPACE_BGR555,
    DISPLAYIO_COLORSPACE_BGR565_SWAPPED,
    DISPLAYIO_COLORSPACE_BGR555_SWAPPED,
} displayio_colorspace_t;

typedef bool (*display_bus_bus_reset)(mp_obj_t bus);
typedef bool (*display_bus_bus_free)(mp_obj_t bus);
typedef bool (*display_bus_begin_transaction)(mp_obj_t bus);
typedef void (*display_bus_send)(mp_obj_t bus, display_byte_type_t byte_type,
    display_chip_select_behavior_t chip_select, const uint8_t *data, uint32_t data_length);
typedef void (*display_bus_end_transaction)(mp_obj_t bus);

void common_hal_displayio_release_displays(void);

extern const mp_obj_type_t displayio_colorspace_type;
extern const cp_enum_obj_t displayio_colorspace_RGB888_obj;

#endif  // MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO___INIT___H
