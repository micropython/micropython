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

#include "shared-module/displayio/display_core.h"

#include "py/gc.h"
#include "py/runtime.h"
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
    uint16_t width, uint16_t height, uint16_t rotation,
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
    self->last_refresh = 0;

    supervisor_start_terminal(width, height);

    self->width = width;
    self->height = height;

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

bool displayio_display_core_set_root_group(displayio_display_core_t *self, displayio_group_t *root_group) {

    if (root_group == NULL) {
        // Show nothing on the display
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

bool displayio_display_core_start_refresh(displayio_display_core_t *self) {
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
    if (self->current_group != NULL) {
        return displayio_group_fill_area(self->current_group, &self->colorspace, area, mask, buffer);
    }
    return false;
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
