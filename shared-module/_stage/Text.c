// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Radomir Dopieralski
//
// SPDX-License-Identifier: MIT

#include "Text.h"
#include "__init__.h"


// Get the color of the pixel on the text.
uint16_t get_text_pixel(text_obj_t *text, int16_t x, int16_t y) {

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
