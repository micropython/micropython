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

#include "extmod/font_z1fonts.h"

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
#define FRAMEBUF_MVLSB    (0)
#define FRAMEBUF_RGB565   (1)
#define FRAMEBUF_GS2_HMSB (5)
#define FRAMEBUF_GS4_HMSB (2)
#define FRAMEBUF_GS8      (6)
#define FRAMEBUF_MHLSB    (3)
#define FRAMEBUF_MHMSB    (4)

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

// Functions for RGB565 format

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
    [FRAMEBUF_GS2_HMSB] = {gs2_hmsb_setpixel, gs2_hmsb_getpixel, gs2_hmsb_fill_rect},
    [FRAMEBUF_GS4_HMSB] = {gs4_hmsb_setpixel, gs4_hmsb_getpixel, gs4_hmsb_fill_rect},
    [FRAMEBUF_GS8] = {gs8_setpixel, gs8_getpixel, gs8_fill_rect},
    [FRAMEBUF_MHLSB] = {mono_horiz_setpixel, mono_horiz_getpixel, mono_horiz_fill_rect},
    [FRAMEBUF_MHMSB] = {mono_horiz_setpixel, mono_horiz_getpixel, mono_horiz_fill_rect},
};

static inline void setpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, uint32_t col) {
    formats[fb->format].setpixel(fb, x, y, col);
}

static void setpixel_checked(const mp_obj_framebuf_t *fb, mp_int_t x, mp_int_t y, mp_int_t col, mp_int_t mask) {
    if (mask && 0 <= x && x < fb->width && 0 <= y && y < fb->height) {
        setpixel(fb, x, y, col);
    }
}

static inline uint32_t getpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y) {
    return formats[fb->format].getpixel(fb, x, y);
}

static void fill_rect(const mp_obj_framebuf_t *fb, int x, int y, int w, int h, uint32_t col) {
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

static mp_obj_t framebuf_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args_in) {
    mp_arg_check_num(n_args, n_kw, 4, 5, false);

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
            bpp = 16;
            break;
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("invalid format"));
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args_in[0], &bufinfo, MP_BUFFER_WRITE);

    if ((strides_required * stride + (height_required - strides_required) * width_required) * bpp / 8 > bufinfo.len) {
        mp_raise_ValueError(NULL);
    }

    mp_obj_framebuf_t *o = mp_obj_malloc(mp_obj_framebuf_t, type);
    o->buf_obj = args_in[0];
    o->buf = bufinfo.buf;
    o->width = width;
    o->height = height;
    o->format = format;
    o->stride = stride;

    return MP_OBJ_FROM_PTR(o);
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
    fill_rect(self, args[0], args[1], args[2], args[3], args[4]);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_fill_rect_obj, 6, 6, framebuf_fill_rect);

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
            setpixel(self, x, y, mp_obj_get_int(args_in[3]));
        }
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_pixel_obj, 3, 4, framebuf_pixel);

static mp_obj_t framebuf_hline(size_t n_args, const mp_obj_t *args_in) {
    (void)n_args;

    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args_in[0]);
    mp_int_t args[4]; // x, y, w, col
    framebuf_args(args_in, args, 4);

    fill_rect(self, args[0], args[1], args[2], 1, args[3]);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_hline_obj, 5, 5, framebuf_hline);

static mp_obj_t framebuf_vline(size_t n_args, const mp_obj_t *args_in) {
    (void)n_args;

    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args_in[0]);
    mp_int_t args[4]; // x, y, h, col
    framebuf_args(args_in, args, 4);

    fill_rect(self, args[0], args[1], 1, args[2], args[3]);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_vline_obj, 5, 5, framebuf_vline);

static mp_obj_t framebuf_rect(size_t n_args, const mp_obj_t *args_in) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args_in[0]);
    mp_int_t args[5]; // x, y, w, h, col
    framebuf_args(args_in, args, 5);
    if (n_args > 6 && mp_obj_is_true(args_in[6])) {
        fill_rect(self, args[0], args[1], args[2], args[3], args[4]);
    } else {
        fill_rect(self, args[0], args[1], args[2], 1, args[4]);
        fill_rect(self, args[0], args[1] + args[3] - 1, args[2], 1, args[4]);
        fill_rect(self, args[0], args[1], 1, args[3], args[4]);
        fill_rect(self, args[0] + args[2] - 1, args[1], 1, args[3], args[4]);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_rect_obj, 6, 7, framebuf_rect);

static void line(const mp_obj_framebuf_t *fb, mp_int_t x1, mp_int_t y1, mp_int_t x2, mp_int_t y2, mp_int_t col) {
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
                setpixel(fb, y1, x1, col);
            }
        } else {
            if (0 <= x1 && x1 < fb->width && 0 <= y1 && y1 < fb->height) {
                setpixel(fb, x1, y1, col);
            }
        }
        while (e >= 0) {
            y1 += sy;
            e -= 2 * dx;
        }
        x1 += sx;
        e += 2 * dy;
    }

    if (0 <= x2 && x2 < fb->width && 0 <= y2 && y2 < fb->height) {
        setpixel(fb, x2, y2, col);
    }
}

static mp_obj_t framebuf_line(size_t n_args, const mp_obj_t *args_in) {
    (void)n_args;

    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args_in[0]);
    mp_int_t args[5]; // x1, y1, x2, y2, col
    framebuf_args(args_in, args, 5);

    line(self, args[0], args[1], args[2], args[3], args[4]);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_line_obj, 6, 6, framebuf_line);

// Q2 Q1
// Q3 Q4
#define ELLIPSE_MASK_FILL (0x10)
#define ELLIPSE_MASK_ALL (0x0f)
#define ELLIPSE_MASK_Q1 (0x01)
#define ELLIPSE_MASK_Q2 (0x02)
#define ELLIPSE_MASK_Q3 (0x04)
#define ELLIPSE_MASK_Q4 (0x08)

static void draw_ellipse_points(const mp_obj_framebuf_t *fb, mp_int_t cx, mp_int_t cy, mp_int_t x, mp_int_t y, mp_int_t col, mp_int_t mask) {
    if (mask & ELLIPSE_MASK_FILL) {
        if (mask & ELLIPSE_MASK_Q1) {
            fill_rect(fb, cx, cy - y, x + 1, 1, col);
        }
        if (mask & ELLIPSE_MASK_Q2) {
            fill_rect(fb, cx - x, cy - y, x + 1, 1, col);
        }
        if (mask & ELLIPSE_MASK_Q3) {
            fill_rect(fb, cx - x, cy + y, x + 1, 1, col);
        }
        if (mask & ELLIPSE_MASK_Q4) {
            fill_rect(fb, cx, cy + y, x + 1, 1, col);
        }
    } else {
        setpixel_checked(fb, cx + x, cy - y, col, mask & ELLIPSE_MASK_Q1);
        setpixel_checked(fb, cx - x, cy - y, col, mask & ELLIPSE_MASK_Q2);
        setpixel_checked(fb, cx - x, cy + y, col, mask & ELLIPSE_MASK_Q3);
        setpixel_checked(fb, cx + x, cy + y, col, mask & ELLIPSE_MASK_Q4);
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
    if (args[2] == 0 && args[3] == 0) {
        setpixel_checked(self, args[0], args[1], args[4], mask & ELLIPSE_MASK_ALL);
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
    while (stoppingx >= stoppingy) {   // 1st set of points,  y' > -1
        draw_ellipse_points(self, args[0], args[1], x, y, args[4], mask);
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
        draw_ellipse_points(self, args[0], args[1], x, y, args[4], mask);
        x += 1;
        stoppingx += two_bsquare;
        ellipse_error += xchange;
        xchange += two_bsquare;
        if ((2 * ellipse_error + ychange) > 0) {
            y -= 1;
            stoppingy -= two_asquare;
            ellipse_error += ychange;
            ychange += two_asquare;
        }
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_ellipse_obj, 6, 8, framebuf_ellipse);

#if MICROPY_PY_ARRAY

static mp_int_t poly_int(mp_buffer_info_t *bufinfo, size_t index) {
    return mp_obj_get_int(mp_binary_get_val_array(bufinfo->typecode, bufinfo->buf, index));
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
        return mp_const_none;
    }

    mp_int_t col = mp_obj_get_int(args_in[4]);
    bool fill = n_args > 5 && mp_obj_is_true(args_in[5]);

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
                        setpixel_checked(self, x + px2, y + py2, col, 1);
                    } else if (py2 < py1) {
                        setpixel_checked(self, x + px1, y + py1, col, 1);
                    } else {
                        // Even though this is a hline and would be faster to
                        // use fill_rect, use line() because it handles x2 <
                        // x1.
                        line(self, x + px1, y + py1, x + px2, y + py2, col);
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
                fill_rect(self, x + nodes[i], y + row, (nodes[i + 1] - nodes[i]) + 1, 1, col);
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
            line(self, x + px1, y + py1, x + px2, y + py2, col);
            px1 = px2;
            py1 = py2;
        } while (i >= 0);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_poly_obj, 5, 6, framebuf_poly);

#endif // MICROPY_PY_ARRAY

static mp_obj_t framebuf_blit(size_t n_args, const mp_obj_t *args_in) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args_in[0]);
    mp_obj_t source_in = mp_obj_cast_to_native_base(args_in[1], MP_OBJ_FROM_PTR(&mp_type_framebuf));
    if (source_in == MP_OBJ_NULL) {
        mp_raise_TypeError(NULL);
    }
    mp_obj_framebuf_t *source = MP_OBJ_TO_PTR(source_in);

    mp_int_t x = mp_obj_get_int(args_in[2]);
    mp_int_t y = mp_obj_get_int(args_in[3]);
    mp_int_t key = -1;
    if (n_args > 4) {
        key = mp_obj_get_int(args_in[4]);
    }
    mp_obj_framebuf_t *palette = NULL;
    if (n_args > 5 && args_in[5] != mp_const_none) {
        palette = MP_OBJ_TO_PTR(mp_obj_cast_to_native_base(args_in[5], MP_OBJ_FROM_PTR(&mp_type_framebuf)));
    }

    if (
        (x >= self->width) ||
        (y >= self->height) ||
        (-x >= source->width) ||
        (-y >= source->height)
        ) {
        // Out of bounds, no-op.
        return mp_const_none;
    }

    // Clip.
    int x0 = MAX(0, x);
    int y0 = MAX(0, y);
    int x1 = MAX(0, -x);
    int y1 = MAX(0, -y);
    int x0end = MIN(self->width, x + source->width);
    int y0end = MIN(self->height, y + source->height);

    for (; y0 < y0end; ++y0) {
        int cx1 = x1;
        for (int cx0 = x0; cx0 < x0end; ++cx0) {
            uint32_t col = getpixel(source, cx1, y1);
            if (palette) {
                col = getpixel(palette, col, 0);
            }
            if (col != (uint32_t)key) {
                setpixel(self, cx0, y0, col);
            }
            ++cx1;
        }
        ++y1;
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_blit_obj, 4, 6, framebuf_blit);

static mp_obj_t framebuf_scroll(mp_obj_t self_in, mp_obj_t xstep_in, mp_obj_t ystep_in) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t xstep = mp_obj_get_int(xstep_in);
    mp_int_t ystep = mp_obj_get_int(ystep_in);
    int sx, y, xend, yend, dx, dy;
    if (xstep < 0) {
        sx = 0;
        xend = self->width + xstep;
        if (xend <= 0) {
            return mp_const_none;
        }
        dx = 1;
    } else {
        sx = self->width - 1;
        xend = xstep - 1;
        if (xend >= sx) {
            return mp_const_none;
        }
        dx = -1;
    }
    if (ystep < 0) {
        y = 0;
        yend = self->height + ystep;
        if (yend <= 0) {
            return mp_const_none;
        }
        dy = 1;
    } else {
        y = self->height - 1;
        yend = ystep - 1;
        if (yend >= y) {
            return mp_const_none;
        }
        dy = -1;
    }
    for (; y != yend; y += dy) {
        for (int x = sx; x != xend; x += dx) {
            setpixel(self, x, y, getpixel(self, x - xstep, y - ystep));
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
    mp_int_t font_id = 1;
    if (n_args >= 5) {
        col = mp_obj_get_int(args_in[4]);
    }
    if (n_args >= 6) {
        font_id = mp_obj_get_int(args_in[5]);
    }


    int fixed_width; // -1 indicates variable width
    int bold; // 0: regular, 1: bold

    switch (font_id) {
        case 0:
            // monospace 6x8 font
            fixed_width = 6;
            bold = 0;
            break;
        case 1:
            // monospace 8x8 font(bold)
            fixed_width = 8;
            bold = 1;
            break;
        case 2:
            // proportional 6x8 font
            fixed_width = -1;
            bold = 0;
            break;
        case 3:
            // proportional 8x8 font(bold)
            fixed_width = -1;
            bold = 1;
            break;
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("unsupported font_id"));
    }

    // track the initial x position to calculate the drawn length
    mp_int_t initial_x0 = x0;

    // clip bounds
    mp_int_t max_x = self->width;
    mp_int_t max_y = self->height;

    // loop over chars
    for (; *str; ++str) {
        mp_int_t char_x0 = x0;
        // get char and make sure its in range of font
        int chr = *(uint8_t *)str;
        if (chr < 32 || chr > 127) {
            chr = 127;
        }
        // get char data
        const uint8_t *chr_data = &font_z1prop8_base_8x8[(chr - 32) * 8];

        // columns
        int columns = chr_data[5] >> 4;

        // align to center
        if (fixed_width > 0 && columns <= 4) { // align data only exists when columns <= 4(last byte LSB store p2m & r2b column appends)
            int align = (chr_data[7] & 0x0F) >> 2; // mono append columns
            if (bold) {
                align += chr_data[7] & 0x03; // bold append columns
            }
            // ascending columns
            align = (fixed_width - (columns + 1 + align)) >> 1;
            x0 += align;
        }

        // loop over char data
        for (int j = 0; j < columns && x0 < max_x && 0 <= x0; j++, x0++) {
            // m 0x8, da 0x4, dp 0x2, dn 0x1
            int column_type = (chr_data[5 + ((j + 1) >> 1)] >> ((j & 0x01) << 2)) & 0x0F;
            int x1 = x0;
            int x2 = x0;
            // dup prev: bold && dp
            if (bold && (column_type & 0x02)) {
                x1--;
            }
            // dup next:monospace && m || bold && da/dn
            if ((fixed_width > 0 && (column_type & 0x08)) || (bold && (column_type & 0x05))) {
                x2++;
                // append insert: m/da
                if ((fixed_width > 0 && (column_type & 0x08)) || (bold && (column_type & 0x04))) {
                    x0++;
                }
            }

            for (int x = x1; x <= x2 && x < max_x; x++) {
                uint vline_data = chr_data[j]; // each byte is a column of 8 pixels, LSB at top
                for (int y = y0; vline_data && y < max_y && 0 <= y; vline_data >>= 1, y++) { // scan over vertical column
                    if (vline_data & 1) { // only draw if pixel set
                        setpixel(self, x, y, col);
                    }
                }
            }
        }
        if (fixed_width > 0) {
            x0 = char_x0 + fixed_width;
        } else {
            x0++;
        }
    }

    // calculate the length of the drawn text
    mp_int_t text_length = x0 - initial_x0;
    if (x0 > max_x) {
        text_length = max_x - initial_x0;
    }
    return MP_OBJ_NEW_SMALL_INT(text_length);
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
    { MP_ROM_QSTR(MP_QSTR_GS2_HMSB), MP_ROM_INT(FRAMEBUF_GS2_HMSB) },
    { MP_ROM_QSTR(MP_QSTR_GS4_HMSB), MP_ROM_INT(FRAMEBUF_GS4_HMSB) },
    { MP_ROM_QSTR(MP_QSTR_GS8), MP_ROM_INT(FRAMEBUF_GS8) },
    { MP_ROM_QSTR(MP_QSTR_MONO_HLSB), MP_ROM_INT(FRAMEBUF_MHLSB) },
    { MP_ROM_QSTR(MP_QSTR_MONO_HMSB), MP_ROM_INT(FRAMEBUF_MHMSB) },
};

static MP_DEFINE_CONST_DICT(framebuf_module_globals, framebuf_module_globals_table);

const mp_obj_module_t mp_module_framebuf = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&framebuf_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_framebuf, mp_module_framebuf);
#endif

#endif // MICROPY_PY_FRAMEBUF
