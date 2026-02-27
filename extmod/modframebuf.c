/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
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

#include <stdio.h>
#include <string.h>

#include "py/runtime.h"
#include "py/binary.h"

#if MICROPY_PY_FRAMEBUF

#include "extmod/font_petme128_8x8.h"

typedef struct _mp_obj_framebuf_t {
    mp_obj_base_t base;
    mp_obj_t buf_obj; // need to store this to prevent GC from reclaiming buf
    void *buf;
    uint16_t width, height, stride;
    uint8_t format;
} mp_obj_framebuf_t;

#if !MICROPY_ENABLE_DYNRUNTIME
static const mp_obj_type_t mp_type_framebuf;
#endif

typedef void (*setpixel_t)(const mp_obj_framebuf_t *, unsigned int, unsigned int, uint32_t);
typedef uint32_t (*getpixel_t)(const mp_obj_framebuf_t *, unsigned int, unsigned int);
typedef void (*fill_rect_t)(const mp_obj_framebuf_t *, unsigned int, unsigned int, unsigned int, unsigned int, uint32_t);

typedef struct _mp_framebuf_p_t {
    setpixel_t setpixel;
    getpixel_t getpixel;
    fill_rect_t fill_rect;
} mp_framebuf_p_t;

// constants for formats
#define FRAMEBUF_MVLSB     (0)
#define FRAMEBUF_RGB565    (1)
#define FRAMEBUF_RGB565_BE (7)
#define FRAMEBUF_RGB565_LE (8)
#define FRAMEBUF_RGB565_BS (9)
#define FRAMEBUF_GS2_HMSB  (5)
#define FRAMEBUF_GS4_HMSB  (2)
#define FRAMEBUF_GS8       (6)
#define FRAMEBUF_MHLSB     (3)
#define FRAMEBUF_MHMSB     (4)

// Functions for MHLSB and MHMSB

static void mono_horiz_setpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, uint32_t col) {
    size_t index = (x + y * fb->stride) >> 3;
    unsigned int offset = fb->format == FRAMEBUF_MHMSB ? x & 0x07 : 7 - (x & 0x07);
    ((uint8_t *)fb->buf)[index] = (((uint8_t *)fb->buf)[index] & ~(0x01 << offset)) | ((col != 0) << offset);
}

static uint32_t mono_horiz_getpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y) {
    size_t index = (x + y * fb->stride) >> 3;
    unsigned int offset = fb->format == FRAMEBUF_MHMSB ? x & 0x07 : 7 - (x & 0x07);
    return (((uint8_t *)fb->buf)[index] >> (offset)) & 0x01;
}

static void mono_horiz_fill_rect(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, unsigned int w, unsigned int h, uint32_t col) {
    unsigned int reverse = fb->format == FRAMEBUF_MHMSB;
    unsigned int advance = fb->stride >> 3;
    while (w--) {
        uint8_t *b = &((uint8_t *)fb->buf)[(x >> 3) + y * advance];
        unsigned int offset = reverse ?  x & 7 : 7 - (x & 7);
        for (unsigned int hh = h; hh; --hh) {
            *b = (*b & ~(0x01 << offset)) | ((col != 0) << offset);
            b += advance;
        }
        ++x;
    }
}

// Functions for MVLSB format

static void mvlsb_setpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, uint32_t col) {
    size_t index = (y >> 3) * fb->stride + x;
    uint8_t offset = y & 0x07;
    ((uint8_t *)fb->buf)[index] = (((uint8_t *)fb->buf)[index] & ~(0x01 << offset)) | ((col != 0) << offset);
}

static uint32_t mvlsb_getpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y) {
    return (((uint8_t *)fb->buf)[(y >> 3) * fb->stride + x] >> (y & 0x07)) & 0x01;
}

static void mvlsb_fill_rect(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, unsigned int w, unsigned int h, uint32_t col) {
    while (h--) {
        uint8_t *b = &((uint8_t *)fb->buf)[(y >> 3) * fb->stride + x];
        uint8_t offset = y & 0x07;
        for (unsigned int ww = w; ww; --ww) {
            *b = (*b & ~(0x01 << offset)) | ((col != 0) << offset);
            ++b;
        }
        ++y;
    }
}

// Functions for RGB565 formats
//
// Internally we use 'native' and 'non-native' formats, and then expose those as big- or little-
// endian to Python as appropriate.

static void rgb565_setpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, uint32_t col) {
    ((uint16_t *)fb->buf)[x + y * fb->stride] = col;
}

static uint32_t rgb565_getpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y) {
    return ((uint16_t *)fb->buf)[x + y * fb->stride];
}

static void rgb565_fill_rect(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, unsigned int w, unsigned int h, uint32_t col) {
    uint16_t *b = &((uint16_t *)fb->buf)[x + y * fb->stride];
    while (h--) {
        for (unsigned int ww = w; ww; --ww) {
            *b++ = col;
        }
        b += fb->stride - w;
    }
}

static void rgb565_non_native_setpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, uint32_t col) {
    col = __builtin_bswap16(col);
    ((uint16_t *)fb->buf)[x + y * fb->stride] = col;
}

static uint32_t rgb565_non_native_getpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y) {
    uint32_t col = ((uint16_t *)fb->buf)[x + y * fb->stride];
    return __builtin_bswap16(col);
}

static void rgb565_non_native_fill_rect(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, unsigned int w, unsigned int h, uint32_t col) {
    col = __builtin_bswap16(col);
    uint16_t *b = &((uint16_t *)fb->buf)[x + y * fb->stride];
    while (h--) {
        for (unsigned int ww = w; ww; --ww) {
            *b++ = col;
        }
        b += fb->stride - w;
    }
}

// Functions for GS2_HMSB format

static void gs2_hmsb_setpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, uint32_t col) {
    uint8_t *pixel = &((uint8_t *)fb->buf)[(x + y * fb->stride) >> 2];
    uint8_t shift = (x & 0x3) << 1;
    uint8_t mask = 0x3 << shift;
    uint8_t color = (col & 0x3) << shift;
    *pixel = color | (*pixel & (~mask));
}

static uint32_t gs2_hmsb_getpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y) {
    uint8_t pixel = ((uint8_t *)fb->buf)[(x + y * fb->stride) >> 2];
    uint8_t shift = (x & 0x3) << 1;
    return (pixel >> shift) & 0x3;
}

static void gs2_hmsb_fill_rect(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, unsigned int w, unsigned int h, uint32_t col) {
    for (unsigned int xx = x; xx < x + w; xx++) {
        for (unsigned int yy = y; yy < y + h; yy++) {
            gs2_hmsb_setpixel(fb, xx, yy, col);
        }
    }
}

// Functions for GS4_HMSB format

static void gs4_hmsb_setpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, uint32_t col) {
    uint8_t *pixel = &((uint8_t *)fb->buf)[(x + y * fb->stride) >> 1];

    if (x % 2) {
        *pixel = ((uint8_t)col & 0x0f) | (*pixel & 0xf0);
    } else {
        *pixel = ((uint8_t)col << 4) | (*pixel & 0x0f);
    }
}

static uint32_t gs4_hmsb_getpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y) {
    if (x % 2) {
        return ((uint8_t *)fb->buf)[(x + y * fb->stride) >> 1] & 0x0f;
    }

    return ((uint8_t *)fb->buf)[(x + y * fb->stride) >> 1] >> 4;
}

static void gs4_hmsb_fill_rect(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, unsigned int w, unsigned int h, uint32_t col) {
    col &= 0x0f;
    uint8_t *pixel_pair = &((uint8_t *)fb->buf)[(x + y * fb->stride) >> 1];
    uint8_t col_shifted_left = col << 4;
    uint8_t col_pixel_pair = col_shifted_left | col;
    unsigned int pixel_count_till_next_line = (fb->stride - w) >> 1;
    bool odd_x = (x % 2 == 1);

    while (h--) {
        unsigned int ww = w;

        if (odd_x && ww > 0) {
            *pixel_pair = (*pixel_pair & 0xf0) | col;
            pixel_pair++;
            ww--;
        }

        memset(pixel_pair, col_pixel_pair, ww >> 1);
        pixel_pair += ww >> 1;

        if (ww % 2) {
            *pixel_pair = col_shifted_left | (*pixel_pair & 0x0f);
            if (!odd_x) {
                pixel_pair++;
            }
        }

        pixel_pair += pixel_count_till_next_line;
    }
}

// Functions for GS8 format

static void gs8_setpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, uint32_t col) {
    uint8_t *pixel = &((uint8_t *)fb->buf)[(x + y * fb->stride)];
    *pixel = col & 0xff;
}

static uint32_t gs8_getpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y) {
    return ((uint8_t *)fb->buf)[(x + y * fb->stride)];
}

static void gs8_fill_rect(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, unsigned int w, unsigned int h, uint32_t col) {
    uint8_t *pixel = &((uint8_t *)fb->buf)[(x + y * fb->stride)];
    while (h--) {
        memset(pixel, col, w);
        pixel += fb->stride;
    }
}

static mp_framebuf_p_t formats[] = {
    [FRAMEBUF_MVLSB] = {mvlsb_setpixel, mvlsb_getpixel, mvlsb_fill_rect},
    [FRAMEBUF_RGB565] = {rgb565_setpixel, rgb565_getpixel, rgb565_fill_rect},
    [FRAMEBUF_RGB565_BS] = {rgb565_setpixel, rgb565_getpixel, rgb565_fill_rect},
    #if MP_ENDIANNESS_LITTLE
    [FRAMEBUF_RGB565_LE] = {rgb565_setpixel, rgb565_getpixel, rgb565_fill_rect},
    [FRAMEBUF_RGB565_BE] = {rgb565_non_native_setpixel, rgb565_non_native_getpixel, rgb565_non_native_fill_rect},
    #else // MP_ENDIANNESS_LITTLE
    [FRAMEBUF_RGB565_BE] = {rgb565_setpixel, rgb565_getpixel, rgb565_fill_rect},
    [FRAMEBUF_RGB565_LE] = {rgb565_non_native_setpixel, rgb565_non_native_getpixel, rgb565_non_native_fill_rect},
    #endif // MP_ENDIANNESS_LITTLE
    [FRAMEBUF_GS2_HMSB] = {gs2_hmsb_setpixel, gs2_hmsb_getpixel, gs2_hmsb_fill_rect},
    [FRAMEBUF_GS4_HMSB] = {gs4_hmsb_setpixel, gs4_hmsb_getpixel, gs4_hmsb_fill_rect},
    [FRAMEBUF_GS8] = {gs8_setpixel, gs8_getpixel, gs8_fill_rect},
    [FRAMEBUF_MHLSB] = {mono_horiz_setpixel, mono_horiz_getpixel, mono_horiz_fill_rect},
    [FRAMEBUF_MHMSB] = {mono_horiz_setpixel, mono_horiz_getpixel, mono_horiz_fill_rect},
};

#if MICROPY_PY_FRAMEBUF_ALPHA

#ifndef FRAMEBUF_GET_ALPHA_ARG
#define FRAMEBUF_GET_ALPHA_ARG(idx) ((n_args > idx) ? mp_obj_get_int(args_in[idx]) : 0xFF)
#endif // GET_ALPHA_ARG

typedef struct __attribute__((packed)) rgb565 {
    uint8_t b : 5;
    uint8_t g : 6;
    uint8_t r : 5;
} rgb565;
typedef union {
    uint16_t u16;
    rgb565 rgb;
} urgb565;

static uint32_t alpha_mult(uint32_t c, uint32_t alpha) {
    // Efficient, correct alpha multiplication following Van Aken.
    // See https://arxiv.org/pdf/2202.02864
    c *= alpha;
    c += 0x80U;
    c += c >> 8;
    return c >> 8;
}

static uint32_t alpha_blend(uint32_t c1, uint32_t c2, uint32_t alpha) {
    return alpha_mult(c1, 0xff - alpha) + alpha_mult(c2, alpha);
}

static void setpixel(const mp_obj_framebuf_t *fb, mp_int_t x, mp_int_t y, uint32_t col, mp_int_t alpha) {
    if (alpha <= 0) {
        // nothing to do
        return;
    } else if (alpha < 0xff) {
        uint16_t pix_col = formats[fb->format].getpixel(fb, x, y);
        uint16_t col16 = col;
        urgb565 pix_col_struct;
        urgb565 col_struct;
        switch (fb->format) {
            case FRAMEBUF_RGB565:
            case FRAMEBUF_RGB565_BS:
            case FRAMEBUF_RGB565_BE:
            case FRAMEBUF_RGB565_LE:
                if (fb->format == FRAMEBUF_RGB565_BS) {
                    // The colors are specified in non-native endianness in Python.
                    // We need to byteswap to get native endianness.
                    col16 = __builtin_bswap16(col);
                    pix_col = __builtin_bswap16(pix_col);
                }
                // convert to bit-packed rgb struct
                pix_col_struct = *(urgb565 *)&pix_col;
                col_struct = *(urgb565 *)&col16;
                // blend channels
                col_struct.rgb.r = alpha_blend(pix_col_struct.rgb.r, col_struct.rgb.r, alpha);
                col_struct.rgb.g = alpha_blend(pix_col_struct.rgb.g, col_struct.rgb.g, alpha);
                col_struct.rgb.b = alpha_blend(pix_col_struct.rgb.b, col_struct.rgb.b, alpha);
                // convert back to int
                col = *(uint16_t *)&col_struct;
                if (fb->format == FRAMEBUF_RGB565_BS) {
                    // byteswap back to get non-native endianness for storage.
                    col = __builtin_bswap16(col);
                }
                break;
            default:
                col = alpha_blend(pix_col, col, alpha);
        }
    }
    formats[fb->format].setpixel(fb, x, y, col);
}
#else // MICROPY_PY_FRAMEBUF_ALPHA

#ifndef FRAMEBUF_GET_ALPHA_ARG
#define FRAMEBUF_GET_ALPHA_ARG(idx) (0xFF)
#endif // GET_ALPHA_ARG

static inline void setpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, uint32_t col, mp_int_t alpha) {
    (void)alpha;
    formats[fb->format].setpixel(fb, x, y, col);
}
#endif // MICROPY_PY_FRAMEBUF_ALPHA

static void setpixel_checked(const mp_obj_framebuf_t *fb, mp_int_t x, mp_int_t y, mp_int_t col, mp_int_t mask, mp_int_t alpha) {
    if (mask && alpha > 0 && 0 <= x && x < fb->width && 0 <= y && y < fb->height) {
        setpixel(fb, x, y, col, alpha);
    }
}

static inline uint32_t getpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y) {
    return formats[fb->format].getpixel(fb, x, y);
}

#if MICROPY_PY_FRAMEBUF_ALPHA
static void fill_rect(const mp_obj_framebuf_t *fb, int x, int y, int w, int h, uint32_t col, mp_int_t alpha) {
    if (alpha == 0 || h < 1 || w < 1 || x + w <= 0 || y + h <= 0 || y >= fb->height || x >= fb->width) {
        // No operation needed.
        return;
    }

    // clip to the framebuffer
    int xend = MIN(fb->width, x + w);
    int yend = MIN(fb->height, y + h);
    x = MAX(x, 0);
    y = MAX(y, 0);

    if (alpha >= 0xff) {
        formats[fb->format].fill_rect(fb, x, y, xend - x, yend - y, col);
    } else {
        for (; y < yend; ++y) {
            for (int x0 = x; x0 < xend; ++x0) {
                setpixel(fb, x0, y, col, alpha);
            }
        }
    }
}
#else // MICROPY_PY_FRAMEBUF_ALPHA
static void fill_rect(const mp_obj_framebuf_t *fb, int x, int y, int w, int h, uint32_t col, mp_int_t alpha) {
    (void)alpha;

    if (h < 1 || w < 1 || x + w <= 0 || y + h <= 0 || y >= fb->height || x >= fb->width) {
        // No operation needed.
        return;
    }

    // clip to the framebuffer
    int xend = MIN(fb->width, x + w);
    int yend = MIN(fb->height, y + h);
    x = MAX(x, 0);
    y = MAX(y, 0);

    formats[fb->format].fill_rect(fb, x, y, xend - x, yend - y, col);
}
#endif // MICROPY_PY_FRAMEBUF_ALPHA

static mp_obj_t framebuf_make_new_helper(size_t n_args, const mp_obj_t *args_in, unsigned int buf_flags, mp_obj_framebuf_t *o) {

    mp_int_t width = mp_obj_get_int(args_in[1]);
    mp_int_t height = mp_obj_get_int(args_in[2]);
    mp_int_t format = mp_obj_get_int(args_in[3]);
    mp_int_t stride = n_args >= 5 ? mp_obj_get_int(args_in[4]) : width;

    if (width < 1 || height < 1 || width > 0xffff || height > 0xffff || stride > 0xffff || stride < width) {
        mp_raise_ValueError(NULL);
    }

    size_t bpp = 1;
    size_t height_required = height;
    size_t width_required = width;
    size_t strides_required = height - 1;

    switch (format) {
        case FRAMEBUF_MVLSB:
            height_required = (height + 7) & ~7;
            strides_required = height_required - 8;
            break;
        case FRAMEBUF_MHLSB:
        case FRAMEBUF_MHMSB:
            stride = (stride + 7) & ~7;
            width_required = (width + 7) & ~7;
            break;
        case FRAMEBUF_GS2_HMSB:
            stride = (stride + 3) & ~3;
            width_required = (width + 3) & ~3;
            bpp = 2;
            break;
        case FRAMEBUF_GS4_HMSB:
            stride = (stride + 1) & ~1;
            width_required = (width + 1) & ~1;
            bpp = 4;
            break;
        case FRAMEBUF_GS8:
            bpp = 8;
            break;
        case FRAMEBUF_RGB565:
        case FRAMEBUF_RGB565_BS:
        case FRAMEBUF_RGB565_BE:
        case FRAMEBUF_RGB565_LE:
            bpp = 16;
            break;
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("invalid format"));
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args_in[0], &bufinfo, buf_flags);

    if ((strides_required * stride + (height_required - strides_required) * width_required) * bpp / 8 > bufinfo.len) {
        mp_raise_ValueError(NULL);
    }

    if (o == NULL) {
        o = mp_obj_malloc(mp_obj_framebuf_t, (const mp_obj_type_t *)&mp_type_framebuf);
    }
    o->buf_obj = args_in[0];
    o->buf = bufinfo.buf;
    o->width = width;
    o->height = height;
    o->format = format;
    o->stride = stride;

    return MP_OBJ_FROM_PTR(o);
}

static mp_obj_t framebuf_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args_in) {
    mp_arg_check_num(n_args, n_kw, 4, 5, false);
    return framebuf_make_new_helper(n_args, args_in, MP_BUFFER_WRITE, NULL);
}

static void framebuf_args(const mp_obj_t *args_in, mp_int_t *args_out, int n) {
    for (int i = 0; i < n; ++i) {
        args_out[i] = mp_obj_get_int(args_in[i + 1]);
    }
}

static mp_int_t framebuf_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_get_buffer(self->buf_obj, bufinfo, flags) ? 0 : 1;
}

static mp_obj_t framebuf_fill(mp_obj_t self_in, mp_obj_t col_in) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t col = mp_obj_get_int(col_in);
    formats[self->format].fill_rect(self, 0, 0, self->width, self->height, col);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(framebuf_fill_obj, framebuf_fill);

static mp_obj_t framebuf_fill_rect(size_t n_args, const mp_obj_t *args_in) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args_in[0]);
    mp_int_t args[5]; // x, y, w, h, col
    framebuf_args(args_in, args, 5);
    fill_rect(self, args[0], args[1], args[2], args[3], args[4], FRAMEBUF_GET_ALPHA_ARG(6));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_fill_rect_obj, 6, 7, framebuf_fill_rect);

static mp_obj_t framebuf_pixel(size_t n_args, const mp_obj_t *args_in) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args_in[0]);
    mp_int_t x = mp_obj_get_int(args_in[1]);
    mp_int_t y = mp_obj_get_int(args_in[2]);
    if (0 <= x && x < self->width && 0 <= y && y < self->height) {
        if (n_args == 3) {
            // get
            return MP_OBJ_NEW_SMALL_INT(getpixel(self, x, y));
        } else {
            // set
            setpixel(self, x, y, mp_obj_get_int(args_in[3]), FRAMEBUF_GET_ALPHA_ARG(4));
        }
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_pixel_obj, 3, 5, framebuf_pixel);

static mp_obj_t framebuf_hline(size_t n_args, const mp_obj_t *args_in) {
    (void)n_args;

    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args_in[0]);
    mp_int_t args[4]; // x, y, w, col
    framebuf_args(args_in, args, 4);

    fill_rect(self, args[0], args[1], args[2], 1, args[3], FRAMEBUF_GET_ALPHA_ARG(5));

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_hline_obj, 5, 6, framebuf_hline);

static mp_obj_t framebuf_vline(size_t n_args, const mp_obj_t *args_in) {
    (void)n_args;

    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args_in[0]);
    mp_int_t args[4]; // x, y, h, col
    framebuf_args(args_in, args, 4);

    fill_rect(self, args[0], args[1], 1, args[2], args[3], FRAMEBUF_GET_ALPHA_ARG(5));

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_vline_obj, 5, 6, framebuf_vline);

static mp_obj_t framebuf_rect(size_t n_args, const mp_obj_t *args_in) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args_in[0]);
    mp_int_t args[5]; // x, y, w, h, col
    framebuf_args(args_in, args, 5);
    mp_int_t alpha = FRAMEBUF_GET_ALPHA_ARG(7);
    if (n_args > 6 && mp_obj_is_true(args_in[6])) {
        fill_rect(self, args[0], args[1], args[2], args[3], args[4], alpha);
    } else {
        fill_rect(self, args[0], args[1], args[2], 1, args[4], alpha);
        fill_rect(self, args[0], args[1] + args[3] - 1, args[2], 1, args[4], alpha);
        fill_rect(self, args[0], args[1] + 1, 1, args[3] - 2, args[4], alpha);
        fill_rect(self, args[0] + args[2] - 1, args[1] + 1, 1, args[3] - 2, args[4], alpha);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_rect_obj, 6, 8, framebuf_rect);

#if MICROPY_PY_FRAMEBUF_ALPHA
static void line(const mp_obj_framebuf_t *fb, mp_int_t x1, mp_int_t y1, mp_int_t x2, mp_int_t y2, mp_int_t col, mp_int_t alpha, bool draw_last) {
    // This implements Wu's antialiased line algorithm in 8-bit fixed-point.
    // See https://en.wikipedia.org/wiki/Xiaolin_Wu%27s_line_algorithm

    setpixel_checked(fb, x1, y1, col, 1, alpha);
    if (x1 == x2 && y1 == y2) {
        // nothing more to do
        return;
    }
    if (draw_last) {
        setpixel_checked(fb, x2, y2, col, 1, alpha);
    }

    mp_int_t dx = x2 - x1;
    mp_int_t dy = y2 - y1;
    if (dx + dy < 0) {
        // swap ends
        mp_int_t temp;
        dx = -dx;
        dy = -dy;
        temp = x1;
        x1 = x2;
        x2 = temp;
        temp = y1;
        y1 = y2;
        y2 = temp;
    }

    bool steep;
    if (dy > dx) {
        // swap x and y
        mp_int_t temp;
        temp = x1;
        x1 = y1;
        y1 = temp;
        temp = dx;
        dx = dy;
        dy = temp;
        steep = true;
    } else {
        steep = false;
    }

    // Fixed point with 12 bits of fractional part.
    // dx != 0 is guaranteed
    mp_int_t gradient = ((dy * (1 << 12)) / dx);

    mp_int_t y_intercept = (y1 * (1 << 12)) + gradient;
    if (steep) {
        for (mp_int_t x = x1 + 1; x < x1 + dx; ++x) {
            setpixel_checked(fb, y_intercept >> 12, x, col, 1, alpha_mult(0xFF - ((y_intercept >> 4) & 0xff), alpha));
            setpixel_checked(fb, (y_intercept >> 12) + 1, x, col, 1, alpha_mult((y_intercept >> 4) & 0xff, alpha));
            y_intercept += gradient;
        }
    } else {
        for (mp_int_t x = x1 + 1; x < x1 + dx; ++x) {
            setpixel_checked(fb, x, y_intercept >> 12, col, 1, alpha_mult(0xFF - ((y_intercept >> 4) & 0xff), alpha));
            setpixel_checked(fb, x, (y_intercept >> 12) + 1, col, 1, alpha_mult((y_intercept >> 4) & 0xff, alpha));
            y_intercept += gradient;
        }
    }
}
#else // MICROPY_PY_FRAMEBUF_ALPHA
static void line(const mp_obj_framebuf_t *fb, mp_int_t x1, mp_int_t y1, mp_int_t x2, mp_int_t y2, mp_int_t col, mp_int_t alpha, bool draw_last) {
    // This implements Bresenham's line algorithm, see https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
    (void)draw_last;

    mp_int_t dx = x2 - x1;
    mp_int_t sx;
    if (dx > 0) {
        sx = 1;
    } else {
        dx = -dx;
        sx = -1;
    }

    mp_int_t dy = y2 - y1;
    mp_int_t sy;
    if (dy > 0) {
        sy = 1;
    } else {
        dy = -dy;
        sy = -1;
    }

    bool steep;
    if (dy > dx) {
        mp_int_t temp;
        temp = x1;
        x1 = y1;
        y1 = temp;
        temp = dx;
        dx = dy;
        dy = temp;
        temp = sx;
        sx = sy;
        sy = temp;
        steep = true;
    } else {
        steep = false;
    }

    mp_int_t e = 2 * dy - dx;
    for (mp_int_t i = 0; i < dx; ++i) {
        if (steep) {
            if (0 <= y1 && y1 < fb->width && 0 <= x1 && x1 < fb->height) {
                setpixel(fb, y1, x1, col, alpha);
            }
        } else {
            if (0 <= x1 && x1 < fb->width && 0 <= y1 && y1 < fb->height) {
                setpixel(fb, x1, y1, col, alpha);
            }
        }
        while (e >= 0) {
            y1 += sy;
            e -= 2 * dx;
        }
        x1 += sx;
        e += 2 * dy;
    }

    setpixel_checked(fb, x2, y2, col, 1, alpha);
}
#endif // MICROPY_PY_FRAMEBUF_ALPHA

static mp_obj_t framebuf_line(size_t n_args, const mp_obj_t *args_in) {
    (void)n_args;

    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args_in[0]);
    mp_int_t args[5]; // x1, y1, x2, y2, col
    framebuf_args(args_in, args, 5);

    line(self, args[0], args[1], args[2], args[3], args[4], FRAMEBUF_GET_ALPHA_ARG(6), true);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_line_obj, 6, 7, framebuf_line);

// Q2 Q1
// Q3 Q4
#define ELLIPSE_MASK_FILL (0x10)
#define ELLIPSE_MASK_ALL (0x0f)
#define ELLIPSE_MASK_Q1 (0x01)
#define ELLIPSE_MASK_Q2 (0x02)
#define ELLIPSE_MASK_Q3 (0x04)
#define ELLIPSE_MASK_Q4 (0x08)

static void draw_ellipse_points(const mp_obj_framebuf_t *fb, mp_int_t cx, mp_int_t cy, mp_int_t x, mp_int_t y, mp_int_t col, mp_int_t mask, mp_int_t alpha) {
    // Care needs to be taken to avoid drawing the same pixel twice when using transparency.
    if (mask & ELLIPSE_MASK_FILL) {
        if (y == 0 && (mask & ELLIPSE_MASK_ALL)) {
            // on y-axis, draw one hline
            mp_int_t x_min = (mask & (ELLIPSE_MASK_Q2 | ELLIPSE_MASK_Q3)) ? -x : 0;
            mp_int_t x_max = (mask & (ELLIPSE_MASK_Q1 | ELLIPSE_MASK_Q4)) ? x : 0;
            fill_rect(fb, cx + x_min, cy, x_max - x_min + 1, 1, col, alpha);
        } else {
            if (mask & (ELLIPSE_MASK_Q1 | ELLIPSE_MASK_Q2)) {
                // draw one hline above
                mp_int_t x_min = (mask & ELLIPSE_MASK_Q2) ? -x : 0;
                mp_int_t x_max = (mask & ELLIPSE_MASK_Q1) ? x : 0;
                fill_rect(fb, cx + x_min, cy - y, x_max - x_min + 1, 1, col, alpha);
            }
            if (mask & (ELLIPSE_MASK_Q3 | ELLIPSE_MASK_Q4)) {
                // draw one hline below
                mp_int_t x_min = (mask & ELLIPSE_MASK_Q3) ? -x : 0;
                mp_int_t x_max = (mask & ELLIPSE_MASK_Q4) ? x : 0;
                fill_rect(fb, cx + x_min, cy + y, x_max - x_min + 1, 1, col, alpha);
            }
        }
    } else {
        if (y == 0) {
            setpixel_checked(fb, cx + x, cy, col, mask & (ELLIPSE_MASK_Q1 | ELLIPSE_MASK_Q4), alpha);
            setpixel_checked(fb, cx - x, cy, col, mask & (ELLIPSE_MASK_Q2 | ELLIPSE_MASK_Q3), alpha);
        } else if (x == 0) {
            setpixel_checked(fb, cx, cy - y, col, mask & (ELLIPSE_MASK_Q1 | ELLIPSE_MASK_Q2), alpha);
            setpixel_checked(fb, cx, cy + y, col, mask & (ELLIPSE_MASK_Q3 | ELLIPSE_MASK_Q4), alpha);
        } else {
            setpixel_checked(fb, cx + x, cy - y, col, mask & ELLIPSE_MASK_Q1, alpha);
            setpixel_checked(fb, cx - x, cy - y, col, mask & ELLIPSE_MASK_Q2, alpha);
            setpixel_checked(fb, cx - x, cy + y, col, mask & ELLIPSE_MASK_Q3, alpha);
            setpixel_checked(fb, cx + x, cy + y, col, mask & ELLIPSE_MASK_Q4, alpha);
        }
    }
}

static mp_obj_t framebuf_ellipse(size_t n_args, const mp_obj_t *args_in) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args_in[0]);
    mp_int_t args[5];
    framebuf_args(args_in, args, 5); // cx, cy, xradius, yradius, col
    mp_int_t mask = (n_args > 6 && mp_obj_is_true(args_in[6])) ? ELLIPSE_MASK_FILL : 0;
    if (n_args > 7) {
        mask |= mp_obj_get_int(args_in[7]) & ELLIPSE_MASK_ALL;
    } else {
        mask |= ELLIPSE_MASK_ALL;
    }
    mp_int_t alpha = FRAMEBUF_GET_ALPHA_ARG(8);
    if (alpha <= 0) {
        // nothing to do
        return mp_const_none;
    }
    if (args[2] == 0 && args[3] == 0) {
        setpixel_checked(self, args[0], args[1], args[4], mask & ELLIPSE_MASK_ALL, alpha);
        return mp_const_none;
    }
    mp_int_t two_asquare = 2 * args[2] * args[2];
    mp_int_t two_bsquare = 2 * args[3] * args[3];
    mp_int_t x = args[2];
    mp_int_t y = 0;
    mp_int_t xchange = args[3] * args[3] * (1 - 2 * args[2]);
    mp_int_t ychange = args[2] * args[2];
    mp_int_t ellipse_error = 0;
    mp_int_t stoppingx = two_bsquare * args[2];
    mp_int_t stoppingy = 0;
    mp_int_t last_drawn_x = -1;
    mp_int_t last_drawn_y = -1;
    while (stoppingx >= stoppingy) {   // 1st set of points,  y' > -1
        draw_ellipse_points(self, args[0], args[1], x, y, args[4], mask, alpha);
        last_drawn_x = x;
        last_drawn_y = y;
        y += 1;
        stoppingy += two_asquare;
        ellipse_error += ychange;
        ychange += two_asquare;
        if ((2 * ellipse_error + xchange) > 0) {
            x -= 1;
            stoppingx -= two_bsquare;
            ellipse_error += xchange;
            xchange += two_bsquare;
        }
    }
    // 1st point set is done start the 2nd set of points
    x = 0;
    y = args[3];
    xchange = args[3] * args[3];
    ychange = args[2] * args[2] * (1 - 2 * args[3]);
    ellipse_error = 0;
    stoppingx = 0;
    stoppingy = two_asquare * args[3];
    while (stoppingx <= stoppingy) {  // 2nd set of points, y' < -1
        if (!(mask & ELLIPSE_MASK_FILL) && (y != last_drawn_y || x != last_drawn_x)) {
            draw_ellipse_points(self, args[0], args[1], x, y, args[4], mask, alpha);
        }
        x += 1;
        stoppingx += two_bsquare;
        ellipse_error += xchange;
        xchange += two_bsquare;
        if ((2 * ellipse_error + ychange) > 0) {
            if ((mask & ELLIPSE_MASK_FILL) && y != last_drawn_y) {
                // moving to new scanline, draw line *once*
                draw_ellipse_points(self, args[0], args[1], x - 1, y, args[4], mask, alpha);
            }
            y -= 1;
            stoppingy -= two_asquare;
            ellipse_error += ychange;
            ychange += two_asquare;
        }
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_ellipse_obj, 6, 9, framebuf_ellipse);

#if MICROPY_PY_ARRAY

static mp_int_t poly_int(mp_buffer_info_t *bufinfo, size_t index) {
    return mp_obj_get_int(mp_binary_get_val_array(bufinfo->typecode, bufinfo->buf, index));
}

#if MICROPY_PY_FRAMEBUF_ALPHA

static inline uint32_t popcount(uint32_t x) {
    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4)) & 0x0F0F0F0F;
    return (x * 0x01010101) >> 24;
}

typedef struct edge {
    mp_int_t y1;
    mp_int_t y2;
    mp_int_t x1;
    mp_int_t slope;
} edge;

static void insert_edge(edge *edge_table, int n_edges, mp_int_t py1, mp_int_t py2, mp_int_t px1, mp_int_t slope) {
    edge e = {
        py1,
        py2,
        px1 + (slope >> 2), // bump to first sub-scanline intersection (increment by a quarter of the slope)
        slope,
    };
    edge current;
    // simple linear ordered insertion
    for (int i = 0; i < n_edges; ++i) {
        current = edge_table[i];
        if (e.y1 <= current.y1) {
            edge_table[i] = e;
            e = current;
        }
    }
    edge_table[n_edges] = e;
}

typedef struct node {
    mp_int_t x;
    uint32_t mask;
} node;

static int insert_node(node *node_table, int n_nodes, mp_int_t x, uint32_t mask) {
    node n = {x, mask};
    node current;
    // simple linear ordered insertion
    for (int i = 0; i < n_nodes; ++i) {
        current = node_table[i];
        if (n.x == current.x) {
            n.mask = n.mask ^ current.mask;
            node_table[i] = n;
            return 0;
        } else if (n.x < current.x) {
            node_table[i] = n;
            n = current;
        }
    }
    node_table[n_nodes] = n;
    return 1;
}

static mp_obj_t framebuf_poly(size_t n_args, const mp_obj_t *args_in) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args_in[0]);

    mp_int_t x = mp_obj_get_int(args_in[1]);
    mp_int_t y = mp_obj_get_int(args_in[2]);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args_in[3], &bufinfo, MP_BUFFER_READ);
    // If an odd number of values was given, this rounds down to multiple of two.
    int n_poly = bufinfo.len / (mp_binary_get_size('@', bufinfo.typecode, NULL) * 2);

    if (n_poly == 0) {
        // Nothing to do.
        return mp_const_none;
    }

    mp_int_t col = mp_obj_get_int(args_in[4]);
    bool fill = n_args > 5 && mp_obj_is_true(args_in[5]);
    mp_int_t alpha = FRAMEBUF_GET_ALPHA_ARG(6);
    if (alpha <= 0) {
        // Nothing to do.
        return mp_const_none;
    }

    if (fill) {
        // This implements an integer version of the A-buffer algorithm for antialiased polygon fill.

        // We compute two scanlines per row, one at -0.25 and one at +0.25 from
        // the pixel center (where the integer coordinates are). Each scanline
        // samples at the 0.25 multiples.
        //
        //   +---+---+ -0.5
        //   X-X-X-X-|
        //   +---+---+ 0
        //   X-X-X-X-|
        //   +---+---+ +0.5
        // -0.5  0 +0.5
        //
        // Where an edge intersects a scanline, we round and set a bit mask of all
        // bits to the right of it, and xor that with the mask from the previous edges.
        //
        // When we move to the next pixel we copy the last bit on each scanline
        // and multiply by 0b1111 to extend it to the entire row, and use that as
        // the initial mask for that pixel.
        //
        // Original paper: https://dl.acm.org/doi/pdf/10.1145/964965.808585
        // Explanation in the context of embedded systems: https://aykevl.nl/2024/02/tinygl-polygon/
        // We don't use a lookup table.

        // Increase alpha for mono buffers to get sharp corners.
        if (self->format == FRAMEBUF_MHLSB || self->format == FRAMEBUF_MHMSB || self->format == FRAMEBUF_MVLSB) {
            alpha *= 2;
        }

        // Build an ordered table of edges and data
        // The table consists of entries (y_min, y_max, x_min, 1/slope) and is ordered by y_min.
        // The value of 1/slope is stored with 12 bits of fixed precision.
        // Horizontal lines are ignored.

        edge edge_table[n_poly];
        int n_edges = 0;
        mp_int_t px1 = x + poly_int(&bufinfo, 2 * n_poly - 2);
        mp_int_t py1 = y + poly_int(&bufinfo, 2 * n_poly - 1);
        mp_int_t y_start = py1;
        mp_int_t y_end = py1 + 1;
        for (int i = 0; i < n_poly; ++i) {
            mp_int_t px2 = x + poly_int(&bufinfo, 2 * i);
            mp_int_t py2 = y + poly_int(&bufinfo, 2 * i + 1);

            // track the min and max extent of the polygon
            y_start = MIN(y_start, py2);
            y_end = MAX(y_end, py2 + 1);

            if (py1 < py2) {
                // going up
                if (py1 <= self->height || py2 >= 0) {
                    // intersects buffer vertically
                    insert_edge(edge_table, n_edges, py1, py2, px1 * (1 << 12), ((px2 - px1) * (1 << 12)) / (py2 - py1));
                    ++n_edges;
                }
            } else if (py1 > py2) {
                // going down
                if (py2 <= self->height || py1 >= 0) {
                    // intersects buffer vertically
                    insert_edge(edge_table, n_edges, py2, py1, px2 * (1 << 12), ((px2 - px1) * (1 << 12)) / (py2 - py1));
                    ++n_edges;
                }
            } // ... and ignore horizontal edges

            px1 = px2;
            py1 = py2;
        }
        if (n_edges == 0) {
            // No non-horizontal edges: nothing to draw.
            return mp_const_none;
        }
        y_start = MAX(0, y_start);
        y_end = MIN(self->height, y_end);

        // Track which edges are can possibly intersect subsample scanlines.
        int last_edge_index = 0;

        for (mp_int_t row = y_start; row < y_end; row++) {
            // Add any new edges that may intersect the subsample lines to those we consider.
            while (last_edge_index < n_edges && edge_table[last_edge_index].y1 <= row) {
                ++last_edge_index;
            }

            // Build an ordered table of intersection locations and masks on subsample scanlines.
            // The table is ordered by pixel column and holds the mask for that pixel.
            int n_nodes = 0;
            node nodes[2 * last_edge_index];

            for (int line = 0; line < 2; ++line) {
                // For each subsample line...
                // Get y-value with 2 bits of fixed precision
                mp_int_t y1 = (line == 0) ? ((row << 2) - 1) : ((row << 2) + 1);
                for (int i = 0; i < last_edge_index; ++i) {
                    // For each edge...
                    edge *e = &(edge_table[i]);
                    if ((e->y2 * (1 << 2)) < y1) {
                        // Edge is below subsample line, ignore.
                        continue;
                    } else if ((e->y1 * (1 << 2)) > y1) {
                        // Edge above subsample line (can happen for lower subsample line at start of edge).
                        continue;
                    }
                    // Find pixel and sub-pixel offsets.
                    // We adjust x so integer coordinates are in the center of each pixel and intersections
                    // with subsample pixels round to nearest quarter.  This makes the antialiased values
                    // symmetric when given a symmetric shape.
                    mp_int_t x_adjusted = e->x1 + (1 << 11) + (1 << 9);
                    mp_int_t column;
                    // We need integer floor division here, as remainder matters.
                    if (x_adjusted > 0) {
                        column = x_adjusted >> 12;
                    } else {
                        // avoid undefined behaviour of shifting negatives.
                        column = ~((~x_adjusted) >> 12);
                    }
                    if (column >= self->width) {
                        // Outside of buffer width: don't care about these points for this row,
                        // but need to bump the x-value in case line eventually comes inside the buffer.
                        e->x1 += (e->slope >> 1);
                        continue;
                    }
                    mp_int_t subpixel_offset = (x_adjusted - (column * (1 << 12))) >> 10;

                    // Compute mask for subpixel scanline.
                    uint32_t mask = ((1 << (4 - subpixel_offset)) - 1) << (line << 2);

                    // Insert the node, xor-ing the mask if there is a column match, otherwise sorting.
                    n_nodes += insert_node(nodes, n_nodes, column, mask);

                    // Bump edge x value to next sub-scanline (increments by half the slope).
                    e->x1 += (e->slope >> 1);
                }
            }
            if (!n_nodes) {
                // No intersections we care about, go to next row.
                continue;
            }

            // Now draw the pixels by running through the intersection nodes.
            uint32_t mask = 0;
            node current;
            for (int i = 0; i < n_nodes; ++i) {
                current = nodes[i];

                // Update the mask.
                mask ^= current.mask;

                if (current.x >= 0) {
                    // The pixel is inside the buffer, so draw the pixel.
                    // The alpha of pixel is scaled by the number of bits in the mask (0-8, inclusive),
                    // So we multiply popcount by 255/9 with one bit of fixed point precision.
                    setpixel(self, current.x, row, col, alpha_mult((popcount(mask) * 0b1001001) >> 1, alpha));
                }

                // Extend mask by last bits of each subscanline.
                mask = (mask & 0b00010001) * 0b1111;

                if (mask) {
                    // Fill with a run of pixels with same mask - can be fast.
                    // Width is either distance to next node, or to width of buffer if no more nodes.
                    mp_int_t width;
                    if (i + 1 < n_nodes) {
                        width = nodes[i + 1].x - current.x - 1;
                    } else {
                        width = self->width - current.x - 1;
                    }
                    // Use fill_rect as it accounts for rectangles wider than buffer.
                    // See above for discussion of alpha computation.
                    fill_rect(self, current.x + 1, row, width, 1, col, alpha_mult((popcount(mask) * 0b1001001) >> 1, alpha));
                }
            }
        }
    } else {
        // Outline only.
        mp_int_t px1 = poly_int(&bufinfo, 0);
        mp_int_t py1 = poly_int(&bufinfo, 1);
        int i = n_poly * 2 - 1;
        do {
            mp_int_t py2 = poly_int(&bufinfo, i--);
            mp_int_t px2 = poly_int(&bufinfo, i--);
            line(self, x + px1, y + py1, x + px2, y + py2, col, alpha, false);
            px1 = px2;
            py1 = py2;
        } while (i >= 0);
    }

    return mp_const_none;
}
#else // MICROPY_PY_FRAMEBUF_ALPHA
static mp_obj_t framebuf_poly(size_t n_args, const mp_obj_t *args_in) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args_in[0]);

    mp_int_t x = mp_obj_get_int(args_in[1]);
    mp_int_t y = mp_obj_get_int(args_in[2]);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args_in[3], &bufinfo, MP_BUFFER_READ);
    // If an odd number of values was given, this rounds down to multiple of two.
    int n_poly = bufinfo.len / (mp_binary_get_size('@', bufinfo.typecode, NULL) * 2);

    if (n_poly == 0) {
        return mp_const_none;
    }

    mp_int_t col = mp_obj_get_int(args_in[4]);
    bool fill = n_args > 5 && mp_obj_is_true(args_in[5]);
    mp_int_t alpha = FRAMEBUF_GET_ALPHA_ARG(6);

    if (fill) {
        // This implements an integer version of http://alienryderflex.com/polygon_fill/

        // The idea is for each scan line, compute the sorted list of x
        // coordinates where the scan line intersects the polygon edges,
        // then fill between each resulting pair.

        // Restrict just to the scan lines that include the vertical extent of
        // this polygon.
        mp_int_t y_min = INT_MAX, y_max = INT_MIN;
        for (int i = 0; i < n_poly; i++) {
            mp_int_t py = poly_int(&bufinfo, i * 2 + 1);
            y_min = MIN(y_min, py);
            y_max = MAX(y_max, py);
        }

        for (mp_int_t row = y_min; row <= y_max; row++) {
            // Each node is the x coordinate where an edge crosses this scan line.
            mp_int_t nodes[n_poly];
            int n_nodes = 0;
            mp_int_t px1 = poly_int(&bufinfo, 0);
            mp_int_t py1 = poly_int(&bufinfo, 1);
            int i = n_poly * 2 - 1;
            do {
                mp_int_t py2 = poly_int(&bufinfo, i--);
                mp_int_t px2 = poly_int(&bufinfo, i--);

                // Don't include the bottom pixel of a given edge to avoid
                // duplicating the node with the start of the next edge. This
                // will miss some pixels on the boundary, and in particular
                // at a local minima or inflection point.
                if (py1 != py2 && ((py1 > row && py2 <= row) || (py1 <= row && py2 > row))) {
                    mp_int_t node = (32 * px1 + 32 * (px2 - px1) * (row - py1) / (py2 - py1) + 16) / 32;
                    nodes[n_nodes++] = node;
                } else if (row == MAX(py1, py2)) {
                    // At local-minima, try and manually fill in the pixels that get missed above.
                    if (py1 < py2) {
                        setpixel_checked(self, x + px2, y + py2, col, 1, alpha);
                    } else if (py2 < py1) {
                        setpixel_checked(self, x + px1, y + py1, col, 1, alpha);
                    } else {
                        // Even though this is a hline and would be faster to
                        // use fill_rect, use line() because it handles x2 <
                        // x1.
                        line(self, x + px1, y + py1, x + px2, y + py2, col, alpha, true);
                    }
                }

                px1 = px2;
                py1 = py2;
            } while (i >= 0);

            if (!n_nodes) {
                continue;
            }

            // Sort the nodes left-to-right (bubble-sort for code size).
            i = 0;
            while (i < n_nodes - 1) {
                if (nodes[i] > nodes[i + 1]) {
                    mp_int_t swap = nodes[i];
                    nodes[i] = nodes[i + 1];
                    nodes[i + 1] = swap;
                    if (i) {
                        i--;
                    }
                } else {
                    i++;
                }
            }

            // Fill between each pair of nodes.
            for (i = 0; i < n_nodes; i += 2) {
                fill_rect(self, x + nodes[i], y + row, (nodes[i + 1] - nodes[i]) + 1, 1, col, alpha);
            }
        }
    } else {
        // Outline only.
        mp_int_t px1 = poly_int(&bufinfo, 0);
        mp_int_t py1 = poly_int(&bufinfo, 1);
        int i = n_poly * 2 - 1;
        do {
            mp_int_t py2 = poly_int(&bufinfo, i--);
            mp_int_t px2 = poly_int(&bufinfo, i--);
            line(self, x + px1, y + py1, x + px2, y + py2, col, alpha, true);
            px1 = px2;
            py1 = py2;
        } while (i >= 0);
    }

    return mp_const_none;
}
#endif // MICROPY_PY_FRAMEBUF_ALPHA
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_poly_obj, 5, 7, framebuf_poly);

#endif // MICROPY_PY_ARRAY

static void get_readonly_framebuffer(mp_obj_t arg, mp_obj_framebuf_t *rofb) {
    mp_obj_t fb = mp_obj_cast_to_native_base(arg, MP_OBJ_FROM_PTR(&mp_type_framebuf));
    if (fb != MP_OBJ_NULL) {
        *rofb = *(mp_obj_framebuf_t *)MP_OBJ_TO_PTR(fb);
    } else {
        // A tuple/list of the form: (buffer, width, height, format[, stride]).
        size_t len;
        mp_obj_t *items;
        mp_obj_get_array(arg, &len, &items);
        if (len < 4 || len > 5) {
            mp_raise_ValueError(NULL);
        }
        framebuf_make_new_helper(len, items, MP_BUFFER_READ, rofb);
    }
}

static mp_obj_t framebuf_blit(size_t n_args, const mp_obj_t *args_in) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args_in[0]);

    mp_obj_framebuf_t source;
    get_readonly_framebuffer(args_in[1], &source);

    mp_int_t x = mp_obj_get_int(args_in[2]);
    mp_int_t y = mp_obj_get_int(args_in[3]);
    // Key and palette argument handling.
    mp_int_t key = -1;
    if (n_args > 4) {
        key = mp_obj_get_int(args_in[4]);
    }
    mp_obj_framebuf_t palette;
    palette.buf = NULL;
    if (n_args > 5 && args_in[5] != mp_const_none) {
        get_readonly_framebuffer(args_in[5], &palette);
    }

    if (
        (x >= self->width) ||
        (y >= self->height) ||
        (-x >= source.width) ||
        (-y >= source.height)
        ) {
        // Out of bounds, no-op.
        return mp_const_none;
    }

    // Clip.
    int x0 = MAX(0, x);
    int y0 = MAX(0, y);
    int x1 = MAX(0, -x);
    int y1 = MAX(0, -y);
    int x0end = MIN(self->width, x + source.width);
    int y0end = MIN(self->height, y + source.height);

    mp_int_t alpha = 0xFF;
    mp_int_t alpha_mul = 0;
    mp_obj_framebuf_t mask;
    if (n_args > 6 && args_in[6] != mp_const_none) {
        if (mp_obj_get_type(args_in[6]) == &mp_type_int) {
            alpha = FRAMEBUF_GET_ALPHA_ARG(6);
            if (alpha <= 0) {
                // nothing to do
                return mp_const_none;
            }
        } else {
            get_readonly_framebuffer(args_in[6], &mask);
            if (mask.width != source.width || mask.height != source.height) {
                // mask and source must be the same shape
                mp_raise_ValueError(MP_ERROR_TEXT("Mask and source different sizes."));
            }
            switch (mask.format) {
                case FRAMEBUF_MVLSB:
                case FRAMEBUF_MHLSB:
                case FRAMEBUF_MHMSB:
                    alpha_mul = 0xff;
                    break;
                #if MICROPY_PY_FRAMEBUF_ALPHA
                case FRAMEBUF_GS8:
                    alpha_mul = 0x1;
                    break;
                case FRAMEBUF_GS4_HMSB:
                    alpha_mul = 0x10001;
                    break;
                case FRAMEBUF_GS2_HMSB:
                    alpha_mul = 0x01010101;
                    break;
                #endif // MICROPY_PY_FRAMEBUF_ALPHA
                default:
                    // other formats can't easily be converted to alpha
                    mp_raise_ValueError(MP_ERROR_TEXT("invalid mask format"));
            }
        }
    }

    for (; y0 < y0end; ++y0) {
        int cx1 = x1;
        for (int cx0 = x0; cx0 < x0end; ++cx0) {
            uint32_t col = getpixel(&source, cx1, y1);
            if (palette.buf) {
                col = getpixel(&palette, col, 0);
            }
            if (alpha_mul) {
                alpha = getpixel(&mask, cx1, y1) * alpha_mul;
            }
            if (col != (uint32_t)key && alpha) {
                setpixel(self, cx0, y0, col, alpha);
            }
            ++cx1;
        }
        ++y1;
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_blit_obj, 4, 7, framebuf_blit);

static mp_obj_t framebuf_scroll(mp_obj_t self_in, mp_obj_t xstep_in, mp_obj_t ystep_in) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t xstep = mp_obj_get_int(xstep_in);
    mp_int_t ystep = mp_obj_get_int(ystep_in);
    unsigned int sx, y, xend, yend;
    int dx, dy;
    if (xstep < 0) {
        if (-xstep >= self->width) {
            return mp_const_none;
        }
        sx = 0;
        xend = self->width + (int)xstep;
        dx = 1;
    } else {
        if (xstep >= self->width) {
            return mp_const_none;
        }
        sx = self->width - 1;
        xend = (int)xstep - 1;
        dx = -1;
    }
    if (ystep < 0) {
        if (-ystep >= self->height) {
            return mp_const_none;
        }
        y = 0;
        yend = self->height + (int)ystep;
        dy = 1;
    } else {
        if (ystep >= self->height) {
            return mp_const_none;
        }
        y = self->height - 1;
        yend = (int)ystep - 1;
        dy = -1;
    }
    for (; y != yend; y += dy) {
        for (unsigned x = sx; x != xend; x += dx) {
            setpixel(self, x, y, getpixel(self, x - xstep, y - ystep), 0xFF);
        }
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(framebuf_scroll_obj, framebuf_scroll);

static mp_obj_t framebuf_text(size_t n_args, const mp_obj_t *args_in) {
    // extract arguments
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args_in[0]);
    const char *str = mp_obj_str_get_str(args_in[1]);
    mp_int_t x0 = mp_obj_get_int(args_in[2]);
    mp_int_t y0 = mp_obj_get_int(args_in[3]);
    mp_int_t col = 1;
    if (n_args >= 5) {
        col = mp_obj_get_int(args_in[4]);
    }
    mp_int_t alpha = FRAMEBUF_GET_ALPHA_ARG(5);
    if (alpha <= 0) {
        // nothing to do
        return mp_const_none;
    }

    // loop over chars
    for (; *str; ++str) {
        // get char and make sure its in range of font
        int chr = *(uint8_t *)str;
        if (chr < 32 || chr > 127) {
            chr = 127;
        }
        // get char data
        const uint8_t *chr_data = &font_petme128_8x8[(chr - 32) * 8];
        // loop over char data
        for (int j = 0; j < 8; j++, x0++) {
            if (0 <= x0 && x0 < self->width) { // clip x
                uint vline_data = chr_data[j]; // each byte is a column of 8 pixels, LSB at top
                for (int y = y0; vline_data; vline_data >>= 1, y++) { // scan over vertical column
                    if (vline_data & 1) { // only draw if pixel set
                        if (0 <= y && y < self->height) { // clip y
                            setpixel(self, x0, y, col, alpha);
                        }
                    }
                }
            }
        }
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_text_obj, 4, 6, framebuf_text);

#if !MICROPY_ENABLE_DYNRUNTIME
static const mp_rom_map_elem_t framebuf_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_fill), MP_ROM_PTR(&framebuf_fill_obj) },
    { MP_ROM_QSTR(MP_QSTR_fill_rect), MP_ROM_PTR(&framebuf_fill_rect_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel), MP_ROM_PTR(&framebuf_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_hline), MP_ROM_PTR(&framebuf_hline_obj) },
    { MP_ROM_QSTR(MP_QSTR_vline), MP_ROM_PTR(&framebuf_vline_obj) },
    { MP_ROM_QSTR(MP_QSTR_rect), MP_ROM_PTR(&framebuf_rect_obj) },
    { MP_ROM_QSTR(MP_QSTR_line), MP_ROM_PTR(&framebuf_line_obj) },
    { MP_ROM_QSTR(MP_QSTR_ellipse), MP_ROM_PTR(&framebuf_ellipse_obj) },
    #if MICROPY_PY_ARRAY
    { MP_ROM_QSTR(MP_QSTR_poly), MP_ROM_PTR(&framebuf_poly_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_blit), MP_ROM_PTR(&framebuf_blit_obj) },
    { MP_ROM_QSTR(MP_QSTR_scroll), MP_ROM_PTR(&framebuf_scroll_obj) },
    { MP_ROM_QSTR(MP_QSTR_text), MP_ROM_PTR(&framebuf_text_obj) },
};
static MP_DEFINE_CONST_DICT(framebuf_locals_dict, framebuf_locals_dict_table);

static MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_framebuf,
    MP_QSTR_FrameBuffer,
    MP_TYPE_FLAG_NONE,
    make_new, framebuf_make_new,
    buffer, framebuf_get_buffer,
    locals_dict, &framebuf_locals_dict
    );
#endif

#if !MICROPY_ENABLE_DYNRUNTIME
// This factory function is provided for backwards compatibility with the old
// FrameBuffer1 class which did not support a format argument.
static mp_obj_t legacy_framebuffer1(size_t n_args, const mp_obj_t *args_in) {
    mp_obj_t args[] = {args_in[0], args_in[1], args_in[2], MP_OBJ_NEW_SMALL_INT(FRAMEBUF_MVLSB), n_args >= 4 ? args_in[3] : args_in[1] };
    return framebuf_make_new(&mp_type_framebuf, 5, 0, args);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(legacy_framebuffer1_obj, 3, 4, legacy_framebuffer1);

static const mp_rom_map_elem_t framebuf_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_framebuf) },
    { MP_ROM_QSTR(MP_QSTR_FrameBuffer), MP_ROM_PTR(&mp_type_framebuf) },
    { MP_ROM_QSTR(MP_QSTR_FrameBuffer1), MP_ROM_PTR(&legacy_framebuffer1_obj) },
    { MP_ROM_QSTR(MP_QSTR_MVLSB), MP_ROM_INT(FRAMEBUF_MVLSB) },
    { MP_ROM_QSTR(MP_QSTR_MONO_VLSB), MP_ROM_INT(FRAMEBUF_MVLSB) },
    { MP_ROM_QSTR(MP_QSTR_RGB565), MP_ROM_INT(FRAMEBUF_RGB565) },
    { MP_ROM_QSTR(MP_QSTR_RGB565_BS), MP_ROM_INT(FRAMEBUF_RGB565_BS) },
    { MP_ROM_QSTR(MP_QSTR_RGB565_BE), MP_ROM_INT(FRAMEBUF_RGB565_BE) },
    { MP_ROM_QSTR(MP_QSTR_RGB565_LE), MP_ROM_INT(FRAMEBUF_RGB565_LE) },
    { MP_ROM_QSTR(MP_QSTR_GS2_HMSB), MP_ROM_INT(FRAMEBUF_GS2_HMSB) },
    { MP_ROM_QSTR(MP_QSTR_GS4_HMSB), MP_ROM_INT(FRAMEBUF_GS4_HMSB) },
    { MP_ROM_QSTR(MP_QSTR_GS8), MP_ROM_INT(FRAMEBUF_GS8) },
    { MP_ROM_QSTR(MP_QSTR_MONO_HLSB), MP_ROM_INT(FRAMEBUF_MHLSB) },
    { MP_ROM_QSTR(MP_QSTR_MONO_HMSB), MP_ROM_INT(FRAMEBUF_MHMSB) },
    { MP_ROM_QSTR(MP_QSTR_ALPHA), MP_ROM_INT(MICROPY_PY_FRAMEBUF_ALPHA) },
};

static MP_DEFINE_CONST_DICT(framebuf_module_globals, framebuf_module_globals_table);

const mp_obj_module_t mp_module_framebuf = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&framebuf_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_framebuf, mp_module_framebuf);
#endif

#endif // MICROPY_PY_FRAMEBUF
