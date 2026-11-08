// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mikey Sklar for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

// One row of the fixed-point rotozoom path: for i in [0, count), copy source pixel
// (u + i * du) >> 16 to dest_pixels[i], skipping skip_index unless skip_index_none.
// bits_per_value is 8 or 16, the same for both bitmaps, and u stays below 2^31.
//
// The default is portable C. A port may replace it with a faster version. Never raises,
// never allocates.
void bitmaptools_rotozoom_row(void *dest_pixels, const void *source_row,
    uint32_t u, uint32_t du, int count, uint8_t bits_per_value,
    uint32_t skip_index, bool skip_index_none);
