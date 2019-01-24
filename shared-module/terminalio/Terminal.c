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

#include "shared-bindings/displayio/TileGrid.h"

void common_hal_terminalio_terminal_construct(terminalio_terminal_obj_t *self, displayio_tilegrid_t* tilegrid, uint8_t* unicode_characters, uint16_t unicode_characters_len) {
    self->cursor_x = 0;
    self->cursor_y = 0;
    self->tilegrid = tilegrid;
    self->unicode_characters = unicode_characters;
    self->unicode_characters_len = unicode_characters_len;
}

size_t common_hal_terminalio_terminal_write(terminalio_terminal_obj_t *self, const byte *data, size_t len, int *errcode) {
    const byte* i = data;
    while (i < data + len) {
        unichar c = utf8_get_char(i);
        i = utf8_next_char(i);
        // Always handle ASCII.
        if (c < 128) {
            if (c >= 0x20 && c <= 0x7e) {
                common_hal_displayio_textgrid_set_tile(self->tilegrid, self->cursor_x, self->cursor_y, c - 0x20);
                self->cursor_x++;
            }
            if (c == '\r') {
                self->cursor_x = 0;
            } else if (c == '\n') {
                self->cursor_y++;
            }
        } else {
            // Do a linear search of the mapping for unicode.
            const byte* j = self->unicode_characters;
            uint8_t k = 0;
            while (j < self->unicode_characters + self->unicode_characters_len) {
                unichar potential_c = utf8_get_char(j);
                j = utf8_next_char(j);
                if (c == potential_c) {
                    common_hal_displayio_textgrid_set_tile(self->tilegrid, self->cursor_x, self->cursor_y, 0x7f - 0x20 + k);
                    self->cursor_x++;
                    break;
                }
            }
        }
        if (self->cursor_x >= self->tilegrid->width_in_tiles) {
            self->cursor_y++;
            self->cursor_x %= self->tilegrid->width_in_tiles;
        }
        if (self->cursor_y >= self->tilegrid->height_in_tiles) {
            self->cursor_y %= self->tilegrid->height_in_tiles;
        }
    }
    return i - data;
}

bool common_hal_terminalio_terminal_ready_to_tx(terminalio_terminal_obj_t *self) {
    return true;
}
