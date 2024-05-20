// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "lib/tjpgd/src/tjpgd.h"
#include "shared-module/displayio/Bitmap.h"

#define TJPGD_WORKSPACE_SIZE 3500

typedef struct jpegio_jpegdecoder_obj {
    mp_obj_base_t base;
    JDEC decoder;
    byte workspace[TJPGD_WORKSPACE_SIZE];
    mp_obj_t data_obj;
    mp_buffer_info_t bufinfo;
    displayio_bitmap_t *dest;
    uint16_t x, y;
    bitmaptools_rect_t lim;
    uint32_t skip_source_index, skip_dest_index;
    bool skip_source_index_none, skip_dest_index_none;
    uint8_t scale;
} jpegio_jpegdecoder_obj_t;
