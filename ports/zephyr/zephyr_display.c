/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 MASSDRIVER EI (massdriver.space)
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

#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <stdlib.h>

#include "modzephyr.h"
#include "py/runtime.h"

#if defined(CONFIG_DISPLAY) && defined(MICROPY_PY_FRAMEBUF)

#if DT_HAS_COMPAT_STATUS_OKAY(zephyr_displays)
#define ENUMERATE_DISPLAY_DEVS(node_id, prop, idx) DEVICE_DT_GET(DT_PROP_BY_IDX(node_id, prop, idx)),

static const struct device *zephyr_display_devices[DT_ZEPHYR_DISPLAYS_COUNT] = {
	DT_FOREACH_PROP_ELEM(DT_COMPAT_GET_ANY_STATUS_OKAY(zephyr_displays), displays, ENUMERATE_DISPLAY_DEVS)
};
#elif DT_HAS_CHOSEN(zephyr_display)
static const struct device *zephyr_display_devices[] = {
	DEVICE_DT_GET(DT_CHOSEN(zephyr_display)),
};
#endif

extern const mp_obj_type_t mp_type_framebuf;

const mp_obj_type_t zephyr_display_type;

typedef struct _zephyr_display_obj_t {
    mp_obj_base_t   base;
    uint8_t         id;
    mp_obj_t        framebuf;
    mp_obj_t        buffer;
    uint8_t         *buffer_data;
    size_t          buffer_size;
} zephyr_display_obj_t;

static void zephyr_display_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    zephyr_display_obj_t *self = self_in;
    mp_printf(print, "Display(%d)", self->id);
}

static mp_obj_t zephyr_display_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    int id = mp_obj_get_int(args[0]);
    const mp_obj_type_t *parent = MP_OBJ_TYPE_GET_SLOT(type, parent);
    struct display_capabilities capabilities;
    int format = -1;

    if (id < 0 || id >= DT_ZEPHYR_DISPLAYS_COUNT) {
        mp_raise_ValueError("Invalid display ID");
        return mp_const_none;
    }
    if (!device_is_ready(zephyr_display_devices[id])){
        mp_raise_msg(&mp_type_RuntimeError, "Display is not ready");
        return mp_const_none;
    }

    display_get_capabilities(zephyr_display_devices[id], &capabilities);

    switch(capabilities.current_pixel_format) {
        case PIXEL_FORMAT_MONO10:
        case PIXEL_FORMAT_MONO01:
            if (capabilities.screen_info & SCREEN_INFO_MONO_VTILED && capabilities.screen_info & SCREEN_INFO_MONO_MSB_FIRST)
                format = -1; // no VMSB
            else if (capabilities.screen_info & SCREEN_INFO_MONO_VTILED)
                format = 0; // FRAMEBUF_MVLSB
            else if (capabilities.screen_info & SCREEN_INFO_MONO_MSB_FIRST)
                format = 4; // FRAMEBUF_MHMSB
            else
                format = 3; // FRAMEBUF_MHLSB
            break;
        case PIXEL_FORMAT_L_8:
            format = 6; // FRAMEBUF_GS8
            break;
        case PIXEL_FORMAT_RGB_565:
            format = 1; // FRAMEBUF_RGB565
            break;
        default:
            format = -1;
    }
    if (format < 0) {
        mp_raise_msg(&mp_type_RuntimeError, "Display is not of a compatible pixel format");
        return mp_const_none;
    }


    zephyr_display_obj_t *self = mp_obj_malloc(zephyr_display_obj_t, type);
    self->id = id;
    self->buffer_size = capabilities.x_resolution * capabilities.y_resolution
        * DISPLAY_BITS_PER_PIXEL(capabilities.current_pixel_format) / 8;
    mp_make_new_fun_t make_new_parent = MP_OBJ_TYPE_GET_SLOT(parent, make_new);
    self->buffer_data = m_malloc(self->buffer_size);
    if (self->buffer_data == NULL) {
        mp_raise_msg(&mp_type_RuntimeError, "Couldn't allocate framebuf");
        return mp_const_none;
    }
    self->buffer = mp_obj_new_bytearray_by_ref(self->buffer_size , self->buffer_data);
    const mp_obj_t framebuf_args[] = {
        self->buffer,
        mp_obj_new_int(capabilities.x_resolution),
        mp_obj_new_int(capabilities.y_resolution),
        mp_obj_new_int(format),
    };
    self->framebuf = make_new_parent(&mp_type_framebuf, 4, 0, framebuf_args);

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t zephyr_display_show(mp_obj_t self_in)
{
    zephyr_display_obj_t *self = MP_OBJ_TO_PTR(self_in);
    struct display_capabilities capabilities;
    int ret;

    display_get_capabilities(zephyr_display_devices[self->id], &capabilities);

    const struct display_buffer_descriptor desc = {
        .buf_size = self->buffer_size,
        .width = capabilities.x_resolution,
        .height = capabilities.y_resolution,
        .pitch = capabilities.x_resolution,
    };

    ret = display_write(zephyr_display_devices[self->id], 0, 0, &desc, self->buffer_data);
    return mp_obj_new_int(ret);
}

static mp_int_t zephyr_display_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    zephyr_display_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_get_buffer(self->buffer, bufinfo, flags) ? 0 : 1;
}

static MP_DEFINE_CONST_FUN_OBJ_1(zephyr_display_show_obj, zephyr_display_show);


static mp_obj_t zephyr_display_rgb(size_t n_args, const mp_obj_t *args_in)
{
    (void)n_args;
    zephyr_display_obj_t *self = MP_OBJ_TO_PTR(args_in[0]);
    struct display_capabilities capabilities;

    int r = mp_obj_get_int(args_in[1]);
    int g = mp_obj_get_int(args_in[2]);
    int b = mp_obj_get_int(args_in[3]);

    display_get_capabilities(zephyr_display_devices[self->id], &capabilities);

    switch(capabilities.current_pixel_format) {
        case PIXEL_FORMAT_MONO10:
            if(r > 127 || g > 127 || b > 127)
                return mp_obj_new_int(0x0);
            else
                return mp_obj_new_int(0xFF);
        case PIXEL_FORMAT_MONO01:
            if(r > 127 || g > 127 || b > 127)
                return mp_obj_new_int(0xFF);
            else
                return mp_obj_new_int(0x0);
        case PIXEL_FORMAT_L_8:
            return mp_obj_new_int((r + g + b) / 3);
        case PIXEL_FORMAT_RGB_565:
            return mp_obj_new_int((r & 0xe0) | ((g >> 3) & 0x1c) | (b >> 6));
        default:
            return mp_obj_new_int(0);
    }
	return mp_obj_new_int(0);
}

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(zephyr_display_rgb_obj, 4, 4, zephyr_display_rgb);

/* framebuf callers */

static mp_obj_t zephyr_display_fill(mp_obj_t self_in, mp_obj_t col_in)
{
    zephyr_display_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_obj_t args[3];
    mp_load_method(self->framebuf, MP_QSTR_fill, args);
    args[2] = col_in;
	return mp_call_method_n_kw(1, 0, args);
}

static MP_DEFINE_CONST_FUN_OBJ_2(zephyr_display_fill_obj, zephyr_display_fill);

static mp_obj_t zephyr_display_fill_rect(size_t n_args, const mp_obj_t *args_in)
{
    zephyr_display_obj_t *self = MP_OBJ_TO_PTR(args_in[0]);

    mp_obj_t args[7];
    mp_load_method(self->framebuf, MP_QSTR_fill_rect, args);
    args[2] = args_in[1];
    args[3] = args_in[2];
    args[4] = args_in[3];
    args[5] = args_in[4];
    args[6] = args_in[5];
	return mp_call_method_n_kw(5, 0, args);
}

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(zephyr_display_fill_rect_obj, 6, 6, zephyr_display_fill_rect);

static mp_obj_t zephyr_display_pixel(size_t n_args, const mp_obj_t *args_in)
{
    zephyr_display_obj_t *self = MP_OBJ_TO_PTR(args_in[0]);

    mp_obj_t args[5];
    mp_load_method(self->framebuf, MP_QSTR_pixel, args);
    args[2] = args_in[1];
    args[3] = args_in[2];
    if (n_args > 3) {
        args[4] = args_in[3];
        return mp_call_method_n_kw(3, 0, args);
    }
    return mp_call_method_n_kw(2, 0, args);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(zephyr_display_pixel_obj, 3, 4, zephyr_display_pixel);

static mp_obj_t zephyr_display_hline(size_t n_args, const mp_obj_t *args_in)
{
    (void)n_args;
    zephyr_display_obj_t *self = MP_OBJ_TO_PTR(args_in[0]);

    mp_obj_t args[6];
    mp_load_method(self->framebuf, MP_QSTR_hline, args);
    args[2] = args_in[1];
    args[3] = args_in[2];
    args[4] = args_in[3];
    args[5] = args_in[4];
	return mp_call_method_n_kw(4, 0, args);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(zephyr_display_hline_obj, 5, 5, zephyr_display_hline);

static mp_obj_t zephyr_display_vline(size_t n_args, const mp_obj_t *args_in)
{
    (void)n_args;
    zephyr_display_obj_t *self = MP_OBJ_TO_PTR(args_in[0]);

    mp_obj_t args[6];
    mp_load_method(self->framebuf, MP_QSTR_vline, args);
    args[2] = args_in[1];
    args[3] = args_in[2];
    args[4] = args_in[3];
    args[5] = args_in[4];
	return mp_call_method_n_kw(4, 0, args);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(zephyr_display_vline_obj, 5, 5, zephyr_display_vline);

static mp_obj_t zephyr_display_rect(size_t n_args, const mp_obj_t *args_in)
{
    zephyr_display_obj_t *self = MP_OBJ_TO_PTR(args_in[0]);

    mp_obj_t args[8];
    mp_load_method(self->framebuf, MP_QSTR_rect, args);
    args[2] = args_in[1];
    args[3] = args_in[2];
    args[4] = args_in[3];
    args[5] = args_in[4];
    args[6] = args_in[5];
    if (n_args > 6) {
        args[7] = args_in[6];
        return mp_call_method_n_kw(6, 0, args);
    }
    return mp_call_method_n_kw(5, 0, args);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(zephyr_display_rect_obj, 6, 7, zephyr_display_rect);

static mp_obj_t zephyr_display_line(size_t n_args, const mp_obj_t *args_in)
{
    (void)n_args;
    zephyr_display_obj_t *self = MP_OBJ_TO_PTR(args_in[0]);

    mp_obj_t args[7];
    mp_load_method(self->framebuf, MP_QSTR_line, args);
    args[2] = args_in[1];
    args[3] = args_in[2];
    args[4] = args_in[3];
    args[5] = args_in[4];
    args[6] = args_in[5];
	return mp_call_method_n_kw(5, 0, args);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(zephyr_display_line_obj, 6, 6, zephyr_display_line);

static mp_obj_t zephyr_display_ellipse(size_t n_args, const mp_obj_t *args_in)
{
    zephyr_display_obj_t *self = MP_OBJ_TO_PTR(args_in[0]);

    mp_obj_t args[9];
    mp_load_method(self->framebuf, MP_QSTR_ellipse, args);
    args[2] = args_in[1];
    args[3] = args_in[2];
    args[4] = args_in[3];
    args[5] = args_in[4];
    args[6] = args_in[5];
    if (n_args > 7) {
        args[7] = args_in[6];
        args[8] = args_in[7];
        return mp_call_method_n_kw(7, 0, args);
    } else if (n_args > 6) {
        args[7] = args_in[6];
        return mp_call_method_n_kw(6, 0, args);
    }
    return mp_call_method_n_kw(5, 0, args);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(zephyr_display_ellipse_obj, 6, 8, zephyr_display_ellipse);

static mp_obj_t zephyr_display_poly(size_t n_args, const mp_obj_t *args_in)
{
    zephyr_display_obj_t *self = MP_OBJ_TO_PTR(args_in[0]);

    mp_obj_t args[7];
    mp_load_method(self->framebuf, MP_QSTR_poly, args);
    args[2] = args_in[1];
    args[3] = args_in[2];
    args[4] = args_in[3];
    args[5] = args_in[4];
    if (n_args > 5) {
        args[6] = args_in[5];
        return mp_call_method_n_kw(5, 0, args);
    }
    return mp_call_method_n_kw(4, 0, args);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(zephyr_display_poly_obj, 5, 6, zephyr_display_poly);

static mp_obj_t zephyr_display_blit(size_t n_args, const mp_obj_t *args_in)
{
    zephyr_display_obj_t *self = MP_OBJ_TO_PTR(args_in[0]);

    mp_obj_t args[7];
    mp_load_method(self->framebuf, MP_QSTR_blit, args);
    args[2] = args_in[1];
    args[3] = args_in[2];
    args[4] = args_in[3];
    if (n_args > 5) {
        args[5] = args_in[4];
        args[6] = args_in[5];
        return mp_call_method_n_kw(5, 0, args);
    } else if (n_args > 4) {
        args[5] = args_in[4];
        return mp_call_method_n_kw(4, 0, args);
    }
    return mp_call_method_n_kw(3, 0, args);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(zephyr_display_blit_obj, 4, 6, zephyr_display_blit);

static mp_obj_t zephyr_display_scroll(mp_obj_t self_in, mp_obj_t xstep_in, mp_obj_t ystep_in)
{
    zephyr_display_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_obj_t args[4];
    mp_load_method(self->framebuf, MP_QSTR_line, args);
    args[2] = xstep_in;
    args[3] = ystep_in;
	return mp_call_method_n_kw(2, 0, args);
}
static MP_DEFINE_CONST_FUN_OBJ_3(zephyr_display_scroll_obj, zephyr_display_scroll);

static mp_obj_t zephyr_display_text(size_t n_args, const mp_obj_t *args_in)
{
    zephyr_display_obj_t *self = MP_OBJ_TO_PTR(args_in[0]);

    mp_obj_t args[6];
    mp_load_method(self->framebuf, MP_QSTR_text, args);
    args[2] = args_in[1];
    args[3] = args_in[2];
    args[4] = args_in[3];
    if (n_args > 4) {
        args[5] = args_in[4];
        return mp_call_method_n_kw(4, 0, args);
    }
    return mp_call_method_n_kw(3, 0, args);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(zephyr_display_text_obj, 4, 5, zephyr_display_text);

static const mp_rom_map_elem_t zephyr_display_locals_dict_table[] = {
    /* framebuf class */
    { MP_ROM_QSTR(MP_QSTR_fill), MP_ROM_PTR(&zephyr_display_fill_obj) },
    { MP_ROM_QSTR(MP_QSTR_fill_rect), MP_ROM_PTR(&zephyr_display_fill_rect_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel), MP_ROM_PTR(&zephyr_display_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_hline), MP_ROM_PTR(&zephyr_display_hline_obj) },
    { MP_ROM_QSTR(MP_QSTR_vline), MP_ROM_PTR(&zephyr_display_vline_obj) },
    { MP_ROM_QSTR(MP_QSTR_rect), MP_ROM_PTR(&zephyr_display_rect_obj) },
    { MP_ROM_QSTR(MP_QSTR_line), MP_ROM_PTR(&zephyr_display_line_obj) },
    { MP_ROM_QSTR(MP_QSTR_ellipse), MP_ROM_PTR(&zephyr_display_ellipse_obj) },
    #if MICROPY_PY_ARRAY
    { MP_ROM_QSTR(MP_QSTR_poly), MP_ROM_PTR(&zephyr_display_poly_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_blit), MP_ROM_PTR(&zephyr_display_blit_obj) },
    { MP_ROM_QSTR(MP_QSTR_scroll), MP_ROM_PTR(&zephyr_display_scroll_obj) },
    { MP_ROM_QSTR(MP_QSTR_text), MP_ROM_PTR(&zephyr_display_text_obj) },
    /* this class */
    { MP_ROM_QSTR(MP_QSTR_show), MP_ROM_PTR(&zephyr_display_show_obj) },
    { MP_ROM_QSTR(MP_QSTR_rgb), MP_ROM_PTR(&zephyr_display_rgb_obj) },
    { MP_ROM_QSTR(MP_QSTR_display_count), MP_ROM_INT(DT_ZEPHYR_DISPLAYS_COUNT) },
};
static MP_DEFINE_CONST_DICT(zephyr_display_locals_dict, zephyr_display_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    zephyr_display_type,
    MP_QSTR_Display,
    MP_TYPE_FLAG_NONE,
    make_new, zephyr_display_make_new,
    print, zephyr_display_print,
    locals_dict, &zephyr_display_locals_dict,
    buffer, zephyr_display_get_buffer,
    parent, &mp_type_framebuf
    );
#endif //  defined(CONFIG_DISPLAY) && defined(MICROPY_PY_FRAMEBUF)
