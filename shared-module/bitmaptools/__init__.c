// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Kevin Matocha, Jose David Montoya
//
// SPDX-License-Identifier: MIT

#include "shared/runtime/interrupt_char.h"
#include "shared-bindings/bitmaptools/__init__.h"
#include "shared-bindings/displayio/Bitmap.h"
#include "shared-bindings/displayio/Palette.h"
#include "shared-bindings/displayio/ColorConverter.h"
#include "shared-module/displayio/Bitmap.h"

#include "py/mperrno.h"
#include "py/runtime.h"
#include "py/stream.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BITMAP_DEBUG(...) (void)0
// #define BITMAP_DEBUG(...) mp_printf(&mp_plat_print, __VA_ARGS__)

void common_hal_bitmaptools_rotozoom(displayio_bitmap_t *self, int16_t ox, int16_t oy,
    int16_t dest_clip0_x, int16_t dest_clip0_y,
    int16_t dest_clip1_x, int16_t dest_clip1_y,
    displayio_bitmap_t *source, int16_t px, int16_t py,
    int16_t source_clip0_x, int16_t source_clip0_y,
    int16_t source_clip1_x, int16_t source_clip1_y,
    mp_float_t angle,
    mp_float_t scale,
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


    int16_t x, y;

    int16_t minx = dest_clip1_x;
    int16_t miny = dest_clip1_y;
    int16_t maxx = dest_clip0_x;
    int16_t maxy = dest_clip0_y;

    mp_float_t sinAngle = MICROPY_FLOAT_C_FUN(sin)(angle);
    mp_float_t cosAngle = MICROPY_FLOAT_C_FUN(cos)(angle);

    mp_float_t dx, dy;

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

    mp_float_t dvCol = cosAngle / scale;
    mp_float_t duCol = sinAngle / scale;

    mp_float_t duRow = dvCol;
    mp_float_t dvRow = -duCol;

    mp_float_t startu = px - (ox * dvCol + oy * duCol);
    mp_float_t startv = py - (ox * dvRow + oy * duRow);

    mp_float_t rowu = startu + miny * duCol;
    mp_float_t rowv = startv + miny * dvCol;

    displayio_area_t dirty_area = {minx, miny, maxx + 1, maxy + 1, NULL};
    displayio_bitmap_set_dirty_area(self, &dirty_area);

    for (y = miny; y <= maxy; y++) {
        mp_float_t u = rowu + minx * duRow;
        mp_float_t v = rowv + minx * dvRow;
        for (x = minx; x <= maxx; x++) {
            if (u >= source_clip0_x && u < source_clip1_x && v >= source_clip0_y && v < source_clip1_y) {
                uint32_t c = common_hal_displayio_bitmap_get_pixel(source, (int)u, (int)v);
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

void common_hal_bitmaptools_fill_region(displayio_bitmap_t *destination,
    int16_t x1, int16_t y1,
    int16_t x2, int16_t y2,
    uint32_t value) {
    // writes the value (a bitmap color index) into a bitmap in the specified rectangular region
    //
    // input checks should ensure that x1 < x2 and y1 < y2 and are within the bitmap region

    displayio_area_t area = { x1, y1, x2, y2, NULL };
    displayio_area_canon(&area);

    displayio_area_t bitmap_area = { 0, 0, destination->width, destination->height, NULL };
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

void common_hal_bitmaptools_boundary_fill(displayio_bitmap_t *destination,
    int16_t x, int16_t y,
    uint32_t fill_color_value, uint32_t replaced_color_value) {

    if (fill_color_value == replaced_color_value) {
        // There is nothing to do
        return;
    }

    uint32_t current_point_color_value;

    // the list of points that we'll check
    mp_obj_t fill_area = mp_obj_new_list(0, NULL);

    // first point is the one user passed in
    mp_obj_t point[] = { mp_obj_new_int(x), mp_obj_new_int(y) };
    mp_obj_list_append(
        fill_area,
        mp_obj_new_tuple(2, point)
        );

    int16_t minx = x;
    int16_t miny = y;
    int16_t maxx = x;
    int16_t maxy = y;

    if (replaced_color_value == INT_MAX) {
        current_point_color_value = common_hal_displayio_bitmap_get_pixel(
            destination,
            mp_obj_get_int(point[0]),
            mp_obj_get_int(point[1]));
        replaced_color_value = (uint32_t)current_point_color_value;
    }

    mp_obj_t *fill_points;
    size_t list_length = 0;
    mp_obj_list_get(fill_area, &list_length, &fill_points);

    mp_obj_t current_point;

    size_t tuple_len = 0;
    mp_obj_t *tuple_items;

    int cur_x, cur_y;

    // while there are still points to check
    while (list_length > 0) {
        mp_obj_list_get(fill_area, &list_length, &fill_points);
        current_point = mp_obj_list_pop(fill_area, 0);
        mp_obj_tuple_get(current_point, &tuple_len, &tuple_items);
        current_point_color_value = common_hal_displayio_bitmap_get_pixel(
            destination,
            mp_obj_get_int(tuple_items[0]),
            mp_obj_get_int(tuple_items[1]));

        // if the current point is not background color ignore it
        if (current_point_color_value != replaced_color_value) {
            mp_obj_list_get(fill_area, &list_length, &fill_points);
            continue;
        }

        cur_x = mp_obj_int_get_checked(tuple_items[0]);
        cur_y = mp_obj_int_get_checked(tuple_items[1]);

        if (cur_x < minx) {
            minx = (int16_t)cur_x;
        }
        if (cur_x > maxx) {
            maxx = (int16_t)cur_x;
        }
        if (cur_y < miny) {
            miny = (int16_t)cur_y;
        }
        if (cur_y > maxy) {
            maxy = (int16_t)cur_y;
        }

        // fill the current point with fill color
        displayio_bitmap_write_pixel(
            destination,
            mp_obj_get_int(tuple_items[0]),
            mp_obj_get_int(tuple_items[1]),
            fill_color_value);

        // add all 4 surrounding points to the list to check

        // ignore points outside of the bitmap
        if (mp_obj_int_get_checked(tuple_items[1]) - 1 >= 0) {
            mp_obj_t above_point[] = {
                tuple_items[0],
                MP_OBJ_NEW_SMALL_INT(mp_obj_int_get_checked(tuple_items[1]) - 1)
            };
            mp_obj_list_append(
                fill_area,
                mp_obj_new_tuple(2, above_point));
        }

        // ignore points outside of the bitmap
        if (mp_obj_int_get_checked(tuple_items[0]) - 1 >= 0) {
            mp_obj_t left_point[] = {
                MP_OBJ_NEW_SMALL_INT(mp_obj_int_get_checked(tuple_items[0]) - 1),
                tuple_items[1]
            };
            mp_obj_list_append(
                fill_area,
                mp_obj_new_tuple(2, left_point));
        }

        // ignore points outside of the bitmap
        if (mp_obj_int_get_checked(tuple_items[0]) + 1 < destination->width) {
            mp_obj_t right_point[] = {
                MP_OBJ_NEW_SMALL_INT(mp_obj_int_get_checked(tuple_items[0]) + 1),
                tuple_items[1]
            };
            mp_obj_list_append(
                fill_area,
                mp_obj_new_tuple(2, right_point));
        }

        // ignore points outside of the bitmap
        if (mp_obj_int_get_checked(tuple_items[1]) + 1 < destination->height) {
            mp_obj_t below_point[] = {
                tuple_items[0],
                MP_OBJ_NEW_SMALL_INT(mp_obj_int_get_checked(tuple_items[1]) + 1)
            };
            mp_obj_list_append(
                fill_area,
                mp_obj_new_tuple(2, below_point));
        }

        mp_obj_list_get(fill_area, &list_length, &fill_points);
        RUN_BACKGROUND_TASKS;
        if (mp_hal_is_interrupted()) {
            return;
        }
    }

    // set dirty the area so displayio will draw
    displayio_area_t area = { minx, miny, maxx + 1, maxy + 1, NULL};
    displayio_bitmap_set_dirty_area(destination, &area);

}

static void draw_line(displayio_bitmap_t *destination,
    int16_t x0, int16_t y0,
    int16_t x1, int16_t y1,
    uint32_t value) {

    int16_t temp, x, y;

    if (x0 == x1) { // vertical line
        if (y0 > y1) { // ensure y1 > y0
            temp = y0;
            y0 = y1;
            y1 = temp;
        }
        y0 = MAX(0, y0); // only draw inside bitmap
        y1 = MIN(y1, destination->height - 1);
        for (y = y0; y < (y1 + 1); y++) { // write a horizontal line
            displayio_bitmap_write_pixel(destination, x0, y, value);
        }
    } else if (y0 == y1) { // horizontal line
        if (x0 > x1) { // ensure y1 > y0
            temp = x0;
            x0 = x1;
            x1 = temp;
        }
        x0 = MAX(0, x0); // only draw inside bitmap
        x1 = MIN(x1, destination->width - 1);
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

        mp_float_t err = dx / 2;

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
    displayio_area_t area = { xbb0, ybb0, xbb1, ybb1, NULL };
    displayio_area_t bitmap_area = { 0, 0, destination->width, destination->height, NULL };
    displayio_area_compute_overlap(&area, &bitmap_area, &area);

    displayio_bitmap_set_dirty_area(destination, &area);

    draw_line(destination, x0, y0, x1, y1, value);
}

static int32_t ith(void *data, size_t i, int element_size) {
    switch (element_size) {
        default:
        case 1:
            return *((int8_t *)data + i);
        case 2:
            return *((int16_t *)data + i);
        case 4:
            return *((int32_t *)data + i);
    }
}

void common_hal_bitmaptools_draw_polygon(displayio_bitmap_t *destination, void *xs, void *ys, size_t points_len, int point_size, uint32_t value, bool close) {
    int16_t x0, y0, xmin, xmax, ymin, ymax, xprev, yprev, x, y;
    x0 = ith(xs, 0, point_size);
    xmin = x0;
    xmax = x0;
    xprev = x0;
    y0 = ith(ys, 0, point_size);
    ymin = y0;
    ymax = y0;
    yprev = y0;

    for (size_t i = 1; i < points_len; i++) {
        x = ith(xs, i, point_size);
        y = ith(ys, i, point_size);
        draw_line(destination, xprev, yprev, x, y, value);
        xprev = x;
        yprev = y;
        xmin = MIN(xmin, x);
        xmax = MAX(xmax, x);
        ymin = MIN(ymin, y);
        ymax = MAX(ymax, y);
    }
    if (close) {
        draw_line(destination, xprev, yprev, x0, y0, value);
    }

    displayio_area_t area = { xmin, ymin, xmax, ymax, NULL };
    displayio_area_t bitmap_area = { 0, 0, destination->width, destination->height, NULL };
    displayio_area_compute_overlap(&area, &bitmap_area, &area);
    displayio_bitmap_set_dirty_area(destination, &area);
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
    displayio_area_t area = { x1, y1, x2, y2, NULL };
    displayio_bitmap_set_dirty_area(self, &area);
}

void common_hal_bitmaptools_readinto(displayio_bitmap_t *self, mp_obj_t *file, int element_size, int bits_per_pixel, bool reverse_pixels_in_element, bool swap_bytes, bool reverse_rows) {
    uint32_t mask = (1 << common_hal_displayio_bitmap_get_bits_per_value(self)) - 1;

    const mp_stream_p_t *file_proto = mp_get_stream_raise(file, MP_STREAM_OP_READ);

    displayio_area_t a = {0, 0, self->width, self->height, NULL};
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


        int error = 0;
        mp_uint_t bytes_read = file_proto->read(file, rowdata32, rowsize, &error);
        if (error) {
            mp_raise_OSError(error);
        }
        if (bytes_read != rowsize) {
            mp_raise_msg(&mp_type_EOFError, NULL);
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
                    value = (rowdata8[x * 3] << 16) | (rowdata8[x * 3 + 1] << 8) | rowdata8[x * 3 + 2];
                    break;

                case 32:
                    value = rowdata32[x];
                    break;
            }
            displayio_bitmap_write_pixel(self, x, y_draw, value & mask);
        }
    }
}

typedef struct {
    uint8_t count; // The number of items in terms[]
    uint8_t mx; // the maximum of the absolute value of the dx values
    uint8_t dl; // the scaled dither value applied to the pixel at distance [1,0]
    struct { // dl is the scaled dither values applied to the pixel at [dx,dy]
        int8_t dx, dy, dl;
    } terms[];
} bitmaptools_dither_algorithm_info_t;

static bitmaptools_dither_algorithm_info_t atkinson = {
    4, 2, 256 / 8, {
        {2, 0, 256 / 8},
        {-1, 1, 256 / 8},
        {0, 1, 256 / 8},
        {0, 2, 256 / 8},
    }
};

static bitmaptools_dither_algorithm_info_t floyd_stenberg = {
    3, 1, 7 * 256 / 16,
    {
        {-1, 1, 3 * 256 / 16},
        {0, 1, 5 * 256 / 16},
        {1, 1, 1 * 256 / 16},
    }
};

bitmaptools_dither_algorithm_info_t *algorithms[] = {
    [DITHER_ALGORITHM_ATKINSON] = &atkinson,
    [DITHER_ALGORITHM_FLOYD_STENBERG] = &floyd_stenberg,
};

enum {
    SWAP_BYTES = 1 << 0,
    SWAP_RB = 1 << 1,
};

static void fill_row(displayio_bitmap_t *bitmap, int swap, int16_t *luminance_data, int y, int mx) {
    if (y >= bitmap->height) {
        return;
    }

    // zero out padding area
    for (int i = 0; i < mx; i++) {
        luminance_data[-mx + i] = 0;
        luminance_data[bitmap->width + i] = 0;
    }

    if (bitmap->bits_per_value == 8) {
        uint8_t *pixel_data = (uint8_t *)(bitmap->data + bitmap->stride * y);
        for (int x = 0; x < bitmap->width; x++) {
            *luminance_data++ = *pixel_data++;
        }
    } else {
        uint16_t *pixel_data = (uint16_t *)(bitmap->data + bitmap->stride * y);
        for (int x = 0; x < bitmap->width; x++) {
            uint16_t pixel = *pixel_data++;
            if (swap & SWAP_BYTES) {
                pixel = __builtin_bswap16(pixel);
            }
            int r = (pixel >> 8) & 0xf8;
            int g = (pixel >> 3) & 0xfc;
            int b = (pixel << 3) & 0xf8;

            if (swap & SWAP_RB) {
                uint8_t tmp = r;
                r = b;
                b = tmp;
            }

            // ideal coefficients are around .299, .587, .114 (according to
            // ppmtopnm), this differs from the 'other' luma-converting
            // function in circuitpython (why?)

            // we correct for the fact that the input ranges are 0..0xf8 (or
            // 0xfc) rather than 0x00..0xff
            // Check: (0xf8 *  78 + 0xfc * 154 + 0xf8 * 29) // 256 == 255
            *luminance_data++ = (r * 78 + g * 154 + b * 29) / 256;
        }
    }
}

static void write_pixels(displayio_bitmap_t *bitmap, int y, bool *data) {
    if (bitmap->bits_per_value == 1) {
        uint32_t *pixel_data = (uint32_t *)(bitmap->data + bitmap->stride * y);
        for (int i = 0; i < bitmap->width; i++) {
            uint32_t p = 0;
            for (int j = 0; j < 32; j++) {
                p = (p << 1);
                if (*data++) {
                    p |= 1;
                }
            }
            *pixel_data++ = p;
        }
    } else {
        uint16_t *pixel_data = (uint16_t *)(bitmap->data + bitmap->stride * y);
        for (int i = 0; i < bitmap->width; i++) {
            *pixel_data++ = *data++ ? 65535 : 0;
        }
    }
}

void common_hal_bitmaptools_dither(displayio_bitmap_t *dest_bitmap, displayio_bitmap_t *source_bitmap, displayio_colorspace_t colorspace, bitmaptools_dither_algorithm_t algorithm) {
    int height = dest_bitmap->height, width = dest_bitmap->width;

    int swap = 0;
    if (colorspace == DISPLAYIO_COLORSPACE_RGB565_SWAPPED || colorspace == DISPLAYIO_COLORSPACE_BGR565_SWAPPED) {
        swap |= SWAP_BYTES;
    }
    if (colorspace == DISPLAYIO_COLORSPACE_BGR565 || colorspace == DISPLAYIO_COLORSPACE_BGR565_SWAPPED) {
        swap |= SWAP_RB;
    }

    bitmaptools_dither_algorithm_info_t *info = algorithms[algorithm];
    // rowdata holds 3 rows of data.  Each one is larger than the input
    // bitmap's width, because `mx` extra pixels are allocated at the start and
    // end of the row so that no conditionals are needed when storing the error data.
    int16_t rowdata[(width + 2 * info->mx) * 3];
    int16_t *rows[3] = {
        rowdata + info->mx, rowdata + width + info->mx * 3, rowdata + 2 * width + info->mx * 5
    };
    // out holds one output row of pixels, and is padded to be a multiple of 32 so that the 1bpp storage loop can be simplified
    bool out[(width + 31) / 32 * 32];

    fill_row(source_bitmap, swap, rows[0], 0, info->mx);
    fill_row(source_bitmap, swap, rows[1], 1, info->mx);
    fill_row(source_bitmap, swap, rows[2], 2, info->mx);

    int16_t err = 0;

    for (int y = 0; y < height; y++) {

        // Serpentine dither.  Going left-to-right...
        for (int x = 0; x < width; x++) {
            int32_t pixel_in = rows[0][x] + err;
            bool pixel_out = pixel_in >= 128;
            out[x] = pixel_out;

            err = pixel_in - (pixel_out ? 255 : 0);

            for (int i = 0; i < info->count; i++) {
                int x1 = x + info->terms[i].dx;
                int dy = info->terms[i].dy;

                rows[dy][x1] = ((info->terms[i].dl * err) / 256) + rows[dy][x1];
            }
            err = (err * info->dl) / 256;
        }
        write_pixels(dest_bitmap, y, out);

        // Cycle the rows by shuffling pointers, this is faster than copying the data.
        int16_t *tmp = rows[0];
        rows[0] = rows[1];
        rows[1] = rows[2];
        rows[2] = tmp;

        y++;
        if (y == height) {
            break;
        }

        fill_row(source_bitmap, swap, rows[2], y + 2, info->mx);

        // Serpentine dither.   Going right-to-left...
        for (int x = width; x--;) {
            int16_t pixel_in = rows[0][x] + err;
            bool pixel_out = pixel_in >= 128;
            out[x] = pixel_out;
            err = pixel_in - (pixel_out ? 255 : 0);

            for (int i = 0; i < info->count; i++) {
                int x1 = x - info->terms[i].dx;
                int dy = info->terms[i].dy;

                rows[dy][x1] = ((info->terms[i].dl * err) / 256) + rows[dy][x1];
            }
            err = (err * info->dl) / 256;
        }
        write_pixels(dest_bitmap, y, out);

        tmp = rows[0];
        rows[0] = rows[1];
        rows[1] = rows[2];
        rows[2] = tmp;

        fill_row(source_bitmap, swap, rows[2], y + 3, info->mx);
    }

    displayio_area_t a = { 0, 0, width, height, NULL };
    displayio_bitmap_set_dirty_area(dest_bitmap, &a);
}

void common_hal_bitmaptools_alphablend(displayio_bitmap_t *dest, displayio_bitmap_t *source1, displayio_bitmap_t *source2, displayio_colorspace_t colorspace, mp_float_t factor1, mp_float_t factor2,
    bitmaptools_blendmode_t blendmode, uint32_t skip_source1_index, bool skip_source1_index_none, uint32_t skip_source2_index, bool skip_source2_index_none) {
    displayio_area_t a = {0, 0, dest->width, dest->height, NULL};
    displayio_bitmap_set_dirty_area(dest, &a);

    int ifactor1 = (int)(factor1 * 256);
    int ifactor2 = (int)(factor2 * 256);
    bool blend_source1, blend_source2;

    if (colorspace == DISPLAYIO_COLORSPACE_L8) {
        for (int y = 0; y < dest->height; y++) {
            uint8_t *dptr = (uint8_t *)(dest->data + y * dest->stride);
            uint8_t *sptr1 = (uint8_t *)(source1->data + y * source1->stride);
            uint8_t *sptr2 = (uint8_t *)(source2->data + y * source2->stride);
            int pixel;
            for (int x = 0; x < dest->width; x++) {
                blend_source1 = skip_source1_index_none || *sptr1 != (uint8_t)skip_source1_index;
                blend_source2 = skip_source2_index_none || *sptr2 != (uint8_t)skip_source2_index;
                if (blend_source1 && blend_source2) {
                    // Premultiply by the alpha factor
                    int sda = *sptr1++ *ifactor1;
                    int sca = *sptr2++ *ifactor2;
                    // Blend
                    int blend;
                    if (blendmode == BITMAPTOOLS_BLENDMODE_SCREEN) {
                        blend = sca + sda - (sca * sda / 65536);
                    } else {
                        blend = sca + sda * (256 - ifactor2) / 256;
                    }
                    // Divide by the alpha factor
                    pixel = (blend / (ifactor1 + ifactor2 - ifactor1 * ifactor2 / 256));
                } else if (blend_source1) {
                    // Apply iFactor1 to source1 only
                    pixel = *sptr1++ *ifactor1 / 256;
                } else if (blend_source2) {
                    // Apply iFactor2 to source1 only
                    pixel = *sptr2++ *ifactor2 / 256;
                } else {
                    // Use the destination value
                    pixel = *dptr;
                }
                *dptr++ = MIN(255, MAX(0, pixel));
            }
        }
    } else {
        bool swap = (colorspace == DISPLAYIO_COLORSPACE_RGB565_SWAPPED) || (colorspace == DISPLAYIO_COLORSPACE_BGR565_SWAPPED);
        uint16_t pixel;
        for (int y = 0; y < dest->height; y++) {
            uint16_t *dptr = (uint16_t *)(dest->data + y * dest->stride);
            uint16_t *sptr1 = (uint16_t *)(source1->data + y * source1->stride);
            uint16_t *sptr2 = (uint16_t *)(source2->data + y * source2->stride);
            for (int x = 0; x < dest->width; x++) {
                int spix1 = *sptr1++;
                int spix2 = *sptr2++;

                if (swap) {
                    spix1 = __builtin_bswap16(spix1);
                    spix2 = __builtin_bswap16(spix2);
                }
                const int r_mask = 0xf800; // (or b mask, if BGR)
                const int g_mask = 0x07e0;
                const int b_mask = 0x001f; // (or r mask, if BGR)

                blend_source1 = skip_source1_index_none || spix1 != (int)skip_source1_index;
                blend_source2 = skip_source2_index_none || spix2 != (int)skip_source2_index;

                if (blend_source1 && blend_source2) {
                    // Blend based on the SVG alpha compositing specs
                    // https://dev.w3.org/SVG/modules/compositing/master/#alphaCompositing

                    int ifactor_blend = ifactor1 + ifactor2 - ifactor1 * ifactor2 / 256;

                    // Premultiply the colors by the alpha factor
                    int red_dca = ((spix1 & r_mask) >> 8) * ifactor1;
                    int grn_dca = ((spix1 & g_mask) >> 3) * ifactor1;
                    int blu_dca = ((spix1 & b_mask) << 3) * ifactor1;

                    int red_sca = ((spix2 & r_mask) >> 8) * ifactor2;
                    int grn_sca = ((spix2 & g_mask) >> 3) * ifactor2;
                    int blu_sca = ((spix2 & b_mask) << 3) * ifactor2;

                    int red_blend, grn_blend, blu_blend;
                    if (blendmode == BITMAPTOOLS_BLENDMODE_SCREEN) {
                        // Perform a screen blend Sca + Dca - Sca × Dca
                        red_blend = red_sca + red_dca - (red_sca * red_dca / 65536);
                        grn_blend = grn_sca + grn_dca - (grn_sca * grn_dca / 65536);
                        blu_blend = blu_sca + blu_dca - (blu_sca * blu_dca / 65536);
                    } else {
                        // Perform a normal (src-over) blend
                        red_blend = red_sca + red_dca * (256 - ifactor2) / 256;
                        grn_blend = grn_sca + grn_dca * (256 - ifactor2) / 256;
                        blu_blend = blu_sca + blu_dca * (256 - ifactor2) / 256;
                    }

                    // Divide by the alpha factor
                    int r = ((red_blend / ifactor_blend) << 8) & r_mask;
                    int g = ((grn_blend / ifactor_blend) << 3) & g_mask;
                    int b = ((blu_blend / ifactor_blend) >> 3) & b_mask;

                    // Clamp to the appropriate range
                    r = MIN(r_mask, MAX(0, r)) & r_mask;
                    g = MIN(g_mask, MAX(0, g)) & g_mask;
                    b = MIN(b_mask, MAX(0, b)) & b_mask;

                    pixel = r | g | b;

                    if (swap) {
                        pixel = __builtin_bswap16(pixel);
                    }
                } else if (blend_source1) {
                    // Apply iFactor1 to source1 only
                    int r = (spix1 & r_mask) * ifactor1 / 256;
                    int g = (spix1 & g_mask) * ifactor1 / 256;
                    int b = (spix1 & b_mask) * ifactor1 / 256;
                    pixel = r | g | b;
                } else if (blend_source2) {
                    // Apply iFactor2 to source1 only
                    int r = (spix2 & r_mask) * ifactor2 / 256;
                    int g = (spix2 & g_mask) * ifactor2 / 256;
                    int b = (spix2 & b_mask) * ifactor2 / 256;
                    pixel = r | g | b;
                } else {
                    // Use the destination value
                    pixel = *dptr;
                }

                *dptr++ = pixel;
            }
        }
    }
}

static void draw_circle(displayio_bitmap_t *destination,
    int16_t x, int16_t y,
    int16_t radius, uint32_t value) {

    int16_t d, yb;

    mp_arg_validate_int_range(x, SHRT_MIN, SHRT_MAX, MP_QSTR_x);
    mp_arg_validate_int_range(y, SHRT_MIN, SHRT_MAX, MP_QSTR_y);

    x = MIN(x, destination->width);
    x = MAX(0, x);
    y = MIN(y, destination->height);
    y = MAX(0, y);

    BITMAP_DEBUG("x, y, radius    (%4d, %4d, %4d)\n", x, y, radius);

    yb = radius;
    d = 3 - 2 * radius;

    // Bresenham's circle algorithm
    for (int xb = 0; xb <= yb; xb++) {
        displayio_bitmap_write_pixel(destination, xb + x, yb + y, value);
        displayio_bitmap_write_pixel(destination, -xb + x, -yb + y, value);
        displayio_bitmap_write_pixel(destination, -xb + x, yb + y, value);
        displayio_bitmap_write_pixel(destination, xb + x, -yb + y, value);
        displayio_bitmap_write_pixel(destination, yb + x, xb + y, value);
        displayio_bitmap_write_pixel(destination, -yb + x, xb + y, value);
        displayio_bitmap_write_pixel(destination, -yb + x, -xb + y, value);
        displayio_bitmap_write_pixel(destination, yb + x, -xb + y, value);
        if (d <= 0) {
            d = d + (4 * xb) + 6;
        } else {
            d = d + 4 * (xb - yb) + 10;
            yb = yb - 1;
        }
    }
}

void common_hal_bitmaptools_draw_circle(displayio_bitmap_t *destination,
    int16_t x, int16_t y,
    int16_t radius,
    uint32_t value) {


    // update the dirty area
    int16_t xbb0, xbb1, ybb0, ybb1;

    xbb0 = x - radius;
    xbb1 = x + radius;
    ybb0 = y - radius;
    ybb1 = y + radius;

    displayio_area_t area = { xbb0, ybb0, xbb1, ybb1, NULL };
    displayio_area_t bitmap_area = { 0, 0, destination->width, destination->height, NULL };
    displayio_area_compute_overlap(&area, &bitmap_area, &area);

    displayio_bitmap_set_dirty_area(destination, &area);

    draw_circle(destination, x, y, radius, value);
}

void common_hal_bitmaptools_blit(displayio_bitmap_t *destination, displayio_bitmap_t *source, int16_t x, int16_t y,
    int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint32_t skip_source_index, bool skip_source_index_none, uint32_t skip_dest_index,
    bool skip_dest_index_none) {

    if (destination->read_only) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Read-only"));
    }
    // Copy region of "source" bitmap into "destination" bitmap at location x,y in the "destination"
    // If skip_value is encountered in the source bitmap, it will not be copied.
    // If skip_value is `None`, then all pixels are copied.
    // This function assumes input checks were performed for pixel index entries.

    // Update the dirty area
    int16_t dirty_x_max = (x + (x2 - x1));
    if (dirty_x_max > destination->width) {
        dirty_x_max = destination->width;
    }
    int16_t dirty_y_max = y + (y2 - y1);
    if (dirty_y_max > destination->height) {
        dirty_y_max = destination->height;
    }

    displayio_area_t a = { x, y, dirty_x_max, dirty_y_max, NULL};
    displayio_bitmap_set_dirty_area(destination, &a);

    bool x_reverse = false;
    bool y_reverse = false;

    // Add reverse direction option to protect blitting of destination bitmap back into destination bitmap
    if (x > x1) {
        x_reverse = true;
    }
    if (y > y1) {
        y_reverse = true;
    }

    // simplest version - use internal functions for get/set pixels
    for (int16_t i = 0; i < (x2 - x1); i++) {

        const int xs_index = x_reverse ? ((x2) - i - 1) : x1 + i; // x-index into the source bitmap
        const int xd_index = x_reverse ? ((x + (x2 - x1)) - i - 1) : x + i; // x-index into the destination bitmap

        if ((xd_index >= 0) && (xd_index < destination->width)) {
            for (int16_t j = 0; j < (y2 - y1); j++) {

                const int ys_index = y_reverse ? ((y2) - j - 1) : y1 + j;  // y-index into the source bitmap
                const int yd_index = y_reverse ? ((y + (y2 - y1)) - j - 1) : y + j; // y-index into the destination bitmap

                if ((yd_index >= 0) && (yd_index < destination->height)) {
                    uint32_t value = common_hal_displayio_bitmap_get_pixel(source, xs_index, ys_index);
                    if (skip_dest_index_none) { // if skip_dest_index is none, then only check source skip
                        if ((skip_source_index_none) || (value != skip_source_index)) {   // write if skip_value_none is True
                            displayio_bitmap_write_pixel(destination, xd_index, yd_index, value);
                        }
                    } else { // check dest_value index against skip_dest_index and skip if they match
                        uint32_t dest_value = common_hal_displayio_bitmap_get_pixel(destination, xd_index, yd_index);
                        if (dest_value != skip_dest_index) {
                            if ((skip_source_index_none) || (value != skip_source_index)) {   // write if skip_value_none is True
                                displayio_bitmap_write_pixel(destination, xd_index, yd_index, value);
                            }
                        }
                    }
                }
            }
        }
    }
}
