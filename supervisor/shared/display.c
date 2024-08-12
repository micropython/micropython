// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/shared/display.h"

#include <string.h>

#include "py/mpstate.h"
#include "shared-bindings/displayio/Bitmap.h"
#include "shared-bindings/displayio/Group.h"
#include "shared-bindings/displayio/Palette.h"
#include "shared-bindings/displayio/TileGrid.h"

#if CIRCUITPY_RGBMATRIX
#include "shared-module/displayio/__init__.h"
#endif

#if CIRCUITPY_SHARPDISPLAY
#include "shared-module/displayio/__init__.h"
#include "shared-bindings/sharpdisplay/SharpMemoryFramebuffer.h"
#include "shared-module/sharpdisplay/SharpMemoryFramebuffer.h"
#endif

#if CIRCUITPY_AURORA_EPAPER
#include "shared-module/displayio/__init__.h"
#include "shared-bindings/aurora_epaper/aurora_framebuffer.h"
#include "shared-module/aurora_epaper/aurora_framebuffer.h"
#endif

#if CIRCUITPY_STATUS_BAR
#include "supervisor/shared/status_bar.h"
#endif

#if CIRCUITPY_TERMINALIO
#include "supervisor/port.h"
#endif

#if CIRCUITPY_REPL_LOGO
extern uint32_t blinka_bitmap_data[];
extern displayio_bitmap_t blinka_bitmap;
#endif
extern displayio_group_t circuitpython_splash;

#if CIRCUITPY_TERMINALIO
static uint8_t *tilegrid_tiles = NULL;
static size_t tilegrid_tiles_size = 0;
#endif

void supervisor_start_terminal(uint16_t width_px, uint16_t height_px) {
    if (supervisor_terminal_started()) {
        return;
    }
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
        if (tilegrid_tiles_size != total_tiles) {
            port_free(tilegrid_tiles);
            tilegrid_tiles = NULL;
            tilegrid_tiles_size = 0;
            reset_tiles = true;
        }
    }
    if (!tilegrid_tiles) {
        tilegrid_tiles = port_malloc(total_tiles, false);
        reset_tiles = true;
        if (!tilegrid_tiles) {
            return;
        }
    }

    if (reset_tiles) {
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
        status_bar->tiles = tilegrid_tiles;
        status_bar->full_change = true;

        scroll_area->width_in_tiles = width_in_tiles;
        // Leave space for the status bar, no matter if we have logo or not.
        scroll_area->height_in_tiles = height_in_tiles - 1;
        scroll_area->pixel_width = scroll_area->width_in_tiles * scroll_area->tile_width;
        scroll_area->pixel_height = scroll_area->height_in_tiles * scroll_area->tile_height;
        // Right align the scroll area to give margin to the start of each line.
        scroll_area->x = width_px - scroll_area->pixel_width;
        scroll_area->top_left_y = 0;
        // Align the scroll area to the bottom so that the newest line isn't cutoff. The top line
        // may be clipped by the status bar and that's ok.
        scroll_area->y = height_px - scroll_area->pixel_height;
        scroll_area->tiles = tilegrid_tiles + width_in_tiles;
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
        port_free(tilegrid_tiles);
        tilegrid_tiles = NULL;
        tilegrid_tiles_size = 0;
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
mp_obj_t members[] = { &supervisor_terminal_scroll_area_text_grid, &supervisor_terminal_status_bar_text_grid, };
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
    .hidden_by_parent = false,
    .readonly = true,
};
