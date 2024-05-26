// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-module/terminalio/Terminal.h"

#include "shared-module/fontio/BuiltinFont.h"
#include "shared-bindings/displayio/TileGrid.h"
#include "shared-bindings/terminalio/Terminal.h"

#if CIRCUITPY_STATUS_BAR
#include "shared-bindings/supervisor/__init__.h"
#include "shared-bindings/supervisor/StatusBar.h"
#endif

void terminalio_terminal_clear_status_bar(terminalio_terminal_obj_t *self) {
    if (self->status_bar) {
        common_hal_displayio_tilegrid_set_all_tiles(self->status_bar, 0);
    }
}

void common_hal_terminalio_terminal_construct(terminalio_terminal_obj_t *self,
    displayio_tilegrid_t *scroll_area, const fontio_builtinfont_t *font,
    displayio_tilegrid_t *status_bar) {
    self->cursor_x = 0;
    self->cursor_y = 0;
    self->font = font;
    self->scroll_area = scroll_area;
    self->status_bar = status_bar;
    self->status_x = 0;
    self->status_y = 0;
    self->first_row = 0;
    common_hal_displayio_tilegrid_set_all_tiles(self->scroll_area, 0);
    if (self->status_bar) {
        common_hal_displayio_tilegrid_set_all_tiles(self->status_bar, 0);
    }

    common_hal_displayio_tilegrid_set_top_left(self->scroll_area, 0, 1);
}

size_t common_hal_terminalio_terminal_write(terminalio_terminal_obj_t *self, const byte *data, size_t len, int *errcode) {
    // Make sure the terminal is initialized before we do anything with it.
    if (self->scroll_area == NULL) {
        return len;
    }

    const byte *i = data;
    uint16_t start_y = self->cursor_y;
    while (i < data + len) {
        unichar c = utf8_get_char(i);
        i = utf8_next_char(i);
        if (self->in_osc_command) {
            if (c == 0x1b && i[0] == '\\') {
                self->in_osc_command = false;
                self->status_x = 0;
                self->status_y = 0;
                i += 1;
            } else if (
                self->osc_command == 0 &&
                self->status_bar != NULL &&
                self->status_y < self->status_bar->height_in_tiles) {
                uint8_t tile_index = fontio_builtinfont_get_glyph_index(self->font, c);
                if (tile_index != 0xff) {
                    // Clear the tile grid before we start putting new info.
                    if (self->status_x == 0 && self->status_y == 0) {
                        common_hal_displayio_tilegrid_set_all_tiles(self->status_bar, 0);
                    }
                    common_hal_displayio_tilegrid_set_tile(self->status_bar, self->status_x, self->status_y, tile_index);
                    self->status_x++;
                    if (self->status_x >= self->status_bar->width_in_tiles) {
                        self->status_y++;
                        self->status_x %= self->status_bar->width_in_tiles;
                    }
                }
            }
            continue;
        }
        // Always handle ASCII.
        if (c < 128) {
            if (c >= 0x20 && c <= 0x7e) {
                uint8_t tile_index = fontio_builtinfont_get_glyph_index(self->font, c);
                common_hal_displayio_tilegrid_set_tile(self->scroll_area, self->cursor_x, self->cursor_y, tile_index);
                self->cursor_x++;
            } else if (c == '\r') {
                self->cursor_x = 0;
            } else if (c == '\n') {
                self->cursor_y++;
                // Commands below are used by MicroPython in the REPL
            } else if (c == '\b') {
                if (self->cursor_x > 0) {
                    self->cursor_x--;
                }
            } else if (c == 0x1b) {
                // Handle commands of the form [ESC].<digits><command-char> where . is not yet known.
                uint16_t n = 0;
                uint8_t j = 1;
                for (; j < 6; j++) {
                    if ('0' <= i[j] && i[j] <= '9') {
                        n = n * 10 + (i[j] - '0');
                    } else {
                        c = i[j];
                        break;
                    }
                }
                if (i[0] == '[') {
                    if (i[1] == 'K') {
                        // Clear the rest of the line.
                        for (uint16_t k = self->cursor_x; k < self->scroll_area->width_in_tiles; k++) {
                            common_hal_displayio_tilegrid_set_tile(self->scroll_area, k, self->cursor_y, 0);
                        }
                        i += 2;
                    } else {
                        if (c == 'D') {
                            if (n > self->cursor_x) {
                                self->cursor_x = 0;
                            } else {
                                self->cursor_x -= n;
                            }
                        }
                        if (c == 'J') {
                            if (n == 2) {
                                common_hal_displayio_tilegrid_set_top_left(self->scroll_area, 0, 0);
                                self->cursor_x = self->cursor_y = start_y = 0;
                                n = 0;
                                common_hal_displayio_tilegrid_set_all_tiles(self->scroll_area, 0);
                            }
                        }
                        if (c == ';') {
                            uint16_t m = 0;
                            for (++j; j < 9; j++) {
                                if ('0' <= i[j] && i[j] <= '9') {
                                    m = m * 10 + (i[j] - '0');
                                } else {
                                    c = i[j];
                                    break;
                                }
                            }
                            if (c == 'H') {
                                if (n > 0) {
                                    n--;
                                }
                                if (m > 0) {
                                    m--;
                                }
                                if (n >= self->scroll_area->height_in_tiles) {
                                    n = self->scroll_area->height_in_tiles - 1;
                                }
                                if (m >= self->scroll_area->width_in_tiles) {
                                    m = self->scroll_area->width_in_tiles - 1;
                                }
                                n = (n + self->scroll_area->top_left_y) % self->scroll_area->height_in_tiles;
                                self->cursor_x = m;
                                self->cursor_y = n;
                                start_y = self->cursor_y;
                            }
                        }
                        i += j + 1;
                        continue;
                    }
                } else if (i[0] == ']' && c == ';') {
                    self->in_osc_command = true;
                    self->osc_command = n;
                    i += j + 1;
                }
            }
        } else {
            uint8_t tile_index = fontio_builtinfont_get_glyph_index(self->font, c);
            if (tile_index != 0xff) {
                common_hal_displayio_tilegrid_set_tile(self->scroll_area, self->cursor_x, self->cursor_y, tile_index);
                self->cursor_x++;

            }
        }
        if (self->cursor_x >= self->scroll_area->width_in_tiles) {
            self->cursor_y++;
            self->cursor_x %= self->scroll_area->width_in_tiles;
        }
        if (self->cursor_y >= self->scroll_area->height_in_tiles) {
            self->cursor_y %= self->scroll_area->height_in_tiles;
        }
        if (self->cursor_y != start_y) {
            // clear the new row in case of scroll up
            if (self->cursor_y == self->scroll_area->top_left_y) {
                for (uint16_t j = 0; j < self->scroll_area->width_in_tiles; j++) {
                    common_hal_displayio_tilegrid_set_tile(self->scroll_area, j, self->cursor_y, 0);
                }
                common_hal_displayio_tilegrid_set_top_left(self->scroll_area, 0, (self->cursor_y + self->scroll_area->height_in_tiles + 1) % self->scroll_area->height_in_tiles);
            }
            start_y = self->cursor_y;
        }
    }
    return i - data;
}

bool common_hal_terminalio_terminal_ready_to_tx(terminalio_terminal_obj_t *self) {
    return self->scroll_area != NULL;
}
