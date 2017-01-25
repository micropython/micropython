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

#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "genhdr/pins.h"

#include "lcd_ssd1289_driver.h"

#if MICROPY_PY_DISPLAY_LCD_SSD1289

/// \moduleref display
/// \class SSD1289 - SSD1289 TFT LCD display driver.

#include "moddisplay.h"
#include "framebuffer.h"
#include "pin.h"

typedef struct _lcd_ssd1289_obj_t {
    mp_obj_base_t base;
    display_draw_callbacks_t draw_callbacks;
    framebuffer_t * framebuffer;
    pin_obj_t * pin_cs;
    pin_obj_t * pin_rs;
    pin_obj_t * pin_wr;
    pin_obj_t * pin_reset;
    pin_obj_t * pin_d0;
    pin_obj_t * pin_d1;
    pin_obj_t * pin_d2;
    pin_obj_t * pin_d3;
    pin_obj_t * pin_d4;
    pin_obj_t * pin_d5;
    pin_obj_t * pin_d6;
    pin_obj_t * pin_d7;
} lcd_ssd1289_obj_t;

#define LCD_SSD1289_COLOR_BLACK 0
#define LCD_SSD1289_COLOR_WHITE 1

static void set_pixel(void * p_display,
                      uint16_t    x,
                      uint16_t    y,
                      uint16_t    color) {
    lcd_ssd1289_obj_t *self = (lcd_ssd1289_obj_t *)p_display;

    if (color == LCD_SSD1289_COLOR_BLACK) {
        framebuffer_pixel_clear(self->framebuffer, x, y);
    } else {
        framebuffer_pixel_set(self->framebuffer, x, y);
    }
}

/// \method __str__()
/// Return a string describing the SSD1289 object.
STATIC void lcd_ssd1289_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    lcd_ssd1289_obj_t *self = o;

    mp_printf(print, "SSD1289(cs=(port=%u, pin=%u), rs=(port=%u, pin=%u),\n",
                     self->pin_cs->port,
                     self->pin_cs->pin,
                     self->pin_rs->port,
                     self->pin_rs->pin);

    mp_printf(print, "        wr=(port=%u, pin=%u), reset=(port=%u, pin=%u),\n",
                     self->pin_wr->port,
                     self->pin_wr->pin,
                     self->pin_reset->port,
                     self->pin_reset->pin);

    mp_printf(print, "        d0=(port=%u, pin=%u), d1=(port=%u, pin=%u),\n",
                     self->pin_d0->port,
                     self->pin_d0->pin,
                     self->pin_d1->port,
                     self->pin_d1->pin);

    mp_printf(print, "        d2=(port=%u, pin=%u), d3=(port=%u, pin=%u),\n",
                     self->pin_d2->port,
                     self->pin_d2->pin,
                     self->pin_d3->port,
                     self->pin_d3->pin);

    mp_printf(print, "        d4=(port=%u, pin=%u), d5=(port=%u, pin=%u),\n",
                     self->pin_d4->port,
                     self->pin_d4->pin,
                     self->pin_d5->port,
                     self->pin_d5->pin);

    mp_printf(print, "        d6=(port=%u, pin=%u), d7=(port=%u, pin=%u),\n",
                     self->pin_d6->port,
                     self->pin_d6->pin,
                     self->pin_d7->port,
                     self->pin_d7->pin);


    mp_printf(print, "        FB(width=%u, height=%u, dir=%u, fb_stride=%u, fb_dirty_stride=%u))\n",
                     self->framebuffer->screen_width,
                     self->framebuffer->screen_height,
                     self->framebuffer->line_orientation,
                     self->framebuffer->fb_stride,
                     self->framebuffer->fb_dirty_stride);
}

// for make_new
enum {
    ARG_NEW_WIDTH,
    ARG_NEW_HEIGHT,
    ARG_NEW_CS,
    ARG_NEW_RS,
    ARG_NEW_WR,
    ARG_NEW_RESET,
    ARG_NEW_D0,
    ARG_NEW_D1,
    ARG_NEW_D2,
    ARG_NEW_D3,
    ARG_NEW_D4,
    ARG_NEW_D5,
    ARG_NEW_D6,
    ARG_NEW_D7,

};

/*

Example for nrf51822 / pca10028:

from machine import Pin
from display import SSD1289
import draw
cs = Pin("A1", mode=Pin.OUT, pull=Pin.PULL_UP)
rs = Pin("A2", mode=Pin.OUT, pull=Pin.PULL_UP)
wr = Pin("A3", mode=Pin.OUT, pull=Pin.PULL_UP)
reset = Pin("A4", mode=Pin.OUT, pull=Pin.PULL_UP)

d0 = Pin("A12", mode=Pin.OUT, pull=Pin.PULL_UP)
d1 = Pin("A13", mode=Pin.OUT, pull=Pin.PULL_UP)
d2 = Pin("A14", mode=Pin.OUT, pull=Pin.PULL_UP)
d3 = Pin("A15", mode=Pin.OUT, pull=Pin.PULL_UP)
d4 = Pin("A16", mode=Pin.OUT, pull=Pin.PULL_UP)
d5 = Pin("A17", mode=Pin.OUT, pull=Pin.PULL_UP)
d6 = Pin("A18", mode=Pin.OUT, pull=Pin.PULL_UP)
d7 = Pin("A19", mode=Pin.OUT, pull=Pin.PULL_UP)

d = SSD1289(320, 240, cs, rs, wr, reset, d0, d1, d2, d3, d4, d5, d6, d7)
draw.text(d, "Hello World!", 32, 32)
d.show()

Example for nrf52832 / pca10040:

from machine import Pin
from display import SSD1289
import draw
cs = Pin("A3", mode=Pin.OUT, pull=Pin.PULL_UP)
rs = Pin("A4", mode=Pin.OUT, pull=Pin.PULL_UP)
wr = Pin("A28", mode=Pin.OUT, pull=Pin.PULL_UP)
reset = Pin("A29", mode=Pin.OUT, pull=Pin.PULL_UP)

d0 = Pin("A11", mode=Pin.OUT, pull=Pin.PULL_UP)
d1 = Pin("A12", mode=Pin.OUT, pull=Pin.PULL_UP)
d2 = Pin("A13", mode=Pin.OUT, pull=Pin.PULL_UP)
d3 = Pin("A14", mode=Pin.OUT, pull=Pin.PULL_UP)
d4 = Pin("A15", mode=Pin.OUT, pull=Pin.PULL_UP)
d5 = Pin("A16", mode=Pin.OUT, pull=Pin.PULL_UP)
d6 = Pin("A17", mode=Pin.OUT, pull=Pin.PULL_UP)
d7 = Pin("A18", mode=Pin.OUT, pull=Pin.PULL_UP)

d = SSD1289(240, 320, cs, rs, wr, reset, d0, d1, d2, d3, d4, d5, d6, d7)
draw.text(d, "Hello World!", 32, 32)
d.show()

Example for nrf52840 / pca10056:

from machine import Pin
from display import SSD1289
import draw
cs = Pin("A3", mode=Pin.OUT, pull=Pin.PULL_UP)
rs = Pin("A4", mode=Pin.OUT, pull=Pin.PULL_UP)
wr = Pin("A28", mode=Pin.OUT, pull=Pin.PULL_UP)
reset = Pin("A29", mode=Pin.OUT, pull=Pin.PULL_UP)

d0 = Pin("B1", mode=Pin.OUT, pull=Pin.PULL_UP)
d1 = Pin("B2", mode=Pin.OUT, pull=Pin.PULL_UP)
d2 = Pin("B3", mode=Pin.OUT, pull=Pin.PULL_UP)
d3 = Pin("B4", mode=Pin.OUT, pull=Pin.PULL_UP)
d4 = Pin("B5", mode=Pin.OUT, pull=Pin.PULL_UP)
d5 = Pin("B6", mode=Pin.OUT, pull=Pin.PULL_UP)
d6 = Pin("B7", mode=Pin.OUT, pull=Pin.PULL_UP)
d7 = Pin("B8", mode=Pin.OUT, pull=Pin.PULL_UP)

d = SSD1289(320, 240, cs, rs, wr, reset, d0, d1, d2, d3, d4, d5, d6, d7)
draw.text(d, "Hello World!", 32, 32)
d.show()

*/
STATIC mp_obj_t lcd_ssd1289_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    static const mp_arg_t allowed_args[] = {
        { ARG_NEW_WIDTH,  MP_ARG_REQUIRED | MP_ARG_INT },
        { ARG_NEW_HEIGHT, MP_ARG_REQUIRED | MP_ARG_INT },
        { ARG_NEW_CS,     MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { ARG_NEW_RS,     MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { ARG_NEW_WR,     MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { ARG_NEW_RESET,  MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { ARG_NEW_D0,     MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { ARG_NEW_D1,     MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { ARG_NEW_D2,     MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { ARG_NEW_D3,     MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { ARG_NEW_D4,     MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { ARG_NEW_D5,     MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { ARG_NEW_D6,     MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { ARG_NEW_D7,     MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    lcd_ssd1289_obj_t *s = m_new_obj_with_finaliser(lcd_ssd1289_obj_t);
    s->base.type = type;
    s->draw_callbacks.pixel_set = set_pixel;

    mp_int_t width;
    mp_int_t height;

    if (args[ARG_NEW_WIDTH].u_int > 0) {
        width = args[ARG_NEW_WIDTH].u_int;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  "Display width not set"));
    }

    if (args[ARG_NEW_HEIGHT].u_int > 0) {
        height = args[ARG_NEW_HEIGHT].u_int;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  "Display height not set"));
    }

    if (args[ARG_NEW_CS].u_obj != MP_OBJ_NULL) {
        s->pin_cs = args[ARG_NEW_CS].u_obj;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  "Display CS Pin not set"));
    }

    if (args[ARG_NEW_RS].u_obj != MP_OBJ_NULL) {
        s->pin_rs = args[ARG_NEW_RS].u_obj;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  "Display RS Pin not set"));
    }

    if (args[ARG_NEW_WR].u_obj != MP_OBJ_NULL) {
        s->pin_wr = args[ARG_NEW_WR].u_obj;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  "Display WR Pin not set"));
    }

    if (args[ARG_NEW_RESET].u_obj != MP_OBJ_NULL) {
        s->pin_reset = args[ARG_NEW_RESET].u_obj;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  "Display Reset Pin not set"));
    }

    if (args[ARG_NEW_D0].u_obj != MP_OBJ_NULL) {
        s->pin_d0 = args[ARG_NEW_D0].u_obj;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  "Display Data 0 Pin not set"));
    }

    if (args[ARG_NEW_D1].u_obj != MP_OBJ_NULL) {
        s->pin_d1 = args[ARG_NEW_D1].u_obj;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  "Display Data 1 Pin not set"));
    }

    if (args[ARG_NEW_D2].u_obj != MP_OBJ_NULL) {
        s->pin_d2 = args[ARG_NEW_D2].u_obj;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  "Display Data 2 Pin not set"));
    }

    if (args[ARG_NEW_D3].u_obj != MP_OBJ_NULL) {
        s->pin_d3 = args[ARG_NEW_D3].u_obj;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  "Display Data 3 Pin not set"));
    }

    if (args[ARG_NEW_D4].u_obj != MP_OBJ_NULL) {
        s->pin_d4 = args[ARG_NEW_D4].u_obj;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  "Display Data 4 Pin not set"));
    }

    if (args[ARG_NEW_D5].u_obj != MP_OBJ_NULL) {
        s->pin_d5 = args[ARG_NEW_D5].u_obj;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  "Display Data 5 Pin not set"));
    }

    if (args[ARG_NEW_D6].u_obj != MP_OBJ_NULL) {
        s->pin_d6 = args[ARG_NEW_D6].u_obj;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  "Display Data 6 Pin not set"));
    }

    if (args[ARG_NEW_D7].u_obj != MP_OBJ_NULL) {
        s->pin_d7 = args[ARG_NEW_D7].u_obj;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  "Display Data 7 Pin not set"));
    }

    framebuffer_init_t init_conf = {
        .width = width,
        .height = height,
        .line_orientation = FRAMEBUFFER_LINE_DIR_HORIZONTAL,
        .double_buffer = false
    };

    s->framebuffer = m_new(framebuffer_t, sizeof(framebuffer_t));

    framebuffer_init(s->framebuffer, &init_conf);

    driver_ssd1289_init(s->pin_cs,
                        s->pin_rs,
                        s->pin_wr,
                        s->pin_reset,
                        s->pin_d0,
                        s->pin_d1,
                        s->pin_d2,
                        s->pin_d3,
                        s->pin_d4,
                        s->pin_d5,
                        s->pin_d6,
                        s->pin_d7);

    // Default to black background
    driver_ssd1289_clear(0x00FF);

    framebuffer_clear(s->framebuffer);

    return MP_OBJ_FROM_PTR(s);
}

// text

/// \method fill(color)
/// Fill framebuffer with the color defined as argument.
STATIC mp_obj_t lcd_ssd1289_fill(mp_obj_t self_in, mp_obj_t color) {
    lcd_ssd1289_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (color == MP_OBJ_NEW_SMALL_INT(LCD_SSD1289_COLOR_BLACK)) {
        framebuffer_clear(self->framebuffer);
    } else {
        framebuffer_fill(self->framebuffer);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(lcd_ssd1289_fill_obj, lcd_ssd1289_fill);

static void render(framebuffer_t * p_framebuffer) {
    for (uint16_t i = 0; i < p_framebuffer->fb_dirty_stride; i++) {
        if (p_framebuffer->fb_dirty[i].byte != 0) {
            for (uint16_t b = 0; b < 8; b++) {
                if ((((p_framebuffer->fb_dirty[i].byte >> b) & 0x01) == 1)) {
                    uint16_t line_num = (i * 8) + b;
                    driver_ssd1289_update_line(line_num,
                        &p_framebuffer->fb_new[line_num * p_framebuffer->fb_stride],
						p_framebuffer->fb_stride);
                }
            }

            p_framebuffer->fb_dirty[i].byte = 0x00;
        }
    }
}

/// \method show()
/// Display content in framebuffer.
STATIC mp_obj_t lcd_ssd1289_show(size_t n_args, const mp_obj_t *args) {
    lcd_ssd1289_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    render(self->framebuffer);
    framebuffer_flip(self->framebuffer);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lcd_ssd1289_show_obj, 1, 2, lcd_ssd1289_show);

/// \method refresh([num_of_refresh])
/// Refresh content in framebuffer.
///
///   - With no argument, 1 refresh will be done.
///   - With `num_of_refresh` given, The whole framebuffer will be considered
///   dirty and will be refreshed the given number of times.
STATIC mp_obj_t lcd_ssd1289_refresh(mp_obj_t self_in) {
    lcd_ssd1289_obj_t *self = MP_OBJ_TO_PTR(self_in);

    (void)self;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(lcd_ssd1289_refresh_obj, lcd_ssd1289_refresh);

/// \method pixel(x, y, [color])
/// Write one pixel in framebuffer.
///
///   - With no argument, the color of the pixel in framebuffer will be returend.
///   - With `color` given, sets the pixel to the color given.
STATIC mp_obj_t lcd_ssd1289_pixel(size_t n_args, const mp_obj_t *args) {
    lcd_ssd1289_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t color = mp_obj_get_int(args[3]);

    set_pixel(self, x, y, color);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lcd_ssd1289_pixel_obj, 4, 4, lcd_ssd1289_pixel);

/// \method pixel(text, x, y, [color])
/// Write one pixel in framebuffer.
///
///   - With no argument, the color will be the opposite of background (fill color).
///   - With `color` given, sets the pixel to the color given.
STATIC mp_obj_t lcd_ssd1289_text(size_t n_args, const mp_obj_t *args) {
    lcd_ssd1289_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    const char *str = mp_obj_str_get_str(args[1]);
    mp_int_t x = mp_obj_get_int(args[2]);
    mp_int_t y = mp_obj_get_int(args[3]);
    mp_int_t color;
    if (n_args >= 4) {
        color = mp_obj_get_int(args[3]);
    }

    //display_print_string(self->framebuffer, x, y, str);

    (void)x;
    (void)y;
    (void)self;
    (void)str;
    (void)color;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lcd_ssd1289_text_obj, 4, 5, lcd_ssd1289_text);

STATIC mp_obj_t lcd_ssd1289_del(mp_obj_t self_in) {
    lcd_ssd1289_obj_t *self = MP_OBJ_TO_PTR(self_in);

    (void)self;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(lcd_ssd1289_del_obj, lcd_ssd1289_del);

STATIC const mp_map_elem_t lcd_ssd1289_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___del__), (mp_obj_t)(&lcd_ssd1289_del_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_fill), (mp_obj_t)(&lcd_ssd1289_fill_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_show), (mp_obj_t)(&lcd_ssd1289_show_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_text), (mp_obj_t)(&lcd_ssd1289_text_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pixel), (mp_obj_t)(&lcd_ssd1289_pixel_obj) },
#if 0
    { MP_OBJ_NEW_QSTR(MP_QSTR_bitmap), (mp_obj_t)(&lcd_ssd1289_bitmap_obj) },
#endif
    { MP_OBJ_NEW_QSTR(MP_QSTR_COLOR_BLACK), MP_OBJ_NEW_SMALL_INT(LCD_SSD1289_COLOR_BLACK) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_COLOR_WHITE), MP_OBJ_NEW_SMALL_INT(LCD_SSD1289_COLOR_WHITE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_VERTICAL), MP_OBJ_NEW_SMALL_INT(0) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_HORIZONTAL), MP_OBJ_NEW_SMALL_INT(1) },
};

STATIC MP_DEFINE_CONST_DICT(lcd_ssd1289_locals_dict, lcd_ssd1289_locals_dict_table);

const mp_obj_type_t lcd_ssd1289_type = {
    { &mp_type_type },
    .name = MP_QSTR_SSD1289,
    .print = lcd_ssd1289_print,
    .make_new = lcd_ssd1289_make_new,
    .locals_dict = (mp_obj_t)&lcd_ssd1289_locals_dict
};

#endif // MICROPY_PY_DISPLAY_LCD_SSD1289
