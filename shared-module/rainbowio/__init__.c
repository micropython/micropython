// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Kattni Rembor
//
// SPDX-License-Identifier: MIT

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
