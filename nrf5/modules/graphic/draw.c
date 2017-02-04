/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Glenn Ruben Bakke
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

#include <string.h>

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/mphal.h"

#include "moddisplay.h"
#include "font_petme128_8x8.h"

#if MICROPY_PY_DISPLAY_GRAPHICS

/// \method circle(display, radius, x, y, color, [fill])
/// Draw a circle in the display framebuffer.
STATIC mp_obj_t draw_circle(mp_uint_t n_args, const mp_obj_t *args) {
    display_t * screen = MP_OBJ_TO_PTR(args[0]);
	
    mp_uint_t radius = mp_obj_get_int(args[1]);
    mp_uint_t xc = mp_obj_get_int(args[2]);
    mp_uint_t yc = mp_obj_get_int(args[3]);
    mp_uint_t color = mp_obj_get_int(args[4]);

    pixel_set_callback_t pixel_set_cb = screen->draw_callbacks.pixel_set;

    // algorithm borrowed from:
    // http://stackoverflow.com/a/35541416

    uint16_t y = radius;
    uint16_t x = 0;
    int d = 3 - 2 * radius;

    while (x <= y) {
        for (uint16_t hor = 0; hor < x + 1; hor++) {
            pixel_set_cb(screen, xc+hor, yc+y, color);
            pixel_set_cb(screen, xc-hor, yc+y, color);
            pixel_set_cb(screen, xc+hor, yc-y, color);
            pixel_set_cb(screen, xc-hor, yc-y, color);
            pixel_set_cb(screen, xc+x, yc+hor, color);
            pixel_set_cb(screen, xc-x, yc+hor, color);
            pixel_set_cb(screen, xc+x, yc-hor, color);
            pixel_set_cb(screen, xc-x, yc-hor, color);
            pixel_set_cb(screen, xc+hor, yc+x, color);
            pixel_set_cb(screen, xc-hor, yc+x, color);
            pixel_set_cb(screen, xc+hor, yc-x, color);
            pixel_set_cb(screen, xc-hor, yc-x, color);
            pixel_set_cb(screen, xc+y, yc+hor, color);
            pixel_set_cb(screen, xc-y, yc+hor, color);
            pixel_set_cb(screen, xc+y, yc-hor, color);
            pixel_set_cb(screen, xc-y, yc-hor, color);
        }

        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x-y) + 10;
            y = y - 1;
        }

        x = x + 1;
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(graphics_draw_circle_obj, 5, 6, draw_circle);

STATIC void graphics_draw_char(display_t * screen, uint16_t x, uint16_t y, char ch, uint16_t color, uint8_t font_size) {

   pixel_set_callback_t pixel_set_cb = screen->draw_callbacks.pixel_set;

    uint16_t col = x;
    for (uint8_t i = 0; i < 8; i++) {
        uint16_t current_col = col + (i * font_size);

        for (uint8_t y_pos = 0; y_pos < 8; y_pos++) {
            if ((((uint8_t)font_petme128_8x8[((ch - 32) * 8) + i]) >> y_pos) & 0x01) {
                for (uint8_t s_w = 0; s_w < font_size; s_w++) {
                    for (uint8_t s_h = 0; s_h < font_size; s_h++) {

                    	uint16_t pix_x = current_col + s_w;
                    	uint16_t pix_y = y + (y_pos * font_size) + s_h;

                    	pixel_set_cb(screen, pix_x, pix_y, color);
                    }
                }
            } else {
                for (uint8_t s_w = 0; s_w < font_size; s_w++) {
                    for (uint8_t s_h = 0; s_h < font_size; s_h++) {
                    	uint16_t pix_x = current_col + s_w;
                    	uint16_t pix_y = y + (y_pos * font_size) + s_h;

                    	pixel_set_cb(screen, pix_x, pix_y, !color);
                    }
                }
            }
        }
    }
}

STATIC mp_obj_t graphics_draw_text(size_t n_args, const mp_obj_t *args) {
    display_t * screen = MP_OBJ_TO_PTR(args[0]);

    const char *str = mp_obj_str_get_str(args[1]);
    mp_int_t x = mp_obj_get_int(args[2]);
    mp_int_t y = mp_obj_get_int(args[3]);
    mp_int_t color = 0;
    if (n_args >= 4) {
        color = mp_obj_get_int(args[4]);
    }

    uint8_t font_size = 1;

    uint16_t str_len = strlen(str);
    for (uint16_t i = 0; i < str_len; i++) {
        graphics_draw_char(screen, x + (i * 8 * font_size), y, str[i], color, font_size);
    }

    (void)color;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(graphics_draw_text_obj, 4, 5, graphics_draw_text);

/// \method rectangle(display, x0, y0, x1, y1, color, [fill])
STATIC mp_obj_t draw_rectangle(mp_uint_t n_args, const mp_obj_t *args) {
    display_t * screen = MP_OBJ_TO_PTR(args[0]);
    mp_uint_t x0 = mp_obj_get_int(args[1]);
    mp_uint_t y0 = mp_obj_get_int(args[2]);
    mp_uint_t x1 = mp_obj_get_int(args[3]);
    mp_uint_t y1 = mp_obj_get_int(args[4]);
    mp_uint_t color = mp_obj_get_int(args[5]);
    
    pixel_set_callback_t pixel_set_cb = screen->draw_callbacks.pixel_set;
    
    // horizontals
    for (mp_uint_t hpos = x0; hpos <= x1; hpos++) {
        pixel_set_cb(screen, hpos, y0, color);
        pixel_set_cb(screen, hpos, y1, color);
    }
    // verticals
    for (mp_uint_t vpos = y0; vpos <= y1; vpos++) {
        pixel_set_cb(screen, x0, vpos, color);
        pixel_set_cb(screen, x1, vpos, color);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(graphics_draw_rectangle_obj, 6, 7, draw_rectangle);


STATIC const mp_map_elem_t graphics_globals_dict_table[] = {
    // class methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_circle), (mp_obj_t)&graphics_draw_circle_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_text), (mp_obj_t)&graphics_draw_text_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_rectangle), (mp_obj_t)&graphics_draw_rectangle_obj },
};

STATIC MP_DEFINE_CONST_DICT(graphics_globals_dict, graphics_globals_dict_table);

const mp_obj_module_t graphics_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&graphics_globals_dict,
};

#endif // MICROPY_PY_DISPLAY_GRAPHICS
