/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Dryw Wade Clere at SparkFun Electronics
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
#ifndef MICROPY_INCLUDED_EXTMOD_MODJPEG_H
#define MICROPY_INCLUDED_EXTMOD_MODJPEG_H

#include "py/obj.h"

// Output formats, with the same values as the framebuf module where it has them.
#define MP_JPEG_FORMAT_RGB565 (1)
#define MP_JPEG_FORMAT_GS8 (6)
#define MP_JPEG_FORMAT_RGB888 (7)

// State of a decode.  The port's decoder fills in the image layout, and modjpeg.c
// the destination.
typedef struct _mp_jpeg_decode_t {
    // Image layout.  Each MCU (minimum coded unit) is a run of 8x8 blocks of 8-bit
    // samples: the luma blocks in raster order, then one Cb and one Cr block (or just
    // one luma block for greyscale).
    uint16_t width;
    uint16_t height;
    uint8_t mcu_w;          // MCU size in pixels
    uint8_t mcu_h;
    uint8_t num_blocks;     // blocks per MCU
    uint8_t y_blocks_h;     // luma blocks per MCU horizontally
    uint8_t chroma_shift_x; // log2 of the chroma subsampling
    uint8_t chroma_shift_y;
    bool gray;

    // Destination.
    uint8_t *dest;
    int dest_w;
    int dest_h;
    int dest_x;
    int dest_y;
    int scale;
    int format;
    bool swap;

    // Position of the next MCU, in image pixels.
    int mcu_x;
    int mcu_y;
} mp_jpeg_decode_t;

// Implemented by the port: decode the baseline JPEG in src, which is word aligned and
// has Huffman tables, filling in the layout of d before calling mp_jpeg_output_mcu()
// for each MCU in raster order.  Raises on error.
void mp_jpeg_port_decode(mp_jpeg_decode_t *d, const uint8_t *src, size_t len);

// Draw an MCU into the destination.
void mp_jpeg_output_mcu(mp_jpeg_decode_t *d, const uint8_t *mcu);

#endif // MICROPY_INCLUDED_EXTMOD_MODJPEG_H
