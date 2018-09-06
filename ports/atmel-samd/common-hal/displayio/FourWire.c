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

#include "shared-bindings/displayio/FourWire.h"

#include <stdint.h>

#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/digitalio/DigitalInOut.h"

#include "tick.h"

void common_hal_displayio_fourwire_construct(displayio_fourwire_obj_t* self,
    const mcu_pin_obj_t* clock, const mcu_pin_obj_t* data, const mcu_pin_obj_t* command,
    const mcu_pin_obj_t* chip_select, const mcu_pin_obj_t* reset, uint16_t width,
    uint16_t height, int16_t colstart, int16_t rowstart, uint16_t color_depth,
    uint8_t set_column_command, uint8_t set_row_command, uint8_t write_ram_command) {

    common_hal_busio_spi_construct(&self->bus, clock, data, mp_const_none);
    common_hal_digitalio_digitalinout_construct(&self->command, command);
    common_hal_digitalio_digitalinout_switch_to_output(&self->command, true, DRIVE_MODE_PUSH_PULL);
    common_hal_digitalio_digitalinout_construct(&self->chip_select, chip_select);
    common_hal_digitalio_digitalinout_switch_to_output(&self->chip_select, true, DRIVE_MODE_PUSH_PULL);

    common_hal_digitalio_digitalinout_construct(&self->reset, reset);
    common_hal_digitalio_digitalinout_switch_to_output(&self->reset, true, DRIVE_MODE_PUSH_PULL);

    self->width = width;
    self->height = height;
    self->color_depth = color_depth;
    self->set_column_command = set_column_command;
    self->set_row_command = set_row_command;
    self->write_ram_command = write_ram_command;
    self->current_group = NULL;
    self->colstart = colstart;
    self->rowstart = rowstart;
}

bool common_hal_displayio_fourwire_begin_transaction(displayio_fourwire_obj_t* self) {
    if (!common_hal_busio_spi_try_lock(&self->bus)) {
        return false;
    }
    // TODO(tannewt): Stop hardcoding SPI frequency, polarity and phase.
    common_hal_busio_spi_configure(&self->bus, 12000000, 0, 0, 8);
    common_hal_digitalio_digitalinout_set_value(&self->chip_select, false);
    return true;
}

void common_hal_displayio_fourwire_send(displayio_fourwire_obj_t* self, bool command, uint8_t *data, uint32_t data_length) {
    common_hal_digitalio_digitalinout_set_value(&self->command, !command);
    common_hal_busio_spi_write(&self->bus, data, data_length);
}

void common_hal_displayio_fourwire_end_transaction(displayio_fourwire_obj_t* self) {
    common_hal_digitalio_digitalinout_set_value(&self->chip_select, true);
    common_hal_busio_spi_unlock(&self->bus);
}

void common_hal_displayio_fourwire_show(displayio_fourwire_obj_t* self, displayio_group_t* root_group) {
    self->current_group = root_group;
    common_hal_displayio_fourwire_refresh_soon(self);
}

void common_hal_displayio_fourwire_refresh_soon(displayio_fourwire_obj_t* self) {
    self->refresh = true;
}

int32_t common_hal_displayio_fourwire_wait_for_frame(displayio_fourwire_obj_t* self) {
    uint64_t last_refresh = self->last_refresh;
    while (last_refresh == self->last_refresh) {
        MICROPY_VM_HOOK_LOOP
    }
    return 0;
}

void displayio_fourwire_start_region_update(displayio_fourwire_obj_t* self, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    // TODO(tannewt): Handle displays with single byte bounds.
    common_hal_displayio_fourwire_begin_transaction(self);
    uint16_t data[2];
    common_hal_displayio_fourwire_send(self, true, &self->set_column_command, 1);
    data[0] = __builtin_bswap16(x0 + self->colstart);
    data[1] = __builtin_bswap16(x1-1 + self->colstart);
    common_hal_displayio_fourwire_send(self, false, (uint8_t*) data, 4);
    common_hal_displayio_fourwire_send(self, true, &self->set_row_command, 1);
    data[0] = __builtin_bswap16(y0 + 1 + self->rowstart);
    data[1] = __builtin_bswap16(y1 + self->rowstart);
    common_hal_displayio_fourwire_send(self, false, (uint8_t*) data, 4);
    common_hal_displayio_fourwire_send(self, true, &self->write_ram_command, 1);
}

bool displayio_fourwire_send_pixels(displayio_fourwire_obj_t* self, uint32_t* pixels, uint32_t length) {
    // TODO: Set this up so its async and 32 bit DMA transfers.
    common_hal_displayio_fourwire_send(self, false, (uint8_t*) pixels, length*4);
    return true;
}

void displayio_fourwire_finish_region_update(displayio_fourwire_obj_t* self) {
    common_hal_displayio_fourwire_end_transaction(self);
}

bool displayio_fourwire_frame_queued(displayio_fourwire_obj_t* self) {
    // Refresh at ~30 fps.
    return (ticks_ms - self->last_refresh) > 32;
}

bool displayio_fourwire_refresh_queued(displayio_fourwire_obj_t* self) {
    return self->refresh || (self->current_group != NULL && displayio_group_needs_refresh(self->current_group));
}

void displayio_fourwire_finish_refresh(displayio_fourwire_obj_t* self) {
    if (self->current_group != NULL) {
        displayio_group_finish_refresh(self->current_group);
    }
    self->refresh = false;
    self->last_refresh = ticks_ms;
}
