/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
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

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "py/nlr.h"
#include "py/obj.h"
#include "py/runtime.h"

#include "lcd_mono_fb.h"
#include "font_petme128_8x8.h"

#if MICROPY_PY_LCD_MONO_FB

STATIC void lcd_enable_pixel(mp_obj_framebuf_t * p_framebuffer, uint16_t x, uint16_t y) {
    uint16_t column  = (x / 8);
    uint16_t line    = y;
    uint8_t  bit_pos = x % 8;

    p_framebuffer->fb_bytes[line * (p_framebuffer->bytes_stride) + column].byte |= (1 << bit_pos);
    p_framebuffer->fb_dirty[y / 8].byte |= (uint8_t)(0x1 << y % 8);
}

STATIC void lcd_disable_pixel(mp_obj_framebuf_t * p_framebuffer, uint16_t x, uint16_t y) {
    uint16_t column  = (x / 8);
    uint16_t line    = y;
    uint8_t  bit_pos = x % 8;

    p_framebuffer->fb_bytes[line * (p_framebuffer->bytes_stride) + column].byte &= ~(1 << bit_pos);
    p_framebuffer->fb_dirty[y/8].byte |= (uint8_t)(0x1 << y % 8);
}

STATIC void lcd_init(mp_obj_framebuf_t * p_framebuffer) {
    p_framebuffer->fg_color = LCD_BLACK;
    p_framebuffer->bg_color = LCD_WHITE;

    memset(p_framebuffer->fb_bytes, 0x00, p_framebuffer->bytes_stride * p_framebuffer->height);
    memset(p_framebuffer->fb_dirty, 0x00, p_framebuffer->dirty_stride);
}

STATIC void lcd_fg_color_set(mp_obj_framebuf_t * p_framebuffer, uint16_t color) {
    p_framebuffer->fg_color = (color == 0) ? LCD_BLACK : LCD_WHITE;
}

#if 0
STATIC uint16_t lcd_fg_color_get(mp_obj_framebuf_t * p_framebuffer) {
    return p_framebuffer->fg_color;
}
#endif

STATIC void lcd_bg_color_set(mp_obj_framebuf_t * p_framebuffer, uint16_t color) {
    p_framebuffer->bg_color = (color == 0) ? LCD_BLACK : LCD_WHITE;
}

#if 0
STATIC uint16_t lcd_bg_color_get(mp_obj_framebuf_t * p_framebuffer) {
    return p_framebuffer->bg_color;
}
#endif

void display_clear_screen(mp_obj_framebuf_t * p_framebuffer, uint8_t color) {

    lcd_bg_color_set(p_framebuffer, color);
    lcd_fg_color_set(p_framebuffer, !color);

    if (p_framebuffer->bg_color == LCD_BLACK) {
        memset(p_framebuffer->fb_bytes, 0x00, p_framebuffer->bytes_stride * p_framebuffer->height);
    } else {
        memset(p_framebuffer->fb_bytes, 0xFF, p_framebuffer->bytes_stride * p_framebuffer->height);
    }
    memset(p_framebuffer->fb_dirty, 0xFF, p_framebuffer->dirty_stride);
}

STATIC void lcd_print_char(mp_obj_framebuf_t * p_framebuffer, uint16_t x, uint16_t y, char ch) {
    uint16_t col = x;
    for (uint8_t i = 0; i < 8; i++) {

        uint16_t current_col = col + (i * p_framebuffer->font_size);

        for (uint8_t y_pos = 0; y_pos < 8; y_pos++) {
            if ((((uint8_t)font_petme128_8x8[((ch - 32) * 8) + i]) >> y_pos) & 0x01) {
                for (uint8_t s_w = 0; s_w < p_framebuffer->font_size; s_w++) {
                    for (uint8_t s_h = 0; s_h < p_framebuffer->font_size; s_h++) {
                        if (p_framebuffer->fg_color < LCD_WHITE) {
                            lcd_disable_pixel(p_framebuffer,
                                              current_col + s_w,
                                              y + (y_pos * p_framebuffer->font_size) + s_h);
                        } else {
                            lcd_enable_pixel(p_framebuffer,
                                             current_col + s_w,
                                             y + (y_pos * p_framebuffer->font_size) + s_h);
                        }
                    }
                }
            } else {
                for (uint8_t s_w = 0; s_w < p_framebuffer->font_size; s_w++) {
                    for (uint8_t s_h = 0; s_h < p_framebuffer->font_size; s_h++) {
                        if (p_framebuffer->bg_color < LCD_WHITE) {
                            lcd_disable_pixel(p_framebuffer,
                                              current_col + s_w,
                                              y + (y_pos * p_framebuffer->font_size) + s_h);
                        } else {
                            lcd_enable_pixel(p_framebuffer,
                                             current_col + s_w,
                                             y + (y_pos * p_framebuffer->font_size) + s_h);
                        }
                    }
                }
            }
        }
    }
}

#if 0
STATIC void lcd_font_size_set(mp_obj_framebuf_t * p_framebuffer, uint8_t size) {
    m_font_size = size;
}

STATIC uint8_t lcd_font_size_get(mp_obj_framebuf_t * p_framebuffer) {
    return m_font_size;
}
#endif

void display_print_string(mp_obj_framebuf_t * p_framebuffer, uint16_t x, uint16_t y, const char * p_str) {
    uint16_t str_len = strlen(p_str);
    for (uint16_t i = 0; i < str_len; i++) {
        lcd_print_char(p_framebuffer, x + (i * 8 * p_framebuffer->font_size), y, p_str[i]);
    }
}

STATIC void lcd_pixel_draw(mp_obj_framebuf_t * p_framebuffer, uint16_t x, uint16_t y, uint16_t color) {
    if (color < LCD_WHITE) {
        lcd_disable_pixel(p_framebuffer, x, y);
    } else {
        lcd_enable_pixel(p_framebuffer, x, y);
    }
}

void display_update(mp_obj_framebuf_t * p_framebuffer, bool refresh, lcd_update_line_callback_t c_callback) {
    for (uint16_t i = 0; i < p_framebuffer->dirty_stride; i++) {
        if (p_framebuffer->fb_dirty[i].byte != 0 || refresh) {
            for (uint16_t b = 0; b < 8; b++) {
                if ((((p_framebuffer->fb_dirty[i].byte >> b) & 0x01) == 1) || refresh) {
                    uint16_t line_num = (i * 8) + b;

                    if (c_callback != NULL)
                    {
                        if (p_framebuffer->fb_old == NULL) {
                            c_callback(p_framebuffer,
                                       line_num,
                                       &p_framebuffer->fb_bytes[line_num * p_framebuffer->bytes_stride],
                                       NULL);
                        }
                    } else {
                        mp_obj_t args[4];
                        mp_uint_t num_of_args = 3;
                        args[0] = p_framebuffer;
                        args[1] = MP_OBJ_NEW_SMALL_INT(line_num);

                        if (refresh == false) {
                            args[2] = mp_obj_new_bytearray_by_ref(p_framebuffer->bytes_stride,
                                                                  &p_framebuffer->fb_bytes[line_num * p_framebuffer->bytes_stride]);
                        } else {
                            args[2] = mp_const_none;
                        }

                        if (p_framebuffer->fb_old != NULL) {
                            args[3] = mp_obj_new_bytearray_by_ref(p_framebuffer->bytes_stride,
                                                                  &p_framebuffer->fb_bytes[line_num * p_framebuffer->bytes_stride]);
                            num_of_args = 4;
                        }

                        mp_call_function_n_kw(p_framebuffer->line_update_cb, num_of_args, 0, args);
                    }

                    // update old buffer
                    if (p_framebuffer->fb_old != NULL) {
                        memcpy(&p_framebuffer->fb_old[line_num * p_framebuffer->bytes_stride],
                               &p_framebuffer->fb_bytes[line_num * p_framebuffer->bytes_stride],
                               p_framebuffer->dirty_stride);
                    }
                }
            }

            if (refresh == false) {
                p_framebuffer->fb_dirty[i].byte = 0x00;
            }
        }
    }
}

mp_obj_t lcd_mono_fb_helper_make_new(mp_int_t width, mp_int_t height, mp_int_t vertical) {

    mp_obj_framebuf_t *o = m_new_obj(mp_obj_framebuf_t);
    o->base.type = &mp_type_object;

    o->width = width;
    o->height = height;

    o->bytes_stride = o->width / 8;
    o->dirty_stride = o->height / 8;

    o->fb_bytes = m_new(fb_byte_t, (o->bytes_stride) * o->height);
    o->fb_dirty = m_new(fb_byte_t, o->dirty_stride);

    // default to not use double buffer
    o->fb_old = NULL;

    o->font_size = 1;

    lcd_init(o);

    return MP_OBJ_FROM_PTR(o);
}

STATIC mp_obj_t lcd_mono_fb_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 3, 4, false);

    mp_obj_framebuf_t *o = m_new_obj(mp_obj_framebuf_t);
    o->base.type = type;

    o->line_update_cb = args[0];

    o->width = mp_obj_get_int(args[1]);
    o->height = mp_obj_get_int(args[2]);

    o->bytes_stride = o->width / 8;
    o->dirty_stride = o->height / 8;

    o->fb_bytes = m_new(fb_byte_t, (o->bytes_stride) * o->height);
    o->fb_dirty = m_new(fb_byte_t, o->dirty_stride);

    // default to not use double buffer
    o->fb_old = NULL;

    o->font_size = 1;

    if (n_args >= 4) {
        if (mp_obj_is_true(args[3])) {
            o->fb_old = m_new(fb_byte_t, (o->bytes_stride) * o->height);
        }
    }

    lcd_init(o);

    return MP_OBJ_FROM_PTR(o);
}

STATIC mp_obj_t lcd_mono_fb_fill(mp_obj_t self_in, mp_obj_t col_in) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t col = mp_obj_get_int(col_in);

    display_clear_screen(self, col);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(lcd_mono_fb_fill_obj, lcd_mono_fb_fill);

STATIC mp_obj_t lcd_mono_fb_pixel(size_t n_args, const mp_obj_t *args) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t color = mp_obj_get_int(args[3]);

    lcd_pixel_draw(self, x, y, color);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lcd_mono_fb_pixel_obj, 3, 4, lcd_mono_fb_pixel);

STATIC mp_obj_t lcd_mono_fb_scroll(mp_obj_t self_in, mp_obj_t xstep_in, mp_obj_t ystep_in) {
#if 0
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t xstep = mp_obj_get_int(xstep_in);
    mp_int_t ystep = mp_obj_get_int(ystep_in);
#endif
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(lcd_mono_fb_scroll_obj, lcd_mono_fb_scroll);

STATIC mp_obj_t lcd_mono_fb_text(size_t n_args, const mp_obj_t *args) {
    // extract arguments
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args[0]);
    const char *str = mp_obj_str_get_str(args[1]);
    mp_int_t x0 = mp_obj_get_int(args[2]);
    mp_int_t y0 = mp_obj_get_int(args[3]);

    display_print_string(self, x0, y0, str);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lcd_mono_fb_text_obj, 4, 5, lcd_mono_fb_text);


STATIC mp_obj_t lcd_mono_fb_show(mp_obj_t self_in) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(self_in);

    display_update(self, false, NULL);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(lcd_mono_fb_show_obj, lcd_mono_fb_show);

STATIC mp_obj_t lcd_mono_fb_refresh(mp_obj_t self_in) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(self_in);

    display_update(self, true, NULL);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(lcd_mono_fb_refresh_obj, lcd_mono_fb_refresh);


STATIC mp_obj_t lcd_mono_fb_del(mp_obj_t self_in) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(self_in);

    m_free(self->fb_bytes);
    m_free(self->fb_dirty);

    if (self->fb_old != NULL) {
        // free double buffer if used
        m_free(self->fb_old);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(lcd_mono_fb_del_obj, lcd_mono_fb_del);

STATIC const mp_rom_map_elem_t lcd_mono_fb_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&lcd_mono_fb_del_obj) },
    { MP_ROM_QSTR(MP_QSTR_fill), MP_ROM_PTR(&lcd_mono_fb_fill_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel), MP_ROM_PTR(&lcd_mono_fb_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_scroll), MP_ROM_PTR(&lcd_mono_fb_scroll_obj) },
    { MP_ROM_QSTR(MP_QSTR_text), MP_ROM_PTR(&lcd_mono_fb_text_obj) },
    { MP_ROM_QSTR(MP_QSTR_show), MP_ROM_PTR(&lcd_mono_fb_show_obj) },
    { MP_ROM_QSTR(MP_QSTR_refresh), MP_ROM_PTR(&lcd_mono_fb_refresh_obj) },
};
STATIC MP_DEFINE_CONST_DICT(lcd_mono_fb_locals_dict, lcd_mono_fb_locals_dict_table);

STATIC const mp_obj_type_t mp_type_lcd_mono_fb = {
    { &mp_type_type },
    .name = MP_QSTR_MonoFB,
    .make_new = lcd_mono_fb_make_new,
    .locals_dict = (mp_obj_t)&lcd_mono_fb_locals_dict,
};

STATIC const mp_rom_map_elem_t lcd_mono_fb_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_lcd_mono_fb) },
    { MP_ROM_QSTR(MP_QSTR_MonoFB), MP_ROM_PTR(&mp_type_lcd_mono_fb) },
};

STATIC MP_DEFINE_CONST_DICT(lcd_mono_fb_module_globals, lcd_mono_fb_module_globals_table);

const mp_obj_module_t mp_module_lcd_mono_fb = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&lcd_mono_fb_module_globals,
};

#endif // MICROPY_PY_LCD_MONO_FB
