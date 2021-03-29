/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Kevin Matocha
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


#include "shared-bindings/bitmaptools/__init__.h"
#include "shared-bindings/displayio/Bitmap.h"
#include "shared-module/displayio/Bitmap.h"

#include "py/runtime.h"
#include "py/mperrno.h"

#include "math.h"
#include "stdlib.h"

void common_hal_bitmaptools_rotozoom(displayio_bitmap_t *self, int16_t ox, int16_t oy,
    int16_t dest_clip0_x, int16_t dest_clip0_y,
    int16_t dest_clip1_x, int16_t dest_clip1_y,
    displayio_bitmap_t *source, int16_t px, int16_t py,
    int16_t source_clip0_x, int16_t source_clip0_y,
    int16_t source_clip1_x, int16_t source_clip1_y,
    float angle,
    float scale,
    uint32_t skip_index, bool skip_index_none) {

    // Copies region from source to the destination bitmap, including rotation,
    // scaling and clipping of either the source or destination regions
    //
    // *self: destination bitmap
    // ox: the (ox, oy) destination point where the source (px,py) point is placed
    // oy:
    // dest_clip0: (x,y) is the corner of the clip window on the destination bitmap
    // dest_clip1: (x,y) is the other corner of the clip window of the destination bitmap
    // *source: the source bitmap
    // px: the (px, py) point of rotation of the source bitmap
    // py:
    // source_clip0: (x,y) is the corner of the clip window on the source bitmap
    // source_clip1: (x,y) is the other of the clip window on the source bitmap
    // angle: angle of rotation in radians, positive is clockwise
    // scale: scale factor
    // skip_index: color index that should be ignored (and not copied over)
    // skip_index_none: if skip_index_none is True, then all color indexes should be copied
    //                                                     (that is, no color indexes should be skipped)


    // Copy complete "source" bitmap into "self" bitmap at location x,y in the "self"
    // Add a boolean to determine if all values are copied, or only if non-zero
    // If skip_value is encountered in the source bitmap, it will not be copied.
    // If skip_value is `None`, then all pixels are copied.


    // # Credit from https://github.com/wernsey/bitmap
    // # MIT License from
    // #  * Copyright (c) 2017 Werner Stoop <wstoop@gmail.com>
    // #
    // # *
    // # * #### `void bm_rotate_blit(Bitmap *dst, int ox, int oy, Bitmap *src, int px, int py, double angle, double scale);`
    // # *
    // # * Rotates a source bitmap `src` around a pivot point `px,py` and blits it onto a destination bitmap `dst`.
    // # *
    // # * The bitmap is positioned such that the point `px,py` on the source is at the offset `ox,oy` on the destination.
    // # *
    // # * The `angle` is clockwise, in radians. The bitmap is also scaled by the factor `scale`.
    // #
    // # void bm_rotate_blit(Bitmap *dst, int ox, int oy, Bitmap *src, int px, int py, double angle, double scale);


    // #     /*
    // #    Reference:
    // #    "Fast Bitmap Rotation and Scaling" By Steven Mortimer, Dr Dobbs' Journal, July 01, 2001
    // #    http://www.drdobbs.com/architecture-and-design/fast-bitmap-rotation-and-scaling/184416337
    // #    See also http://www.efg2.com/Lab/ImageProcessing/RotateScanline.htm
    // #    */


    int16_t x,y;

    int16_t minx = dest_clip1_x;
    int16_t miny = dest_clip1_y;
    int16_t maxx = dest_clip0_x;
    int16_t maxy = dest_clip0_y;

    float sinAngle = sinf(angle);
    float cosAngle = cosf(angle);

    float dx, dy;

    /* Compute the position of where each corner on the source bitmap
    will be on the destination to get a bounding box for scanning */
    dx = -cosAngle * px * scale + sinAngle * py * scale + ox;
    dy = -sinAngle * px * scale - cosAngle * py * scale + oy;
    if (dx < minx) {
        minx = (int16_t)dx;
    }
    if (dx > maxx) {
        maxx = (int16_t)dx;
    }
    if (dy < miny) {
        miny = (int16_t)dy;
    }
    if (dy > maxy) {
        maxy = (int16_t)dy;
    }

    dx = cosAngle * (source->width - px) * scale + sinAngle * py * scale + ox;
    dy = sinAngle * (source->width - px) * scale - cosAngle * py * scale + oy;
    if (dx < minx) {
        minx = (int16_t)dx;
    }
    if (dx > maxx) {
        maxx = (int16_t)dx;
    }
    if (dy < miny) {
        miny = (int16_t)dy;
    }
    if (dy > maxy) {
        maxy = (int16_t)dy;
    }

    dx = cosAngle * (source->width - px) * scale - sinAngle * (source->height - py) * scale + ox;
    dy = sinAngle * (source->width - px) * scale + cosAngle * (source->height - py) * scale + oy;
    if (dx < minx) {
        minx = (int16_t)dx;
    }
    if (dx > maxx) {
        maxx = (int16_t)dx;
    }
    if (dy < miny) {
        miny = (int16_t)dy;
    }
    if (dy > maxy) {
        maxy = (int16_t)dy;
    }

    dx = -cosAngle * px * scale - sinAngle * (source->height - py) * scale + ox;
    dy = -sinAngle * px * scale + cosAngle * (source->height - py) * scale + oy;
    if (dx < minx) {
        minx = (int16_t)dx;
    }
    if (dx > maxx) {
        maxx = (int16_t)dx;
    }
    if (dy < miny) {
        miny = (int16_t)dy;
    }
    if (dy > maxy) {
        maxy = (int16_t)dy;
    }

    /* Clipping */
    if (minx < dest_clip0_x) {
        minx = dest_clip0_x;
    }
    if (maxx > dest_clip1_x - 1) {
        maxx = dest_clip1_x - 1;
    }
    if (miny < dest_clip0_y) {
        miny = dest_clip0_y;
    }
    if (maxy > dest_clip1_y - 1) {
        maxy = dest_clip1_y - 1;
    }

    float dvCol = cosAngle / scale;
    float duCol = sinAngle / scale;

    float duRow = dvCol;
    float dvRow = -duCol;

    float startu = px - (ox * dvCol + oy * duCol);
    float startv = py - (ox * dvRow + oy * duRow);

    float rowu = startu + miny * duCol;
    float rowv = startv + miny * dvCol;

    displayio_area_t dirty_area = {minx, miny, maxx + 1, maxy + 1};
    displayio_bitmap_set_dirty_area(self, &dirty_area);

    for (y = miny; y <= maxy; y++) {
        float u = rowu + minx * duRow;
        float v = rowv + minx * dvRow;
        for (x = minx; x <= maxx; x++) {
            if (u >= source_clip0_x && u < source_clip1_x && v >= source_clip0_y && v < source_clip1_y) {
                uint32_t c = common_hal_displayio_bitmap_get_pixel(source, u, v);
                if ((skip_index_none) || (c != skip_index)) {
                    displayio_bitmap_write_pixel(self, x, y, c);
                }
            }
            u += duRow;
            v += dvRow;
        }
        rowu += duCol;
        rowv += dvCol;
    }
}

int16_t constrain(int16_t input, int16_t min, int16_t max) {
    // constrain the input between the min and max values
    if (input < min) {
        return min;
    }
    if (input > max) {
        return max;
    }
    return input;
}

void common_hal_bitmaptools_fill_region(displayio_bitmap_t *destination,
    int16_t x1, int16_t y1,
    int16_t x2, int16_t y2,
    uint32_t value) {
    // writes the value (a bitmap color index) into a bitmap in the specified rectangular region
    //
    // input checks should ensure that x1 < x2 and y1 < y2 and are within the bitmap region

    displayio_area_t area = { x1, y1, x2, y2 };
    displayio_area_canon(&area);

    displayio_area_t bitmap_area = { 0, 0, destination->width, destination->height };
    displayio_area_compute_overlap(&area, &bitmap_area, &area);

    // update the dirty rectangle
    displayio_bitmap_set_dirty_area(destination, &area);

    int16_t x, y;
    for (x = area.x1; x < area.x2; x++) {
        for (y = area.y1; y < area.y2; y++) {
            displayio_bitmap_write_pixel(destination, x, y, value);
        }
    }
}

void common_hal_bitmaptools_draw_line(displayio_bitmap_t *destination,
    int16_t x0, int16_t y0,
    int16_t x1, int16_t y1,
    uint32_t value) {

    //
    // adapted from Adafruit_CircuitPython_Display_Shapes.Polygon._line
    //

    // update the dirty rectangle
    int16_t xbb0, xbb1, ybb0, ybb1;
    if (x0 < x1) {
        xbb0 = x0;
        xbb1 = x1 + 1;
    } else {
        xbb0 = x1;
        xbb1 = x0 + 1;
    }
    if (y0 < y1) {
        ybb0 = y0;
        ybb1 = y1 + 1;
    } else {
        ybb0 = y1;
        ybb1 = y0 + 1;
    }
    displayio_area_t area = { xbb0, ybb0, xbb1, ybb1 };
    displayio_area_t bitmap_area = { 0, 0, destination->width, destination->height };
    displayio_area_compute_overlap(&area, &bitmap_area, &area);

    displayio_bitmap_set_dirty_area(destination, &area);

    int16_t temp, x, y;

    if (x0 == x1) { // vertical line
        if (y0 > y1) { // ensure y1 > y0
            temp = y0;
            y0 = y1;
            y1 = temp;
        }
        for (y = y0; y < (y1 + 1); y++) { // write a horizontal line
            displayio_bitmap_write_pixel(destination, x0, y, value);
        }
    } else if (y0 == y1) { // horizontal line
        if (x0 > x1) { // ensure y1 > y0
            temp = x0;
            x0 = x1;
            x1 = temp;
        }
        for (x = x0; x < (x1 + 1); x++) { // write a horizontal line
            displayio_bitmap_write_pixel(destination, x, y0, value);
        }
    } else {
        bool steep;
        steep = (abs(y1 - y0) > abs(x1 - x0));

        if (steep) {   // flip x0<->y0 and x1<->y1
            temp = x0;
            x0 = y0;
            y0 = temp;
            temp = x1;
            x1 = y1;
            y1 = temp;
        }

        if (x0 > x1) { // flip x0<->x1 and y0<->y1
            temp = x0;
            x0 = x1;
            x1 = temp;
            temp = y0;
            y0 = y1;
            y1 = temp;
        }

        int16_t dx, dy, ystep;
        dx = x1 - x0;
        dy = abs(y1 - y0);

        float err = dx / 2;

        if (y0 < y1) {
            ystep = 1;
        } else {
            ystep = -1;
        }

        for (x = x0; x < (x1 + 1); x++) {
            if (steep) {
                displayio_bitmap_write_pixel(destination, y0, x, value);
            } else {
                displayio_bitmap_write_pixel(destination, x, y0, value);
            }
            err -= dy;
            if (err < 0) {
                y0 += ystep;
                err += dx;
            }
        }
    }
}

void common_hal_bitmaptools_arrayblit(displayio_bitmap_t *self, void *data, int element_size, int x1, int y1, int x2, int y2, bool skip_specified, uint32_t skip_value) {
    uint32_t mask = (1 << common_hal_displayio_bitmap_get_bits_per_value(self)) - 1;

    for (int y = y1; y < y2; y++) {
        for (int x = x1; x < x2; x++) {
            uint32_t value;
            switch (element_size) {
                default:
                case 1:
                    value = *(uint8_t *)data;
                    data = (void *)((uint8_t *)data + 1);
                    break;
                case 2:
                    value = *(uint16_t *)data;
                    data = (void *)((uint16_t *)data + 1);
                    break;
                case 4:
                    value = *(uint32_t *)data;
                    data = (void *)((uint32_t *)data + 1);
                    break;
            }
            if (!skip_specified || value != skip_value) {
                displayio_bitmap_write_pixel(self, x, y, value & mask);
            }
        }
    }
    displayio_area_t area = { x1, y1, x2, y2 };
    displayio_bitmap_set_dirty_area(self, &area);
}

void common_hal_bitmaptools_readinto(displayio_bitmap_t *self, pyb_file_obj_t *file, int element_size, int bits_per_pixel, bool reverse_pixels_in_element, bool swap_bytes, bool reverse_rows) {
    uint32_t mask = (1 << common_hal_displayio_bitmap_get_bits_per_value(self)) - 1;

    displayio_area_t a = {0, 0, self->width, self->height};
    displayio_bitmap_set_dirty_area(self, &a);

    size_t elements_per_row = (self->width * bits_per_pixel + element_size * 8 - 1) / (element_size * 8);
    size_t rowsize = element_size * elements_per_row;
    size_t rowsize_in_u32 = (rowsize + sizeof(uint32_t) - 1) / sizeof(uint32_t);
    size_t rowsize_in_u16 = (rowsize + sizeof(uint16_t) - 1) / sizeof(uint16_t);

    for (int y = 0; y < self->height; y++) {
        uint32_t rowdata32[rowsize_in_u32];
        uint16_t *rowdata16 = (uint16_t *)rowdata32;
        uint8_t *rowdata8 = (uint8_t *)rowdata32;
        const int y_draw = reverse_rows ? (self->height) - 1 - y : y;

        UINT bytes_read = 0;
        if (f_read(&file->fp, rowdata32, rowsize, &bytes_read) != FR_OK || bytes_read != rowsize) {
            mp_raise_OSError(MP_EIO);
        }

        if (swap_bytes) {
            switch (element_size) {
                case 2:
                    for (size_t i = 0; i < rowsize_in_u16; i++) {
                        rowdata16[i] = __builtin_bswap16(rowdata16[i]);
                    }
                    break;
                case 4:
                    for (size_t i = 0; i < rowsize_in_u32; i++) {
                        rowdata32[i] = __builtin_bswap32(rowdata32[i]);
                    }
                default:
                    break;
            }
        }

        for (int x = 0; x < self->width; x++) {
            int value = 0;
            switch (bits_per_pixel) {
                case 1: {
                    int byte_offset = x / 8;
                    int bit_offset = reverse_pixels_in_element ? (7 - x % 8) : x % 8;

                    value = (rowdata8[byte_offset] >> bit_offset) & 1;
                    break;
                }
                case 2: {
                    int byte_offset = x / 4;
                    int bit_offset = 2 * (reverse_pixels_in_element ? (3 - x % 4) : x % 4);

                    value = (rowdata8[byte_offset] >> bit_offset) & 3;
                    break;
                }
                case 4: {
                    int byte_offset = x / 2;
                    int bit_offset = 4 * (reverse_pixels_in_element ? (1 - x % 2) : x % 2);

                    value = (rowdata8[byte_offset] >> bit_offset) & 0xf;
                    break;
                }
                case 8:
                    value = rowdata8[x];
                    break;

                case 16:
                    value = rowdata16[x];
                    break;

                case 24:
                    value = (rowdata8[x * 3] << 16) | (rowdata8[x * 3 + 1] << 8) | (rowdata8[x * 3 + 2] << 8);
                    break;

                case 32:
                    value = rowdata32[x];
                    break;
            }
            displayio_bitmap_write_pixel(self, x, y_draw, value & mask);
        }
    }
}
