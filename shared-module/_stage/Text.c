/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Radomir Dopieralski
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

#include "Text.h"
#include "__init__.h"


// Get the color of the pixel on the text.
uint16_t get_text_pixel(text_obj_t *text, uint16_t x, uint16_t y) {

    // Shift by the text's position offset.
    x -= text->x;
    y -= text->y;

    // Bounds check.
    if ((x < 0) || (x >= text->width << 3) ||
        (y < 0) || (y >= text->height << 3)) {
        return TRANSPARENT;
    }

    // Get the tile from the grid location or from sprite frame.
    uint8_t tx = x >> 3;
    uint8_t ty = y >> 3;
    uint8_t c = text->chars[ty * text->width + tx];
    uint8_t color_offset = 0;
    if (c & 0x80) {
        color_offset = 4;
    }
    c &= 0x7f;
    if (!c) {
        return TRANSPARENT;
    }

    // Get the position within the char.
    x &= 0x07;
    y &= 0x07;

    // Get the value of the pixel.
    uint8_t pixel = text->font[(c << 4) + (y << 1) + (x >> 2)];
    pixel = ((pixel >> ((x & 0x03) << 1)) & 0x03) + color_offset;

    // Convert to 16-bit color using the palette.
    return text->palette[pixel << 1] | text->palette[(pixel << 1) + 1] << 8;
}
