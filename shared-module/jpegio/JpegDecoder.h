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
    bool software_ok;
    #if CIRCUITPY_JPEGIO_HW
    void *hw_decoder; // port state for the hardware decoder, NULL if unavailable
    #endif
} jpegio_jpegdecoder_obj_t;

#if CIRCUITPY_JPEGIO_HW
// Ports with JPEG decode hardware provide this. Called when the decoder object is
// constructed. Sets `self->hw_decoder`, or NULL if the hardware is unavailable, in which
// case decode() uses TJpgDec. Must not raise.
void jpegio_jpegdecoder_hw_init(jpegio_jpegdecoder_obj_t *self);

// Ports with JPEG decode hardware provide this. It is called by decode() only when the
// source passed to open() is an in-memory buffer; `jpeg` is that whole buffer. The decode
// arguments (x, y, lim, skip indices) are already stored in `self`, and `self->dest` is
// the target bitmap.
// Returns true if the port decoded the image into `self->dest`. Returns false to decode
// with TJpgDec instead, which rewrites every pixel the image covers. If
// `self->software_ok` is false, raises ValueError instead of returning false.
bool jpegio_jpegdecoder_hw_decode(jpegio_jpegdecoder_obj_t *self, const mp_buffer_info_t *jpeg, int scale);

// Ports with JPEG decode hardware provide this. It releases `self->hw_decoder`, if set,
// and sets it to NULL. Called when the decoder object is collected. Must not raise.
void jpegio_jpegdecoder_hw_deinit(jpegio_jpegdecoder_obj_t *self);
#endif
