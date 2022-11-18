/*
 * This file is part of the CircuitPython project, https://github.com/adafruit/circuitpython
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Kattni Rembor
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

#include "shared-bindings/rainbowio/__init__.h"

int32_t colorwheel(mp_float_t pos) {
    pos = pos - ((uint32_t)(pos / 256) * 256);
    int shift1, shift2;
    if (pos < 85) {
        shift1 = 8;
        shift2 = 16;
    } else if (pos < 170) {
        pos -= 85;
        shift1 = 0;
        shift2 = 8;
    } else {
        pos -= 170;
        shift1 = 16;
        shift2 = 0;
    }
    int p = (int)(pos * 3);
    p = (p < 256) ? p : 255;
    return (p << shift1) | ((255 - p) << shift2);
}
