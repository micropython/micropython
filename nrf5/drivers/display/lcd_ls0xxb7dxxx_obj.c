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

#include "lcd_ls0xxb7dxxx_driver.h"

#if MICROPY_PY_DISPLAY_LCD_LS0XXB7DXXX

/// \moduleref display
/// \class LS0XXB7DXXX - LS0XXB7DXXX TFT LCD display driver.

#include "moddisplay.h"
#include "framebuffer.h"
#include "pin.h"
#include "spi.h"

typedef struct _lcd_ls0xxb7dxxx_obj_t {
    mp_obj_base_t base;
    display_draw_callbacks_t draw_callbacks;
    framebuffer_t * framebuffer;
    machine_hard_spi_obj_t *spi;
    pin_obj_t * pin_cs;
    pin_obj_t * pin_disp;
    pin_obj_t * pin_extcomin;
    pin_obj_t * pin_extmode;
    pin_obj_t * pin_power_control;
    pin_obj_t * pin_power_charge;
} lcd_ls0xxb7dxxx_obj_t;

#define LCD_LS0XXB7DXXX_COLOR_BLACK 0
#define LCD_LS0XXB7DXXX_COLOR_WHITE 1

static void set_pixel(void     * p_display,
                      uint16_t   x,
                      uint16_t   y,
                      uint16_t   color) {
    lcd_ls0xxb7dxxx_obj_t *self = (lcd_ls0xxb7dxxx_obj_t *)p_display;

    if (color == LCD_LS0XXB7DXXX_COLOR_BLACK) {
        framebuffer_pixel_set(self->framebuffer, x, y);
    } else {
        framebuffer_pixel_clear(self->framebuffer, x, y);
    }
}

/// \method __str__()
/// Return a string describing the LS0XXB7DXXX object.
STATIC void lcd_ls0xxb7dxxx_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    lcd_ls0xxb7dxxx_obj_t *self = o;

    mp_printf(print, "LS0XXB7DXXX(SPI(mosi=(port=%u, pin=%u), clk=(port=%u, pin=%u)),\n",
                     self->spi->pyb->spi->init.mosi_pin->port,
                     self->spi->pyb->spi->init.mosi_pin->pin,
                     self->spi->pyb->spi->init.clk_pin->port,
                     self->spi->pyb->spi->init.clk_pin->pin);

    mp_printf(print, "        cs=(port=%u, pin=%u), disp=(port=%u, pin=%u), extcomin=(port=%u, pin=%u),\n",
                     self->pin_cs->port,
                     self->pin_cs->pin,
                     self->pin_disp->port,
                     self->pin_disp->pin,
                     self->pin_extcomin->port,
                     self->pin_extcomin->pin);
    mp_printf(print, "        extmode=(port=%u, pin=%u), power_control=(port=%u, pin=%u), power_charge=(port=%u, pin=%u),\n",
                     self->pin_extmode->port,
                     self->pin_extmode->pin,
                     self->pin_power_control->port,
                     self->pin_power_control->pin,
                     self->pin_power_charge->port,
                     self->pin_power_charge->pin);

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
    ARG_NEW_SPI,
    ARG_NEW_CS,
    ARG_NEW_DISP,
    ARG_NEW_EXTCOMIN,
    ARG_NEW_EXTMODE,
    ARG_NEW_POWER_CONTROL,
    ARG_NEW_POWER_CHARGE
};

/*

Example for nrf52840 / pca10056:

from machine import Pin, SPI
from display import LS0XXB7DXXX
import draw
cs = Pin("B3", mode=Pin.OUT, pull=Pin.PULL_UP)
disp = Pin("B4", mode=Pin.OUT, pull=Pin.PULL_UP)
extcomin = Pin("A28", mode=Pin.OUT, pull=Pin.PULL_UP)
extmode = Pin("B5", mode=Pin.OUT, pull=Pin.PULL_UP)
power_control = Pin("A29", mode=Pin.OUT, pull=Pin.PULL_UP)
power_charge = Pin("A30", mode=Pin.OUT, pull=Pin.PULL_UP)
spi = SPI(0, baudrate=2000000, firstbit=SPI.LSB)
d = LS0XXB7DXXX(400, 240, spi, cs, disp, extcomin, extmode, power_control, power_charge)
draw.text(d, "Hello World!", 32, 32)
d.show()

*/
STATIC mp_obj_t lcd_ls0xxb7dxxx_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    static const mp_arg_t allowed_args[] = {
        { ARG_NEW_WIDTH,         MP_ARG_REQUIRED | MP_ARG_INT },
        { ARG_NEW_HEIGHT,        MP_ARG_REQUIRED | MP_ARG_INT },
        { ARG_NEW_SPI,           MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { ARG_NEW_CS,            MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { ARG_NEW_DISP,          MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { ARG_NEW_EXTCOMIN,      MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { ARG_NEW_EXTMODE,       MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { ARG_NEW_POWER_CONTROL, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { ARG_NEW_POWER_CHARGE,  MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    lcd_ls0xxb7dxxx_obj_t *s = m_new_obj_with_finaliser(lcd_ls0xxb7dxxx_obj_t);
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

    if (args[ARG_NEW_SPI].u_obj != MP_OBJ_NULL) {
        s->spi = args[ARG_NEW_SPI].u_obj;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  "Display SPI not set"));
    }

    if (args[ARG_NEW_CS].u_obj != MP_OBJ_NULL) {
        s->pin_cs = args[ARG_NEW_CS].u_obj;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  "Display CS Pin not set"));
    }

    if (args[ARG_NEW_DISP].u_obj != MP_OBJ_NULL) {
        s->pin_disp = args[ARG_NEW_DISP].u_obj;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  "Display Disp Pin not set"));
    }

    if (args[ARG_NEW_EXTCOMIN].u_obj != MP_OBJ_NULL) {
        s->pin_extcomin = args[ARG_NEW_EXTCOMIN].u_obj;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  "Display ExtComIn Pin not set"));
    }

    if (args[ARG_NEW_EXTMODE].u_obj != MP_OBJ_NULL) {
        s->pin_extmode = args[ARG_NEW_EXTMODE].u_obj;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  "Display ExtMode Pin not set"));
    }

    if (args[ARG_NEW_POWER_CONTROL].u_obj != MP_OBJ_NULL) {
        s->pin_power_control = args[ARG_NEW_POWER_CONTROL].u_obj;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  "Display PowerControl Pin not set"));
    }

    if (args[ARG_NEW_POWER_CHARGE].u_obj != MP_OBJ_NULL) {
        s->pin_power_charge = args[ARG_NEW_POWER_CHARGE].u_obj;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  "Display PowerCharge Pin not set"));
    }

    framebuffer_init_t init_conf = {
        .width = width,
        .height = height,
        .line_orientation = FRAMEBUFFER_LINE_DIR_HORIZONTAL,
        .double_buffer = false
    };

    s->framebuffer = m_new(framebuffer_t, sizeof(framebuffer_t));

    framebuffer_init(s->framebuffer, &init_conf);

    driver_ls0xxb7dxxx_init(s->spi->pyb->spi->instance,
                            s->pin_cs,
                            s->pin_disp,
                            s->pin_extcomin,
                            s->pin_extmode,
                            s->pin_power_control,
                            s->pin_power_charge);

    // Default to black background
    driver_ls0xxb7dxxx_clear(0x00);

    framebuffer_clear(s->framebuffer);

    return MP_OBJ_FROM_PTR(s);
}

// text

/// \method fill(color)
/// Fill framebuffer with the color defined as argument.
STATIC mp_obj_t lcd_ls0xxb7dxxx_fill(mp_obj_t self_in, mp_obj_t color) {
    lcd_ls0xxb7dxxx_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (color == MP_OBJ_NEW_SMALL_INT(LCD_LS0XXB7DXXX_COLOR_BLACK)) {
        framebuffer_fill(self->framebuffer);
    } else {
        framebuffer_clear(self->framebuffer);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(lcd_ls0xxb7dxxx_fill_obj, lcd_ls0xxb7dxxx_fill);

static void render(framebuffer_t * p_framebuffer) {
    for (uint16_t i = 0; i < p_framebuffer->fb_dirty_stride; i++) {
        if (p_framebuffer->fb_dirty[i].byte != 0) {
            for (uint16_t b = 0; b < 8; b++) {
                if ((((p_framebuffer->fb_dirty[i].byte >> b) & 0x01) == 1)) {
                    uint16_t line_num = (i * 8) + b;
                    driver_ls0xxb7dxxx_update_line(line_num,
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
STATIC mp_obj_t lcd_ls0xxb7dxxx_show(size_t n_args, const mp_obj_t *args) {
    lcd_ls0xxb7dxxx_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    render(self->framebuffer);
    framebuffer_flip(self->framebuffer);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lcd_ls0xxb7dxxx_show_obj, 1, 2, lcd_ls0xxb7dxxx_show);

/// \method refresh([num_of_refresh])
/// Refresh content in framebuffer.
///
///   - With no argument, 1 refresh will be done.
///   - With `num_of_refresh` given, The whole framebuffer will be considered
///   dirty and will be refreshed the given number of times.
STATIC mp_obj_t lcd_ls0xxb7dxxx_refresh(mp_obj_t self_in) {
    lcd_ls0xxb7dxxx_obj_t *self = MP_OBJ_TO_PTR(self_in);

    (void)self;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(lcd_ls0xxb7dxxx_refresh_obj, lcd_ls0xxb7dxxx_refresh);

/// \method pixel(x, y, [color])
/// Write one pixel in framebuffer.
///
///   - With no argument, the color of the pixel in framebuffer will be returend.
///   - With `color` given, sets the pixel to the color given.
STATIC mp_obj_t lcd_ls0xxb7dxxx_pixel(size_t n_args, const mp_obj_t *args) {
    lcd_ls0xxb7dxxx_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t color = mp_obj_get_int(args[3]);

    set_pixel(self, x, y, color);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lcd_ls0xxb7dxxx_pixel_obj, 4, 4, lcd_ls0xxb7dxxx_pixel);

/// \method pixel(text, x, y, [color])
/// Write one pixel in framebuffer.
///
///   - With no argument, the color will be the opposite of background (fill color).
///   - With `color` given, sets the pixel to the color given.
STATIC mp_obj_t lcd_ls0xxb7dxxx_text(size_t n_args, const mp_obj_t *args) {
    lcd_ls0xxb7dxxx_obj_t *self = MP_OBJ_TO_PTR(args[0]);
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
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lcd_ls0xxb7dxxx_text_obj, 4, 5, lcd_ls0xxb7dxxx_text);

STATIC mp_obj_t lcd_ls0xxb7dxxx_del(mp_obj_t self_in) {
    lcd_ls0xxb7dxxx_obj_t *self = MP_OBJ_TO_PTR(self_in);

    (void)self;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(lcd_ls0xxb7dxxx_del_obj, lcd_ls0xxb7dxxx_del);

STATIC const mp_map_elem_t lcd_ls0xxb7dxxx_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___del__), (mp_obj_t)(&lcd_ls0xxb7dxxx_del_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_fill), (mp_obj_t)(&lcd_ls0xxb7dxxx_fill_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_show), (mp_obj_t)(&lcd_ls0xxb7dxxx_show_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_text), (mp_obj_t)(&lcd_ls0xxb7dxxx_text_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pixel), (mp_obj_t)(&lcd_ls0xxb7dxxx_pixel_obj) },
#if 0
    { MP_OBJ_NEW_QSTR(MP_QSTR_bitmap), (mp_obj_t)(&lcd_ls0xxb7dxxx_bitmap_obj) },
#endif
    { MP_OBJ_NEW_QSTR(MP_QSTR_COLOR_BLACK), MP_OBJ_NEW_SMALL_INT(LCD_LS0XXB7DXXX_COLOR_BLACK) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_COLOR_WHITE), MP_OBJ_NEW_SMALL_INT(LCD_LS0XXB7DXXX_COLOR_WHITE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_VERTICAL), MP_OBJ_NEW_SMALL_INT(0) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_HORIZONTAL), MP_OBJ_NEW_SMALL_INT(1) },
};

STATIC MP_DEFINE_CONST_DICT(lcd_ls0xxb7dxxx_locals_dict, lcd_ls0xxb7dxxx_locals_dict_table);

const mp_obj_type_t lcd_ls0xxb7dxxx_type = {
    { &mp_type_type },
    .name = MP_QSTR_LS0XXB7DXXX,
    .print = lcd_ls0xxb7dxxx_print,
    .make_new = lcd_ls0xxb7dxxx_make_new,
    .locals_dict = (mp_obj_t)&lcd_ls0xxb7dxxx_locals_dict
};

#endif // MICROPY_PY_DISPLAY_LCD_LS0XXB7DXXX
