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

#include "Layer.h"
#include "__init__.h"


// Get the color of the pixel on the layer.
uint16_t get_layer_pixel(layer_obj_t *layer, uint16_t x, uint16_t y) {

    // Shift by the layer's position offset.
    x -= layer->x;
    y -= layer->y;

    // Bounds check.
    if ((x < 0) || (x >= layer->width << 4) ||
        (y < 0) || (y >= layer->height << 4)) {
        return TRANSPARENT;
    }

    // Get the tile from the grid location or from sprite frame.
    uint8_t frame = layer->frame;
    if (layer->map) {
        uint8_t tx = x >> 4;
        uint8_t ty = y >> 4;

        frame = layer->map[(ty * layer->width + tx) >> 1];
        if (tx & 0x01) {
            frame &= 0x0f;
        } else {
            frame >>= 4;
        }
    }

    // Get the position within the tile.
    x &= 0x0f;
    y &= 0x0f;

    // Rotate the image.
    uint8_t ty = y; // Temporary variable for swapping.
    switch (layer->rotation) {
        case 1: // 90 degrees clockwise
            y = 15 - x;
            x = ty;
            break;
        case 2: // 180 degrees
            y = 15 - ty;
            x = 15 - x;
            break;
        case 3: // 90 degrees counter-clockwise
            y = x;
            x = 15 - ty;
            break;
        case 4: // 0 degrees, mirrored
            x = 15 - x;
            break;
        case 5: // 90 degrees clockwise, mirrored
            y = x;
            x = ty;
            break;
        case 6: // 180 degrees, mirrored
            y = 15 - ty;
            break;
        case 7: // 90 degrees counter-clockwise, mirrored
            y = 15 - x;
            x = 15 - ty;
            break;
        default: // 0 degrees
            break;
    }

    // Get the value of the pixel.
    uint8_t pixel = layer->graphic[(frame << 7) + (y << 3) + (x >> 1)];
    if (x & 0x01) {
        pixel &= 0x0f;
    } else {
        pixel >>= 4;
    }

    // Convert to 16-bit color using the palette.
    return layer->palette[pixel << 1] | layer->palette[(pixel << 1) + 1] << 8;
}
