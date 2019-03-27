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
    self->total_width = width * tile_width;
    self->total_height = height * tile_height;
    self->tile_width = tile_width;
    self->tile_height = tile_height;
    self->bitmap = bitmap;
    self->pixel_shader = pixel_shader;
    self->x = x;
    self->y = y;
}


mp_int_t common_hal_displayio_tilegrid_get_x(displayio_tilegrid_t *self) {
    return self->x;
}
void common_hal_displayio_tilegrid_set_x(displayio_tilegrid_t *self, mp_int_t x) {
    self->needs_refresh = self->x != x;
    self->x = x;
}
mp_int_t common_hal_displayio_tilegrid_get_y(displayio_tilegrid_t *self) {
    return self->y;
}

void common_hal_displayio_tilegrid_set_y(displayio_tilegrid_t *self, mp_int_t y) {
    self->needs_refresh = self->y != y;
    self->y = y;
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
}
bool displayio_tilegrid_get_pixel(displayio_tilegrid_t *self, int16_t x, int16_t y, uint16_t* pixel) {
    x -= self->x;
    y -= self->y;
    if (y < 0 || y >= self->total_height || x >= self->total_width || x < 0) {
        return false;
    }

    uint8_t* tiles = self->tiles;
    if (self->inline_tiles) {
        tiles = (uint8_t*) &self->tiles;
    }
    if (tiles == NULL) {
        return false;
    }
    uint16_t tile_location = ((y / self->tile_height + self->top_left_y) % self->height_in_tiles) * self->width_in_tiles + (x / self->tile_width + self->top_left_x) % self->width_in_tiles;
    uint8_t tile = tiles[tile_location];
    uint16_t tile_x = tile_x = (tile % self->bitmap_width_in_tiles) * self->tile_width + x % self->tile_width;
    uint16_t tile_y = tile_y = (tile / self->bitmap_width_in_tiles) * self->tile_height + y % self->tile_height;

    uint32_t value = 0;
    if (MP_OBJ_IS_TYPE(self->bitmap, &displayio_bitmap_type)) {
        value = common_hal_displayio_bitmap_get_pixel(self->bitmap, tile_x, tile_y);
    } else if (MP_OBJ_IS_TYPE(self->bitmap, &displayio_shape_type)) {
        value = common_hal_displayio_shape_get_pixel(self->bitmap, tile_x, tile_y);
    } else if (MP_OBJ_IS_TYPE(self->bitmap, &displayio_ondiskbitmap_type)) {
        value = common_hal_displayio_ondiskbitmap_get_pixel(self->bitmap, tile_x, tile_y);
    }

    if (self->pixel_shader == mp_const_none) {
        *pixel = value;
        return true;
    } else if (MP_OBJ_IS_TYPE(self->pixel_shader, &displayio_palette_type) && displayio_palette_get_color(self->pixel_shader, value, pixel)) {
        return true;
    } else if (MP_OBJ_IS_TYPE(self->pixel_shader, &displayio_colorconverter_type) && common_hal_displayio_colorconverter_convert(self->pixel_shader, value, pixel)) {
        return true;
    }

    return false;
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
