// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Kevin Matocha
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/displayio/Bitmap.h"
#include "shared-bindings/displayio/ColorConverter.h"
#include "shared-bindings/displayio/__init__.h"
#include "shared-module/displayio/Palette.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "extmod/vfs_fat.h"

typedef enum {
    DITHER_ALGORITHM_ATKINSON, DITHER_ALGORITHM_FLOYD_STENBERG,
} bitmaptools_dither_algorithm_t;

extern const mp_obj_type_t bitmaptools_dither_algorithm_type;

typedef enum {
    BITMAPTOOLS_BLENDMODE_NORMAL, BITMAPTOOLS_BLENDMODE_SCREEN,
} bitmaptools_blendmode_t;

extern const mp_obj_type_t bitmaptools_blendmode_type;
extern const cp_enum_obj_t bitmaptools_blendmode_Normal_obj;

void common_hal_bitmaptools_rotozoom(displayio_bitmap_t *self, int16_t ox, int16_t oy,
    int16_t dest_clip0_x, int16_t dest_clip0_y,
    int16_t dest_clip1_x, int16_t dest_clip1_y,
    displayio_bitmap_t *source, int16_t px, int16_t py,
    int16_t source_clip0_x, int16_t source_clip0_y,
    int16_t source_clip1_x, int16_t source_clip1_y,
    mp_float_t angle,
    mp_float_t scale,
    uint32_t skip_index, bool skip_index_none);

void common_hal_bitmaptools_fill_region(displayio_bitmap_t *destination,
    int16_t x1, int16_t y1,
    int16_t x2, int16_t y2,
    uint32_t value);

void common_hal_bitmaptools_boundary_fill(displayio_bitmap_t *destination,
    int16_t x, int16_t y,
    uint32_t fill_color_value, uint32_t replaced_color_value);

void common_hal_bitmaptools_draw_line(displayio_bitmap_t *destination,
    int16_t x0, int16_t y0,
    int16_t x1, int16_t y1,
    uint32_t value);

void common_hal_bitmaptools_draw_circle(displayio_bitmap_t *destination,
    int16_t x, int16_t y,
    int16_t radius,
    uint32_t value);

void common_hal_bitmaptools_blit(displayio_bitmap_t *destination, displayio_bitmap_t *source, int16_t x, int16_t y,
    int16_t x1, int16_t y1, int16_t x2, int16_t y2,
    uint32_t skip_source_index, bool skip_source_index_none, uint32_t skip_dest_index, bool skip_dest_index_none);

void common_hal_bitmaptools_draw_polygon(displayio_bitmap_t *destination, void *xs, void *ys, size_t points_len, int point_size, uint32_t value, bool close);
void common_hal_bitmaptools_readinto(displayio_bitmap_t *self, mp_obj_t *file, int element_size, int bits_per_pixel, bool reverse_pixels_in_word, bool swap_bytes, bool reverse_rows);
void common_hal_bitmaptools_arrayblit(displayio_bitmap_t *self, void *data, int element_size, int x1, int y1, int x2, int y2, bool skip_specified, uint32_t skip_index);
void common_hal_bitmaptools_dither(displayio_bitmap_t *dest_bitmap, displayio_bitmap_t *source_bitmap, displayio_colorspace_t colorspace, bitmaptools_dither_algorithm_t algorithm);

void common_hal_bitmaptools_alphablend(displayio_bitmap_t *destination, displayio_bitmap_t *source1, displayio_bitmap_t *source2, displayio_colorspace_t colorspace, mp_float_t factor1, mp_float_t factor2,
    bitmaptools_blendmode_t blendmode, uint32_t skip_source1_index, bool skip_source1_index_none, uint32_t skip_source2_index, bool skip_source2_index_none);

typedef struct {
    union {
        struct {
            int16_t x1, y1, x2, y2;
        };
        int16_t arr[4];
    };
} bitmaptools_rect_t;

#define ARGS_X1_Y1_X2_Y2 ARG_x1, ARG_y1, ARG_x2, ARG_y2
#define ALLOWED_ARGS_X1_Y1_X2_Y2(if_required1, if_required2) \
    {MP_QSTR_x1, if_required1 | MP_ARG_OBJ, {.u_obj = MP_ROM_INT(0)}}, \
    {MP_QSTR_y1, if_required2 | MP_ARG_OBJ, {.u_obj = MP_ROM_INT(0)}}, \
    {MP_QSTR_x2, if_required1 | MP_ARG_OBJ, {.u_obj = MP_ROM_NONE}}, \
    {MP_QSTR_y2, if_required2 | MP_ARG_OBJ, {.u_obj = MP_ROM_NONE}}

bitmaptools_rect_t bitmaptools_validate_coord_range_pair(const mp_arg_val_t in[4], int width, int height);
