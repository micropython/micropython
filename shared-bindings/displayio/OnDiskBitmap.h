// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_ONDISKBITMAP_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_ONDISKBITMAP_H

#include "shared-module/displayio/OnDiskBitmap.h"
#include "extmod/vfs_fat.h"

extern const mp_obj_type_t displayio_ondiskbitmap_type;

void common_hal_displayio_ondiskbitmap_construct(displayio_ondiskbitmap_t *self, pyb_file_obj_t *file);

uint32_t common_hal_displayio_ondiskbitmap_get_pixel(displayio_ondiskbitmap_t *bitmap,
    int16_t x, int16_t y);

uint16_t common_hal_displayio_ondiskbitmap_get_height(displayio_ondiskbitmap_t *self);
mp_obj_t common_hal_displayio_ondiskbitmap_get_pixel_shader(displayio_ondiskbitmap_t *self);
uint16_t common_hal_displayio_ondiskbitmap_get_width(displayio_ondiskbitmap_t *self);
#endif // MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_ONDISKBITMAP_H
