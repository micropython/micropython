/*
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 * Copyright (c) 2024 Jeff Epler for Adafruit Industries
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 * Adapted from https://github.com/openmv/openmv/blob/master/bitmap/omv/imlib/filter.c#L2083
 */

#include <stdbool.h>
#include <math.h>

#include "py/runtime.h"

#include "shared-bindings/displayio/Bitmap.h"
#include "shared-bindings/displayio/Palette.h"
#include "shared-bindings/bitmapfilter/__init__.h"
#include "shared-module/bitmapfilter/__init__.h"

#if defined(UNIX)
#include <stdlib.h>
#define port_free free
#define port_malloc(sz, hint) (malloc(sz))
#define port_realloc realloc
#else
#include "supervisor/port_heap.h"
#endif

// Triggered by use of IM_MIN(IM_MAX(...)); this is a spurious diagnostic.
#pragma GCC diagnostic ignored "-Wshadow"

static void check_matching_details(displayio_bitmap_t *b1, displayio_bitmap_t *b2) {
    if (b1->width != b2->width || b1->height != b2->height) {
        mp_raise_ValueError(MP_ERROR_TEXT("bitmap size and depth must match"));
    }
}

size_t scratchpad_size = 0;
static void *scratchpad = NULL;

static void *scratchpad_alloc(size_t sz) {
    if (sz == 0) {
        if (scratchpad) {
            port_free(scratchpad);
        }
        scratchpad_size = sz;
        scratchpad = NULL;
    } else {
        if (scratchpad) {
            if (sz > scratchpad_size) {
                void *tmp = port_realloc(scratchpad, sz);
                if (!tmp) {
                    port_free(scratchpad);
                    scratchpad = NULL;
                } else {
                    scratchpad = tmp;
                    scratchpad_size = sz;
                }
            }
        } else {
            scratchpad = port_malloc(sz, false);
            scratchpad_size = sz;
        }
        if (!scratchpad) {
            m_malloc_fail(sz);
        }
    }
    return scratchpad;
}

static void scratch_bitmap16(displayio_bitmap_t *buf, int rows, int cols) {
    int stride = (cols + 1) / 2;
    size_t sz = rows * stride * sizeof(uint32_t);
    void *data = scratchpad_alloc(sz);
    // memset(data, 0, sz);
    buf->width = cols;
    buf->height = rows;
    buf->stride = stride;
    buf->data = data;
}

#define IM_MAX(a, b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })
#define IM_MIN(a, b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })
#define IM_DIV(a, b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _b ? (_a / _b) : 0; })
#define IM_MOD(a, b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _b ? (_a % _b) : 0; })

#define IMAGE_RGB565_LINE_LEN_BYTES(bitmap) \
    ((bitmap)->width * 2)

#define IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(bitmap, y) \
    (uint16_t *)(&(bitmap)->data[(bitmap)->stride * (y)])
#define IMAGE_GET_RGB565_PIXEL_FAST(rowptr, x) \
    __builtin_bswap16((rowptr)[(x)])
#define IMAGE_PUT_RGB565_PIXEL_FAST(rowptr, x, val) \
    ((rowptr)[(x)] = __builtin_bswap16((val)))
#define COLOR_R5_G6_B5_TO_RGB565(r, g, b) \
    (((r) << 11) | ((g) << 5) | (b))
#define COLOR_R8_G8_B8_TO_RGB565(r8, g8, b8)    ((((r8) & 0xF8) << 8) | (((g8) & 0xFC) << 3) | ((b8) >> 3))

#define COLOR_RGB565_TO_R5(pixel) (((pixel) >> 11) & 0x1F)
#define COLOR_RGB565_TO_R8(pixel) \
    ({ \
        __typeof__ (pixel) __pixel = (pixel); \
        __pixel = (__pixel >> 8) & 0xF8; \
        __pixel | (__pixel >> 5); \
    })

#define COLOR_RGB565_TO_G6(pixel) (((pixel) >> 5) & 0x3F)
#define COLOR_RGB565_TO_G8(pixel) \
    ({ \
        __typeof__ (pixel) __pixel = (pixel); \
        __pixel = (__pixel >> 3) & 0xFC; \
        __pixel | (__pixel >> 6); \
    })

#define COLOR_RGB565_TO_B5(pixel) ((pixel) & 0x1F)
#define COLOR_RGB565_TO_B8(pixel) \
    ({ \
        __typeof__ (pixel) __pixel = (pixel); \
        __pixel = (__pixel << 3) & 0xF8; \
        __pixel | (__pixel >> 5); \
    })
#define COLOR_R5_MAX (0x1F)
#define COLOR_G6_MAX (0x3F)
#define COLOR_B5_MAX (0x1F)
#define COLOR_R8_MIN                            0
#define COLOR_R8_MAX                            255
#define COLOR_G8_MIN                            0
#define COLOR_G8_MAX                            255
#define COLOR_B8_MIN                            0
#define COLOR_B8_MAX                            255

#define COLOR_RGB565_BINARY_MAX (0xffff)
#define COLOR_RGB565_BINARY_MIN (0x0000)

#define COLOR_RGB888_TO_Y(r8, g8, b8) ((((r8) * 38) + ((g8) * 75) + ((b8) * 15)) >> 7) // 0.299R + 0.587G + 0.114B
#define COLOR_RGB565_TO_Y(rgb565) \
    ({ \
        __typeof__ (rgb565) __rgb565 = (rgb565); \
        int r = COLOR_RGB565_TO_R8(__rgb565); \
        int g = COLOR_RGB565_TO_G8(__rgb565); \
        int b = COLOR_RGB565_TO_B8(__rgb565); \
        COLOR_RGB888_TO_Y(r, g, b); \
    })

#define COLOR_RGB888_TO_U(r8, g8, b8)           ((((r8) * -21) - ((g8) * 43) + ((b8) * 64)) >> 7) // -0.168736R - 0.331264G + 0.5B
#define COLOR_RGB565_TO_U(rgb565)                \
    ({                                           \
        __typeof__ (rgb565) __rgb565 = (rgb565); \
        int r = COLOR_RGB565_TO_R8(__rgb565);    \
        int g = COLOR_RGB565_TO_G8(__rgb565);    \
        int b = COLOR_RGB565_TO_B8(__rgb565);    \
        COLOR_RGB888_TO_U(r, g, b);              \
    })

#define COLOR_RGB888_TO_V(r8, g8, b8)           ((((r8) * 64) - ((g8) * 54) - ((b8) * 10)) >> 7) // 0.5R - 0.418688G - 0.081312B
#define COLOR_RGB565_TO_V(rgb565)                \
    ({                                           \
        __typeof__ (rgb565) __rgb565 = (rgb565); \
        int r = COLOR_RGB565_TO_R8(__rgb565);    \
        int g = COLOR_RGB565_TO_G8(__rgb565);    \
        int b = COLOR_RGB565_TO_B8(__rgb565);    \
        COLOR_RGB888_TO_V(r, g, b);              \
    })

// https://en.wikipedia.org/wiki/YCbCr -> JPEG Conversion
STATIC uint16_t imlib_yuv_to_rgb(uint8_t y, int8_t u, int8_t v) {
    uint32_t r = IM_MAX(IM_MIN(y + ((91881 * v) >> 16), COLOR_R8_MAX), COLOR_R8_MIN);
    uint32_t g = IM_MAX(IM_MIN(y - (((22554 * u) + (46802 * v)) >> 16), COLOR_G8_MAX), COLOR_G8_MIN);
    uint32_t b = IM_MAX(IM_MIN(y + ((116130 * u) >> 16), COLOR_B8_MAX), COLOR_B8_MIN);

    return COLOR_R8_G8_B8_TO_RGB565(r, g, b);
}

// CIRCUITPY-CHANGE (vs openmv): an offset is removed so that the Y value is the
// same as from COLOR_RGB565_TO_Y.
#define COLOR_YUV_TO_RGB565(y, u, v)            imlib_yuv_to_rgb((y), u, v)


void shared_module_bitmapfilter_morph(
    displayio_bitmap_t *bitmap,
    displayio_bitmap_t *mask,
    const int ksize,
    const int *krn,
    const mp_float_t m,
    const mp_float_t b,
    bool threshold,
    int offset,
    bool invert) {

    int brows = ksize + 1;

    const int32_t m_int = (int32_t)MICROPY_FLOAT_C_FUN(round)(65536 * m);
    const int32_t b_int = (int32_t)MICROPY_FLOAT_C_FUN(round)(65536 * COLOR_G6_MAX * b);

    check_matching_details(bitmap, bitmap);

    switch (bitmap->bits_per_value) {
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("unsupported bitmap depth"));
        case 16: {
            displayio_bitmap_t buf;
            scratch_bitmap16(&buf, brows, bitmap->width);

            for (int y = 0, yy = bitmap->height; y < yy; y++) {
                uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(bitmap, y);
                uint16_t *buf_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(&buf, (y % brows));

                for (int x = 0, xx = bitmap->width; x < xx; x++) {
                    if (mask && common_hal_displayio_bitmap_get_pixel(mask, x, y)) {
                        IMAGE_PUT_RGB565_PIXEL_FAST(buf_row_ptr, x, IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x));
                        continue; // Short circuit.

                    }
                    int32_t r_acc = 0, g_acc = 0, b_acc = 0, ptr = 0;

                    if (x >= ksize && x < bitmap->width - ksize && y >= ksize && y < bitmap->height - ksize) {
                        for (int j = -ksize; j <= ksize; j++) {
                            uint16_t *k_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(bitmap, y + j);
                            for (int k = -ksize; k <= ksize; k++) {
                                int pixel = IMAGE_GET_RGB565_PIXEL_FAST(k_row_ptr, x + k);
                                r_acc += krn[ptr] * COLOR_RGB565_TO_R5(pixel);
                                g_acc += krn[ptr] * COLOR_RGB565_TO_G6(pixel);
                                b_acc += krn[ptr++] * COLOR_RGB565_TO_B5(pixel);
                            }
                        }
                    } else {
                        for (int j = -ksize; j <= ksize; j++) {
                            uint16_t *k_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(bitmap,
                                IM_MIN(IM_MAX(y + j, 0), (bitmap->height - 1)));
                            for (int k = -ksize; k <= ksize; k++) {
                                int pixel = IMAGE_GET_RGB565_PIXEL_FAST(k_row_ptr,
                                    IM_MIN(IM_MAX(x + k, 0), (bitmap->width - 1)));
                                r_acc += krn[ptr] * COLOR_RGB565_TO_R5(pixel);
                                g_acc += krn[ptr] * COLOR_RGB565_TO_G6(pixel);
                                b_acc += krn[ptr++] * COLOR_RGB565_TO_B5(pixel);
                            }
                        }
                    }
                    r_acc = (r_acc * m_int + b_int) >> 16;
                    if (r_acc > COLOR_R5_MAX) {
                        r_acc = COLOR_R5_MAX;
                    } else if (r_acc < 0) {
                        r_acc = 0;
                    }
                    g_acc = (g_acc * m_int + b_int * 2) >> 16;
                    if (g_acc > COLOR_G6_MAX) {
                        g_acc = COLOR_G6_MAX;
                    } else if (g_acc < 0) {
                        g_acc = 0;
                    }
                    b_acc = (b_acc * m_int + b_int) >> 16;
                    if (b_acc > COLOR_B5_MAX) {
                        b_acc = COLOR_B5_MAX;
                    } else if (b_acc < 0) {
                        b_acc = 0;
                    }

                    int pixel = COLOR_R5_G6_B5_TO_RGB565(r_acc, g_acc, b_acc);

                    if (threshold) {
                        if (((COLOR_RGB565_TO_Y(pixel) - offset) < COLOR_RGB565_TO_Y(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x))) ^ invert) {
                            pixel = COLOR_RGB565_BINARY_MAX;
                        } else {
                            pixel = COLOR_RGB565_BINARY_MIN;
                        }
                    }

                    IMAGE_PUT_RGB565_PIXEL_FAST(buf_row_ptr, x, pixel);
                }

                if (y >= ksize) {     // Transfer buffer lines...
                    memcpy(IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(bitmap, (y - ksize)),
                        IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(&buf, ((y - ksize) % brows)),
                        IMAGE_RGB565_LINE_LEN_BYTES(bitmap));
                }
            }

            // Copy any remaining lines from the buffer image...
            for (int y = IM_MAX(bitmap->height - ksize, 0), yy = bitmap->height; y < yy; y++) {
                memcpy(IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(bitmap, y),
                    IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(&buf, (y % brows)),
                    IMAGE_RGB565_LINE_LEN_BYTES(bitmap));
            }

            break;
        }
    }
}

void shared_module_bitmapfilter_mix(
    displayio_bitmap_t *bitmap,
    displayio_bitmap_t *mask,
    const mp_float_t weights[12]) {

    int wt[12];
    for (int i = 0; i < 12; i++) {
        // The different scale factors correct for G having 6 bits while R, G have 5
        // by doubling the scale for R/B->G and halving the scale for G->R/B.
        // As well, the final value in each row has to be scaled up by the
        // component's maxval.
        int scale =
            (i == 1 || i == 9) ? 32768 : // Mixing G into R/B
            (i == 4 || i == 6) ? 131072 : // Mixing R/B into G
            (i == 3 || i == 11) ? 65535 * COLOR_B5_MAX : // Offset for R/B
            (i == 7) ? 65535 * COLOR_G6_MAX : // Offset for G
            65536;
        wt[i] = (int32_t)MICROPY_FLOAT_C_FUN(round)(scale * weights[i]);
    }

    check_matching_details(bitmap, bitmap);

    switch (bitmap->bits_per_value) {
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("unsupported bitmap depth"));
        case 16: {
            for (int y = 0, yy = bitmap->height; y < yy; y++) {
                uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(bitmap, y);
                for (int x = 0, xx = bitmap->width; x < xx; x++) {
                    if (mask && common_hal_displayio_bitmap_get_pixel(mask, x, y)) {
                        continue; // Short circuit.
                    }
                    int pixel = IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x);
                    int32_t r_acc = 0, g_acc = 0, b_acc = 0;
                    int r = COLOR_RGB565_TO_R5(pixel);
                    int g = COLOR_RGB565_TO_G6(pixel);
                    int b = COLOR_RGB565_TO_B5(pixel);
                    r_acc = r * wt[0] + g * wt[1] + b * wt[2] + wt[3];
                    r_acc >>= 16;
                    if (r_acc < 0) {
                        r_acc = 0;
                    } else if (r_acc > COLOR_R5_MAX) {
                        r_acc = COLOR_R5_MAX;
                    }

                    g_acc = r * wt[4] + g * wt[5] + b * wt[6] + wt[7];
                    g_acc >>= 16;
                    if (g_acc < 0) {
                        g_acc = 0;
                    } else if (g_acc > COLOR_G6_MAX) {
                        g_acc = COLOR_G6_MAX;
                    }

                    b_acc = r * wt[8] + g * wt[9] + b * wt[10] + wt[11];
                    b_acc >>= 16;
                    if (b_acc < 0) {
                        b_acc = 0;
                    } else if (b_acc > COLOR_B5_MAX) {
                        b_acc = COLOR_B5_MAX;
                    }

                    pixel = COLOR_R5_G6_B5_TO_RGB565(r_acc, g_acc, b_acc);
                    IMAGE_PUT_RGB565_PIXEL_FAST(row_ptr, x, pixel);
                }
            }
            break;
        }
    }
}

void shared_module_bitmapfilter_solarize(
    displayio_bitmap_t *bitmap,
    displayio_bitmap_t *mask,
    const mp_float_t threshold) {

    int threshold_i = (int32_t)MICROPY_FLOAT_C_FUN(round)(256 * threshold);
    switch (bitmap->bits_per_value) {
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("unsupported bitmap depth"));
        case 16: {
            for (int y = 0, yy = bitmap->height; y < yy; y++) {
                uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(bitmap, y);
                for (int x = 0, xx = bitmap->width; x < xx; x++) {
                    if (mask && common_hal_displayio_bitmap_get_pixel(mask, x, y)) {
                        continue; // Short circuit.
                    }
                    int pixel = IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x);
                    int y = COLOR_RGB565_TO_Y(pixel);
                    if (y > threshold_i) {
                        y = MIN(255, MAX(0, 2 * threshold_i - y));
                        int u = COLOR_RGB565_TO_U(pixel);
                        int v = COLOR_RGB565_TO_V(pixel);
                        pixel = COLOR_YUV_TO_RGB565(y, u, v);
                        IMAGE_PUT_RGB565_PIXEL_FAST(row_ptr, x, pixel);
                    }
                }
            }
            break;
        }
    }
}

void shared_module_bitmapfilter_lookup(
    displayio_bitmap_t *bitmap,
    displayio_bitmap_t *mask,
    const bitmapfilter_lookup_table_t *table) {

    switch (bitmap->bits_per_value) {
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("unsupported bitmap depth"));
        case 16: {
            for (int y = 0, yy = bitmap->height; y < yy; y++) {
                uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(bitmap, y);
                for (int x = 0, xx = bitmap->width; x < xx; x++) {
                    if (mask && common_hal_displayio_bitmap_get_pixel(mask, x, y)) {
                        continue; // Short circuit.
                    }
                    int pixel = IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x);
                    int r = COLOR_RGB565_TO_R5(pixel);
                    int g = COLOR_RGB565_TO_G6(pixel);
                    int b = COLOR_RGB565_TO_B5(pixel);

                    r = table->r[r];
                    g = table->g[g];
                    b = table->b[b];

                    pixel = COLOR_R5_G6_B5_TO_RGB565(r, g, b);
                    IMAGE_PUT_RGB565_PIXEL_FAST(row_ptr, x, pixel);
                }
            }
            break;
        }
    }
}

void shared_module_bitmapfilter_false_color(
    displayio_bitmap_t *bitmap,
    displayio_bitmap_t *mask,
    _displayio_color_t palette[256]) {

    uint16_t table[256];
    for (int i = 0; i < 256; i++) {
        uint32_t rgb888 = palette[i].rgb888;
        int r = rgb888 >> 16;
        int g = (rgb888 >> 8) & 0xff;
        int b = rgb888 & 0xff;
        table[i] = COLOR_R8_G8_B8_TO_RGB565(r, g, b);
    }

    switch (bitmap->bits_per_value) {
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("unsupported bitmap depth"));
        case 16: {
            for (int y = 0, yy = bitmap->height; y < yy; y++) {
                uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(bitmap, y);
                for (int x = 0, xx = bitmap->width; x < xx; x++) {
                    if (mask && common_hal_displayio_bitmap_get_pixel(mask, x, y)) {
                        continue; // Short circuit.
                    }
                    int pixel = IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x);
                    int y = COLOR_RGB565_TO_Y(pixel);
                    pixel = table[y];
                    IMAGE_PUT_RGB565_PIXEL_FAST(row_ptr, x, pixel);
                }
            }
        }
    }
}
