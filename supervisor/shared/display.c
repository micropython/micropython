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

#if CIRCUITPY_REPL_LOGO
extern uint32_t blinka_bitmap_data[];
extern displayio_bitmap_t blinka_bitmap;
#endif
extern displayio_group_t circuitpython_splash;

#if CIRCUITPY_TERMINALIO
static supervisor_allocation *tilegrid_tiles = NULL;
#endif

void supervisor_start_terminal(uint16_t width_px, uint16_t height_px) {
    // Default the scale to 2 because we may show blinka without the terminal for
    // languages that don't have font support.
    uint8_t scale = 2;

    #if CIRCUITPY_TERMINALIO
    displayio_tilegrid_t *scroll_area = &supervisor_terminal_scroll_area_text_grid;
    displayio_tilegrid_t *title_bar = &supervisor_terminal_title_bar_text_grid;
    bool reset_tiles = false;
    uint16_t width_in_tiles = width_px / scroll_area->tile_width;
    // determine scale based on h
    if (width_in_tiles < 80) {
        scale = 1;
    }

    width_in_tiles = width_px / (scroll_area->tile_width * scale);
    if (width_in_tiles < 1) {
        width_in_tiles = 1;
    }
    uint16_t height_in_tiles = height_px / (scroll_area->tile_height * scale);

    uint16_t total_tiles = width_in_tiles * height_in_tiles;

    // check if the terminal tile dimensions are the same
    if ((scroll_area->width_in_tiles != width_in_tiles) ||
        (scroll_area->height_in_tiles != height_in_tiles - 1)) {
        reset_tiles = true;
    }
    // Reuse the previous allocation if possible
    if (tilegrid_tiles) {
        if (get_allocation_length(tilegrid_tiles) != align32_size(total_tiles)) {
            free_memory(tilegrid_tiles);
            tilegrid_tiles = NULL;
            reset_tiles = true;
        }
    }
    if (!tilegrid_tiles) {
        tilegrid_tiles = allocate_memory(align32_size(total_tiles), false, true);
        reset_tiles = true;
        if (!tilegrid_tiles) {
            return;
        }
    }

    if (reset_tiles) {
        uint8_t *tiles = (uint8_t *)tilegrid_tiles->ptr;

        #if CIRCUITPY_REPL_LOGO
        title_bar->x = blinka_bitmap.width;
        // Align the title bar to the bottom of the logo.
        title_bar->y = blinka_bitmap.height - title_bar->tile_height;
        #else
        title_bar->x = 0;
        title_bar->y = 0;
        #endif
        title_bar->top_left_y = 0;
        title_bar->width_in_tiles = width_in_tiles;
        title_bar->height_in_tiles = 1;
        assert(width_in_tiles > 0);
        title_bar->pixel_width = width_in_tiles * title_bar->tile_width;
        title_bar->pixel_height = title_bar->tile_height;
        title_bar->tiles = tiles;
        title_bar->full_change = true;

        scroll_area->x = 0;
        #if CIRCUITPY_REPL_LOGO
        scroll_area->y = blinka_bitmap.height;
        #else
        scroll_area->y = scroll_area->tile_height;
        #endif
        scroll_area->top_left_y = 0;
        scroll_area->width_in_tiles = width_in_tiles;
        scroll_area->height_in_tiles = height_in_tiles - 1;
        assert(width_in_tiles > 0);
        assert(height_in_tiles > 1);
        scroll_area->pixel_width = width_in_tiles * scroll_area->tile_width;
        scroll_area->pixel_height = (height_in_tiles - 1) * scroll_area->tile_height;
        scroll_area->tiles = tiles + width_in_tiles;
        scroll_area->full_change = true;

        common_hal_terminalio_terminal_construct(&supervisor_terminal, scroll_area, &supervisor_terminal_font, title_bar);
    }
    #endif

    circuitpython_splash.scale = scale;
}

void supervisor_stop_terminal(void) {
    #if CIRCUITPY_TERMINALIO
    if (tilegrid_tiles != NULL) {
        free_memory(tilegrid_tiles);
        tilegrid_tiles = NULL;
        supervisor_terminal_scroll_area_text_grid.tiles = NULL;
        supervisor_terminal_title_bar_text_grid.tiles = NULL;
        supervisor_terminal.scroll_area = NULL;
        supervisor_terminal.title_bar = NULL;
    }
    #endif
}

void supervisor_display_move_memory(void) {
    #if CIRCUITPY_TERMINALIO
    displayio_tilegrid_t *scroll_area = &supervisor_terminal_scroll_area_text_grid;
    displayio_tilegrid_t *title_bar = &supervisor_terminal_title_bar_text_grid;
    if (tilegrid_tiles != NULL) {
        title_bar->tiles = (uint8_t *)tilegrid_tiles->ptr;
        scroll_area->tiles = (uint8_t *)tilegrid_tiles->ptr + scroll_area->width_in_tiles;
    } else {
        scroll_area->tiles = NULL;
        title_bar->tiles = NULL;
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

#if CIRCUITPY_REPL_LOGO
uint32_t blinka_bitmap_data[32] = {
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
#endif

#if CIRCUITPY_TERMINALIO
#if CIRCUITPY_REPL_LOGO
mp_obj_t members[] = { &blinka_sprite, &supervisor_terminal_title_bar_text_grid, &supervisor_terminal_scroll_area_text_grid, };
mp_obj_list_t splash_children = {
    .base = {.type = &mp_type_list },
    .alloc = 3,
    .len = 3,
    .items = members,
};
#else
mp_obj_t members[] = { &supervisor_terminal_title_bar_text_grid, &supervisor_terminal_scroll_area_text_grid, };
mp_obj_list_t splash_children = {
    .base = {.type = &mp_type_list },
    .alloc = 2,
    .len = 2,
    .items = members,
};
#endif
#else
#if CIRCUITPY_REPL_LOGO
mp_obj_t members[] = { &blinka_sprite };
mp_obj_list_t splash_children = {
    .base = {.type = &mp_type_list },
    .alloc = 1,
    .len = 1,
    .items = members,
};
#else
mp_obj_t members[] = {};
mp_obj_list_t splash_children = {
    .base = {.type = &mp_type_list },
    .alloc = 0,
    .len = 0,
    .items = members,
};
#endif
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
