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
#include "shared-bindings/displayio/I2CDisplay.h"
#include "shared-bindings/displayio/ParallelBus.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/time/__init__.h"
#include "shared-module/displayio/__init__.h"
#include "supervisor/shared/display.h"

#include <stdint.h>
#include <string.h>

#include "tick.h"

void common_hal_displayio_display_construct(displayio_display_obj_t* self,
        mp_obj_t bus, uint16_t width, uint16_t height, int16_t colstart, int16_t rowstart, uint16_t rotation,
        uint16_t color_depth, bool grayscale, bool pixels_in_byte_share_row, uint8_t bytes_per_cell, bool reverse_pixels_in_byte,
        uint8_t set_column_command, uint8_t set_row_command,
        uint8_t write_ram_command, uint8_t set_vertical_scroll, uint8_t* init_sequence, uint16_t init_sequence_len,
        const mcu_pin_obj_t* backlight_pin, uint16_t brightness_command, mp_float_t brightness, bool auto_brightness,
        bool single_byte_bounds, bool data_as_commands) {
    self->colorspace.depth = color_depth;
    self->colorspace.grayscale = grayscale;
    self->colorspace.pixels_in_byte_share_row = pixels_in_byte_share_row;
    self->colorspace.bytes_per_cell = bytes_per_cell;
    self->colorspace.reverse_pixels_in_byte = reverse_pixels_in_byte;
    self->set_column_command = set_column_command;
    self->set_row_command = set_row_command;
    self->write_ram_command = write_ram_command;
    self->refresh = false;
    self->current_group = NULL;
    self->colstart = colstart;
    self->rowstart = rowstart;
    self->brightness_command = brightness_command;
    self->auto_brightness = auto_brightness;
    self->data_as_commands = data_as_commands;
    self->single_byte_bounds = single_byte_bounds;

    if (MP_OBJ_IS_TYPE(bus, &displayio_parallelbus_type)) {
        self->begin_transaction = common_hal_displayio_parallelbus_begin_transaction;
        self->send = common_hal_displayio_parallelbus_send;
        self->end_transaction = common_hal_displayio_parallelbus_end_transaction;
    } else if (MP_OBJ_IS_TYPE(bus, &displayio_fourwire_type)) {
        self->begin_transaction = common_hal_displayio_fourwire_begin_transaction;
        self->send = common_hal_displayio_fourwire_send;
        self->end_transaction = common_hal_displayio_fourwire_end_transaction;
    } else if (MP_OBJ_IS_TYPE(bus, &displayio_i2cdisplay_type)) {
        self->begin_transaction = common_hal_displayio_i2cdisplay_begin_transaction;
        self->send = common_hal_displayio_i2cdisplay_send;
        self->end_transaction = common_hal_displayio_i2cdisplay_end_transaction;
    } else {
        mp_raise_ValueError(translate("Unsupported display bus type"));
    }
    self->bus = bus;

    uint32_t i = 0;
    while (!self->begin_transaction(self->bus)) {
        RUN_BACKGROUND_TASKS;
    }
    while (i < init_sequence_len) {
        uint8_t *cmd = init_sequence + i;
        uint8_t data_size = *(cmd + 1);
        bool delay = (data_size & DELAY) != 0;
        data_size &= ~DELAY;
        uint8_t *data = cmd + 2;
        if (self->data_as_commands) {
            uint8_t full_command[data_size + 1];
            full_command[0] = cmd[0];
            memcpy(full_command + 1, data, data_size);
            self->send(self->bus, true, true, full_command, data_size + 1);
        } else {
            self->send(self->bus, true, true, cmd, 1);
            self->send(self->bus, false, false, data, data_size);
        }
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

    self->width = width;
    self->height = height;
    self->rotation = rotation % 360;
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

    // Always set the backlight type in case we're reusing memory.
    self->backlight_inout.base.type = &mp_type_NoneType;
    if (backlight_pin != NULL && common_hal_mcu_pin_is_free(backlight_pin)) {
        pwmout_result_t result = common_hal_pulseio_pwmout_construct(&self->backlight_pwm, backlight_pin, 0, 50000, false);
        if (result != PWMOUT_OK) {
            self->backlight_inout.base.type = &digitalio_digitalinout_type;
            common_hal_digitalio_digitalinout_construct(&self->backlight_inout, backlight_pin);
            never_reset_pin_number(backlight_pin->number);
        } else {
            self->backlight_pwm.base.type = &pulseio_pwmout_type;
            common_hal_pulseio_pwmout_never_reset(&self->backlight_pwm);
        }
    }
    if (!self->auto_brightness && (self->backlight_inout.base.type != &mp_type_NoneType ||
                                   brightness_command != NO_BRIGHTNESS_COMMAND)) {
        common_hal_displayio_display_set_brightness(self, brightness);
    } else {
        self->current_brightness = -1.0;
    }

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

    // Set the group after initialization otherwise we may send pixels while we delay in
    // initialization.
    common_hal_displayio_display_show(self, &circuitpython_splash);
}

bool common_hal_displayio_display_show(displayio_display_obj_t* self, displayio_group_t* root_group) {
    if (root_group == NULL) {
        if (!circuitpython_splash.in_group) {
            root_group = &circuitpython_splash;
        } else if (self->current_group == &circuitpython_splash) {
            return false;
        }
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
    }
    self->current_group = root_group;
    self->full_refresh = true;
    common_hal_displayio_display_refresh_soon(self);
    return true;
}

const displayio_area_t* displayio_display_get_refresh_areas(displayio_display_obj_t *self) {
    if (self->full_refresh) {
        self->area.next = NULL;
        return &self->area;
    } else {
        if (self->current_group == NULL || self->current_group->base.type != &displayio_group_type) {
            asm("bkpt");
        }
        return displayio_group_get_refresh_areas(self->current_group, NULL);
    }
}

int32_t common_hal_displayio_display_wait_for_frame(displayio_display_obj_t* self) {
    uint64_t last_refresh = self->last_refresh;
    // Don't try to refresh if we got an exception.
    while (last_refresh == self->last_refresh && MP_STATE_VM(mp_pending_exception) == NULL) {
        RUN_BACKGROUND_TASKS;
    }
    return 0;
}

STATIC bool refresh_area(displayio_display_obj_t* display, const displayio_area_t* area) {
    uint16_t buffer_size = 128; // In uint32_ts

    displayio_area_t clipped;
    // Clip the area to the display by overlapping the areas. If there is no overlap then we're done.
    if (!displayio_display_clip_area(display, area, &clipped)) {
        return true;
    }
    uint16_t subrectangles = 1;
    uint16_t rows_per_buffer = displayio_area_height(&clipped);
    uint8_t pixels_per_word = (sizeof(uint32_t) * 8) / display->colorspace.depth;
    uint16_t pixels_per_buffer = displayio_area_size(&clipped);
    if (displayio_area_size(&clipped) > buffer_size * pixels_per_word) {
        rows_per_buffer = buffer_size * pixels_per_word / displayio_area_width(&clipped);
        if (rows_per_buffer == 0) {
            rows_per_buffer = 1;
        }
        // If pixels are packed by column then ensure rows_per_buffer is on a byte boundary.
        if (display->colorspace.depth < 8 && !display->colorspace.pixels_in_byte_share_row) {
            uint8_t pixels_per_byte = 8 / display->colorspace.depth;
            if (rows_per_buffer % pixels_per_byte != 0) {
                rows_per_buffer -= rows_per_buffer % pixels_per_byte;
            }
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
    uint16_t remaining_rows = displayio_area_height(&clipped);

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

        displayio_display_begin_transaction(display);
        displayio_display_set_region_to_update(display, &subrectangle);
        displayio_display_end_transaction(display);

        uint16_t subrectangle_size_bytes;
        if (display->colorspace.depth >= 8) {
            subrectangle_size_bytes = displayio_area_size(&subrectangle) * (display->colorspace.depth / 8);
        } else {
            subrectangle_size_bytes = displayio_area_size(&subrectangle) / (8 / display->colorspace.depth);
        }

        for (uint16_t k = 0; k < mask_length; k++) {
            mask[k] = 0x00000000;
        }
        for (uint16_t k = 0; k < buffer_size; k++) {
            buffer[k] = 0x00000000;
        }

        displayio_display_fill_area(display, &subrectangle, mask, buffer);

        if (!displayio_display_begin_transaction(display)) {
            // Can't acquire display bus; skip the rest of the data. Try next display.
            return false;
        }
        displayio_display_send_pixels(display, (uint8_t*) buffer, subrectangle_size_bytes);
        displayio_display_end_transaction(display);

        // TODO(tannewt): Make refresh displays faster so we don't starve other
        // background tasks.
        usb_background();
    }
    return true;
}

STATIC void refresh_display(displayio_display_obj_t* self) {
    if (!displayio_display_begin_transaction(self)) {
        // Can't acquire display bus; skip updating this display. Try next display.
        continue;
    }
    displayio_display_end_transaction(self);
    displayio_display_start_refresh(self);
    const displayio_area_t* current_area = displayio_display_get_refresh_areas(self);
    while (current_area != NULL) {
        refresh_area(self, current_area);
        current_area = current_area->next;
    }
    displayio_display_finish_refresh(self);
}

void common_hal_displayio_display_refresh(displayio_display_obj_t* self) {
    // Time to refresh at specified frame rate?
    while (!displayio_display_frame_queued(self)) {
        // Too soon. Try next display.
        continue;
    }
    refresh_display(self);
}

bool common_hal_displayio_display_get_auto_brightness(displayio_display_obj_t* self) {
    return self->auto_brightness;
}

uint16_t common_hal_displayio_display_get_width(displayio_display_obj_t* self){
    return self->width;
}

uint16_t common_hal_displayio_display_get_height(displayio_display_obj_t* self){
    return self->height;
}

uint16_t common_hal_displayio_display_get_rotation(displayio_display_obj_t* self){
    return self->rotation;
}

void common_hal_displayio_display_set_auto_brightness(displayio_display_obj_t* self, bool auto_brightness) {
    self->auto_brightness = auto_brightness;
}

mp_float_t common_hal_displayio_display_get_brightness(displayio_display_obj_t* self) {
    return self->current_brightness;
}

bool common_hal_displayio_display_set_brightness(displayio_display_obj_t* self, mp_float_t brightness) {
    self->updating_backlight = true;
    bool ok = false;
    if (self->backlight_pwm.base.type == &pulseio_pwmout_type) {
        common_hal_pulseio_pwmout_set_duty_cycle(&self->backlight_pwm, (uint16_t) (0xffff * brightness));
        ok = true;
    } else if (self->backlight_inout.base.type == &digitalio_digitalinout_type) {
        common_hal_digitalio_digitalinout_set_value(&self->backlight_inout, brightness > 0.99);
        ok = true;
    } else if (self->brightness_command != NO_BRIGHTNESS_COMMAND) {
        ok = self->begin_transaction(self->bus);
        if (ok) {
            if (self->data_as_commands) {
                uint8_t set_brightness[2] = {self->brightness_command, (uint8_t) (0xff * brightness)};
                self->send(self->bus, true, true, set_brightness, 2);
            } else {
                uint8_t command = self->brightness_command;
                uint8_t hex_brightness = 0xff * brightness;
                self->send(self->bus, true, true, &command, 1);
                self->send(self->bus, false, false, &hex_brightness, 1);
            }
            self->end_transaction(self->bus);
        }

    }
    self->updating_backlight = false;
    if (ok) {
        self->current_brightness = brightness;
    }
    return ok;
}

bool displayio_display_begin_transaction(displayio_display_obj_t* self) {
    return self->begin_transaction(self->bus);
}

void displayio_display_end_transaction(displayio_display_obj_t* self) {
    self->end_transaction(self->bus);
}

void displayio_display_set_region_to_update(displayio_display_obj_t* self, displayio_area_t* area) {
    uint16_t x1 = area->x1;
    uint16_t x2 = area->x2;
    uint16_t y1 = area->y1;
    uint16_t y2 = area->y2;
    // Collapse down the dimension where multiple pixels are in a byte.
    if (self->colorspace.depth < 8) {
        uint8_t pixels_per_byte = 8 / self->colorspace.depth;
        if (self->colorspace.pixels_in_byte_share_row) {
            x1 /= pixels_per_byte * self->colorspace.bytes_per_cell;
            x2 /= pixels_per_byte * self->colorspace.bytes_per_cell;
        } else {
            y1 /= pixels_per_byte * self->colorspace.bytes_per_cell;
            y2 /= pixels_per_byte * self->colorspace.bytes_per_cell;
        }
    }

    // Set column.
    uint8_t data[5];
    data[0] = self->set_column_command;
    uint8_t data_length = 1;
    if (!self->data_as_commands) {
        self->send(self->bus, true, true, data, 1);
        data_length = 0;
    }
    if (self->single_byte_bounds) {
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
    self->send(self->bus, self->data_as_commands, self->data_as_commands, data, data_length);

    // Set row.
    data[0] = self->set_row_command;
    data_length = 1;
    if (!self->data_as_commands) {
        self->send(self->bus, true, true, data, 1);
        data_length = 0;
    }
    if (self->single_byte_bounds) {
        data[data_length++] = y1 + self->rowstart;
        data[data_length++] = y2 - 1 + self->rowstart;
    } else {
        y1 += self->rowstart;
        y2 += self->rowstart - 1;
        data[data_length++] = y1 >> 8;
        data[data_length++] = y1 & 0xff;
        data[data_length++] = y2 >> 8;
        data[data_length++] = y2 & 0xff;
    }
    self->send(self->bus, self->data_as_commands, self->data_as_commands, data, data_length);
}

void displayio_display_start_refresh(displayio_display_obj_t* self) {
    self->last_refresh = ticks_ms;
}

bool displayio_display_frame_queued(displayio_display_obj_t* self) {
    if (self->current_group == NULL) {
        return false;
    }
    // Refresh at ~60 fps.
    return (ticks_ms - self->last_refresh) > 16;
}

void displayio_display_finish_refresh(displayio_display_obj_t* self) {
    if (self->current_group != NULL) {
        displayio_group_finish_refresh(self->current_group);
    }
    self->refresh = false;
    self->full_refresh = false;
    self->last_refresh = ticks_ms;
}

void displayio_display_send_pixels(displayio_display_obj_t* self, uint8_t* pixels, uint32_t length) {
    if (!self->data_as_commands) {
        self->send(self->bus, true, true, &self->write_ram_command, 1);
    }
    self->send(self->bus, false, false, pixels, length);
}

void displayio_display_update_backlight(displayio_display_obj_t* self) {
    if (!self->auto_brightness || self->updating_backlight) {
        return;
    }
    if (ticks_ms - self->last_backlight_refresh < 100) {
        return;
    }
    // TODO(tannewt): Fade the backlight based on it's existing value and a target value. The target
    // should account for ambient light when possible.
    common_hal_displayio_display_set_brightness(self, 1.0);

    self->last_backlight_refresh = ticks_ms;
}

void displayio_display_background(displayio_display_obj_t* self) {
    displayio_display_update_backlight(self);

    if (self->auto_refresh && (ticks_ms - self->last_refresh) > 16) {
        display_refresh(self);
    }
}

void release_display(displayio_display_obj_t* self) {
    if (self->current_group != NULL) {
        self->current_group->in_group = false;
    }
    if (self->backlight_pwm.base.type == &pulseio_pwmout_type) {
        common_hal_pulseio_pwmout_reset_ok(&self->backlight_pwm);
        common_hal_pulseio_pwmout_deinit(&self->backlight_pwm);
    } else if (self->backlight_inout.base.type == &digitalio_digitalinout_type) {
        common_hal_digitalio_digitalinout_deinit(&self->backlight_inout);
    }
}

bool displayio_display_fill_area(displayio_display_obj_t *self, displayio_area_t* area, uint32_t* mask, uint32_t *buffer) {
    return displayio_group_fill_area(self->current_group, &self->colorspace, area, mask, buffer);
}

bool displayio_display_clip_area(displayio_display_obj_t *self, const displayio_area_t* area, displayio_area_t* clipped) {
    bool overlaps = displayio_area_compute_overlap(&self->area, area, clipped);
    if (!overlaps) {
        return false;
    }
    // Expand the area if we have multiple pixels per byte and we need to byte
    // align the bounds.
    if (self->colorspace.depth < 8) {
        uint8_t pixels_per_byte = 8 / self->colorspace.depth * self->colorspace.bytes_per_cell;
        if (self->colorspace.pixels_in_byte_share_row) {
            if (clipped->x1 % pixels_per_byte != 0) {
                clipped->x1 -= clipped->x1 % pixels_per_byte;
            }
            if (clipped->x2 % pixels_per_byte != 0) {
                clipped->x2 += pixels_per_byte - clipped->x2 % pixels_per_byte;
            }
        } else {
            if (clipped->y1 % pixels_per_byte != 0) {
                clipped->y1 -= clipped->y1 % pixels_per_byte;
            }
            if (clipped->y2 % pixels_per_byte != 0) {
                clipped->y2 += pixels_per_byte - clipped->y2 % pixels_per_byte;
            }
        }
    }
    return true;
}
