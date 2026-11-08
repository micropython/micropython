// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mikey Sklar for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// The fixed-point rotozoom row (shared-module/bitmaptools) on the SIO interpolator.
//
// Lane0 holds u in 16.16 and adds du on every pop (ADD_RAW). Its shifted and masked
// result is the byte offset of the source pixel in the row, and BASE2 is the row address,
// so POP_FULL is the source pixel address. Lane1 stays zero.
//
// Each pixel's address is popped one pixel ahead of its load, so a load never waits on
// the pop just before it. interp0 is per core and is reconfigured on every call, as
// picogame does.

#include "hardware/interp.h"
#include "shared-module/bitmaptools/__init__.h"

#define ROTOZOOM_POP() (interp0->pop[2])

#define ROTOZOOM_COPY(index, pixel) \
    do { \
        if (skip_index_none || (pixel) != skip_index) { \
            dest[index] = (pixel); \
        } \
    } while (0)

#define ROTOZOOM_ROW(type) \
    do { \
        type *dest = dest_pixels; \
        uint32_t address = ROTOZOOM_POP(); \
        int i = 0; \
        for (; i + 4 <= count; i += 4) { \
            uint32_t address1 = ROTOZOOM_POP(); \
            type pixel0 = *(const type *)address; \
            uint32_t address2 = ROTOZOOM_POP(); \
            type pixel1 = *(const type *)address1; \
            uint32_t address3 = ROTOZOOM_POP(); \
            type pixel2 = *(const type *)address2; \
            address = ROTOZOOM_POP(); \
            type pixel3 = *(const type *)address3; \
            ROTOZOOM_COPY(i, pixel0); \
            ROTOZOOM_COPY(i + 1, pixel1); \
            ROTOZOOM_COPY(i + 2, pixel2); \
            ROTOZOOM_COPY(i + 3, pixel3); \
        } \
        for (; i < count; i++) { \
            uint32_t next = ROTOZOOM_POP(); \
            ROTOZOOM_COPY(i, *(const type *)address); \
            address = next; \
        } \
    } while (0)

void bitmaptools_rotozoom_row(void *dest_pixels, const void *source_row,
    uint32_t u, uint32_t du, int count, uint8_t bits_per_value,
    uint32_t skip_index, bool skip_index_none) {
    unsigned log2_bytes = bits_per_value == 16 ? 1 : 0;
    interp_config lane0_config = interp_default_config();
    // u >> 16, times bytes per pixel.
    interp_config_set_shift(&lane0_config, 16 - log2_bytes);
    // The mask stops at bit 15 + log2_bytes: u < 2^31, and on RP2350 SHIFT is a right
    // rotate (RP2040: logical shift), so the fraction bits come back in at the top.
    interp_config_set_mask(&lane0_config, log2_bytes, 15 + log2_bytes);
    interp_config_set_add_raw(&lane0_config, true);
    interp_set_config(interp0, 0, &lane0_config);
    interp_config lane1_config = interp_default_config();
    interp_set_config(interp0, 1, &lane1_config);
    interp0->base[0] = du;
    interp0->base[1] = 0;
    interp0->base[2] = (uintptr_t)source_row;
    interp0->accum[0] = u;
    interp0->accum[1] = 0;

    if (bits_per_value == 16) {
        ROTOZOOM_ROW(uint16_t);
    } else {
        ROTOZOOM_ROW(uint8_t);
    }
}
