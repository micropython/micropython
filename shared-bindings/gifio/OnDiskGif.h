// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Mark Komus
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/gifio/OnDiskGif.h"
#include "extmod/vfs_fat.h"

extern const mp_obj_type_t gifio_ondiskgif_type;

void common_hal_gifio_ondiskgif_construct(gifio_ondiskgif_t *self, pyb_file_obj_t *file, bool use_palette);

uint32_t common_hal_gifio_ondiskgif_get_pixel(gifio_ondiskgif_t *bitmap,
    int16_t x, int16_t y);

uint16_t common_hal_gifio_ondiskgif_get_height(gifio_ondiskgif_t *self);
mp_obj_t common_hal_gifio_ondiskgif_get_bitmap(gifio_ondiskgif_t *self);
mp_obj_t common_hal_gifio_ondiskgif_get_palette(gifio_ondiskgif_t *self);
uint16_t common_hal_gifio_ondiskgif_get_width(gifio_ondiskgif_t *self);
uint32_t common_hal_gifio_ondiskgif_next_frame(gifio_ondiskgif_t *self, bool setDirty);
int32_t common_hal_gifio_ondiskgif_get_duration(gifio_ondiskgif_t *self);
int32_t common_hal_gifio_ondiskgif_get_frame_count(gifio_ondiskgif_t *self);
int32_t common_hal_gifio_ondiskgif_get_min_delay(gifio_ondiskgif_t *self);
int32_t common_hal_gifio_ondiskgif_get_max_delay(gifio_ondiskgif_t *self);
void common_hal_gifio_ondiskgif_deinit(gifio_ondiskgif_t *self);
bool common_hal_gifio_ondiskgif_deinited(gifio_ondiskgif_t *self);
