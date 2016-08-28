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

#include "py/nlr.h"
#include "py/obj.h"
#include "py/runtime.h"

#if MICROPY_PY_FRAMEBUF

#include "stmhal/font_petme128_8x8.h"

typedef struct _mp_obj_framebuf_t {
    mp_obj_base_t base;
    void *buf;
    uint16_t width, height, stride;
    uint8_t format;
} mp_obj_framebuf_t;

typedef void (*setpixel_t)(const mp_obj_framebuf_t*, int, int, uint32_t);
typedef uint32_t (*getpixel_t)(const mp_obj_framebuf_t*, int, int);

typedef struct _mp_framebuf_p_t {
    setpixel_t setpixel;
    getpixel_t getpixel;
} mp_framebuf_p_t;

// Functions for MVLSB format

STATIC void mvlsb_setpixel(const mp_obj_framebuf_t *fb, int x, int y, uint32_t color) {
    size_t index = (y >> 3) * fb->stride + x;
    uint8_t offset = y & 0x07;
    ((uint8_t*)fb->buf)[index] = (((uint8_t*)fb->buf)[index] & ~(0x01 << offset)) | ((color != 0) << offset);
}

STATIC uint32_t mvlsb_getpixel(const mp_obj_framebuf_t *fb, int x, int y) {
    return (((uint8_t*)fb->buf)[(y >> 3) * fb->stride + x] >> (y & 0x07)) & 0x01;
}

// Functions for RGB565 format

STATIC void rgb565_setpixel(const mp_obj_framebuf_t *fb, int x, int y, uint32_t color) {
    ((uint16_t*)fb->buf)[x + y * fb->stride] = color;
}

STATIC uint32_t rgb565_getpixel(const mp_obj_framebuf_t *fb, int x, int y) {
    return ((uint16_t*)fb->buf)[x + y * fb->stride];
}

// constants for formats
#define FRAMEBUF_MVLSB  (0)
#define FRAMEBUF_RGB565 (1)

STATIC mp_framebuf_p_t formats[] = {
    [FRAMEBUF_MVLSB] = {mvlsb_setpixel, mvlsb_getpixel},
    [FRAMEBUF_RGB565] = {rgb565_setpixel, rgb565_getpixel},
};

static inline void setpixel(const mp_obj_framebuf_t *fb, int x, int y, uint32_t color) {
    formats[fb->format].setpixel(fb, x, y, color);
}

static inline uint32_t getpixel(const mp_obj_framebuf_t *fb, int x, int y) {
    return formats[fb->format].getpixel(fb, x, y);
}

STATIC mp_obj_t framebuf_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 4, 5, false);

    mp_obj_framebuf_t *o = m_new_obj(mp_obj_framebuf_t);
    o->base.type = type;

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[0], &bufinfo, MP_BUFFER_WRITE);
    o->buf = bufinfo.buf;

    o->width = mp_obj_get_int(args[1]);
    o->height = mp_obj_get_int(args[2]);
    o->format = mp_obj_get_int(args[3]);
    if (n_args >= 5) {
        o->stride = mp_obj_get_int(args[4]);
    } else {
        o->stride = o->width;
    }

    switch (o->format) {
        case FRAMEBUF_MVLSB:
        case FRAMEBUF_RGB565:
            break;
        default:
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                "invalid format"));
    }

    return MP_OBJ_FROM_PTR(o);
}

STATIC mp_obj_t framebuf_fill(mp_obj_t self_in, mp_obj_t col_in) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t col = mp_obj_get_int(col_in);
    for (int y = 0; y < self->height; ++y) {
        for (int x = 0; x < self->width; ++x) {
            setpixel(self, x, y, col);
        }
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(framebuf_fill_obj, framebuf_fill);

STATIC mp_obj_t framebuf_fill_rect(size_t n_args, const mp_obj_t *args) {
    (void)n_args;

    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t width = mp_obj_get_int(args[3]);
    mp_int_t height = mp_obj_get_int(args[4]);
    mp_int_t color = mp_obj_get_int(args[5]);

    if (x + width <= 0 || y + height <= 0 || y >= self->height || x >= self->width) {
        // No operation needed.
        return mp_const_none;
    }

    // clip to the framebuffer
    int xend = MIN(self->width, x + width);
    int yend = MIN(self->height, y + height);
    x = MAX(MIN(x, self->width), 0);
    y = MAX(MIN(y, self->height), 0);

    for (; y < yend; ++y) {
        for (int xc = x; xc < xend; ++xc) {
            setpixel(self, xc, y, color);
        }
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_fill_rect_obj, 6, 6, framebuf_fill_rect);

STATIC mp_obj_t framebuf_pixel(size_t n_args, const mp_obj_t *args) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    if (0 <= x && x < self->width && 0 <= y && y < self->height) {
        if (n_args == 3) {
            // get
            return MP_OBJ_NEW_SMALL_INT(getpixel(self, x, y));
        } else {
            // set
            setpixel(self, x, y, mp_obj_get_int(args[3]));
        }
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_pixel_obj, 3, 4, framebuf_pixel);

STATIC mp_obj_t framebuf_blit(size_t n_args, const mp_obj_t *args) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_obj_framebuf_t *source = MP_OBJ_TO_PTR(args[1]);
    mp_int_t x = mp_obj_get_int(args[2]);
    mp_int_t y = mp_obj_get_int(args[3]);
    mp_int_t key = -1;
    if (n_args > 4) {
        key = mp_obj_get_int(args[4]);
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
    uint32_t color;

    for (; y0 < y0end; ++y0) {
        int cx1 = x1;
        for (int cx0 = x0; cx0 < x0end; ++cx0) {
            color = getpixel(source, cx1, y1);
            if (color != key) {
                setpixel(self, cx0, y0, color);
            }
            ++cx1;
        }
        ++y1;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_blit_obj, 4, 5, framebuf_blit);

STATIC mp_obj_t framebuf_scroll(mp_obj_t self_in, mp_obj_t xstep_in, mp_obj_t ystep_in) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t xstep = mp_obj_get_int(xstep_in);
    mp_int_t ystep = mp_obj_get_int(ystep_in);
    int sx, y, xend, yend, dx, dy;
    if (xstep < 0) {
        sx = 0;
        xend = self->width + xstep;
        dx = 1;
    } else {
        sx = self->width - 1;
        xend = xstep - 1;
        dx = -1;
    }
    if (ystep < 0) {
        y = 0;
        yend = self->height + ystep;
        dy = 1;
    } else {
        y = self->height - 1;
        yend = ystep - 1;
        dy = -1;
    }
    for (; y != yend; y += dy) {
        for (int x = sx; x != xend; x += dx) {
            setpixel(self, x, y, getpixel(self, x - xstep, y - ystep));
        }
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(framebuf_scroll_obj, framebuf_scroll);

STATIC mp_obj_t framebuf_text(size_t n_args, const mp_obj_t *args) {
    // extract arguments
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args[0]);
    const char *str = mp_obj_str_get_str(args[1]);
    mp_int_t x0 = mp_obj_get_int(args[2]);
    mp_int_t y0 = mp_obj_get_int(args[3]);
    mp_int_t col = 1;
    if (n_args >= 5) {
        col = mp_obj_get_int(args[4]);
    }

    // loop over chars
    for (; *str; ++str) {
        // get char and make sure its in range of font
        int chr = *(uint8_t*)str;
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
                            setpixel(self, x0, y, col);
                        }
                    }
                }
            }
        }
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_text_obj, 4, 5, framebuf_text);

STATIC const mp_rom_map_elem_t framebuf_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_fill), MP_ROM_PTR(&framebuf_fill_obj) },
    { MP_ROM_QSTR(MP_QSTR_fill_rect), MP_ROM_PTR(&framebuf_fill_rect_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel), MP_ROM_PTR(&framebuf_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_blit), MP_ROM_PTR(&framebuf_blit_obj) },
    { MP_ROM_QSTR(MP_QSTR_scroll), MP_ROM_PTR(&framebuf_scroll_obj) },
    { MP_ROM_QSTR(MP_QSTR_text), MP_ROM_PTR(&framebuf_text_obj) },
};
STATIC MP_DEFINE_CONST_DICT(framebuf_locals_dict, framebuf_locals_dict_table);

STATIC const mp_obj_type_t mp_type_framebuf = {
    { &mp_type_type },
    .name = MP_QSTR_FrameBuffer,
    .make_new = framebuf_make_new,
    .locals_dict = (mp_obj_t)&framebuf_locals_dict,
};

STATIC const mp_rom_map_elem_t framebuf_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_framebuf) },
    { MP_ROM_QSTR(MP_QSTR_FrameBuffer), MP_ROM_PTR(&mp_type_framebuf) },
    { MP_ROM_QSTR(MP_QSTR_MVLSB), MP_OBJ_NEW_SMALL_INT(FRAMEBUF_MVLSB) },
    { MP_ROM_QSTR(MP_QSTR_RGB565), MP_OBJ_NEW_SMALL_INT(FRAMEBUF_RGB565) },
};

STATIC MP_DEFINE_CONST_DICT(framebuf_module_globals, framebuf_module_globals_table);

const mp_obj_module_t mp_module_framebuf = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&framebuf_module_globals,
};

#endif // MICROPY_PY_FRAMEBUF
