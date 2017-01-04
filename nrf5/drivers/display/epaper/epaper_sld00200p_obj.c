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

// For now PWM is only enabled for nrf52 targets.
#if MICROPY_PY_DISPLAY_EPAPER_SLD00200P && NRF52

/// \moduleref epaper
/// \class sld00200p - SLD00200P E-paper shield.

#include "hal_spi.h"
#include "hal_pwm.h"
#include "lcd_mono_fb.h"

typedef struct _epaper_sld00200p_obj_t {
    mp_obj_base_t base;
    SPI_HandleTypeDef *spi;
    PWM_HandleTypeDef *pwm;
} epaper_sld00200p_obj_t;

/// \method __str__()
/// Return a string describing the SLD00200P object.
STATIC void epaper_sld00200_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
}

// for make_new
enum {
    ARG_NEW_SPI,
    ARG_NEW_CS,
    ARG_NEW_PWM,
    ARG_NEW_PANEL_ON,
    ARG_NEW_BORDER,
    ARG_NEW_RESET,
    ARG_NEW_BUSY,
    ARG_NEW_DISCHARGE,
    ARG_NEW_TEMP_SENSOR,
};

STATIC mp_obj_t epaper_sld00200p_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,       MP_ARG_OBJ, {.u_obj = MP_OBJ_NEW_SMALL_INT(-1)} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    epaper_sld00200p_obj_t *s = m_new_obj_with_finaliser(epaper_sld00200p_obj_t);
    return MP_OBJ_FROM_PTR(s);
}

// text

/// \method fill(color)
/// Fill framebuffer with the color defined as argument.
STATIC mp_obj_t epaper_sld00200p_fill(mp_obj_t self_in, mp_obj_t color) {
    epaper_sld00200p_obj_t *self = MP_OBJ_TO_PTR(self_in);
    (void)self;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(epaper_sld00200p_fill_obj, epaper_sld00200p_fill);

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

    (void)num_of_refresh;
    (void)self;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(epaper_sld00200p_show_obj, 1, 2, epaper_sld00200p_show);

/// \method refresh([num_of_refresh])
/// Refresh content in framebuffer.
///
///   - With no argument, 1 refresh will be done.
///   - With `num_of_refresh` given, The whole framebuffer will be considered
///   dirty and will be refreshed the given number of times.
STATIC mp_obj_t epaper_sld00200p_refresh(mp_obj_t self_in) {
    epaper_sld00200p_obj_t *self = MP_OBJ_TO_PTR(self_in);

    (void)self;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(epaper_sld00200p_refresh_obj, epaper_sld00200p_refresh);

/// \method pixel(x, y, [color])
/// Write one pixel in framebuffer.
///
///   - With no argument, the color of the pixel in framebuffer will be returend.
///   - With `color` given, sets the pixel to the color given.
STATIC mp_obj_t epaper_sld00200p_pixel(size_t n_args, const mp_obj_t *args) {
    epaper_sld00200p_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t color;
    if (n_args >= 3) {
        color = mp_obj_get_int(args[3]);
    }
    (void)self;
    (void)x;
    (void)y;
    (void)color;

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
    (void)self;
    (void)str;
    (void)x;
    (void)y;
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
    { MP_OBJ_NEW_QSTR(MP_QSTR_COLOR_BLACK), MP_OBJ_NEW_SMALL_INT(LCD_BLACK) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_COLOR_WHITE), MP_OBJ_NEW_SMALL_INT(LCD_WHITE) },

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
