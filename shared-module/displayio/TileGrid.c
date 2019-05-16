/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include "shared-bindings/displayio/TileGrid.h"

#include "shared-bindings/displayio/Bitmap.h"
#include "shared-bindings/displayio/ColorConverter.h"
#include "shared-bindings/displayio/OnDiskBitmap.h"
#include "shared-bindings/displayio/Palette.h"
#include "shared-bindings/displayio/Shape.h"

void common_hal_displayio_tilegrid_construct(displayio_tilegrid_t *self, mp_obj_t bitmap,
        uint16_t bitmap_width_in_tiles,
        mp_obj_t pixel_shader, uint16_t width, uint16_t height,
        uint16_t tile_width, uint16_t tile_height, uint16_t x, uint16_t y, uint8_t default_tile) {
    uint32_t total_tiles = width * height;
    // Sprites will only have one tile so save a little memory by inlining values in the pointer.
    uint8_t inline_tiles = sizeof(uint8_t*);
    if (total_tiles <= inline_tiles) {
        self->tiles = 0;
        // Pack values into the pointer since there are only a few.
        for (uint32_t i = 0; i < inline_tiles; i++) {
            ((uint8_t*) &self->tiles)[i] = default_tile;
        }
        self->inline_tiles = true;
    } else {
        self->tiles = (uint8_t*) m_malloc(total_tiles, false);
        for (uint32_t i = 0; i < total_tiles; i++) {
            self->tiles[i] = default_tile;
        }
        self->inline_tiles = false;
    }
    self->bitmap_width_in_tiles = bitmap_width_in_tiles;
    self->width_in_tiles = width;
    self->height_in_tiles = height;
    self->area.x1 = x;
    self->area.y1 = y;
    // -1 because areas are inclusive
    self->area.x2 = x + width * tile_width - 1;
    self->area.y2 = y + height * tile_height - 1;
    self->tile_width = tile_width;
    self->tile_height = tile_height;
    self->bitmap = bitmap;
    self->pixel_shader = pixel_shader;
}


mp_int_t common_hal_displayio_tilegrid_get_x(displayio_tilegrid_t *self) {
    return self->area.x1;
}
void common_hal_displayio_tilegrid_set_x(displayio_tilegrid_t *self, mp_int_t x) {
    if (self->area.x1 == x) {
        return;
    }
    self->needs_refresh = true;
    self->area.x2 += (self->area.x1 - x);
    self->area.x1 = x;
}
mp_int_t common_hal_displayio_tilegrid_get_y(displayio_tilegrid_t *self) {
    return self->area.y1;
}

void common_hal_displayio_tilegrid_set_y(displayio_tilegrid_t *self, mp_int_t y) {
    if (self->area.y1 == y) {
        return;
    }
    self->needs_refresh = true;
    self->area.y2 += (self->area.y1 - y);
    self->area.y1 = y;
}

mp_obj_t common_hal_displayio_tilegrid_get_pixel_shader(displayio_tilegrid_t *self) {
    return self->pixel_shader;
}

void common_hal_displayio_tilegrid_set_pixel_shader(displayio_tilegrid_t *self, mp_obj_t pixel_shader) {
    self->pixel_shader = pixel_shader;
    self->needs_refresh = true;
}


uint16_t common_hal_displayio_tilegrid_get_width(displayio_tilegrid_t *self) {
    return self->width_in_tiles;
}

uint16_t common_hal_displayio_tilegrid_get_height(displayio_tilegrid_t *self) {
    return self->height_in_tiles;
}

uint8_t common_hal_displayio_tilegrid_get_tile(displayio_tilegrid_t *self, uint16_t x, uint16_t y) {
    uint8_t* tiles = self->tiles;
    if (self->inline_tiles) {
        tiles = (uint8_t*) &self->tiles;
    }
    if (tiles == NULL) {
        return 0;
    }
    return tiles[y * self->width_in_tiles + x];
}

void common_hal_displayio_tilegrid_set_tile(displayio_tilegrid_t *self, uint16_t x, uint16_t y, uint8_t tile_index) {
    uint8_t* tiles = self->tiles;
    if (self->inline_tiles) {
        tiles = (uint8_t*) &self->tiles;
    }
    if (tiles == NULL) {
        return;
    }
    tiles[y * self->width_in_tiles + x] = tile_index;
    self->needs_refresh = true;
}


void common_hal_displayio_tilegrid_set_top_left(displayio_tilegrid_t *self, uint16_t x, uint16_t y) {
    self->top_left_x = x;
    self->top_left_y = y;
    self->needs_refresh = true;
}

bool displayio_tilegrid_get_area(displayio_tilegrid_t *self, displayio_buffer_transform_t* transform, displayio_area_t* area, uint32_t* mask, uint32_t *buffer) {
    // If no tiles are present we have no impact.
    uint8_t* tiles = self->tiles;
    if (self->inline_tiles) {
        tiles = (uint8_t*) &self->tiles;
    }
    if (tiles == NULL) {
        return false;
    }

    displayio_area_t overlap;
    displayio_area_t scaled_area = {
        .x1 = self->area.x1 * transform->scale,
        .y1 = self->area.y1 * transform->scale,
        .x2 = (self->area.x2 + 1) * transform->scale - 1, // Second point is inclusive.
        .y2 = (self->area.y2 + 1) * transform->scale - 1
    };
    if (!displayio_area_compute_overlap(area, &scaled_area, &overlap)) {
        return false;
    }

    int16_t x_stride = 1;
    int16_t y_stride = displayio_area_width(area);
    if (transform->transpose_xy) {
        x_stride = displayio_area_height(area);
        y_stride = 1;
    }
    uint16_t start = 0;
    if (transform->mirror_x) {
        start += (area->x2 - area->x1) * x_stride;
        x_stride *= -1;
    }
    if (transform->mirror_y) {
        start += (area->y2 - area->y1) * y_stride;
        y_stride *= -1;
    }

    bool full_coverage = displayio_area_equal(area, &overlap);

    // TODO(tannewt): Set full coverage to true if all pixels outside the overlap have already been
    // set as well.
    bool always_full_coverage = false;

    // TODO(tannewt): Check to see if the pixel_shader has any transparency. If it doesn't then we
    // can either return full coverage or bulk update the mask.
    int16_t y = overlap.y1 - scaled_area.y1;
    if (y < 0) {
        y = 0;
    }
    int16_t x_shift = area->x1 - scaled_area.x1;
    int16_t y_shift = area->y1 - scaled_area.y1;
    for (; y <= overlap.y2 - scaled_area.y1; y++) {
        int16_t x = overlap.x1 - scaled_area.x1;
        if (x < 0) {
            x = 0;
        }
        int16_t row_start = start + (y - y_shift) * y_stride;
        int16_t local_y = y / transform->scale;
        for (; x <= overlap.x2 - scaled_area.x1; x++) {
            // Compute the destination pixel in the buffer and mask based on the transformations.
            uint16_t offset = row_start + (x - x_shift) * x_stride;

            // Check the mask first to see if the pixel has already been set.
            if ((mask[offset / 32] & (1 << (offset % 32))) != 0) {
                continue;
            }
            int16_t local_x = x / transform->scale;
            uint16_t tile_location = ((local_y / self->tile_height + self->top_left_y) % self->height_in_tiles) * self->width_in_tiles + (local_x / self->tile_width + self->top_left_x) % self->width_in_tiles;
            uint8_t tile = tiles[tile_location];
            uint16_t tile_x = (tile % self->bitmap_width_in_tiles) * self->tile_width + local_x % self->tile_width;
            uint16_t tile_y = (tile / self->bitmap_width_in_tiles) * self->tile_height + local_y % self->tile_height;

            uint32_t value = 0;
            // We always want to read bitmap pixels by row first and then transpose into the destination
            // buffer because most bitmaps are row associated.
            if (MP_OBJ_IS_TYPE(self->bitmap, &displayio_bitmap_type)) {
                value = common_hal_displayio_bitmap_get_pixel(self->bitmap, tile_x, tile_y);
            } else if (MP_OBJ_IS_TYPE(self->bitmap, &displayio_shape_type)) {
                value = common_hal_displayio_shape_get_pixel(self->bitmap, tile_x, tile_y);
            } else if (MP_OBJ_IS_TYPE(self->bitmap, &displayio_ondiskbitmap_type)) {
                value = common_hal_displayio_ondiskbitmap_get_pixel(self->bitmap, tile_x, tile_y);
            }

            uint16_t* pixel = ((uint16_t*) buffer) + offset;
            if (self->pixel_shader == mp_const_none) {
                *pixel = value;
                return true;
            } else if (MP_OBJ_IS_TYPE(self->pixel_shader, &displayio_palette_type)) {
                if (!displayio_palette_get_color(self->pixel_shader, value, pixel)) {
                    // mark the pixel as transparent
                    full_coverage = false;
                } else if (!always_full_coverage) {
                    mask[offset / 32] |= 1 << (offset % 32);
                }
            } else if (MP_OBJ_IS_TYPE(self->pixel_shader, &displayio_colorconverter_type)) {
                if (!common_hal_displayio_colorconverter_convert(self->pixel_shader, value, pixel)) {
                    // mark the pixel as transparent
                    full_coverage = false;
                } else if (!always_full_coverage) {
                    mask[offset / 32] |= 1 << (offset % 32);
                }
            }
        }
    }

    return full_coverage;
}

bool displayio_tilegrid_needs_refresh(displayio_tilegrid_t *self) {
    if (self->needs_refresh) {
        return true;
    } else if (MP_OBJ_IS_TYPE(self->pixel_shader, &displayio_palette_type)) {
        return displayio_palette_needs_refresh(self->pixel_shader);
    } else if (MP_OBJ_IS_TYPE(self->pixel_shader, &displayio_colorconverter_type)) {
        return displayio_colorconverter_needs_refresh(self->pixel_shader);
    }

    return false;
}

void displayio_tilegrid_finish_refresh(displayio_tilegrid_t *self) {
    self->needs_refresh = false;
    if (MP_OBJ_IS_TYPE(self->pixel_shader, &displayio_palette_type)) {
        displayio_palette_finish_refresh(self->pixel_shader);
    } else if (MP_OBJ_IS_TYPE(self->pixel_shader, &displayio_colorconverter_type)) {
        displayio_colorconverter_finish_refresh(self->pixel_shader);
    }
    // TODO(tannewt): We could double buffer changes to position and move them over here.
    // That way they won't change during a refresh and tear.
}
