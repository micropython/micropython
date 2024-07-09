// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/stream.h"
#include "shared-module/displayio/Bitmap.h"
#include "shared-bindings/bitmaptools/__init__.h"

extern const mp_obj_type_t jpegio_jpegdecoder_type;

typedef struct jpegio_jpegdecoder_obj jpegio_jpegdecoder_obj_t;

void common_hal_jpegio_jpegdecoder_construct(jpegio_jpegdecoder_obj_t *self);
void common_hal_jpegio_jpegdecoder_close(jpegio_jpegdecoder_obj_t *self);
mp_obj_t common_hal_jpegio_jpegdecoder_set_source_buffer(jpegio_jpegdecoder_obj_t *self, mp_obj_t jpeg_data);
mp_obj_t common_hal_jpegio_jpegdecoder_set_source_file(jpegio_jpegdecoder_obj_t *self, mp_obj_t file_obj);
void common_hal_jpegio_jpegdecoder_decode_into(
    jpegio_jpegdecoder_obj_t *self,
    displayio_bitmap_t *bitmap, int scale, int16_t x, int16_t y,
    bitmaptools_rect_t *lim,
    uint32_t skip_source_index, bool skip_source_index_none,
    uint32_t skip_dest_index, bool skip_dest_index_none);
