/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#include "shared-bindings/displayio/ColorConverter.h"

void common_hal_displayio_colorconverter_construct(displayio_colorconverter_t* self) {
}

bool common_hal_displayio_colorconverter_convert(displayio_colorconverter_t *self, uint32_t input_color, uint16_t* output_color) {
    // TODO(tannewt): Validate the color input against the input format.
    uint32_t r5 = (input_color >> 19);
    uint32_t g6 = (input_color >> 10) & 0x3f;
    uint32_t b5 = (input_color >> 3) & 0x1f;
    uint32_t packed = r5 << 11 | g6 << 5 | b5;
    // swap bytes
    *output_color = __builtin_bswap16(packed);
    return true;
}

// Currently no refresh logic is needed for a ColorConverter.
bool displayio_colorconverter_needs_refresh(displayio_colorconverter_t *self) {
    return false;
}

void displayio_colorconverter_finish_refresh(displayio_colorconverter_t *self) {
}
