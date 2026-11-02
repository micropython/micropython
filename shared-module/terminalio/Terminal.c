// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-module/terminalio/Terminal.h"

#include "shared-module/fontio/BuiltinFont.h"
#include "shared-bindings/displayio/TileGrid.h"
#include "shared-bindings/displayio/Palette.h"
#include "shared-bindings/terminalio/Terminal.h"
#include "shared-bindings/fontio/BuiltinFont.h"
#if CIRCUITPY_LVFONTIO
#include "shared-bindings/lvfontio/OnDiskFont.h"
#endif

#if CIRCUITPY_STATUS_BAR
#include "shared-bindings/supervisor/__init__.h"
#include "shared-bindings/supervisor/StatusBar.h"
#endif

#include "supervisor/shared/serial.h"

uint16_t terminalio_terminal_get_glyph_index(mp_obj_t font, mp_uint_t codepoint, bool *is_full_width) {
    if (is_full_width != NULL) {
        *is_full_width = false;  // Default to not full width
    }

    #if CIRCUITPY_LVFONTIO
    if (mp_obj_is_type(font, &lvfontio_ondiskfont_type)) {
        // For LV fonts, we need to cache the glyph first
        lvfontio_ondiskfont_t *lv_font = MP_OBJ_TO_PTR(font);
        bool full_width = false;
        int16_t slot = common_hal_lvfontio_ondiskfont_cache_glyph(lv_font, codepoint, &full_width);

        if (is_full_width != NULL) {
            *is_full_width = full_width;
        }

        if (slot == -1) {
            // Not found or couldn't cache
            return 0xffff;
        }
        return (uint16_t)slot;
    }
    #endif

    #if CIRCUITPY_FONTIO
    if (mp_obj_is_type(font, &fontio_builtinfont_type)) {
        // Use the standard fontio function
        fontio_builtinfont_t *fontio_font = MP_OBJ_TO_PTR(font);
        uint8_t index = fontio_builtinfont_get_glyph_index(fontio_font, codepoint);
        if (index == 0xff) {
            return 0xffff;
        }
        return index;
    }
    #endif

    // Unsupported font type
    return 0xffff;
}

static void wrap_cursor(uint16_t width, uint16_t height, uint16_t *cursor_x, uint16_t *cursor_y) {
    if (*cursor_x >= width) {
        *cursor_y = *cursor_y + 1;
        *cursor_x %= width;
    }
    if (*cursor_y >= height) {
        *cursor_y %= height;
    }
}

static void release_current_glyph(displayio_tilegrid_t *tilegrid, mp_obj_t font, uint16_t x, uint16_t y) {
    #if CIRCUITPY_LVFONTIO
    if (!mp_obj_is_type(font, &lvfontio_ondiskfont_type)) {
        return;
    }
    uint16_t current_tile = common_hal_displayio_tilegrid_get_tile(tilegrid, x, y);
    if (current_tile == 0) {
    }
    common_hal_lvfontio_ondiskfont_release_glyph(MP_OBJ_TO_PTR(font), current_tile);
    #endif
}

static void terminalio_terminal_set_tile(terminalio_terminal_obj_t *self, bool status_bar, mp_uint_t character, bool release_glyphs) {
    displayio_tilegrid_t *tilegrid = self->scroll_area;
    uint16_t *x = &self->cursor_x;
    uint16_t *y = &self->cursor_y;
    uint16_t w = self->scroll_area->width_in_tiles;
    uint16_t h = self->scroll_area->height_in_tiles;
    if (status_bar) {
        tilegrid = self->status_bar;
        x = &self->status_x;
        y = &self->status_y;
        w = self->status_bar->width_in_tiles;
        h = self->status_bar->height_in_tiles;
    }
    if (release_glyphs) {
        release_current_glyph(tilegrid, self->font, *x, *y);
    }
    bool is_full_width;
    uint16_t new_tile = terminalio_terminal_get_glyph_index(self->font, character, &is_full_width);
    if (new_tile == 0xffff) {
        // Missing glyph.
        return;
    }
    // If there is only half width left, then fill it with a space and wrap to the next line.
    if (is_full_width && *x == w - 1) {
        uint16_t space = terminalio_terminal_get_glyph_index(self->font, ' ', NULL);
        common_hal_displayio_tilegrid_set_tile(tilegrid, *x, *y, space);
        *x = *x + 1;
        wrap_cursor(w, h, x, y);
        if (release_glyphs) {
            release_current_glyph(tilegrid, self->font, *x, *y);
        }
    }
    common_hal_displayio_tilegrid_set_tile(tilegrid, *x, *y, new_tile);
    *x = *x + 1;
    wrap_cursor(w, h, x, y);
    if (is_full_width) {
        if (release_glyphs) {
            release_current_glyph(tilegrid, self->font, *x, *y);
        }
        common_hal_displayio_tilegrid_set_tile(tilegrid, *x, *y, new_tile + 1);
        *x = *x + 1;
        wrap_cursor(w, h, x, y);
    }
}

// Helper function to set all tiles in a tilegrid with optional glyph release
static void terminalio_terminal_set_all_tiles(terminalio_terminal_obj_t *self, bool status_bar, mp_uint_t character, bool release_glyphs) {
    uint16_t *x = &self->cursor_x;
    uint16_t *y = &self->cursor_y;
    if (status_bar) {
        x = &self->status_x;
        y = &self->status_y;
    }
    *x = 0;
    *y = 0;
    terminalio_terminal_set_tile(self, status_bar, character, release_glyphs);
    while (*x != 0 || *y != 0) {
        terminalio_terminal_set_tile(self, status_bar, character, release_glyphs);
    }
}

void terminalio_terminal_clear_status_bar(terminalio_terminal_obj_t *self) {
    if (self->status_bar) {
        terminalio_terminal_set_all_tiles(self, true, ' ', true);
    }
}


void common_hal_terminalio_terminal_construct(terminalio_terminal_obj_t *self,
    displayio_tilegrid_t *scroll_area, mp_obj_t font,
    displayio_tilegrid_t *status_bar) {
    self->cursor_x = 0;
    self->cursor_y = 0;
    self->font = font;
    self->scroll_area = scroll_area;
    self->status_bar = status_bar;
    self->status_x = 0;
    self->status_y = 0;
    self->first_row = 0;
    self->vt_scroll_top = 0;
    self->vt_scroll_end = self->scroll_area->height_in_tiles - 1;
    terminalio_terminal_set_all_tiles(self, false, ' ', false);
    if (self->status_bar) {
        terminalio_terminal_set_all_tiles(self, true, ' ', false);
    }

    common_hal_displayio_tilegrid_set_top_left(self->scroll_area, 0, 1);
}

size_t common_hal_terminalio_terminal_write(terminalio_terminal_obj_t *self, const byte *data, size_t len, int *errcode) {
    #define SCRNMOD(x) (((x) + (self->scroll_area->top_left_y)) % (self->scroll_area->height_in_tiles))

    // Make sure the terminal is initialized before we do anything with it.
    if (self->scroll_area == NULL) {
        return len;
    }

    #if CIRCUITPY_TERMINALIO_VT100
    uint32_t _select_color(uint16_t ascii_color) {
        uint32_t color_value = 0;
        if ((ascii_color & 1) > 0) {
            color_value += 0xff0000;
        }
        if ((ascii_color & 2) > 0) {
            color_value += 0x00ff00;
        }
        if ((ascii_color & 4) > 0) {
            color_value += 0x0000ff;
        }

        return color_value;
    }

    // Color attributes are stored in entries 0 and 1 of a Palette. Other shaders have no such
    // entries, so the attributes are ignored for them.
    displayio_palette_t *terminal_palette = NULL;
    if (mp_obj_is_type(self->scroll_area->pixel_shader, &displayio_palette_type) &&
        common_hal_displayio_palette_get_len(self->scroll_area->pixel_shader) >= 2) {
        terminal_palette = self->scroll_area->pixel_shader;
    }
    #endif

    const byte *i = data;
    const byte *end = data + len;
    uint16_t start_y = self->cursor_y;
    while (i < end) {
        unichar c = utf8_get_char(i);
        i++;
        // utf8_next_char() assumes NUL-terminated data but callers pass
        // length-delimited buffers, so bound the continuation-byte scan.
        while (i < end && UTF8_IS_CONT(*i)) {
            i++;
        }
        if (self->in_osc_command) {
            if (c == 0x1b && i < end && i[0] == '\\') {
                self->in_osc_command = false;
                self->status_x = 0;
                self->status_y = 0;
                i += 1;
            } else if (
                self->osc_command == 0 &&
                self->status_bar != NULL &&
                self->status_y < self->status_bar->height_in_tiles) {
                // Clear the tile grid before we start putting new info.
                bool at_start = self->status_x == 0 && self->status_y == 0;
                if (at_start) {
                    terminalio_terminal_set_all_tiles(self, true, ' ', true);
                }
                terminalio_terminal_set_tile(self, true, c, true);
                // The cursor wraps to the first tile when the bar is full. Move it past the last
                // row so the height check above skips the rest of the title. A missing first glyph
                // leaves the cursor at the first tile without wrapping, so at_start rules that out.
                if (!at_start && self->status_x == 0 && self->status_y == 0) {
                    self->status_y = self->status_bar->height_in_tiles;
                }
            }
            continue;
        }
        if (c < 0x20) {
            if (c == '\r') {
                self->cursor_x = 0;
            } else if (c == '\t') {
                for (uint8_t space_i = 0; space_i < 4; space_i++) {
                    terminalio_terminal_set_tile(self, false, ' ', true);
                }
            } else if (c == '\n') {
                self->cursor_y++;
                // Commands below are used by MicroPython in the REPL
            } else if (c == '\b') {
                if (self->cursor_x > 0) {
                    self->cursor_x--;
                }
            } else if (c == 0x1b) {
                // Handle commands of the form [ESC].<digits><command-char> where . is not yet known.
                int16_t vt_args[3] = {0, 0, 0};
                uint8_t j = 1;
                #if CIRCUITPY_TERMINALIO_VT100
                uint8_t n_args = 1;
                #endif
                for (; j < 6; j++) {
                    if (i + j >= end) {
                        c = 0;
                        break;
                    }
                    if ('0' <= i[j] && i[j] <= '9') {
                        vt_args[0] = vt_args[0] * 10 + (i[j] - '0');
                    } else {
                        c = i[j];
                        break;
                    }
                }
                if (i < end && i[0] == '[') {
                    for (uint8_t i_args = 1; i_args < 3 && c == ';'; i_args++) {
                        vt_args[i_args] = 0;
                        for (++j; j < 12; j++) {
                            if (i + j >= end) {
                                c = 0;
                                break;
                            }
                            if ('0' <= i[j] && i[j] <= '9') {
                                vt_args[i_args] = vt_args[i_args] * 10 + (i[j] - '0');
                                #if CIRCUITPY_TERMINALIO_VT100
                                n_args = i_args + 1;
                                #endif
                            } else {
                                c = i[j];
                                break;
                            }
                        }
                    }
                    if (c == '?' && i + 4 < end) {
                        #if CIRCUITPY_TERMINALIO_VT100
                        if (i[2] == '2' && i[3] == '5') {
                            // cursor visibility commands
                            if (i[4] == 'h') {
                                // make cursor visible
                                // not implemented yet
                            } else if (i[4] == 'l') {
                                // make cursor invisible
                                // not implemented yet
                            }
                        }
                        i += 5;
                        #endif
                    } else {
                        if (c == 'K') {
                            int16_t original_cursor_x = self->cursor_x;
                            int16_t original_cursor_y = self->cursor_y;
                            int16_t clr_start = self->cursor_x;
                            int16_t clr_end = self->scroll_area->width_in_tiles;
                            #if CIRCUITPY_TERMINALIO_VT100
                            if (vt_args[0] == 1) {
                                clr_start = 0;
                                clr_end = self->cursor_x;
                            } else if (vt_args[0] == 2) {
                                clr_start = 0;
                            }
                            self->cursor_x = clr_start;
                            #endif
                            // Clear the (start/rest/all) of the line.
                            for (uint16_t k = clr_start; k < clr_end; k++) {
                                terminalio_terminal_set_tile(self, false, ' ', true);
                            }
                            self->cursor_x = original_cursor_x;
                            self->cursor_y = original_cursor_y;
                        } else if (c == 'D') {
                            if (vt_args[0] > self->cursor_x) {
                                self->cursor_x = 0;
                            } else {
                                self->cursor_x -= vt_args[0];
                            }
                        } else if (c == 'J') {
                            if (vt_args[0] == 2) {
                                common_hal_displayio_tilegrid_set_top_left(self->scroll_area, 0, 0);
                                self->cursor_x = self->cursor_y = start_y = 0;
                                terminalio_terminal_set_all_tiles(self, false, ' ', true);
                            }
                        } else if (c == 'H') {
                            if (vt_args[0] > 0) {
                                vt_args[0]--;
                            }
                            if (vt_args[1] > 0) {
                                vt_args[1]--;
                            }
                            if (vt_args[0] >= self->scroll_area->height_in_tiles) {
                                vt_args[0] = self->scroll_area->height_in_tiles - 1;
                            }
                            if (vt_args[1] >= self->scroll_area->width_in_tiles) {
                                vt_args[1] = self->scroll_area->width_in_tiles - 1;
                            }
                            vt_args[0] = SCRNMOD(vt_args[0]);
                            self->cursor_x = vt_args[1];
                            self->cursor_y = vt_args[0];
                            start_y = self->cursor_y;
                        #if CIRCUITPY_TERMINALIO_VT100
                        } else if (c == 'm' && terminal_palette != NULL) {
                            for (uint8_t i_args = 0; i_args < n_args; i_args++) {
                                if ((vt_args[i_args] >= 40 && vt_args[i_args] <= 47) || (vt_args[i_args] >= 30 && vt_args[i_args] <= 37)) {
                                    common_hal_displayio_palette_set_color(terminal_palette, 1 - (vt_args[i_args] / 40), _select_color(vt_args[i_args] % 10));
                                }
                                if (vt_args[i_args] == 0) {
                                    common_hal_displayio_palette_set_color(terminal_palette, 0, 0x000000);
                                    common_hal_displayio_palette_set_color(terminal_palette, 1, 0xffffff);
                                }
                            }
                        } else if (c == 'r') {
                            if (vt_args[0] < vt_args[1] && vt_args[0] >= 1 && vt_args[1] <= self->scroll_area->height_in_tiles) {
                                self->vt_scroll_top = vt_args[0] - 1;
                                self->vt_scroll_end = vt_args[1] - 1;
                            } else {
                                self->vt_scroll_top = 0;
                                self->vt_scroll_end = self->scroll_area->height_in_tiles - 1;
                            }
                            self->cursor_x = 0;
                            self->cursor_y = self->scroll_area->top_left_y % self->scroll_area->height_in_tiles;
                            start_y = self->cursor_y;
                        #endif
                        }
                        i += j + 1;
                    }
                #if CIRCUITPY_TERMINALIO_VT100
                } else if (i < end && i[0] == 'M') {
                    if (self->cursor_y != SCRNMOD(self->vt_scroll_top)) {
                        if (self->cursor_y > 0) {
                            self->cursor_y = self->cursor_y - 1;
                        } else {
                            self->cursor_y = self->scroll_area->height_in_tiles - 1;
                        }
                    } else {
                        if (self->vt_scroll_top != 0 || self->vt_scroll_end != self->scroll_area->height_in_tiles - 1) {
                            // Scroll range defined, manually move tiles to perform scroll
                            for (int16_t irow = self->vt_scroll_end - 1; irow >= self->vt_scroll_top; irow--) {
                                for (int16_t icol = 0; icol < self->scroll_area->width_in_tiles; icol++) {
                                    common_hal_displayio_tilegrid_set_tile(self->scroll_area, icol, SCRNMOD(irow + 1), common_hal_displayio_tilegrid_get_tile(self->scroll_area, icol, SCRNMOD(irow)));
                                }
                            }
                            self->cursor_x = 0;
                            int16_t old_y = self->cursor_y;
                            // Fill the row with spaces.
                            for (int16_t icol = 0; icol < self->scroll_area->width_in_tiles; icol++) {
                                terminalio_terminal_set_tile(self, false, ' ', true);
                            }
                            self->cursor_y = old_y;
                        } else {
                            // Full screen scroll, just set new top_y pointer and clear row
                            if (self->cursor_y > 0) {
                                common_hal_displayio_tilegrid_set_top_left(self->scroll_area, 0, self->cursor_y - 1);
                            } else {
                                common_hal_displayio_tilegrid_set_top_left(self->scroll_area, 0, self->scroll_area->height_in_tiles - 1);
                            }

                            self->cursor_x = 0;
                            self->cursor_y = self->scroll_area->top_left_y;
                            // Fill the row with spaces.
                            for (int16_t icol = 0; icol < self->scroll_area->width_in_tiles; icol++) {
                                terminalio_terminal_set_tile(self, false, ' ', true);
                            }
                            self->cursor_y = self->scroll_area->top_left_y;
                        }
                        self->cursor_x = 0;
                    }
                    start_y = self->cursor_y;
                    i++;
                } else if (i < end && i[0] == 'D') {
                    self->cursor_y++;
                    i++;
                #endif
                } else if (i < end && i[0] == ']' && c == ';') {
                    self->in_osc_command = true;
                    self->osc_command = vt_args[0];
                    i += j + 1;
                }
            }
        } else {
            terminalio_terminal_set_tile(self, false, c, true);
        }
        if (self->cursor_x >= self->scroll_area->width_in_tiles) {
            self->cursor_y++;
            self->cursor_x %= self->scroll_area->width_in_tiles;
        }
        if (self->cursor_y >= self->scroll_area->height_in_tiles) {
            self->cursor_y %= self->scroll_area->height_in_tiles;
        }
        if (self->cursor_y != start_y) {
            if (((self->cursor_y + self->scroll_area->height_in_tiles) - 1) % self->scroll_area->height_in_tiles == SCRNMOD(self->vt_scroll_end)) {
                #if CIRCUITPY_TERMINALIO_VT100
                if (self->vt_scroll_top != 0 || self->vt_scroll_end != self->scroll_area->height_in_tiles - 1) {
                    // Scroll range defined, manually move tiles to perform scroll
                    self->cursor_y = SCRNMOD(self->vt_scroll_end);

                    for (int16_t irow = self->vt_scroll_top; irow < self->vt_scroll_end; irow++) {
                        for (int16_t icol = 0; icol < self->scroll_area->width_in_tiles; icol++) {
                            common_hal_displayio_tilegrid_set_tile(self->scroll_area, icol, SCRNMOD(irow), common_hal_displayio_tilegrid_get_tile(self->scroll_area, icol, SCRNMOD(irow + 1)));
                        }
                    }
                }
                #endif
                if (self->vt_scroll_top == 0 && self->vt_scroll_end == self->scroll_area->height_in_tiles - 1) {
                    // Full screen scroll, just set new top_y pointer
                    common_hal_displayio_tilegrid_set_top_left(self->scroll_area, 0, (self->cursor_y + self->scroll_area->height_in_tiles + 1) % self->scroll_area->height_in_tiles);
                }
                // clear the new row in case of scroll up
                self->cursor_x = 0;
                int16_t old_y = self->cursor_y;
                for (int16_t icol = 0; icol < self->scroll_area->width_in_tiles; icol++) {
                    terminalio_terminal_set_tile(self, false, ' ', true);
                }
                self->cursor_x = 0;
                self->cursor_y = old_y;
            }
            start_y = self->cursor_y;
        }
    }
    if (i > end) {
        i = end;
    }
    return i - data;
}

uint16_t common_hal_terminalio_terminal_get_cursor_x(terminalio_terminal_obj_t *self) {
    return self->cursor_x;
}
uint16_t common_hal_terminalio_terminal_get_cursor_y(terminalio_terminal_obj_t *self) {
    return self->cursor_y;
}

bool common_hal_terminalio_terminal_ready_to_tx(terminalio_terminal_obj_t *self) {
    return self->scroll_area != NULL;
}
