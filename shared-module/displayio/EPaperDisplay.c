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

#include "shared-bindings/displayio/EPaperDisplay.h"

#include "py/runtime.h"
#include "shared-bindings/displayio/ColorConverter.h"
#include "shared-bindings/displayio/FourWire.h"
#include "shared-bindings/displayio/I2CDisplay.h"
#include "shared-bindings/displayio/ParallelBus.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/time/__init__.h"
#include "shared-module/displayio/__init__.h"
#include "supervisor/shared/display.h"
#include "supervisor/usb.h"

#include <stdint.h>
#include <string.h>

#include "tick.h"

void common_hal_displayio_epaperdisplay_construct(displayio_epaperdisplay_obj_t* self,
        mp_obj_t bus, uint8_t* start_sequence, uint16_t start_sequence_len, uint8_t* stop_sequence, uint16_t stop_sequence_len,
        uint16_t width, uint16_t height, uint16_t ram_width, uint16_t ram_height,
        int16_t colstart, int16_t rowstart, uint16_t rotation,
        uint16_t set_column_window_command, uint16_t set_row_window_command,
        uint16_t set_current_column_command, uint16_t set_current_row_command,
        uint16_t write_black_ram_command, bool black_bits_inverted, uint16_t write_color_ram_command, bool color_bits_inverted, uint32_t third_color, uint16_t refresh_display_command,
        const mcu_pin_obj_t* busy_pin, bool busy_state, mp_float_t seconds_per_frame, bool always_toggle_chip_select) {
    self->colorspace.depth = 1;
    self->colorspace.grayscale = true;
    self->colorspace.pixels_in_byte_share_row = true;
    self->colorspace.bytes_per_cell = 1;
    self->colorspace.reverse_pixels_in_byte = true;

    if (third_color != 0x000000) {
        self->colorspace.tricolor = true;
        self->colorspace.tricolor_hue = displayio_colorconverter_compute_hue(third_color);
        self->colorspace.tricolor_luma = displayio_colorconverter_compute_luma(third_color);
    }

    self->set_column_window_command = set_column_window_command;
    self->set_row_window_command = set_row_window_command;
    self->set_current_column_command = set_current_column_command;
    self->set_current_row_command = set_current_row_command;
    self->write_black_ram_command = write_black_ram_command;
    self->black_bits_inverted = black_bits_inverted;
    self->write_color_ram_command = write_color_ram_command;
    self->color_bits_inverted = color_bits_inverted;
    self->refresh_display_command = refresh_display_command;
    self->busy_state = busy_state;
    self->refresh = true;
    self->current_group = NULL;
    self->colstart = colstart;
    self->rowstart = rowstart;
    self->refreshing = false;
    self->milliseconds_per_frame = seconds_per_frame * 1000;
    self->always_toggle_chip_select = always_toggle_chip_select;

    self->start_sequence = start_sequence;
    self->start_sequence_len = start_sequence_len;
    self->stop_sequence = stop_sequence;
    self->stop_sequence_len = stop_sequence_len;


    if (MP_OBJ_IS_TYPE(bus, &displayio_parallelbus_type)) {
        self->bus_reset = common_hal_displayio_parallelbus_reset;
        self->bus_free = common_hal_displayio_parallelbus_bus_free;
        self->begin_transaction = common_hal_displayio_parallelbus_begin_transaction;
        self->send = common_hal_displayio_parallelbus_send;
        self->end_transaction = common_hal_displayio_parallelbus_end_transaction;
    } else if (MP_OBJ_IS_TYPE(bus, &displayio_fourwire_type)) {
        self->bus_reset = common_hal_displayio_fourwire_reset;
        self->bus_free = common_hal_displayio_fourwire_bus_free;
        self->begin_transaction = common_hal_displayio_fourwire_begin_transaction;
        self->send = common_hal_displayio_fourwire_send;
        self->end_transaction = common_hal_displayio_fourwire_end_transaction;
    } else if (MP_OBJ_IS_TYPE(bus, &displayio_i2cdisplay_type)) {
        self->bus_reset = common_hal_displayio_i2cdisplay_reset;
        self->bus_free = common_hal_displayio_i2cdisplay_bus_free;
        self->begin_transaction = common_hal_displayio_i2cdisplay_begin_transaction;
        self->send = common_hal_displayio_i2cdisplay_send;
        self->end_transaction = common_hal_displayio_i2cdisplay_end_transaction;
    } else {
        mp_raise_ValueError(translate("Unsupported display bus type"));
    }
    self->bus = bus;

    supervisor_start_terminal(width, height);

    self->width = width;
    self->height = height;
    rotation = rotation % 360;
    self->transform.x = 0;
    self->transform.y = 0;
    self->transform.scale = 1;
    self->transform.mirror_x = false;
    self->transform.mirror_y = false;
    self->transform.transpose_xy = false;
    if (rotation == 0 || rotation == 180) {
        if (rotation == 180) {
            self->transform.mirror_x = true;
            self->transform.mirror_y = true;
        }
    } else {
        self->transform.transpose_xy = true;
        if (rotation == 270) {
            self->transform.mirror_y = true;
        } else {
            self->transform.mirror_x = true;
        }
    }

    self->ram_width = ram_width;
    self->ram_height = ram_height;

    self->area.x1 = 0;
    self->area.y1 = 0;
    self->area.next = NULL;

    self->transform.dx = 1;
    self->transform.dy = 1;
    if (self->transform.transpose_xy) {
        self->area.x2 = height;
        self->area.y2 = width;
        if (self->transform.mirror_x) {
            self->transform.x = height;
            self->transform.dx = -1;
        }
        if (self->transform.mirror_y) {
            self->transform.y = width;
            self->transform.dy = -1;
        }
    } else {
        self->area.x2 = width;
        self->area.y2 = height;
        if (self->transform.mirror_x) {
            self->transform.x = width;
            self->transform.dx = -1;
        }
        if (self->transform.mirror_y) {
            self->transform.y = height;
            self->transform.dy = -1;
        }
    }

    self->busy.base.type = &mp_type_NoneType;
    if (busy_pin != NULL) {
        self->busy.base.type = &digitalio_digitalinout_type;
        common_hal_digitalio_digitalinout_construct(&self->busy, busy_pin);
        never_reset_pin_number(busy_pin->number);
    }

    // Set the group after initialization otherwise we may send pixels while we delay in
    // initialization.
    common_hal_displayio_epaperdisplay_show(self, &circuitpython_splash);
}

bool common_hal_displayio_epaperdisplay_show(displayio_epaperdisplay_obj_t* self, displayio_group_t* root_group) {
    if (root_group == NULL && !circuitpython_splash.in_group) {
        root_group = &circuitpython_splash;
    }
    if (root_group == self->current_group) {
        return true;
    }
    if (root_group != NULL && root_group->in_group) {
        return false;
    }
    if (self->current_group != NULL) {
        self->current_group->in_group = false;
    }
    if (root_group != NULL) {
        displayio_group_update_transform(root_group, &self->transform);
        root_group->in_group = true;
        self->current_group = root_group;
    }
    self->full_refresh = true;
    common_hal_displayio_epaperdisplay_refresh_soon(self);
    return true;
}

void common_hal_displayio_epaperdisplay_refresh_soon(displayio_epaperdisplay_obj_t* self) {
    self->refresh = true;
}

const displayio_area_t* displayio_epaperdisplay_get_refresh_areas(displayio_epaperdisplay_obj_t *self) {
    const displayio_area_t* first_area;
    if (self->current_group == NULL || self->current_group->base.type != &displayio_group_type) {
        asm("bkpt");
    }
    if (self->full_refresh) {
        first_area = &self->area;
    } else {
        first_area = displayio_group_get_refresh_areas(self->current_group, NULL);
    }
    if (first_area != NULL && self->set_row_window_command == NO_COMMAND) {
        self->area.next = NULL;
        return &self->area;
    }
    return first_area;
}

int32_t common_hal_displayio_epaperdisplay_wait_for_frame(displayio_epaperdisplay_obj_t* self) {
    uint64_t last_refresh = self->last_refresh;
    // Don't try to refresh if we got an exception.
    while (last_refresh == self->last_refresh && MP_STATE_VM(mp_pending_exception) == NULL) {
        MICROPY_VM_HOOK_LOOP
    }
    return 0;
}

uint16_t common_hal_displayio_epaperdisplay_get_width(displayio_epaperdisplay_obj_t* self){
    return self->width;
}

uint16_t common_hal_displayio_epaperdisplay_get_height(displayio_epaperdisplay_obj_t* self){
    return self->height;
}

bool displayio_epaperdisplay_bus_free(displayio_epaperdisplay_obj_t *self) {
    return self->bus_free(self->bus);
}

bool displayio_epaperdisplay_begin_transaction(displayio_epaperdisplay_obj_t* self) {
    return self->begin_transaction(self->bus);
}

void displayio_epaperdisplay_end_transaction(displayio_epaperdisplay_obj_t* self) {
    self->end_transaction(self->bus);
}

STATIC void wait_for_busy(displayio_epaperdisplay_obj_t* self) {
    if (self->busy.base.type == &mp_type_NoneType) {
        return;
    }
    while (common_hal_digitalio_digitalinout_get_value(&self->busy) == self->busy_state) {
    #ifdef MICROPY_VM_HOOK_LOOP
        MICROPY_VM_HOOK_LOOP
    #endif
    }
}

STATIC void send_command_sequence(displayio_epaperdisplay_obj_t* self, bool should_wait_for_busy, uint8_t* sequence, uint32_t sequence_len) {
    uint32_t i = 0;
    while (i < sequence_len) {
        uint8_t *cmd = sequence + i;
        uint8_t data_size = *(cmd + 1);
        bool delay = (data_size & DELAY) != 0;
        data_size &= ~DELAY;
        uint8_t *data = cmd + 2;
        self->begin_transaction(self->bus);
        self->send(self->bus, true, self->always_toggle_chip_select, cmd, 1);
        self->send(self->bus, false, self->always_toggle_chip_select, data, data_size);
        self->end_transaction(self->bus);
        uint16_t delay_length_ms = 0;
        if (delay) {
            data_size++;
            delay_length_ms = *(cmd + 1 + data_size);
            if (delay_length_ms == 255) {
                delay_length_ms = 500;
            }
        }
        common_hal_time_delay_ms(delay_length_ms);
        if (should_wait_for_busy) {
            wait_for_busy(self);
        }
        i += 2 + data_size;
    }
}

void displayio_epaperdisplay_set_region_to_update(displayio_epaperdisplay_obj_t* self, displayio_area_t* area) {
    if (self->set_row_window_command == NO_COMMAND) {
        return;
    }
    uint16_t x1 = area->x1;
    uint16_t x2 = area->x2;
    uint16_t y1 = area->y1;
    uint16_t y2 = area->y2;
    // Collapse down the dimension where multiple pixels are in a byte.
    uint8_t pixels_per_byte = 8 / self->colorspace.depth;
    x1 /= pixels_per_byte * self->colorspace.bytes_per_cell;
    x2 /= pixels_per_byte * self->colorspace.bytes_per_cell;


    // Set column.
    uint8_t data[5];
    data[0] = self->set_column_window_command;
    self->send(self->bus, true, self->always_toggle_chip_select, data, 1);
    uint8_t data_length = 0;
    if (self->ram_width / pixels_per_byte < 0x100) {
        data[data_length++] = x1 + self->colstart;
        data[data_length++] = x2 - 1 + self->colstart;
    } else {
        x1 += self->colstart;
        x2 += self->colstart - 1;
        data[data_length++] = x1 >> 8;
        data[data_length++] = x1 & 0xff;
        data[data_length++] = x2 >> 8;
        data[data_length++] = x2 & 0xff;
    }
    self->send(self->bus, false, self->always_toggle_chip_select, data, data_length);
    if (self->set_current_column_command != NO_COMMAND) {
        uint8_t command = self->set_current_column_command;
        self->send(self->bus, true, self->always_toggle_chip_select, &command, 1);
        self->send(self->bus, false, self->always_toggle_chip_select, data, data_length / 2);
    }

    // Set row.
    data[0] = self->set_row_window_command;
    self->send(self->bus, true, self->always_toggle_chip_select, data, 1);
    data_length = 0;
    if (self->ram_height < 0x100) {
        data[data_length++] = y1 + self->rowstart;
        data[data_length++] = y2 - 1 + self->rowstart;
    } else {
        y1 += self->rowstart;
        y2 += self->rowstart - 1;
        data[data_length++] = y1 & 0xff;
        data[data_length++] = y1 >> 8;
        data[data_length++] = y2 & 0xff;
        data[data_length++] = y2 >> 8;
    }
    self->send(self->bus, false, self->always_toggle_chip_select, data, data_length);
    if (self->set_current_row_command != NO_COMMAND) {
        uint8_t command = self->set_current_row_command;
        self->send(self->bus, true, self->always_toggle_chip_select, &command, 1);
        self->send(self->bus, false, self->always_toggle_chip_select, data, data_length / 2);
    }
}

void displayio_epaperdisplay_start_refresh(displayio_epaperdisplay_obj_t* self) {
    // run start sequence
    self->bus_reset(self->bus);

    send_command_sequence(self, true, self->start_sequence, self->start_sequence_len);
    self->last_refresh = ticks_ms;
}

bool displayio_epaperdisplay_frame_queued(displayio_epaperdisplay_obj_t* self) {
    if (self->refreshing && self->busy.base.type == &digitalio_digitalinout_type) {
        if (common_hal_digitalio_digitalinout_get_value(&self->busy) != self->busy_state) {
            self->refreshing = false;
            // Run stop sequence but don't wait for busy because busy is set when sleeping.
            send_command_sequence(self, false, self->stop_sequence, self->stop_sequence_len);
        } else {
            return false;
        }
    }
    if (self->current_group == NULL) {
        return false;
    }
    // Refresh at seconds per frame rate.
    return (ticks_ms - self->last_refresh) > self->milliseconds_per_frame;
}

void displayio_epaperdisplay_finish_refresh(displayio_epaperdisplay_obj_t* self) {
    // Actually refresh the display now that all pixel RAM has been updated.
    displayio_epaperdisplay_begin_transaction(self);
    self->send(self->bus, true, self->always_toggle_chip_select, &self->refresh_display_command, 1);
    displayio_epaperdisplay_end_transaction(self);
    self->refreshing = true;

    if (self->current_group != NULL) {
        displayio_group_finish_refresh(self->current_group);
    }
    self->refresh = false;
    self->full_refresh = false;
    self->last_refresh = ticks_ms;
}

void displayio_epaperdisplay_send_pixels(displayio_epaperdisplay_obj_t* self, uint8_t* pixels, uint32_t length) {
}


bool displayio_epaperdisplay_refresh_area(displayio_epaperdisplay_obj_t* self, const displayio_area_t* area) {
    uint16_t buffer_size = 128; // In uint32_ts

    displayio_area_t clipped;
    // Clip the area to the display by overlapping the areas. If there is no overlap then we're done.
    if (!displayio_epaperdisplay_clip_area(self, area, &clipped)) {
        return true;
    }
    uint16_t subrectangles = 1;
    uint16_t rows_per_buffer = displayio_area_height(&clipped);
    uint8_t pixels_per_word = (sizeof(uint32_t) * 8) / self->colorspace.depth;
    uint16_t pixels_per_buffer = displayio_area_size(&clipped);
    if (displayio_area_size(&clipped) > buffer_size * pixels_per_word) {
        rows_per_buffer = buffer_size * pixels_per_word / displayio_area_width(&clipped);
        if (rows_per_buffer == 0) {
            rows_per_buffer = 1;
        }
        subrectangles = displayio_area_height(&clipped) / rows_per_buffer;
        if (displayio_area_height(&clipped) % rows_per_buffer != 0) {
            subrectangles++;
        }
        pixels_per_buffer = rows_per_buffer * displayio_area_width(&clipped);
        buffer_size = pixels_per_buffer / pixels_per_word;
        if (pixels_per_buffer % pixels_per_word) {
            buffer_size += 1;
        }
    }

    // Allocated and shared as a uint32_t array so the compiler knows the
    // alignment everywhere.
    uint32_t buffer[buffer_size];
    volatile uint32_t mask_length = (pixels_per_buffer / 32) + 1;
    uint32_t mask[mask_length];

    uint8_t passes = 1;
    if (self->colorspace.tricolor) {
        passes = 2;
    }
    for (uint8_t pass = 0; pass < passes; pass++) {
        uint16_t remaining_rows = displayio_area_height(&clipped);

        displayio_epaperdisplay_begin_transaction(self);
        displayio_epaperdisplay_set_region_to_update(self, &clipped);
        displayio_epaperdisplay_end_transaction(self);

        uint8_t write_command = self->write_black_ram_command;
        if (pass == 1) {
            write_command = self->write_color_ram_command;
        }
        displayio_epaperdisplay_begin_transaction(self);
        self->send(self->bus, true, self->always_toggle_chip_select, &write_command, 1);
        displayio_epaperdisplay_end_transaction(self);

        for (uint16_t j = 0; j < subrectangles; j++) {
            displayio_area_t subrectangle = {
                .x1 = clipped.x1,
                .y1 = clipped.y1 + rows_per_buffer * j,
                .x2 = clipped.x2,
                .y2 = clipped.y1 + rows_per_buffer * (j + 1)
            };
            if (remaining_rows < rows_per_buffer) {
                subrectangle.y2 = subrectangle.y1 + remaining_rows;
            }
            remaining_rows -= rows_per_buffer;


            uint16_t subrectangle_size_bytes = displayio_area_size(&subrectangle) / (8 / self->colorspace.depth);

            for (uint16_t k = 0; k < mask_length; k++) {
                mask[k] = 0x00000000;
            }
            for (uint16_t k = 0; k < buffer_size; k++) {
                buffer[k] = 0x00000000;
            }

            self->colorspace.grayscale = true;
            if (pass == 1) {
                self->colorspace.grayscale = false;
            }
            displayio_group_fill_area(self->current_group, &self->colorspace, &subrectangle, mask, buffer);

            // Invert it all.
            if ((pass == 1 && self->color_bits_inverted) ||
                (pass == 0 && self->black_bits_inverted)) {
                for (uint16_t k = 0; k < buffer_size; k++) {
                    buffer[k] = ~buffer[k];
                }
            }

            if (!displayio_epaperdisplay_begin_transaction(self)) {
                // Can't acquire display bus; skip the rest of the data. Try next display.
                return false;
            }
            self->send(self->bus, false, self->always_toggle_chip_select, (uint8_t*) buffer, subrectangle_size_bytes);
            displayio_epaperdisplay_end_transaction(self);

            // TODO(tannewt): Make refresh displays faster so we don't starve other
            // background tasks.
            usb_background();
        }
    }

    return true;
}

void release_epaperdisplay(displayio_epaperdisplay_obj_t* self) {
    if (self->current_group != NULL) {
        self->current_group->in_group = false;
    }
    if (self->busy.base.type == &digitalio_digitalinout_type) {
        common_hal_digitalio_digitalinout_deinit(&self->busy);
    }
}

bool displayio_epaperdisplay_fill_area(displayio_epaperdisplay_obj_t *self, displayio_area_t* area, uint32_t* mask, uint32_t *buffer) {
    return displayio_group_fill_area(self->current_group, &self->colorspace, area, mask, buffer);
}

bool displayio_epaperdisplay_clip_area(displayio_epaperdisplay_obj_t *self, const displayio_area_t* area, displayio_area_t* clipped) {
    bool overlaps = displayio_area_compute_overlap(&self->area, area, clipped);
    if (!overlaps) {
        return false;
    }
    // Expand the area if we have multiple pixels per byte and we need to byte
    // align the bounds.
    uint8_t pixels_per_byte = 8;
    if (clipped->x1 % pixels_per_byte != 0) {
        clipped->x1 -= clipped->x1 % pixels_per_byte;
    }
    if (clipped->x2 % pixels_per_byte != 0) {
        clipped->x2 += pixels_per_byte - clipped->x2 % pixels_per_byte;
    }
    return true;
}
