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

#include "shared-module/displayio/bus_core.h"

#include "py/gc.h"
#include "py/runtime.h"
#if CIRCUITPY_FOURWIRE
#include "shared-bindings/fourwire/FourWire.h"
#endif
#if CIRCUITPY_I2CDISPLAYBUS
#include "shared-bindings/i2cdisplaybus/I2CDisplayBus.h"
#endif
#if CIRCUITPY_PARALLELDISPLAYBUS
#include "shared-bindings/paralleldisplaybus/ParallelBus.h"
#endif
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/time/__init__.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/display_core.h"
#include "supervisor/shared/display.h"
#include "supervisor/shared/tick.h"

#include <stdint.h>
#include <string.h>

#define DISPLAYIO_CORE_DEBUG(...) (void)0
// #define DISPLAYIO_CORE_DEBUG(...) mp_printf(&mp_plat_print __VA_OPT__(,) __VA_ARGS__)

void displayio_display_bus_construct(displayio_display_bus_t *self,
    mp_obj_t bus, uint16_t ram_width, uint16_t ram_height, int16_t colstart, int16_t rowstart,
    uint16_t column_command, uint16_t row_command, uint16_t set_current_column_command, uint16_t set_current_row_command,
    bool data_as_commands, bool always_toggle_chip_select, bool SH1107_addressing, bool address_little_endian) {

    self->ram_width = ram_width;
    self->ram_height = ram_height;
    self->colstart = colstart;
    self->rowstart = rowstart;

    self->column_command = column_command;
    self->row_command = row_command;
    self->set_current_column_command = set_current_column_command;
    self->set_current_row_command = set_current_row_command;
    self->data_as_commands = data_as_commands;
    self->always_toggle_chip_select = always_toggle_chip_select;
    self->SH1107_addressing = SH1107_addressing;
    self->address_little_endian = address_little_endian;

    #if CIRCUITPY_PARALLELDISPLAYBUS
    if (mp_obj_is_type(bus, &paralleldisplaybus_parallelbus_type)) {
        self->bus_reset = common_hal_paralleldisplaybus_parallelbus_reset;
        self->bus_free = common_hal_paralleldisplaybus_parallelbus_bus_free;
        self->begin_transaction = common_hal_paralleldisplaybus_parallelbus_begin_transaction;
        self->send = common_hal_paralleldisplaybus_parallelbus_send;
        self->end_transaction = common_hal_paralleldisplaybus_parallelbus_end_transaction;
        self->collect_ptrs = common_hal_paralleldisplaybus_parallelbus_collect_ptrs;
    } else
    #endif
    #if CIRCUITPY_FOURWIRE
    if (mp_obj_is_type(bus, &fourwire_fourwire_type)) {
        self->bus_reset = common_hal_fourwire_fourwire_reset;
        self->bus_free = common_hal_fourwire_fourwire_bus_free;
        self->begin_transaction = common_hal_fourwire_fourwire_begin_transaction;
        self->send = common_hal_fourwire_fourwire_send;
        self->end_transaction = common_hal_fourwire_fourwire_end_transaction;
        self->collect_ptrs = common_hal_fourwire_fourwire_collect_ptrs;
    } else
    #endif
    #if CIRCUITPY_I2CDISPLAYBUS
    if (mp_obj_is_type(bus, &i2cdisplaybus_i2cdisplaybus_type)) {
        self->bus_reset = common_hal_i2cdisplaybus_i2cdisplaybus_reset;
        self->bus_free = common_hal_i2cdisplaybus_i2cdisplaybus_bus_free;
        self->begin_transaction = common_hal_i2cdisplaybus_i2cdisplaybus_begin_transaction;
        self->send = common_hal_i2cdisplaybus_i2cdisplaybus_send;
        self->end_transaction = common_hal_i2cdisplaybus_i2cdisplaybus_end_transaction;
        self->collect_ptrs = common_hal_i2cdisplaybus_i2cdisplaybus_collect_ptrs;
    } else
    #endif
    {
        mp_raise_ValueError(MP_ERROR_TEXT("Unsupported display bus type"));
    }
    self->bus = bus;
}

bool displayio_display_bus_is_free(displayio_display_bus_t *self) {
    return !self->bus || self->bus_free(self->bus);
}

bool displayio_display_bus_begin_transaction(displayio_display_bus_t *self) {
    return self->begin_transaction(self->bus);
}

void displayio_display_bus_end_transaction(displayio_display_bus_t *self) {
    self->end_transaction(self->bus);
}

void displayio_display_bus_set_region_to_update(displayio_display_bus_t *self, displayio_display_core_t *display, displayio_area_t *area) {
    uint16_t x1 = area->x1 + self->colstart;
    uint16_t x2 = area->x2 + self->colstart;
    uint16_t y1 = area->y1 + self->rowstart;
    uint16_t y2 = area->y2 + self->rowstart;

    // Collapse down the dimension where multiple pixels are in a byte.
    if (display->colorspace.depth < 8) {
        uint8_t pixels_per_byte = 8 / display->colorspace.depth;
        if (display->colorspace.pixels_in_byte_share_row) {
            x1 /= pixels_per_byte * display->colorspace.bytes_per_cell;
            x2 /= pixels_per_byte * display->colorspace.bytes_per_cell;
        } else {
            y1 /= pixels_per_byte * display->colorspace.bytes_per_cell;
            y2 /= pixels_per_byte * display->colorspace.bytes_per_cell;
        }
    }

    x2 -= 1;
    y2 -= 1;

    display_chip_select_behavior_t chip_select = CHIP_SELECT_UNTOUCHED;
    if (self->always_toggle_chip_select || self->data_as_commands) {
        chip_select = CHIP_SELECT_TOGGLE_EVERY_BYTE;
    }

    // Set column.
    displayio_display_bus_begin_transaction(self);
    uint8_t data[5];
    data[0] = self->column_command;
    uint8_t data_length = 1;
    display_byte_type_t data_type = DISPLAY_DATA;
    if (!self->data_as_commands) {
        self->send(self->bus, DISPLAY_COMMAND, CHIP_SELECT_UNTOUCHED, data, 1);
        data_length = 0;
    } else {
        data_type = DISPLAY_COMMAND;
    }

    if (self->ram_width < 0x100) {
        data[data_length++] = x1;
        data[data_length++] = x2;
    } else {
        if (self->address_little_endian) {
            x1 = __builtin_bswap16(x1);
            x2 = __builtin_bswap16(x2);
        }
        data[data_length++] = x1 >> 8;
        data[data_length++] = x1 & 0xff;
        data[data_length++] = x2 >> 8;
        data[data_length++] = x2 & 0xff;
    }

    // Quirk for SH1107 "SH1107_addressing"
    //     Column lower command = 0x00, Column upper command = 0x10
    if (self->SH1107_addressing) {
        data[0] = ((x1 >> 4) & 0x0F) | 0x10; // 0x10 to 0x17
        data[1] = x1 & 0x0F; // 0x00 to 0x0F
        data_length = 2;
    }

    self->send(self->bus, data_type, chip_select, data, data_length);
    displayio_display_bus_end_transaction(self);

    if (self->set_current_column_command != NO_COMMAND) {
        uint8_t command = self->set_current_column_command;
        displayio_display_bus_begin_transaction(self);
        self->send(self->bus, DISPLAY_COMMAND, chip_select, &command, 1);
        // Only send the first half of data because it is the first coordinate.
        self->send(self->bus, DISPLAY_DATA, chip_select, data, data_length / 2);
        displayio_display_bus_end_transaction(self);
    }


    // Set row.
    displayio_display_bus_begin_transaction(self);
    data[0] = self->row_command;
    data_length = 1;
    if (!self->data_as_commands) {
        self->send(self->bus, DISPLAY_COMMAND, CHIP_SELECT_UNTOUCHED, data, 1);
        data_length = 0;
    }

    if (self->ram_height < 0x100) {
        data[data_length++] = y1;
        data[data_length++] = y2;
    } else {
        if (self->address_little_endian) {
            y1 = __builtin_bswap16(y1);
            y2 = __builtin_bswap16(y2);
        }
        data[data_length++] = y1 >> 8;
        data[data_length++] = y1 & 0xff;
        data[data_length++] = y2 >> 8;
        data[data_length++] = y2 & 0xff;
    }

    // Quirk for SH1107 "SH1107_addressing"
    //     Page address command = 0xB0
    if (self->SH1107_addressing) {
        // set the page to our y value
        data[0] = 0xB0 | y1;
        data_length = 1;
    }

    self->send(self->bus, data_type, chip_select, data, data_length);
    displayio_display_bus_end_transaction(self);

    if (self->set_current_row_command != NO_COMMAND) {
        uint8_t command = self->set_current_row_command;
        displayio_display_bus_begin_transaction(self);
        self->send(self->bus, DISPLAY_COMMAND, chip_select, &command, 1);
        // Only send the first half of data because it is the first coordinate.
        self->send(self->bus, DISPLAY_DATA, chip_select, data, data_length / 2);
        displayio_display_bus_end_transaction(self);
    }
}

void displayio_display_bus_collect_ptrs(displayio_display_bus_t *self) {
    self->collect_ptrs(self->bus);
}
