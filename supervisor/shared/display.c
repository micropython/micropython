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

#if CIRCUITPY_STATUS_BAR
#include "supervisor/shared/status_bar.h"
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
    displayio_tilegrid_t *status_bar = &supervisor_terminal_status_bar_text_grid;
    bool reset_tiles = false;
    uint16_t width_in_tiles = width_px / scroll_area->tile_width;
    // determine scale based on width
    if (width_in_tiles <= 80) {
        scale = 1;
    }

    width_in_tiles = MAX(1, width_px / (scroll_area->tile_width * scale));
    uint16_t height_in_tiles = MAX(2, height_px / (scroll_area->tile_height * scale));

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

        // Adjust the display dimensions to account for scale of the outer group.
        width_px /= scale;
        height_px /= scale;

        // Number of tiles from the left edge to inset the status bar.
        size_t min_left_padding = 0;
        #if CIRCUITPY_REPL_LOGO
        // Blinka + 1 px padding minimum
        min_left_padding = supervisor_blinka_sprite.pixel_width + 1;
        // Align the status bar to the bottom of the logo.
        status_bar->y = supervisor_blinka_sprite.pixel_height - status_bar->tile_height;
        #else
        status_bar->y = 0;
        #endif
        status_bar->width_in_tiles = (width_px - min_left_padding) / status_bar->tile_width;
        status_bar->height_in_tiles = 1;
        status_bar->pixel_width = status_bar->width_in_tiles * status_bar->tile_width;
        status_bar->pixel_height = status_bar->tile_height;
        // Right align the status bar.
        status_bar->x = width_px - status_bar->pixel_width;
        status_bar->top_left_y = 0;
        status_bar->tiles = tiles;
        status_bar->full_change = true;

        scroll_area->width_in_tiles = width_in_tiles;
        scroll_area->height_in_tiles = height_in_tiles;
        #if CIRCUITPY_REPL_LOGO
        scroll_area->height_in_tiles -= 1;
        #endif
        scroll_area->pixel_width = scroll_area->width_in_tiles * scroll_area->tile_width;
        scroll_area->pixel_height = scroll_area->height_in_tiles * scroll_area->tile_height;
        // Right align the scroll area to give margin to the start of each line.
        scroll_area->x = width_px - scroll_area->pixel_width;
        scroll_area->top_left_y = 0;
        // Align the scroll area to the bottom so that the newest line isn't cutoff. The top line
        // may be clipped by the status bar and that's ok.
        scroll_area->y = height_px - scroll_area->pixel_height;
        scroll_area->tiles = tiles + width_in_tiles;
        scroll_area->full_change = true;

        common_hal_terminalio_terminal_construct(&supervisor_terminal, scroll_area, &supervisor_terminal_font, status_bar);

        // Do not update status bar until after boot.py has run, in case it is disabled.
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
        supervisor_terminal_status_bar_text_grid.tiles = NULL;
        supervisor_terminal.scroll_area = NULL;
        supervisor_terminal.status_bar = NULL;
    }
    #endif
}

bool supervisor_terminal_started(void) {
    #if CIRCUITPY_TERMINALIO
    return tilegrid_tiles != NULL;
    #else
    return false;
    #endif
}

void supervisor_display_move_memory(void) {
    #if CIRCUITPY_TERMINALIO
    displayio_tilegrid_t *scroll_area = &supervisor_terminal_scroll_area_text_grid;
    displayio_tilegrid_t *status_bar = &supervisor_terminal_status_bar_text_grid;
    if (tilegrid_tiles != NULL) {
        status_bar->tiles = (uint8_t *)tilegrid_tiles->ptr;
        scroll_area->tiles = (uint8_t *)tilegrid_tiles->ptr + scroll_area->width_in_tiles;
    } else {
        scroll_area->tiles = NULL;
        status_bar->tiles = NULL;
    }
    #endif

    #if CIRCUITPY_DISPLAYIO
    for (uint8_t i = 0; i < CIRCUITPY_DISPLAY_LIMIT; i++) {
        #if CIRCUITPY_RGBMATRIX
        if (display_buses[i].rgbmatrix.base.type == &rgbmatrix_RGBMatrix_type) {
            rgbmatrix_rgbmatrix_obj_t *pm = &display_buses[i].rgbmatrix;
            common_hal_rgbmatrix_rgbmatrix_reconstruct(pm, NULL);
        }
        #endif
        #if CIRCUITPY_SHARPDISPLAY
        if (display_buses[i].bus_base.type == &sharpdisplay_framebuffer_type) {
            sharpdisplay_framebuffer_obj_t *sharp = &display_buses[i].sharpdisplay;
            common_hal_sharpdisplay_framebuffer_reconstruct(sharp);
        }
        #endif
    }
    #endif
}

#if CIRCUITPY_TERMINALIO
#if CIRCUITPY_REPL_LOGO
mp_obj_t members[] = { &supervisor_terminal_scroll_area_text_grid, &supervisor_blinka_sprite, &supervisor_terminal_status_bar_text_grid, };
mp_obj_list_t splash_children = {
    .base = {.type = &mp_type_list },
    .alloc = 3,
    .len = 3,
    .items = members,
};
#else
mp_obj_t members[] = { &supervisor_terminal_scroll_area_text_grid, &supervisor_terminal_status_bar_text_grid,};
mp_obj_list_t splash_children = {
    .base = {.type = &mp_type_list },
    .alloc = 2,
    .len = 2,
    .items = members,
};
#endif
#else
#if CIRCUITPY_REPL_LOGO
mp_obj_t members[] = { &supervisor_blinka_sprite };
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
