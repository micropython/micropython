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

#include "shared-module/terminalio/Terminal.h"

#include "shared-module/fontio/BuiltinFont.h"
#include "shared-bindings/displayio/TileGrid.h"

void common_hal_terminalio_terminal_construct(terminalio_terminal_obj_t *self, displayio_tilegrid_t *tilegrid, const fontio_builtinfont_t *font) {
    self->cursor_x = 0;
    self->cursor_y = 0;
    self->font = font;
    self->tilegrid = tilegrid;
    self->first_row = 0;

    for (uint16_t x = 0; x < self->tilegrid->width_in_tiles; x++) {
        for (uint16_t y = 0; y < self->tilegrid->height_in_tiles; y++) {
            common_hal_displayio_tilegrid_set_tile(self->tilegrid, x, y, 0);
        }
    }

    common_hal_displayio_tilegrid_set_top_left(self->tilegrid, 0, 1);
}

size_t common_hal_terminalio_terminal_write(terminalio_terminal_obj_t *self, const byte *data, size_t len, int *errcode) {
    const byte *i = data;
    uint16_t start_y = self->cursor_y;
    while (i < data + len) {
        unichar c = utf8_get_char(i);
        i = utf8_next_char(i);
        // Always handle ASCII.
        if (c < 128) {
            if (c >= 0x20 && c <= 0x7e) {
                uint8_t tile_index = fontio_builtinfont_get_glyph_index(self->font, c);
                common_hal_displayio_tilegrid_set_tile(self->tilegrid, self->cursor_x, self->cursor_y, tile_index);
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
                if (i[0] == '[') {
                    if (i[1] == 'K') {
                        // Clear the rest of the line.
                        for (uint16_t j = self->cursor_x; j < self->tilegrid->width_in_tiles; j++) {
                            common_hal_displayio_tilegrid_set_tile(self->tilegrid, j, self->cursor_y, 0);
                        }
                        i += 2;
                    } else {
                        // Handle commands of the form \x1b[####D
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
                        if (c == 'D') {
                            if (n > self->cursor_x) {
                                self->cursor_x = 0;
                            } else {
                                self->cursor_x -= n;
                            }
                        }
                        if (c == 'J') {
                            if (n == 2) {
                                common_hal_displayio_tilegrid_set_top_left(self->tilegrid, 0, 0);
                                self->cursor_x = self->cursor_y = start_y = 0;
                                n = 0;
                                for (uint16_t x = 0; x < self->tilegrid->width_in_tiles; x++) {
                                    for (uint16_t y = 0; y < self->tilegrid->height_in_tiles; y++) {
                                        common_hal_displayio_tilegrid_set_tile(self->tilegrid, x, y, 0);
                                    }
                                }
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
                                if (n >= self->tilegrid->height_in_tiles) {
                                    n = self->tilegrid->height_in_tiles - 1;
                                }
                                if (m >= self->tilegrid->width_in_tiles) {
                                    m = self->tilegrid->width_in_tiles - 1;
                                }
                                n = (n + self->tilegrid->top_left_y) % self->tilegrid->height_in_tiles;
                                self->cursor_x = m;
                                self->cursor_y = n;
                                start_y = self->cursor_y;
                            }
                        }
                        i += j + 1;
                        continue;
                    }
                }
            }
        } else {
            uint8_t tile_index = fontio_builtinfont_get_glyph_index(self->font, c);
            if (tile_index != 0xff) {
                common_hal_displayio_tilegrid_set_tile(self->tilegrid, self->cursor_x, self->cursor_y, tile_index);
                self->cursor_x++;

            }
        }
        if (self->cursor_x >= self->tilegrid->width_in_tiles) {
            self->cursor_y++;
            self->cursor_x %= self->tilegrid->width_in_tiles;
        }
        if (self->cursor_y >= self->tilegrid->height_in_tiles) {
            self->cursor_y %= self->tilegrid->height_in_tiles;
        }
        if (self->cursor_y != start_y) {
            // clear the new row in case of scroll up
            if (self->cursor_y == self->tilegrid->top_left_y) {
                for (uint16_t j = 0; j < self->tilegrid->width_in_tiles; j++) {
                    common_hal_displayio_tilegrid_set_tile(self->tilegrid, j, self->cursor_y, 0);
                }
                common_hal_displayio_tilegrid_set_top_left(self->tilegrid, 0, (self->cursor_y + self->tilegrid->height_in_tiles + 1) % self->tilegrid->height_in_tiles);
            }
            start_y = self->cursor_y;
        }
    }
    return i - data;
}

bool common_hal_terminalio_terminal_ready_to_tx(terminalio_terminal_obj_t *self) {
    return true;
}
