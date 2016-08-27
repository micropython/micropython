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

// 1-bit frame buffer, each byte is a column of 8 pixels
typedef struct _mp_obj_framebuf_t {
    mp_obj_base_t base;
    uint8_t *buf;
    uint16_t width, height, stride;
} mp_obj_framebuf_t;


typedef void setpixel_t(const mp_obj_framebuf_t *, int, int, uint32_t);
typedef uint32_t getpixel_t(const mp_obj_framebuf_t *, int, int);


// General drawing functions.
STATIC mp_obj_t framebuf_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 3, 4, false);

    mp_obj_framebuf_t *o = m_new_obj(mp_obj_framebuf_t);
    o->base.type = type;

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[0], &bufinfo, MP_BUFFER_WRITE);
    o->buf = bufinfo.buf;

    o->width = mp_obj_get_int(args[1]);
    o->height = mp_obj_get_int(args[2]);
    o->stride = o->width;
    if (n_args >= 4) {
        o->stride = mp_obj_get_int(args[3]);
    }

    return MP_OBJ_FROM_PTR(o);
}

STATIC mp_obj_t framebuf_pixel(size_t n_args, const mp_obj_t *args, setpixel_t setpixel, getpixel_t getpixel) {
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

STATIC void framebuf_text(size_t n_args, const mp_obj_t *args, setpixel_t setpixel) {
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
}

STATIC void framebuf_blit(size_t n_args, const mp_obj_t *args, setpixel_t setpixel, getpixel_t getpixel) {
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
        return;
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
}

STATIC void framebuf_scroll(mp_obj_t self_in, mp_obj_t xstep_in, mp_obj_t ystep_in, setpixel_t setpixel, getpixel_t getpixel) {
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
}

// Functions for FrameBuffer1
STATIC void framebuf1_setpixel(const mp_obj_framebuf_t *fb, int x, int y, uint32_t color) {
    size_t index = (y >> 3) * fb->stride + x;
    uint8_t offset = y & 0x07;
    fb->buf[index] = (fb->buf[index] & ~(0x01 << offset)) | (color != 0) << offset;
}

STATIC uint32_t framebuf1_getpixel(const mp_obj_framebuf_t *fb, int x, int y) {
    return (fb->buf[(y >> 3) * fb->stride + x] >> (y & 0x07)) & 0x01;
}

STATIC mp_obj_t framebuf1_fill(mp_obj_t self_in, mp_obj_t col_in) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t col = mp_obj_get_int(col_in);
    if (col) {
        col = 0xff;
    }
    int end = (self->height + 7) >> 3;
    for (int y = 0; y < end; ++y) {
        memset(self->buf + y * self->stride, col, self->width);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(framebuf1_fill_obj, framebuf1_fill);

STATIC mp_obj_t framebuf1_pixel(size_t n_args, const mp_obj_t *args) {
    return framebuf_pixel(n_args, args, framebuf1_setpixel, framebuf1_getpixel);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf1_pixel_obj, 3, 4, framebuf1_pixel);

STATIC mp_obj_t framebuf1_scroll(mp_obj_t self_in, mp_obj_t xstep_in, mp_obj_t ystep_in) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t xstep = mp_obj_get_int(xstep_in);
    mp_int_t ystep = mp_obj_get_int(ystep_in);
    int end = (self->height + 7) >> 3;
    if (xstep == 0 && ystep > 0) {
        for (int y = end; y > 0;) {
            --y;
            for (int x = 0; x < self->width; ++x) {
                int prev = 0;
                if (y > 0) {
                    prev = (self->buf[(y - 1) * self->stride + x] >> (8 - ystep)) & ((1 << ystep) - 1);
                }
                self->buf[y * self->stride + x] = (self->buf[y * self->stride + x] << ystep) | prev;
            }
        }
    } else if (xstep == 0 && ystep < 0) {
        for (int y = 0; y < end; ++y) {
            for (int x = 0; x < self->width; ++x) {
                int prev = 0;
                if (y + 1 < end) {
                    prev = self->buf[(y + 1) * self->stride + x] << (8 + ystep);
                }
                self->buf[y * self->stride + x] = (self->buf[y * self->stride + x] >> -ystep) | prev;
            }
        }
    }
    // TODO xstep!=0
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(framebuf1_scroll_obj, framebuf1_scroll);


STATIC mp_obj_t framebuf1_text(size_t n_args, const mp_obj_t *args) {
    framebuf_text(n_args, args, framebuf1_setpixel);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf1_text_obj, 4, 5, framebuf1_text);

// Copy one framebuffer onto the other at the specified offset.
// Offset can be negative.
// If color key is specified, that color is not copied.
STATIC mp_obj_t framebuf1_blit(size_t n_args, const mp_obj_t *args) {
    framebuf_blit(n_args, args, framebuf1_setpixel, framebuf1_getpixel);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf1_blit_obj, 4, 5, framebuf1_blit);

// Create a new FrameBuffer object, sharing the buffer, but of smaller size.
STATIC mp_obj_t framebuf1_window(size_t n_args, const mp_obj_t *args) {
    mp_arg_check_num(n_args, 0, 5, 5, false);
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t w = mp_obj_get_int(args[3]);
    mp_int_t h = mp_obj_get_int(args[4]);

    // We can only begin every 8th line.
    if (y & 0x07) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError,
            "y must be divisible by 8"));
    }

    // Crop to framebuffer.
    x = MAX(0, MIN(self->width - 1, x));
    y = MAX(0, MIN(self->height - 1, y));
    w = MAX(1, MIN(self->width - x, w));
    h = MAX(1, MIN(self->height - y, h));

    mp_obj_framebuf_t *win = m_new_obj(mp_obj_framebuf_t);
    win->base.type = self->base.type;
    win->buf = self->buf + (y << 3) * self->stride + x;
    win->width = w;
    win->height = h;
    win->stride = self->stride;

    return MP_OBJ_FROM_PTR(win);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf1_window_obj, 5, 5, framebuf1_window);

// Functions for FrameBuffer16
STATIC void framebuf16_setpixel(const mp_obj_framebuf_t *fb, int x, int y, uint32_t color) {
    // Due to ridiculous warnings, cast to void * first.
    ((uint16_t *)((void *)(fb->buf)))[x + y * fb->stride] = color;

}
STATIC uint32_t framebuf16_getpixel(const mp_obj_framebuf_t *fb, int x, int y) {
    // Due to ridiculous warnings, cast to void * first.
    return ((uint16_t *)((void *)(fb->buf)))[x + y * fb->stride];
}

STATIC mp_obj_t framebuf16_fill(mp_obj_t self_in, mp_obj_t col_in) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t col = mp_obj_get_int(col_in);
    for (int y = 0; y < self->height; ++y) {
        for (int x = 0; x < self->width; ++x) {
            ((uint16_t *)((void *)(self->buf)))[y * self->stride + x] = col;
        }
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(framebuf16_fill_obj, framebuf16_fill);

STATIC mp_obj_t framebuf16_pixel(size_t n_args, const mp_obj_t *args) {
    return framebuf_pixel(n_args, args, framebuf16_setpixel, framebuf16_getpixel);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf16_pixel_obj, 3, 4, framebuf16_pixel);

STATIC mp_obj_t framebuf16_scroll(mp_obj_t self_in, mp_obj_t xstep_in, mp_obj_t ystep_in) {
    framebuf_scroll(self_in, xstep_in, ystep_in, framebuf16_setpixel, framebuf16_getpixel);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(framebuf16_scroll_obj, framebuf16_scroll);

STATIC mp_obj_t framebuf16_text(size_t n_args, const mp_obj_t *args) {
    framebuf_text(n_args, args, framebuf16_setpixel);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf16_text_obj, 4, 5, framebuf16_text);

STATIC mp_obj_t framebuf16_blit(size_t n_args, const mp_obj_t *args) {
    framebuf_blit(n_args, args, framebuf16_setpixel, framebuf16_getpixel);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf16_blit_obj, 4, 5, framebuf16_blit);

// Create a new FrameBuffer object, sharing the buffer, but of smaller size.
STATIC mp_obj_t framebuf16_window(size_t n_args, const mp_obj_t *args) {
    mp_arg_check_num(n_args, 0, 5, 5, false);
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t w = mp_obj_get_int(args[3]);
    mp_int_t h = mp_obj_get_int(args[4]);

    // Crop to framebuffer.
    x = MAX(0, MIN(self->width - 1, x));
    y = MAX(0, MIN(self->height - 1, y));
    w = MAX(1, MIN(self->width - x, w));
    h = MAX(1, MIN(self->height - y, h));

    mp_obj_framebuf_t *win = m_new_obj(mp_obj_framebuf_t);
    win->base.type = self->base.type;
    win->buf = self->buf + (y * self->stride + x) * 2;
    win->width = w;
    win->height = h;
    win->stride = self->stride;

    return MP_OBJ_FROM_PTR(win);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf16_window_obj, 5, 5, framebuf16_window);

STATIC const mp_rom_map_elem_t framebuf1_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_fill), MP_ROM_PTR(&framebuf1_fill_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel), MP_ROM_PTR(&framebuf1_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_scroll), MP_ROM_PTR(&framebuf1_scroll_obj) },
    { MP_ROM_QSTR(MP_QSTR_text), MP_ROM_PTR(&framebuf1_text_obj) },
    { MP_ROM_QSTR(MP_QSTR_blit), MP_ROM_PTR(&framebuf1_blit_obj) },
    { MP_ROM_QSTR(MP_QSTR_window), MP_ROM_PTR(&framebuf1_window_obj) },
};
STATIC MP_DEFINE_CONST_DICT(framebuf1_locals_dict, framebuf1_locals_dict_table);

STATIC const mp_obj_type_t mp_type_framebuf1 = {
    { &mp_type_type },
    .name = MP_QSTR_FrameBuffer1,
    .make_new = framebuf_make_new,
    .locals_dict = (mp_obj_t)&framebuf1_locals_dict,
};

STATIC const mp_rom_map_elem_t framebuf16_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_fill), MP_ROM_PTR(&framebuf16_fill_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel), MP_ROM_PTR(&framebuf16_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_scroll), MP_ROM_PTR(&framebuf16_scroll_obj) },
    { MP_ROM_QSTR(MP_QSTR_text), MP_ROM_PTR(&framebuf16_text_obj) },
    { MP_ROM_QSTR(MP_QSTR_blit), MP_ROM_PTR(&framebuf16_blit_obj) },
    { MP_ROM_QSTR(MP_QSTR_window), MP_ROM_PTR(&framebuf16_window_obj) },
};
STATIC MP_DEFINE_CONST_DICT(framebuf16_locals_dict, framebuf16_locals_dict_table);

STATIC const mp_obj_type_t mp_type_framebuf16 = {
    { &mp_type_type },
    .name = MP_QSTR_FrameBuffer16,
    .make_new = framebuf_make_new,
    .locals_dict = (mp_obj_t)&framebuf16_locals_dict,
};

STATIC const mp_rom_map_elem_t framebuf_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_framebuf) },
    { MP_ROM_QSTR(MP_QSTR_FrameBuffer1), MP_ROM_PTR(&mp_type_framebuf1) },
    { MP_ROM_QSTR(MP_QSTR_FrameBuffer16), MP_ROM_PTR(&mp_type_framebuf16) },
};

STATIC MP_DEFINE_CONST_DICT(framebuf_module_globals, framebuf_module_globals_table);

const mp_obj_module_t mp_module_framebuf = {
    .base = { &mp_type_module },
    .name = MP_QSTR_framebuf,
    .globals = (mp_obj_dict_t*)&framebuf_module_globals,
};

#endif // MICROPY_PY_FRAMEBUF
