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

#include "py/gc.h"
#include "py/runtime.h"
#include "shared-bindings/displayio/FourWire.h"
#include "shared-bindings/displayio/I2CDisplay.h"
#include "shared-bindings/displayio/ParallelBus.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/time/__init__.h"
#include "shared-module/displayio/__init__.h"
#include "supervisor/shared/display.h"
#include "supervisor/shared/tick.h"

#include <stdint.h>
#include <string.h>

#define DISPLAYIO_CORE_DEBUG(...) (void)0
// #define DISPLAYIO_CORE_DEBUG(...) mp_printf(&mp_plat_print __VA_OPT__(,) __VA_ARGS__)

void displayio_display_core_construct(displayio_display_core_t *self,
    mp_obj_t bus, uint16_t width, uint16_t height, uint16_t ram_width, uint16_t ram_height, int16_t colstart, int16_t rowstart, uint16_t rotation,
    uint16_t color_depth, bool grayscale, bool pixels_in_byte_share_row, uint8_t bytes_per_cell, bool reverse_pixels_in_byte, bool reverse_bytes_in_word) {
    self->colorspace.depth = color_depth;
    self->colorspace.grayscale = grayscale;
    self->colorspace.grayscale_bit = 8 - color_depth;
    self->colorspace.pixels_in_byte_share_row = pixels_in_byte_share_row;
    self->colorspace.bytes_per_cell = bytes_per_cell;
    self->colorspace.reverse_pixels_in_byte = reverse_pixels_in_byte;
    self->colorspace.reverse_bytes_in_word = reverse_bytes_in_word;
    self->colorspace.dither = false;
    self->current_group = NULL;
    self->colstart = colstart;
    self->rowstart = rowstart;
    self->last_refresh = 0;

    // (framebufferdisplay already validated its 'bus' is a buffer-protocol object)
    if (bus) {
        if (mp_obj_is_type(bus, &displayio_parallelbus_type)) {
            self->bus_reset = common_hal_displayio_parallelbus_reset;
            self->bus_free = common_hal_displayio_parallelbus_bus_free;
            self->begin_transaction = common_hal_displayio_parallelbus_begin_transaction;
            self->send = common_hal_displayio_parallelbus_send;
            self->end_transaction = common_hal_displayio_parallelbus_end_transaction;
        } else if (mp_obj_is_type(bus, &displayio_fourwire_type)) {
            self->bus_reset = common_hal_displayio_fourwire_reset;
            self->bus_free = common_hal_displayio_fourwire_bus_free;
            self->begin_transaction = common_hal_displayio_fourwire_begin_transaction;
            self->send = common_hal_displayio_fourwire_send;
            self->end_transaction = common_hal_displayio_fourwire_end_transaction;
        } else if (mp_obj_is_type(bus, &displayio_i2cdisplay_type)) {
            self->bus_reset = common_hal_displayio_i2cdisplay_reset;
            self->bus_free = common_hal_displayio_i2cdisplay_bus_free;
            self->begin_transaction = common_hal_displayio_i2cdisplay_begin_transaction;
            self->send = common_hal_displayio_i2cdisplay_send;
            self->end_transaction = common_hal_displayio_i2cdisplay_end_transaction;
        } else {
            mp_raise_ValueError(translate("Unsupported display bus type"));
        }
    }
    self->bus = bus;


    // (offsetof core is equal in all display types)
    if (self == &displays[0].display.core) {
        supervisor_start_terminal(width, height);
    }

    self->width = width;
    self->height = height;
    self->ram_width = ram_width;
    self->ram_height = ram_height;

    displayio_display_core_set_rotation(self, rotation);
}

void displayio_display_core_set_rotation(displayio_display_core_t *self,
    int rotation) {
    int height = self->height;
    int width = self->width;

    rotation = rotation % 360;
    self->rotation = rotation;
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
}

bool displayio_display_core_show(displayio_display_core_t *self, displayio_group_t *root_group) {
    if (root_group == NULL) {
        if (!circuitpython_splash.in_group) {
            root_group = &circuitpython_splash;
        } else if (self->current_group == &circuitpython_splash) {
            return true;
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
    return true;
}

uint16_t displayio_display_core_get_width(displayio_display_core_t *self) {
    return self->width;
}

uint16_t displayio_display_core_get_height(displayio_display_core_t *self) {
    return self->height;
}

void displayio_display_core_set_dither(displayio_display_core_t *self, bool dither) {
    self->colorspace.dither = dither;
}

bool displayio_display_core_get_dither(displayio_display_core_t *self) {
    return self->colorspace.dither;
}

bool displayio_display_core_bus_free(displayio_display_core_t *self) {
    return !self->bus || self->bus_free(self->bus);
}

bool displayio_display_core_begin_transaction(displayio_display_core_t *self) {
    return self->begin_transaction(self->bus);
}

void displayio_display_core_end_transaction(displayio_display_core_t *self) {
    self->end_transaction(self->bus);
}

void displayio_display_core_set_region_to_update(displayio_display_core_t *self, uint8_t column_command,
    uint8_t row_command, uint16_t set_current_column_command, uint16_t set_current_row_command,
    bool data_as_commands, bool always_toggle_chip_select,
    displayio_area_t *area, bool SH1107_addressing) {
    uint16_t x1 = area->x1 + self->colstart;
    uint16_t x2 = area->x2 + self->colstart;
    uint16_t y1 = area->y1 + self->rowstart;
    uint16_t y2 = area->y2 + self->rowstart;

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

    x2 -= 1;
    y2 -= 1;

    display_chip_select_behavior_t chip_select = CHIP_SELECT_UNTOUCHED;
    if (always_toggle_chip_select || data_as_commands) {
        chip_select = CHIP_SELECT_TOGGLE_EVERY_BYTE;
    }

    // Set column.
    displayio_display_core_begin_transaction(self);
    uint8_t data[5];
    data[0] = column_command;
    uint8_t data_length = 1;
    display_byte_type_t data_type = DISPLAY_DATA;
    if (!data_as_commands) {
        self->send(self->bus, DISPLAY_COMMAND, CHIP_SELECT_UNTOUCHED, data, 1);
        data_length = 0;
    } else {
        data_type = DISPLAY_COMMAND;
    }

    if (self->ram_width < 0x100) {
        data[data_length++] = x1;
        data[data_length++] = x2;
    } else {
        data[data_length++] = x1 >> 8;
        data[data_length++] = x1 & 0xff;
        data[data_length++] = x2 >> 8;
        data[data_length++] = x2 & 0xff;
    }

    // Quirk for SH1107 "SH1107_addressing"
    //     Column lower command = 0x00, Column upper command = 0x10
    if (SH1107_addressing) {
        data[0] = ((x1 >> 4) & 0x0F) | 0x10; // 0x10 to 0x17
        data[1] = x1 & 0x0F; // 0x00 to 0x0F
        data_length = 2;
    }

    self->send(self->bus, data_type, chip_select, data, data_length);
    displayio_display_core_end_transaction(self);

    if (set_current_column_command != NO_COMMAND) {
        uint8_t command = set_current_column_command;
        displayio_display_core_begin_transaction(self);
        self->send(self->bus, DISPLAY_COMMAND, chip_select, &command, 1);
        self->send(self->bus, DISPLAY_DATA, chip_select, data, data_length / 2);
        displayio_display_core_end_transaction(self);
    }


    // Set row.
    displayio_display_core_begin_transaction(self);
    data[0] = row_command;
    data_length = 1;
    if (!data_as_commands) {
        self->send(self->bus, DISPLAY_COMMAND, CHIP_SELECT_UNTOUCHED, data, 1);
        data_length = 0;
    }

    if (self->ram_height < 0x100) {
        data[data_length++] = y1;
        data[data_length++] = y2;
    } else {
        data[data_length++] = y1 >> 8;
        data[data_length++] = y1 & 0xff;
        data[data_length++] = y2 >> 8;
        data[data_length++] = y2 & 0xff;
    }

    // Quirk for SH1107 "SH1107_addressing"
    //     Page address command = 0xB0
    if (SH1107_addressing) {
        // set the page to our y value
        data[0] = 0xB0 | y1;
        data_length = 1;
    }

    self->send(self->bus, data_type, chip_select, data, data_length);
    displayio_display_core_end_transaction(self);

    if (set_current_row_command != NO_COMMAND) {
        uint8_t command = set_current_row_command;
        displayio_display_core_begin_transaction(self);
        self->send(self->bus, DISPLAY_COMMAND, chip_select, &command, 1);
        self->send(self->bus, DISPLAY_DATA, chip_select, data, data_length / 2);
        displayio_display_core_end_transaction(self);
    }
}

bool displayio_display_core_start_refresh(displayio_display_core_t *self) {
    if (!displayio_display_core_bus_free(self)) {
        // Can't acquire display bus; skip updating this display. Try next display.
        return false;
    }
    if (self->refresh_in_progress) {
        return false;
    }
    self->refresh_in_progress = true;
    self->last_refresh = supervisor_ticks_ms64();
    return true;
}

void displayio_display_core_finish_refresh(displayio_display_core_t *self) {
    if (self->current_group != NULL) {
        DISPLAYIO_CORE_DEBUG("displayiocore group_finish_refresh\n");
        displayio_group_finish_refresh(self->current_group);
    }
    self->full_refresh = false;
    self->refresh_in_progress = false;
    self->last_refresh = supervisor_ticks_ms64();
}

void release_display_core(displayio_display_core_t *self) {
    if (self->current_group != NULL) {
        self->current_group->in_group = false;
    }
}

void displayio_display_core_collect_ptrs(displayio_display_core_t *self) {
    gc_collect_ptr(self->current_group);
}

bool displayio_display_core_fill_area(displayio_display_core_t *self, displayio_area_t *area, uint32_t *mask, uint32_t *buffer) {
    return displayio_group_fill_area(self->current_group, &self->colorspace, area, mask, buffer);
}

bool displayio_display_core_clip_area(displayio_display_core_t *self, const displayio_area_t *area, displayio_area_t *clipped) {
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
