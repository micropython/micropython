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

#include "supervisor/shared/display.h"

#include <string.h>

#include "py/mpstate.h"
#include "shared-bindings/displayio/Group.h"
#include "shared-bindings/displayio/Palette.h"
#include "shared-bindings/displayio/TileGrid.h"
#include "supervisor/memory.h"

extern size_t blinka_bitmap_data[];
extern displayio_bitmap_t blinka_bitmap;
extern displayio_group_t circuitpython_splash;

static supervisor_allocation* tilegrid_tiles = NULL;

void supervisor_start_terminal(uint16_t width_px, uint16_t height_px) {
    displayio_tilegrid_t* grid = &supervisor_terminal_text_grid;
    uint16_t width_in_tiles = (width_px - blinka_bitmap.width) / grid->tile_width;
    // determine scale based on h
    uint8_t scale = 1;
    if (width_in_tiles > 80) {
        scale = 2;
    }
    width_in_tiles = (width_px - blinka_bitmap.width * scale) / (grid->tile_width * scale);
    uint16_t height_in_tiles = height_px / (grid->tile_height * scale);
    circuitpython_splash.scale = scale;

    uint16_t total_tiles = width_in_tiles * height_in_tiles;

    // First try to allocate outside the heap. This will fail when the VM is running.
    tilegrid_tiles = allocate_memory(align32_size(total_tiles), false);
    uint8_t* tiles;
    if (tilegrid_tiles == NULL) {
        tiles = m_malloc(total_tiles, true);
        MP_STATE_VM(terminal_tilegrid_tiles) = tiles;
    } else {
        tiles = (uint8_t*) tilegrid_tiles->ptr;
    }

    if (tiles == NULL) {
        return;
    }

    grid->width_in_tiles = width_in_tiles;
    grid->height_in_tiles = height_in_tiles;
    grid->total_width = width_in_tiles * grid->tile_width;
    grid->total_height = height_in_tiles * grid->tile_height;
    grid->tiles = tiles;

    supervisor_terminal.cursor_x = 0;
    supervisor_terminal.cursor_y = 0;
}

void supervisor_stop_terminal(void) {
    if (tilegrid_tiles != NULL) {
        free_memory(tilegrid_tiles);
        tilegrid_tiles = NULL;
        supervisor_terminal_text_grid.inline_tiles = false;
        supervisor_terminal_text_grid.tiles = NULL;
    }
}

void supervisor_display_move_memory(void) {
    #if CIRCUITPY_DISPLAYIO
    displayio_tilegrid_t* grid = &supervisor_terminal_text_grid;
    if (MP_STATE_VM(terminal_tilegrid_tiles) == NULL || grid->tiles != MP_STATE_VM(terminal_tilegrid_tiles)) {
        return;
    }
    uint16_t total_tiles = grid->width_in_tiles * grid->height_in_tiles;

    tilegrid_tiles = allocate_memory(align32_size(total_tiles), false);
    if (tilegrid_tiles != NULL) {
        memcpy(tilegrid_tiles->ptr, grid->tiles, total_tiles);
        grid->tiles = (uint8_t*) tilegrid_tiles->ptr;
    } else {
        grid->tiles = NULL;
        grid->inline_tiles = false;
    }
    MP_STATE_VM(terminal_tilegrid_tiles) = NULL;
    #endif
}

size_t blinka_bitmap_data[32] = {
    0x00000011, 0x11000000,
    0x00000111, 0x53100000,
    0x00000111, 0x56110000,
    0x00000111, 0x11140000,
    0x00000111, 0x20002000,
    0x00000011, 0x13000000,
    0x00000001, 0x11200000,
    0x00000000, 0x11330000,
    0x00000000, 0x01122000,
    0x00001111, 0x44133000,
    0x00032323, 0x24112200,
    0x00111114, 0x44113300,
    0x00323232, 0x34112200,
    0x11111144, 0x44443300,
    0x11111111, 0x11144401,
    0x23232323, 0x21111110
};

displayio_bitmap_t blinka_bitmap = {
    .base = {.type = &displayio_bitmap_type },
    .width = 16,
    .height = 16,
    .data = blinka_bitmap_data,
    .stride = 2,
    .bits_per_value = 4,
    .x_shift = 3,
    .x_mask = 0x7,
    .bitmask = 0xf,
    .read_only = true
};

uint32_t blinka_transparency[1] = {0x80000000};

// These colors are RGB 565 with the bytes swapped.
uint32_t blinka_colors[8] = {0x78890000, 0x9F86B8FC, 0xffff0D5A, 0x0000f501,
                             0x00000000, 0x00000000, 0x00000000, 0x00000000};

displayio_palette_t blinka_palette = {
    .base = {.type = &displayio_palette_type },
    .opaque = blinka_transparency,
    .colors = blinka_colors,
    .color_count = 16,
    .needs_refresh = false
};

displayio_tilegrid_t blinka_sprite = {
    .base = {.type = &displayio_tilegrid_type },
    .bitmap = &blinka_bitmap,
    .pixel_shader = &blinka_palette,
    .x = 0,
    .y = 0,
    .bitmap_width_in_tiles = 1,
    .width_in_tiles = 1,
    .height_in_tiles = 1,
    .total_width = 16,
    .total_height = 16,
    .tile_width = 16,
    .tile_height = 16,
    .top_left_x = 16,
    .top_left_y = 16,
    .tiles = 0,
    .needs_refresh = false,
    .inline_tiles = true
};

displayio_group_child_t splash_children[2] = {
    {&blinka_sprite, &blinka_sprite},
    {&supervisor_terminal_text_grid, &supervisor_terminal_text_grid}
};

displayio_group_t circuitpython_splash = {
    .base = {.type = &displayio_group_type },
    .x = 0,
    .y = 0,
    .scale = 2,
    .size = 2,
    .max_size = 2,
    .children = splash_children,
    .needs_refresh = true
};
