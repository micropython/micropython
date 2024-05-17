// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Mark Komus
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_SHARED_MODULE_DISPLAYIO_ONDISKGIF_H
#define MICROPY_INCLUDED_SHARED_MODULE_DISPLAYIO_ONDISKGIF_H

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"

#include "lib/AnimatedGIF/AnimatedGIF_circuitpy.h"
#include "shared-module/displayio/Bitmap.h"
#include "shared-module/displayio/Palette.h"

#include "extmod/vfs_fat.h"

typedef struct {
    mp_obj_base_t base;
    GIFIMAGE gif;
    pyb_file_obj_t *file;
    displayio_bitmap_t *bitmap;
    displayio_palette_t *palette;
    int32_t duration;
    int32_t frame_count;
    int32_t min_delay;
    int32_t max_delay;
} gifio_ondiskgif_t;

#endif // MICROPY_INCLUDED_SHARED_MODULE_DISPLAYIO_ONDISKGIF_H
