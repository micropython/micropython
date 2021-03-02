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


#include "shared-bindings/displayio/Bitmap.h"

#include "py/runtime.h"

#include "math.h"

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


    if (self->read_only) {
        mp_raise_RuntimeError(translate("Read-only object"));
    }

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
    if(dx < minx) minx = (int16_t)dx;
    if(dx > maxx) maxx = (int16_t)dx;
    if(dy < miny) miny = (int16_t)dy;
    if(dy > maxy) maxy = (int16_t)dy;

    dx = cosAngle * (source->width - px) * scale + sinAngle * py * scale + ox;
    dy = sinAngle * (source->width - px) * scale - cosAngle * py * scale + oy;
    if(dx < minx) minx = (int16_t)dx;
    if(dx > maxx) maxx = (int16_t)dx;
    if(dy < miny) miny = (int16_t)dy;
    if(dy > maxy) maxy = (int16_t)dy;

    dx = cosAngle * (source->width - px) * scale - sinAngle * (source->height - py) * scale + ox;
    dy = sinAngle * (source->width - px) * scale + cosAngle * (source->height - py) * scale + oy;
    if(dx < minx) minx = (int16_t)dx;
    if(dx > maxx) maxx = (int16_t)dx;
    if(dy < miny) miny = (int16_t)dy;
    if(dy > maxy) maxy = (int16_t)dy;

    dx = -cosAngle * px * scale - sinAngle * (source->height - py) * scale + ox;
    dy = -sinAngle * px * scale + cosAngle * (source->height - py) * scale + oy;
    if(dx < minx) minx = (int16_t)dx;
    if(dx > maxx) maxx = (int16_t)dx;
    if(dy < miny) miny = (int16_t)dy;
    if(dy > maxy) maxy = (int16_t)dy;

    /* Clipping */
    if(minx < dest_clip0_x) minx = dest_clip0_x;
    if(maxx > dest_clip1_x - 1) maxx = dest_clip1_x - 1;
    if(miny < dest_clip0_y) miny = dest_clip0_y;
    if(maxy > dest_clip1_y - 1) maxy = dest_clip1_y - 1;

    float dvCol = cosAngle / scale;
    float duCol = sinAngle / scale;

    float duRow = dvCol;
    float dvRow = -duCol;

    float startu = px - (ox * dvCol + oy * duCol);
    float startv = py - (ox * dvRow + oy * duRow);

    float rowu = startu + miny * duCol;
    float rowv = startv + miny * dvCol;

    for(y = miny; y <= maxy; y++) {
        float u = rowu + minx * duRow;
        float v = rowv + minx * dvRow;
        for(x = minx; x <= maxx; x++) {
            if(u >= source_clip0_x  && u < source_clip1_x && v >= source_clip0_y && v < source_clip1_y) {
                uint32_t c = common_hal_displayio_bitmap_get_pixel(source, u, v);
                if( (skip_index_none) || (c != skip_index) ) {
                    common_hal_displayio_bitmap_set_pixel(self, x, y, c);
                }
            }
            u += duRow;
            v += dvRow;
        }
        rowu += duCol;
        rowv += dvCol;
    }
}
