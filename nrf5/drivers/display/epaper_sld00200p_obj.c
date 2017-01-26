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

#include "epaper_sld00200p_driver.h"

// For now PWM is only enabled for nrf52 targets.
#if MICROPY_PY_DISPLAY_EPAPER_SLD00200P && NRF52

/// \moduleref epaper
/// \class sld00200p - SLD00200P E-paper shield.

#include "pin.h"
#include "spi.h"
#include "pwm.h"
#include "hal_spi.h"
#include "hal_pwm.h"
#include "moddisplay.h"
#include "framebuffer.h"

typedef struct _epaper_sld00200p_obj_t {
    mp_obj_base_t base;
    display_draw_callbacks_t draw_callbacks;
    framebuffer_t * framebuffer;
    machine_hard_spi_obj_t *spi;
    machine_hard_pwm_obj_t *pwm;
    pin_obj_t * pin_cs;
    pin_obj_t * pin_panel_on;
    pin_obj_t * pin_border;
    pin_obj_t * pin_busy;
    pin_obj_t * pin_reset;
    pin_obj_t * pin_discharge;
#if 0
    pin_obj_t * pin_temp_sensor;
#endif
} epaper_sld00200p_obj_t;

#define EPAPER_SLD00200P_COLOR_BLACK 0
#define EPAPER_SLD00200P_COLOR_WHITE 1

static void set_pixel(void * p_display,
                      uint16_t    x,
                      uint16_t    y,
                      uint16_t    color) {
    epaper_sld00200p_obj_t *self = (epaper_sld00200p_obj_t *)p_display;

    if (color == EPAPER_SLD00200P_COLOR_BLACK) {
        framebuffer_pixel_clear(self->framebuffer, x, y);
    } else {
        framebuffer_pixel_set(self->framebuffer, x, y);
    }
}

/// \method __str__()
/// Return a string describing the SLD00200P object.
STATIC void epaper_sld00200_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    epaper_sld00200p_obj_t *self = o;

    mp_printf(print, "SLD00200(SPI(mosi=(port=%u, pin=%u), miso=(port=%u, pin=%u), clk=(port=%u, pin=%u)),\n",
                     self->spi->pyb->spi->init.mosi_pin->port,
                     self->spi->pyb->spi->init.mosi_pin->pin,
                     self->spi->pyb->spi->init.miso_pin->port,
                     self->spi->pyb->spi->init.miso_pin->pin,
                     self->spi->pyb->spi->init.clk_pin->port,
                     self->spi->pyb->spi->init.clk_pin->pin);

    mp_printf(print, "        PWM(pwm_pin=%u),\n",
                     self->pwm->pyb->pwm->init.pwm_pin);

    mp_printf(print, "        cs=(port=%u, pin=%u), panel_on=(port=%u, pin=%u),\n",
                     self->pin_cs->port,
                     self->pin_cs->pin,
                     self->pin_panel_on->port,
                     self->pin_panel_on->pin);

    mp_printf(print, "        border=(port=%u, pin=%u), busy=(port=%u, pin=%u),\n",
                     self->pin_border->port,
                     self->pin_border->pin,
                     self->pin_busy->port,
                     self->pin_busy->pin);

    mp_printf(print, "        reset=(port=%u, pin=%u), discharge=(port=%u, pin=%u),\n",
                     self->pin_reset->port,
                     self->pin_reset->pin,
                     self->pin_discharge->port,
                     self->pin_discharge->pin);

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
    ARG_NEW_PWM,
    ARG_NEW_CS,
    ARG_NEW_PANEL_ON,
    ARG_NEW_BORDER,
    ARG_NEW_BUSY,
    ARG_NEW_RESET,
    ARG_NEW_DISCHARGE,
    ARG_NEW_TEMP_SENSOR,
};

/*
from machine import Pin, SPI, PWM
from display import SLD00200P
import draw
reset = Pin("A17", mode=Pin.OUT, pull=Pin.PULL_UP)
panel_on = Pin("A13", mode=Pin.OUT, pull=Pin.PULL_UP)
discharge = Pin("A19", mode=Pin.OUT, pull=Pin.PULL_UP)
border = Pin("A14", mode=Pin.OUT, pull=Pin.PULL_UP)
busy = Pin("A18", mode=Pin.IN,  pull=Pin.PULL_DISABLED)
cs = Pin("A22", mode=Pin.OUT, pull=Pin.PULL_UP)
spi = SPI(0, baudrate=8000000)
pwm = PWM(0, Pin("A16", mode=Pin.OUT, pull=Pin.PULL_UP), freq=PWM.FREQ_250KHZ, duty=50, period=2)
d = SLD00200P(264, 176, spi, pwm, cs, panel_on, border, busy, reset, discharge)
draw.text(d, "Hello World!", 32, 32)
d.show()

Example for nrf52840 / pca10056:

from machine import Pin, SPI, PWM
from display import SLD00200P
import draw
reset = Pin("B7", mode=Pin.OUT, pull=Pin.PULL_UP)
panel_on = Pin("B3", mode=Pin.OUT, pull=Pin.PULL_UP)
discharge = Pin("B9", mode=Pin.OUT, pull=Pin.PULL_UP)
border = Pin("B4", mode=Pin.OUT, pull=Pin.PULL_UP)
busy = Pin("B8", mode=Pin.IN,  pull=Pin.PULL_DISABLED)
cs = Pin("B12", mode=Pin.OUT, pull=Pin.PULL_UP)
spi = SPI(0, baudrate=8000000)
pwm = PWM(0, Pin("B6", mode=Pin.OUT, pull=Pin.PULL_UP), freq=PWM.FREQ_250KHZ, duty=50, period=2)
d = SLD00200P(264, 176, spi, pwm, cs, panel_on, border, busy, reset, discharge)
draw.text(d, "Hello World!", 32, 32)
d.show()

*/
STATIC mp_obj_t epaper_sld00200p_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    static const mp_arg_t allowed_args[] = {
        { ARG_NEW_WIDTH,       MP_ARG_REQUIRED | MP_ARG_INT },
        { ARG_NEW_HEIGHT,      MP_ARG_REQUIRED | MP_ARG_INT },
        { ARG_NEW_SPI,         MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { ARG_NEW_PWM,         MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { ARG_NEW_CS,          MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { ARG_NEW_PANEL_ON,    MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { ARG_NEW_BORDER,      MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { ARG_NEW_BUSY,        MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { ARG_NEW_RESET,       MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { ARG_NEW_DISCHARGE,   MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
#if 0
        { ARG_NEW_TEMP_SENSOR, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
#endif
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    epaper_sld00200p_obj_t *s = m_new_obj_with_finaliser(epaper_sld00200p_obj_t);
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

    if (args[ARG_NEW_PWM].u_obj != MP_OBJ_NULL) {
        s->pwm = args[ARG_NEW_PWM].u_obj;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  "Display PWM not set"));
    }

    if (args[ARG_NEW_CS].u_obj != MP_OBJ_NULL) {
        s->pin_cs = args[ARG_NEW_CS].u_obj;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  "Display CS Pin not set"));
    }

    if (args[ARG_NEW_PANEL_ON].u_obj != MP_OBJ_NULL) {
        s->pin_panel_on = args[ARG_NEW_PANEL_ON].u_obj;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  "Display Panel-on Pin not set"));
    }

    if (args[ARG_NEW_BORDER].u_obj != MP_OBJ_NULL) {
        s->pin_border = args[ARG_NEW_BORDER].u_obj;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  "Display Border Pin not set"));
    }

    if (args[ARG_NEW_BUSY].u_obj != MP_OBJ_NULL) {
        s->pin_busy = args[ARG_NEW_BUSY].u_obj;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  "Display Busy Pin not set"));
    }

    if (args[ARG_NEW_RESET].u_obj != MP_OBJ_NULL) {
        s->pin_reset = args[ARG_NEW_RESET].u_obj;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  "Display Reset Pin not set"));
    }

    if (args[ARG_NEW_DISCHARGE].u_obj != MP_OBJ_NULL) {
        s->pin_discharge = args[ARG_NEW_DISCHARGE].u_obj;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  "Display Reset Pin not set"));
    }

#if 0
    if (args[ARG_NEW_TEMP_SENSOR].u_obj != MP_OBJ_NULL) {
        s->pin_temp_sensor = args[ARG_NEW_TEMP_SENSOR].u_obj;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  "Display Busy Pin not set)"));
    }
#endif

    framebuffer_init_t init_conf = {
        .width = width,
        .height = height,
        .line_orientation = FRAMEBUFFER_LINE_DIR_HORIZONTAL,
        .double_buffer = false
    };

    s->framebuffer = m_new(framebuffer_t, sizeof(framebuffer_t));

    framebuffer_init(s->framebuffer, &init_conf);

    driver_sld00200p_init(s->spi->pyb->spi->instance,
                          s->pwm->pyb->pwm->instance,
                          s->pin_cs,
                          s->pin_panel_on,
                          s->pin_border,
                          s->pin_busy,
                          s->pin_reset,
                          s->pin_discharge);

    // Default to white background
    driver_sld00200p_clear(0x00);

    framebuffer_clear(s->framebuffer);

    driver_sld00200p_deinit();

    return MP_OBJ_FROM_PTR(s);
}

// text

/// \method fill(color)
/// Fill framebuffer with the color defined as argument.
STATIC mp_obj_t epaper_sld00200p_fill(mp_obj_t self_in, mp_obj_t color) {
    epaper_sld00200p_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (color == MP_OBJ_NEW_SMALL_INT(EPAPER_SLD00200P_COLOR_BLACK)) {
        framebuffer_clear(self->framebuffer);
    } else {
        framebuffer_fill(self->framebuffer);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(epaper_sld00200p_fill_obj, epaper_sld00200p_fill);

static void render(framebuffer_t * p_framebuffer, bool refresh) {
    for (uint16_t i = 0; i < p_framebuffer->fb_dirty_stride; i++) {
        if (p_framebuffer->fb_dirty[i].byte != 0 || refresh) {
            for (uint16_t b = 0; b < 8; b++) {
                if ((((p_framebuffer->fb_dirty[i].byte >> b) & 0x01) == 1) || refresh) {
                    uint16_t line_num = (i * 8) + b;
                    driver_sld00200p_update_line(line_num,
                        &p_framebuffer->fb_new[line_num * p_framebuffer->fb_stride],
						&p_framebuffer->fb_old[line_num * p_framebuffer->fb_stride],
						p_framebuffer->fb_stride);
                }
            }

            if (refresh == false) {
                p_framebuffer->fb_dirty[i].byte = 0x00;
            }
        }
    }
}

/// \method show([num_of_refresh])
/// Display content in framebuffer.
///
///   - With no argument, no refresh is done.
///   - With `num_of_refresh` given, the lines touched by previous update
///   will be refreshed the given number of times. If no lines have been
///   touched, no update will be performed. To force a refresh, call the
///   refresh() method explicitly.
STATIC mp_obj_t epaper_sld00200p_show(size_t n_args, const mp_obj_t *args) {
    epaper_sld00200p_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    mp_int_t num_of_refresh = 0;

    if (n_args > 1) {
        num_of_refresh = mp_obj_get_int(args[1]);
    }
    driver_sld00200p_reinit();

    render(self->framebuffer, false);
    framebuffer_flip(self->framebuffer);

    if (num_of_refresh > 0) {
        while (num_of_refresh > 0) {
            render(self->framebuffer, true);
            num_of_refresh--;
        }
    }

    driver_sld00200p_deinit();

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(epaper_sld00200p_show_obj, 1, 2, epaper_sld00200p_show);

/// \method refresh([num_of_refresh])
/// Refresh content in framebuffer.
///
///   - With no argument, 1 refresh will be done.
///   - With `num_of_refresh` given, The whole framebuffer will be considered
///   dirty and will be refreshed the given number of times.
STATIC mp_obj_t epaper_sld00200p_refresh(size_t n_args, const mp_obj_t *args) {
    epaper_sld00200p_obj_t *self =  MP_OBJ_TO_PTR(args[0]);

    mp_int_t num_of_refresh = 0;

    if (n_args > 1) {
        num_of_refresh = mp_obj_get_int(args[1]);
    }

    driver_sld00200p_reinit();

    if (num_of_refresh > 0) {
        while (num_of_refresh > 0) {
            render(self->framebuffer, true);
            num_of_refresh--;
        }
    } else {
        // default to one refresh
        render(self->framebuffer, true);
    }

    driver_sld00200p_deinit();

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(epaper_sld00200p_refresh_obj, 1, 2, epaper_sld00200p_refresh);

/// \method pixel(x, y, [color])
/// Write one pixel in framebuffer.
///
///   - With no argument, the color of the pixel in framebuffer will be returend.
///   - With `color` given, sets the pixel to the color given.
STATIC mp_obj_t epaper_sld00200p_pixel(size_t n_args, const mp_obj_t *args) {
    epaper_sld00200p_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t color = mp_obj_get_int(args[3]);

    set_pixel(self, x, y, color);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(epaper_sld00200p_pixel_obj, 3, 4, epaper_sld00200p_pixel);

/// \method pixel(text, x, y, [color])
/// Write one pixel in framebuffer.
///
///   - With no argument, the color will be the opposite of background (fill color).
///   - With `color` given, sets the pixel to the color given.
STATIC mp_obj_t epaper_sld00200p_text(size_t n_args, const mp_obj_t *args) {
    epaper_sld00200p_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    const char *str = mp_obj_str_get_str(args[1]);
    mp_int_t x = mp_obj_get_int(args[2]);
    mp_int_t y = mp_obj_get_int(args[3]);
    mp_int_t color;
    if (n_args >= 4) {
        color = mp_obj_get_int(args[3]);
    }

//    display_print_string(self->framebuffer, x, y, str);

    (void)x;
    (void)y;
    (void)self;
    (void)str;
    (void)color;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(epaper_sld00200p_text_obj, 4, 5, epaper_sld00200p_text);


STATIC mp_obj_t epaper_sld00200p_del(mp_obj_t self_in) {
    epaper_sld00200p_obj_t *self = MP_OBJ_TO_PTR(self_in);

    (void)self;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(epaper_sld00200p_del_obj, epaper_sld00200p_del);

STATIC const mp_rom_map_elem_t epaper_sld00200p_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&epaper_sld00200p_del_obj) },
    { MP_ROM_QSTR(MP_QSTR_fill), MP_ROM_PTR(&epaper_sld00200p_fill_obj) },
    { MP_ROM_QSTR(MP_QSTR_show), MP_ROM_PTR(&epaper_sld00200p_show_obj) },
    { MP_ROM_QSTR(MP_QSTR_refresh), MP_ROM_PTR(&epaper_sld00200p_refresh_obj) },
    { MP_ROM_QSTR(MP_QSTR_text), MP_ROM_PTR(&epaper_sld00200p_text_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel), MP_ROM_PTR(&epaper_sld00200p_pixel_obj) },
#if 0
    { MP_ROM_QSTR(MP_QSTR_bitmap), MP_ROM_PTR(&epaper_sld00200p_bitmap_obj) },
#endif
    { MP_OBJ_NEW_QSTR(MP_QSTR_COLOR_BLACK), MP_OBJ_NEW_SMALL_INT(EPAPER_SLD00200P_COLOR_BLACK) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_COLOR_WHITE), MP_OBJ_NEW_SMALL_INT(EPAPER_SLD00200P_COLOR_WHITE) },

};

STATIC MP_DEFINE_CONST_DICT(epaper_sld00200p_locals_dict, epaper_sld00200p_locals_dict_table);


const mp_obj_type_t epaper_sld00200p_type = {
    { &mp_type_type },
    .name = MP_QSTR_SLD00200P,
    .print = epaper_sld00200_print,
    .make_new = epaper_sld00200p_make_new,
    .locals_dict = (mp_obj_t)&epaper_sld00200p_locals_dict
};

#endif // MICROPY_PY_DISPLAY_EPAPER_SLD00200P
