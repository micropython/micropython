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
#include "shared-bindings/displayio/Bitmap.h"
#include "shared-bindings/displayio/Group.h"
#include "shared-bindings/displayio/Palette.h"
#include "shared-bindings/displayio/TileGrid.h"
#include "supervisor/memory.h"

#if CIRCUITPY_RGBMATRIX
#include "shared-module/displayio/__init__.h"
#endif

#if CIRCUITPY_SHARPDISPLAY
#include "shared-module/displayio/__init__.h"
#include "shared-bindings/sharpdisplay/SharpMemoryFramebuffer.h"
#include "shared-module/sharpdisplay/SharpMemoryFramebuffer.h"
#endif

extern size_t blinka_bitmap_data[];
extern displayio_bitmap_t blinka_bitmap;
extern displayio_group_t circuitpython_splash;

#if CIRCUITPY_TERMINALIO
static supervisor_allocation *tilegrid_tiles = NULL;
#endif

void supervisor_start_terminal(uint16_t width_px, uint16_t height_px) {
    // Default the scale to 2 because we may show blinka without the terminal for
    // languages that don't have font support.
    uint8_t scale = 2;

    #if CIRCUITPY_TERMINALIO
    displayio_tilegrid_t *grid = &supervisor_terminal_text_grid;
    bool tall = height_px > width_px;
    uint16_t terminal_width_px = tall ? width_px : width_px - blinka_bitmap.width;
    uint16_t terminal_height_px = tall ? height_px - blinka_bitmap.height : height_px;
    uint16_t width_in_tiles = terminal_width_px / grid->tile_width;
    // determine scale based on h
    if (width_in_tiles < 80) {
        scale = 1;
    }

    width_in_tiles = terminal_width_px / (grid->tile_width * scale);
    if (width_in_tiles < 1) {
        width_in_tiles = 1;
    }
    uint16_t height_in_tiles = terminal_height_px / (grid->tile_height * scale);
    uint16_t remaining_pixels = tall ? 0 : terminal_height_px % (grid->tile_height * scale);
    if (height_in_tiles < 1 || remaining_pixels > 0) {
        height_in_tiles += 1;
    }

    uint16_t total_tiles = width_in_tiles * height_in_tiles;

    // Reuse the previous allocation if possible
    if (tilegrid_tiles) {
        if (get_allocation_length(tilegrid_tiles) != align32_size(total_tiles)) {
            free_memory(tilegrid_tiles);
            tilegrid_tiles = NULL;
        }
    }
    if (!tilegrid_tiles) {
        tilegrid_tiles = allocate_memory(align32_size(total_tiles), false, true);
        if (!tilegrid_tiles) {
            return;
        }
    }
    uint8_t *tiles = (uint8_t *)tilegrid_tiles->ptr;

    grid->y = tall ? blinka_bitmap.height : 0;
    grid->x = tall ? 0 : blinka_bitmap.width;
    grid->top_left_y = 0;
    if (remaining_pixels > 0) {
        grid->y -= (grid->tile_height - remaining_pixels);
    }
    grid->width_in_tiles = width_in_tiles;
    grid->height_in_tiles = height_in_tiles;
    assert(width_in_tiles > 0);
    assert(height_in_tiles > 0);
    grid->pixel_width = width_in_tiles * grid->tile_width;
    grid->pixel_height = height_in_tiles * grid->tile_height;
    grid->tiles = tiles;
    grid->full_change = true;

    common_hal_terminalio_terminal_construct(&supervisor_terminal, grid, &supervisor_terminal_font);
    #endif

    circuitpython_splash.scale = scale;
}

void supervisor_stop_terminal(void) {
    #if CIRCUITPY_TERMINALIO
    if (tilegrid_tiles != NULL) {
        free_memory(tilegrid_tiles);
        tilegrid_tiles = NULL;
        supervisor_terminal_text_grid.tiles = NULL;
    }
    #endif
}

void supervisor_display_move_memory(void) {
    #if CIRCUITPY_TERMINALIO
    if (tilegrid_tiles != NULL) {
        supervisor_terminal_text_grid.tiles = (uint8_t *)tilegrid_tiles->ptr;
    } else {
        supervisor_terminal_text_grid.tiles = NULL;
    }
    #endif

    #if CIRCUITPY_DISPLAYIO
    for (uint8_t i = 0; i < CIRCUITPY_DISPLAY_LIMIT; i++) {
        #if CIRCUITPY_RGBMATRIX
        if (displays[i].rgbmatrix.base.type == &rgbmatrix_RGBMatrix_type) {
            rgbmatrix_rgbmatrix_obj_t *pm = &displays[i].rgbmatrix;
            common_hal_rgbmatrix_rgbmatrix_reconstruct(pm, NULL);
        }
        #endif
        #if CIRCUITPY_SHARPDISPLAY
        if (displays[i].bus_base.type == &sharpdisplay_framebuffer_type) {
            sharpdisplay_framebuffer_obj_t *sharp = &displays[i].sharpdisplay;
            common_hal_sharpdisplay_framebuffer_reconstruct(sharp);
        }
        #endif
    }
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

_displayio_color_t blinka_colors[7] = {
    {
        .rgb888 = 0x000000,
        .rgb565 = 0x0000,
        .luma = 0x00,
        .chroma = 0,
        .transparent = true
    },
    {
        .rgb888 = 0x8428bc,
        .rgb565 = 0x8978,
        .luma = 0xff, // We cheat the luma here. It is actually 0x60
        .hue = 184,
        .chroma = 148
    },
    {
        .rgb888 = 0xff89bc,
        .rgb565 = 0xFCB8,
        .luma = 0xb5,
        .hue = 222,
        .chroma = 118
    },
    {
        .rgb888 = 0x7beffe,
        .rgb565 = 0x869F,
        .luma = 0xe0,
        .hue = 124,
        .chroma = 131
    },
    {
        .rgb888 = 0x51395f,
        .rgb565 = 0x5A0D,
        .luma = 0x47,
        .hue = 185,
        .chroma = 38
    },
    {
        .rgb888 = 0xffffff,
        .rgb565 = 0xffff,
        .luma = 0xff,
        .chroma = 0
    },
    {
        .rgb888 = 0x0736a0,
        .rgb565 = 0x01f5,
        .luma = 0x44,
        .hue = 147,
        .chroma = 153
    },
};

displayio_palette_t blinka_palette = {
    .base = {.type = &displayio_palette_type },
    .colors = blinka_colors,
    .color_count = 7,
    .needs_refresh = false
};

displayio_tilegrid_t blinka_sprite = {
    .base = {.type = &displayio_tilegrid_type },
    .bitmap = &blinka_bitmap,
    .pixel_shader = &blinka_palette,
    .x = 0,
    .y = 0,
    .pixel_width = 16,
    .pixel_height = 16,
    .bitmap_width_in_tiles = 1,
    .width_in_tiles = 1,
    .height_in_tiles = 1,
    .tile_width = 16,
    .tile_height = 16,
    .top_left_x = 16,
    .top_left_y = 16,
    .tiles = 0,
    .partial_change = false,
    .full_change = false,
    .hidden = false,
    .hidden_by_parent = false,
    .moved = false,
    .inline_tiles = true,
    .in_group = true
};

#if CIRCUITPY_TERMINALIO
mp_obj_t members[] = { &blinka_sprite, &supervisor_terminal_text_grid, };
mp_obj_list_t splash_children = {
    .base = {.type = &mp_type_list },
    .alloc = 2,
    .len = 2,
    .items = members,
};
#else
mp_obj_t members[] = { &blinka_sprite };
mp_obj_list_t splash_children = {
    .base = {.type = &mp_type_list },
    .alloc = 1,
    .len = 1,
    .items = members,
};
#endif

displayio_group_t circuitpython_splash = {
    .base = {.type = &displayio_group_type },
    .x = 0,
    .y = 0,
    .scale = 2,
    .members = &splash_children,
    .item_removed = false,
    .in_group = false,
    .hidden = false,
    .hidden_by_parent = false
};
