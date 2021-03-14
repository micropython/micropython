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
#include "shared-module/displayio/Bitmap.h"

#include "py/runtime.h"

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

    if (destination->read_only) {
        mp_raise_RuntimeError(translate("Read-only object"));
    }

    // Ensure x1 < x2 and y1 < y2
    if (x1 > x2) {
        int16_t temp=x2;
        x2=x1;
        x1=temp;
    }
    if (y1 > y2) {
        int16_t temp=y2;
        y2=y1;
        y1=temp;
    }

    // constrain to bitmap dimensions
    x1 = constrain(x1, 0, destination->width);
    x2 = constrain(x2, 0, destination->width);
    y1 = constrain(y1, 0, destination->height);
    y2 = constrain(y2, 0, destination->height);

    // update the dirty rectangle
    displayio_bitmap_set_dirty_area(destination, x1, y1, x2, y2);

    int16_t x, y;
    for (x = x1; x < x2; x++) {
        for (y = y1; y < y2; y++ ) {
            displayio_bitmap_write_pixel(destination, x, y, value);
        }
    }
}

void common_hal_bitmaptools_draw_line(displayio_bitmap_t *destination,
                                 int16_t x0, int16_t y0,
                                 int16_t x1, int16_t y1,
                                 uint32_t value) {

    if (destination->read_only) {
        mp_raise_RuntimeError(translate("Read-only object"));
    }

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

    xbb0 = constrain(xbb0, 0, destination->width);
    xbb1 = constrain(xbb1, 0, destination->width);
    ybb0 = constrain(ybb0, 0, destination->height);
    ybb1 = constrain(ybb1, 0, destination->height);

    displayio_bitmap_set_dirty_area(destination, xbb0, ybb0, xbb1, ybb1);

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
    }
    else if (y0 == y1) { // horizontal line
        if (x0 > x1) { // ensure y1 > y0
            temp = x0;
            x0 = x1;
            x1 = temp;
        }
        for (x = x0; x < (x1 + 1); x++) { // write a horizontal line
            displayio_bitmap_write_pixel(destination, x, y0, value);
        }
    }
    else {
        bool steep;
        steep = ( abs(y1 - y0) > abs(x1 - x0) );

        if ( steep ) { // flip x0<->y0 and x1<->y1
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
        }
        else {
            ystep = -1;
        }

        for (x = x0; x < (x1 + 1); x++) {
            if (steep) {
                displayio_bitmap_write_pixel(destination, y0, x, value);
            }
            else {
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
