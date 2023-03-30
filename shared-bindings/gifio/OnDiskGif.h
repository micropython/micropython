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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_ONDISKGIF_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_ONDISKGIF_H

#include "shared-module/gifio/OnDiskGif.h"
#include "extmod/vfs_fat.h"

extern const mp_obj_type_t gifio_ondiskgif_type;

void common_hal_gifio_ondiskgif_construct(gifio_ondiskgif_t *self, pyb_file_obj_t *file);

uint32_t common_hal_gifio_ondiskgif_get_pixel(gifio_ondiskgif_t *bitmap,
    int16_t x, int16_t y);

uint16_t common_hal_gifio_ondiskgif_get_height(gifio_ondiskgif_t *self);
mp_obj_t common_hal_gifio_ondiskgif_get_bitmap(gifio_ondiskgif_t *self);
uint16_t common_hal_gifio_ondiskgif_get_width(gifio_ondiskgif_t *self);
uint32_t common_hal_gifio_ondiskgif_next_frame(gifio_ondiskgif_t *self, bool setDirty);
int32_t common_hal_gifio_ondiskgif_get_duration(gifio_ondiskgif_t *self);
int32_t common_hal_gifio_ondiskgif_get_frame_count(gifio_ondiskgif_t *self);
int32_t common_hal_gifio_ondiskgif_get_min_delay(gifio_ondiskgif_t *self);
int32_t common_hal_gifio_ondiskgif_get_max_delay(gifio_ondiskgif_t *self);
void common_hal_gifio_ondiskgif_deinit(gifio_ondiskgif_t *self);
bool common_hal_gifio_ondiskgif_deinited(gifio_ondiskgif_t *self);
#endif // MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_ONDISKGIF_H
