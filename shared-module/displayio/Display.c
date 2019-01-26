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

#include "shared-bindings/displayio/Display.h"

#include "py/runtime.h"
#include "shared-bindings/displayio/FourWire.h"
#include "shared-bindings/displayio/ParallelBus.h"
#include "shared-bindings/time/__init__.h"
#include "shared-module/displayio/__init__.h"
#include "supervisor/shared/display.h"

#include <stdint.h>

#include "tick.h"

#define DELAY 0x80

void common_hal_displayio_display_construct(displayio_display_obj_t* self,
        mp_obj_t bus, uint16_t width, uint16_t height, int16_t colstart, int16_t rowstart,
        uint16_t color_depth, uint8_t set_column_command, uint8_t set_row_command,
        uint8_t write_ram_command, uint8_t* init_sequence, uint16_t init_sequence_len) {
    self->width = width;
    self->height = height;
    self->color_depth = color_depth;
    self->set_column_command = set_column_command;
    self->set_row_command = set_row_command;
    self->write_ram_command = write_ram_command;
    self->refresh = false;
    self->current_group = NULL;
    self->colstart = colstart;
    self->rowstart = rowstart;

    if (MP_OBJ_IS_TYPE(bus, &displayio_parallelbus_type)) {
        self->begin_transaction = common_hal_displayio_parallelbus_begin_transaction;
        self->send = common_hal_displayio_parallelbus_send;
        self->end_transaction = common_hal_displayio_parallelbus_end_transaction;
    } else if (MP_OBJ_IS_TYPE(bus, &displayio_fourwire_type)) {
        self->begin_transaction = common_hal_displayio_fourwire_begin_transaction;
        self->send = common_hal_displayio_fourwire_send;
        self->end_transaction = common_hal_displayio_fourwire_end_transaction;
    } else {
        mp_raise_ValueError(translate("Unsupported display bus type"));
    }
    self->bus = bus;

    uint32_t i = 0;
    self->begin_transaction(self->bus);
    while (i < init_sequence_len) {
        uint8_t *cmd = init_sequence + i;
        uint8_t data_size = *(cmd + 1);
        bool delay = (data_size & DELAY) != 0;
        data_size &= ~DELAY;
        uint8_t *data = cmd + 2;
        self->send(self->bus, true, cmd, 1);
        self->send(self->bus, false, data, data_size);
        uint16_t delay_length_ms = 10;
        if (delay) {
            data_size++;
            delay_length_ms = *(cmd + 1 + data_size);
            if (delay_length_ms == 255) {
                delay_length_ms = 500;
            }
        }
        common_hal_time_delay_ms(delay_length_ms);
        i += 2 + data_size;
    }
    self->end_transaction(self->bus);

    supervisor_start_terminal(width, height);

    // Set the group after initialization otherwise we may send pixels while we delay in
    // initialization.
    self->refresh = true;
    self->current_group = &circuitpython_splash;
}

void common_hal_displayio_display_show(displayio_display_obj_t* self, displayio_group_t* root_group) {
    if (root_group == NULL) {
        root_group = &circuitpython_splash;
    }
    self->current_group = root_group;
    common_hal_displayio_display_refresh_soon(self);
}

void common_hal_displayio_display_refresh_soon(displayio_display_obj_t* self) {
    self->refresh = true;
}

int32_t common_hal_displayio_display_wait_for_frame(displayio_display_obj_t* self) {
    uint64_t last_refresh = self->last_refresh;
    while (last_refresh == self->last_refresh) {
        MICROPY_VM_HOOK_LOOP
    }
    return 0;
}

void displayio_display_start_region_update(displayio_display_obj_t* self, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    // TODO(tannewt): Handle displays with single byte bounds.
    self->begin_transaction(self->bus);
    uint16_t data[2];
    self->send(self->bus, true, &self->set_column_command, 1);
    data[0] = __builtin_bswap16(x0 + self->colstart);
    data[1] = __builtin_bswap16(x1 - 1 + self->colstart);
    self->send(self->bus, false, (uint8_t*) data, 4);
    self->send(self->bus, true, &self->set_row_command, 1);
    data[0] = __builtin_bswap16(y0 + 1 + self->rowstart);
    data[1] = __builtin_bswap16(y1 + self->rowstart);
    self->send(self->bus, false, (uint8_t*) data, 4);
    self->send(self->bus, true, &self->write_ram_command, 1);
}

void displayio_display_finish_region_update(displayio_display_obj_t* self) {
    self->end_transaction(self->bus);
}

bool displayio_display_frame_queued(displayio_display_obj_t* self) {
    // Refresh at ~30 fps.
    return (ticks_ms - self->last_refresh) > 32;
}

bool displayio_display_refresh_queued(displayio_display_obj_t* self) {
    return self->refresh || (self->current_group != NULL && displayio_group_needs_refresh(self->current_group));
}

void displayio_display_finish_refresh(displayio_display_obj_t* self) {
    if (self->current_group != NULL) {
        displayio_group_finish_refresh(self->current_group);
    }
    self->refresh = false;
    self->last_refresh = ticks_ms;
}

bool displayio_display_send_pixels(displayio_display_obj_t* self, uint32_t* pixels, uint32_t length) {
    self->send(self->bus, false, (uint8_t*) pixels, length * 4);
    return true;
}
