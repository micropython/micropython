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
typedef struct _mp_obj_framebuf1_t {
    mp_obj_base_t base;
    uint8_t *buf;
    uint16_t width, height, stride;
} mp_obj_framebuf1_t;

STATIC mp_obj_t framebuf1_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 3, 4, false);

    mp_obj_framebuf1_t *o = m_new_obj(mp_obj_framebuf1_t);
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

STATIC mp_obj_t framebuf1_fill(mp_obj_t self_in, mp_obj_t col_in) {
    mp_obj_framebuf1_t *self = MP_OBJ_TO_PTR(self_in);
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
    mp_obj_framebuf1_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    if (0 <= x && x < self->width && 0 <= y && y < self->height) {
        int index = (y / 8) * self->stride + x;
        if (n_args == 3) {
            // get
            return MP_OBJ_NEW_SMALL_INT((self->buf[index] >> (y & 7)) & 1);
        } else {
            // set
            if (mp_obj_get_int(args[3])) {
                self->buf[index] |= (1 << (y & 7));
            } else {
                self->buf[index] &= ~(1 << (y & 7));
            }
        }
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf1_pixel_obj, 3, 4, framebuf1_pixel);

STATIC mp_obj_t framebuf1_scroll(mp_obj_t self_in, mp_obj_t xstep_in, mp_obj_t ystep_in) {
    mp_obj_framebuf1_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t xstep = mp_obj_get_int(xstep_in);
    mp_int_t ystep = mp_obj_get_int(ystep_in);
    int end = (self->height + 7) >> 3;
    if (ystep > 0) {
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
    } else if (ystep < 0) {
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
    if (xstep < 0) {
        for (int y = 0; y < end; ++y) {
            for (int x = 0; x < self->width + xstep; ++x) {
                self->buf[y * self->stride + x] = self->buf[y * self->stride + x - xstep];
            }
        }
    } else if (xstep > 0) {
        for (int y = 0; y < end; ++y) {
            for (int x = self->width - 1; x >= xstep; --x) {
                self->buf[y * self->stride + x] = self->buf[y * self->stride + x - xstep];
            }
        }
    }
    // TODO: Should we clear the margin created by scrolling?
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(framebuf1_scroll_obj, framebuf1_scroll);

STATIC mp_obj_t framebuf1_text(size_t n_args, const mp_obj_t *args) {
    // extract arguments
    mp_obj_framebuf1_t *self = MP_OBJ_TO_PTR(args[0]);
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
                            uint byte_pos = x0 + self->stride * ((uint)y >> 3);
                            if (col == 0) {
                                // clear pixel
                                self->buf[byte_pos] &= ~(1 << (y & 7));
                            } else {
                                // set pixel
                                self->buf[byte_pos] |= 1 << (y & 7);
                            }
                        }
                    }
                }
            }
        }
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf1_text_obj, 4, 5, framebuf1_text);

STATIC const mp_rom_map_elem_t framebuf1_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_fill), MP_ROM_PTR(&framebuf1_fill_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel), MP_ROM_PTR(&framebuf1_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_scroll), MP_ROM_PTR(&framebuf1_scroll_obj) },
    { MP_ROM_QSTR(MP_QSTR_text), MP_ROM_PTR(&framebuf1_text_obj) },
};
STATIC MP_DEFINE_CONST_DICT(framebuf1_locals_dict, framebuf1_locals_dict_table);

STATIC const mp_obj_type_t mp_type_framebuf1 = {
    { &mp_type_type },
    .name = MP_QSTR_FrameBuffer1,
    .make_new = framebuf1_make_new,
    .locals_dict = (mp_obj_t)&framebuf1_locals_dict,
};

STATIC const mp_rom_map_elem_t framebuf_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_framebuf) },
    { MP_ROM_QSTR(MP_QSTR_FrameBuffer1), MP_ROM_PTR(&mp_type_framebuf1) },
};

STATIC MP_DEFINE_CONST_DICT(framebuf_module_globals, framebuf_module_globals_table);

const mp_obj_module_t mp_module_framebuf = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&framebuf_module_globals,
};

#endif // MICROPY_PY_FRAMEBUF
