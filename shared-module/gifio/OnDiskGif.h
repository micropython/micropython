/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Mark Komus
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
