// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/shared/display.h"

#include <string.h>
#include "supervisor/port.h"

#include "py/mpstate.h"
#include "py/gc.h"
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

#if CIRCUITPY_SETTINGS_TOML
#include "supervisor/shared/settings.h"
#endif

#if CIRCUITPY_LVFONTIO
#include "shared-bindings/lvfontio/OnDiskFont.h"
#include "supervisor/filesystem.h"

#include "supervisor/shared/serial.h"

// Check if a custom font file exists and return its path if found
// Returns true if font file exists, false otherwise
static bool check_for_custom_font(const char **font_path_out) {
    if (!filesystem_present()) {
        return false;
    }

    // Check for the font through the supervisor filesystem
    // API so it works on FAT and littlefs mounts.
    supervisor_vfs_t *vfs = filesystem_circuitpy();
    if (vfs == NULL) {
        return false;
    }

    const char *default_font_path = "/fonts/terminal.lvfontbin";
    const char *font_path = default_font_path;

    #if CIRCUITPY_SETTINGS_TOML
    // Buffer for storing custom font path
    static char custom_font_path[128];
    if (settings_get_str("CIRCUITPY_TERMINAL_FONT", custom_font_path, sizeof(custom_font_path)) == SETTINGS_OK) {
        // Use custom font path from environment variable
        font_path = custom_font_path;
    }
    #endif

    if (supervisor_vfs_stat(vfs, font_path, NULL, NULL, NULL) == SUPERVISOR_FS_OK) {
        if (font_path_out != NULL) {
            *font_path_out = font_path;
        }
        return true;
    }

    // If custom font path doesn't exist, use default font
    font_path = default_font_path;

    if (supervisor_vfs_stat(vfs, font_path, NULL, NULL, NULL) == SUPERVISOR_FS_OK) {
        if (font_path_out != NULL) {
            *font_path_out = font_path;
        }
        return true;
    }

    return false;
}

// Initialize a BuiltinFont object with the specified font file and max_slots
// Returns true on success, false on failure
static bool init_lvfont(lvfontio_ondiskfont_t *font, const char *font_path, uint16_t max_slots) {
    if (font == NULL) {
        return false;
    }

    font->base.type = &lvfontio_ondiskfont_type;

    // Pass false for use_gc_allocator during startup when garbage collector isn't fully initialized
    common_hal_lvfontio_ondiskfont_construct(font, font_path, max_slots, false);

    return !common_hal_lvfontio_ondiskfont_deinited(font);
}
#endif
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

#if CIRCUITPY_LVFONTIO && CIRCUITPY_TERMINALIO
static lvfontio_ondiskfont_t *lvfont = NULL;
static bool skip_lv_font = false;
#endif

void supervisor_start_terminal(uint16_t width_px, uint16_t height_px) {
    if (supervisor_terminal_started()) {
        return;
    }

    #if CIRCUITPY_TERMINALIO
    // Default the scale to 2 because we may show blinka without the terminal for
    // languages that don't have font support.
    mp_int_t scale = 2;

    displayio_tilegrid_t *scroll_area = &supervisor_terminal_scroll_area_text_grid;
    displayio_tilegrid_t *status_bar = &supervisor_terminal_status_bar_text_grid;
    bool reset_tiles = tilegrid_tiles == NULL;

    uint16_t glyph_width = 0;
    uint16_t glyph_height = 0;

    #if CIRCUITPY_LVFONTIO
    // Check if we have a custom terminal font in the filesystem
    bool use_lv_font = false;
    const char *font_path = NULL;
    const uint16_t display_width_px = width_px;
    const uint16_t display_height_px = height_px;

    if (!skip_lv_font && check_for_custom_font(&font_path)) {
        // Initialize a temporary font just to get dimensions
        lvfontio_ondiskfont_t temp_font;
        if (init_lvfont(&temp_font, font_path, 1)) {
            // The terminal fills itself with spaces. With one slot, caching fails if the space is
            // missing or wider than a cell.
            if (common_hal_lvfontio_ondiskfont_cache_glyph(&temp_font, ' ', NULL) >= 0) {
                // Get the font dimensions
                common_hal_lvfontio_ondiskfont_get_dimensions(&temp_font, &glyph_width, &glyph_height);
                use_lv_font = true;
                reset_tiles = true;
                // TODO: We may want to detect when the files modified time hasn't changed.
            }

            // Clean up the temp font - we'll create a proper one later
            common_hal_lvfontio_ondiskfont_deinit(&temp_font);
        }
    }
    #endif
    #if CIRCUITPY_FONTIO
    if (glyph_width == 0) {
        glyph_width = supervisor_terminal_font.width;
        glyph_height = supervisor_terminal_font.height;
    }
    #endif

    uint16_t width_in_tiles = width_px / glyph_width;

    // determine scale based on width
    if (width_in_tiles <= 120) {
        scale = 1;
    }
    #if CIRCUITPY_SETTINGS_TOML
    (void)settings_get_int("CIRCUITPY_TERMINAL_SCALE", &scale);
    #endif

    width_in_tiles = MAX(1, width_px / (glyph_width * scale));
    uint16_t height_in_tiles = MAX(2, height_px / (glyph_height * scale));

    uint16_t total_tiles = width_in_tiles * height_in_tiles;

    // check if the terminal tile dimensions are the same
    if ((scroll_area->width_in_tiles != width_in_tiles) ||
        (scroll_area->height_in_tiles != height_in_tiles - 1)) {
        reset_tiles = true;
    }

    circuitpython_splash.scale = scale;
    if (!reset_tiles) {
        return;
    }

    // Adjust the display dimensions to account for scale of the outer group.
    width_px /= scale;
    height_px /= scale;

    // Number of tiles from the left edge to inset the status bar.
    size_t min_left_padding = 0;
    status_bar->tile_width = glyph_width;
    status_bar->tile_height = glyph_height;
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
    status_bar->full_change = true;

    scroll_area->tile_width = glyph_width;
    scroll_area->tile_height = glyph_height;
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
    scroll_area->full_change = true;

    mp_obj_t new_bitmap = mp_const_none;
    mp_obj_t new_font = mp_const_none;

    #if CIRCUITPY_LVFONTIO
    if (lvfont != NULL) {
        common_hal_lvfontio_ondiskfont_deinit(lvfont);
        // This will also free internal buffers that may change size.
        port_free(lvfont);
        lvfont = NULL;
    }

    if (use_lv_font) {
        // We found a custom terminal font file, use it instead of the built-in font

        lvfont = port_malloc(sizeof(lvfontio_ondiskfont_t), false);
        if (lvfont != NULL) {
            // Use the number of tiles in the terminal and status bar for the number of slots
            // This ensures we have enough slots to display all characters that could appear on screen
            uint16_t num_slots = width_in_tiles * height_in_tiles;

            // Initialize the font with our helper function
            if (init_lvfont(lvfont, font_path, num_slots)) {
                // Get the bitmap from the font
                new_bitmap = common_hal_lvfontio_ondiskfont_get_bitmap(lvfont);
                new_font = MP_OBJ_FROM_PTR(lvfont);
            } else {
                // If font initialization failed, free the memory and fall back to built-in font
                port_free(lvfont);
                lvfont = NULL;
            }
        }
        if (lvfont == NULL) {
            // The layout above used the custom font's dimensions, so redo it for the built-in font.
            skip_lv_font = true;
            supervisor_start_terminal(display_width_px, display_height_px);
            skip_lv_font = false;
            return;
        }
    }
    #endif
    #if CIRCUITPY_FONTIO
    if (new_font == mp_const_none) {
        new_bitmap = MP_OBJ_FROM_PTR(supervisor_terminal_font.bitmap);
        new_font = MP_OBJ_FROM_PTR(&supervisor_terminal_font);
    }
    #endif

    if (new_font != mp_const_none) {
        size_t total_values = common_hal_displayio_bitmap_get_width(new_bitmap) / glyph_width;
        if (tilegrid_tiles) {
            port_free(tilegrid_tiles);
            tilegrid_tiles = NULL;
        }
        size_t bytes_per_tile = 1;
        if (total_tiles > 255) {
            // Two bytes per tile.
            bytes_per_tile = 2;
        }
        tilegrid_tiles = port_malloc(total_tiles * bytes_per_tile, false);
        if (!tilegrid_tiles) {
            return;
        }
        status_bar->tiles = tilegrid_tiles;
        status_bar->tiles_in_bitmap = total_values;
        status_bar->bitmap_width_in_tiles = total_values;
        scroll_area->tiles = tilegrid_tiles + width_in_tiles * bytes_per_tile;
        scroll_area->tiles_in_bitmap = total_values;
        scroll_area->bitmap_width_in_tiles = total_values;

        common_hal_displayio_tilegrid_set_bitmap(scroll_area, new_bitmap);
        common_hal_displayio_tilegrid_set_bitmap(status_bar, new_bitmap);
        common_hal_terminalio_terminal_construct(&supervisor_terminal, scroll_area,
            new_font, status_bar);
    }
    #endif
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
