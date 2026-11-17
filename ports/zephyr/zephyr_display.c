/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025-2026 MASSDRIVER EI (massdriver.space)
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
#include <zephyr/dt-bindings/display/panel.h>
#include <stdlib.h>

#include "modzephyr.h"
#include "zephyr_device.h"
#include "py/runtime.h"

#if defined(CONFIG_DISPLAY)

/* Handle Zephyr 4.4 PIXEL_FORMAT_BGR_565 -> PIXEL_FORMAT_RGB_565X
 * Pixel Formats are an enum, PANEL_* are defines of the same values.
 */
#ifndef PANEL_PIXEL_FORMAT_RGB_565X
#define PIXEL_FORMAT_RGB_565X   PANEL_PIXEL_FORMAT_BGR_565
#endif

#if DT_HAS_COMPAT_STATUS_OKAY(zephyr_displays)
#define ENUMERATE_DISPLAY_DEVS(node_id, prop, idx) DEVICE_DT_GET(DT_PROP_BY_IDX(node_id, prop, idx)),

static const struct device *zephyr_display_devices[DT_ZEPHYR_DISPLAYS_COUNT] = {
    DT_FOREACH_PROP_ELEM(DT_COMPAT_GET_ANY_STATUS_OKAY(zephyr_displays), displays, ENUMERATE_DISPLAY_DEVS)
};
#elif DT_HAS_CHOSEN(zephyr_display)
static const struct device *zephyr_display_devices[] = {
    DEVICE_DT_GET(DT_CHOSEN(zephyr_display)),
};
#else
static const struct device *zephyr_display_devices[1];
#endif

const mp_obj_type_t zephyr_display_type;

typedef struct _zephyr_display_obj_t {
    mp_obj_base_t base;
    const struct device *dev;
} zephyr_display_obj_t;

static void zephyr_display_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    zephyr_display_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "Display(\"%s\")", self->dev->name);
}

static mp_obj_t zephyr_display_make_new(const mp_obj_type_t *type,
    size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    const struct device *dev;

    if (mp_obj_is_str(args[0])) {
        dev = zephyr_device_find(args[0]);
    } else {
        int id = mp_obj_get_int(args[0]);

        if (id < 0 || id >= DT_ZEPHYR_DISPLAYS_COUNT) {
            mp_raise_ValueError(MP_ERROR_TEXT("Invalid display ID"));
        }
        dev = zephyr_display_devices[id];
    }
    if (!device_is_ready(dev)) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Display is not ready"));
    }

    zephyr_display_obj_t *self = mp_obj_malloc(zephyr_display_obj_t, type);
    self->dev = dev;
    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t zephyr_display_write(size_t n_args, const mp_obj_t *args_in) {
    zephyr_display_obj_t *self = MP_OBJ_TO_PTR(args_in[0]);
    struct display_capabilities capabilities;
    int ret;
    size_t len;
    mp_buffer_info_t bufinfo;
    int x = 0;
    int y = 0;
    int sx;
    int sy;

    mp_get_buffer_raise(args_in[1], &bufinfo, MP_BUFFER_READ);

    display_get_capabilities(self->dev, &capabilities);
    sx = capabilities.x_resolution;

    if (n_args > 2) {
        x = mp_obj_get_int(args_in[2]);
    }

    if (n_args > 3) {
        y = mp_obj_get_int(args_in[3]);
    }

    if (n_args > 4) {
        sx = mp_obj_get_int(args_in[4]);
    }

    if (n_args > 5) {
        sy = mp_obj_get_int(args_in[5]);
    } else {
        sy = bufinfo.len / (sx * DISPLAY_BITS_PER_PIXEL(capabilities.current_pixel_format) / 8);
    }

    len = sx * sy * DISPLAY_BITS_PER_PIXEL(capabilities.current_pixel_format) / 8;

    if (len > bufinfo.len) {
        mp_raise_ValueError(MP_ERROR_TEXT("Buffer is shorter than size requires"));
    }

    const struct display_buffer_descriptor desc = {
        .buf_size = len,
        .width = sx,
        .height = sy,
        .pitch = sx,
    };

    ret = display_write(self->dev, x, y, &desc, bufinfo.buf);
    if (ret < 0) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Couldn't write to display"));
    }
    return mp_obj_new_int(ret);
}

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(zephyr_display_write_obj, 2, 6, zephyr_display_write);


static mp_obj_t zephyr_display_rgb(size_t n_args, const mp_obj_t *args_in) {
    (void)n_args;
    zephyr_display_obj_t *self = MP_OBJ_TO_PTR(args_in[0]);
    struct display_capabilities capabilities;

    int r = CLAMP(mp_obj_get_int(args_in[1]), 0, 0xff);
    int g = CLAMP(mp_obj_get_int(args_in[2]), 0, 0xff);
    int b = CLAMP(mp_obj_get_int(args_in[3]), 0, 0xff);

    display_get_capabilities(self->dev, &capabilities);

    switch (capabilities.current_pixel_format) {
        case PIXEL_FORMAT_MONO10:
            if (r > 127 || g > 127 || b > 127) {
                return mp_obj_new_int(0x0);
            } else {
                return mp_obj_new_int(0xFF);
            }
        case PIXEL_FORMAT_MONO01:
            if (r > 127 || g > 127 || b > 127) {
                return mp_obj_new_int(0xFF);
            } else {
                return mp_obj_new_int(0x0);
            }
        case PIXEL_FORMAT_RGB_565:
            return mp_obj_new_int(MP_BSWAP16(((r & 0xf8) << 8) | ((g & 0xfc) << 3) | (b >> 3)));
        case PIXEL_FORMAT_RGB_565X:
            return mp_obj_new_int(((r & 0xf8) << 8) | ((g & 0xfc) << 3) | (b >> 3));
        case PIXEL_FORMAT_L_8:
            return mp_obj_new_int((r + g + b) / 3);
        #ifdef PANEL_PIXEL_FORMAT_L_4
        case PIXEL_FORMAT_L_4:
            return mp_obj_new_int(((r + g + b) / 3) >> 4);
        #endif
        default:
            break;
    }
    mp_raise_ValueError(MP_ERROR_TEXT("Not a framebuf pixel format"));
}

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(zephyr_display_rgb_obj, 4, 4, zephyr_display_rgb);

static int zephyr_display_framebuf_current_format_helper(zephyr_display_obj_t *self) {
    struct display_capabilities capabilities;
    int format = -1;

    display_get_capabilities(self->dev, &capabilities);

    switch (capabilities.current_pixel_format) {
        case PIXEL_FORMAT_MONO10:
        case PIXEL_FORMAT_MONO01:
            if (capabilities.screen_info & SCREEN_INFO_MONO_VTILED
                && capabilities.screen_info & SCREEN_INFO_MONO_MSB_FIRST) {
                format = -1; // no VMSB
            } else if (capabilities.screen_info & SCREEN_INFO_MONO_VTILED) {
                format = 0; // FRAMEBUF_MVLSB
            } else if (capabilities.screen_info & SCREEN_INFO_MONO_MSB_FIRST) {
                format = 4; // FRAMEBUF_MHMSB
            } else {
                format = 3; // FRAMEBUF_MHLSB
            }
            break;
        case PIXEL_FORMAT_RGB_565X:
        case PIXEL_FORMAT_RGB_565:
            format = 1; // FRAMEBUF_RGB565
            break;
        case PIXEL_FORMAT_L_8:
            format = 6; // FRAMEBUF_GS8
            break;
        #ifdef PANEL_PIXEL_FORMAT_L_4
        case PIXEL_FORMAT_L_4:
            format = 2; // FRAMEBUF_GS4
            break;
        #endif
        default:
            format = -1;
    }
    if (format < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("Not a framebuf pixel format"));
    }

    return format;
}

static mp_obj_t zephyr_display_capabilities(const mp_obj_t self_in) {
    zephyr_display_obj_t *self = MP_OBJ_TO_PTR(self_in);
    struct display_capabilities capabilities;
    mp_obj_t tuple_buf[32];
    mp_obj_t caps[7];
    size_t tuple_cnt = 0;

    display_get_capabilities(self->dev, &capabilities);

    for (int i = 0; i < 32; i++) {
        if (capabilities.supported_pixel_formats & 1U << i) {
            tuple_buf[tuple_cnt] = mp_obj_new_int(1U << i);
            tuple_cnt++;
        }
    }

    caps[0] = mp_obj_new_int(capabilities.x_resolution);
    caps[1] = mp_obj_new_int(capabilities.y_resolution);
    caps[2] = mp_obj_new_tuple(tuple_cnt, tuple_buf);
    caps[3] = mp_obj_new_int(capabilities.current_pixel_format);
    caps[4] = mp_obj_new_int(capabilities.current_orientation);

    tuple_cnt = 0;
    if ((capabilities.screen_info & SCREEN_INFO_MONO_VTILED) != 0) {
        tuple_buf[tuple_cnt] = MP_ROM_QSTR(MP_QSTR_VTILED);
        tuple_cnt++;
    }
    if ((capabilities.screen_info & SCREEN_INFO_MONO_MSB_FIRST) != 0) {
        tuple_buf[tuple_cnt] = MP_ROM_QSTR(MP_QSTR_MSB_FIRST);
        tuple_cnt++;
    }
    if ((capabilities.screen_info & SCREEN_INFO_EPD) != 0) {
        tuple_buf[tuple_cnt] = MP_ROM_QSTR(MP_QSTR_EPD);
        tuple_cnt++;
    }
    if ((capabilities.screen_info & SCREEN_INFO_DOUBLE_BUFFER) != 0) {
        tuple_buf[tuple_cnt] = MP_ROM_QSTR(MP_QSTR_DOUBLE_BUFFER);
        tuple_cnt++;
    }
    if ((capabilities.screen_info & SCREEN_INFO_X_ALIGNMENT_WIDTH) != 0) {
        tuple_buf[tuple_cnt] = MP_ROM_QSTR(MP_QSTR_X_ALIGNMENT_WIDTH);
        tuple_cnt++;
    }
    caps[5] = mp_obj_new_tuple(tuple_cnt, tuple_buf);
    caps[6] = mp_obj_new_int(zephyr_display_framebuf_current_format_helper(self));

    return mp_obj_new_tuple(7, caps);
}

static MP_DEFINE_CONST_FUN_OBJ_1(zephyr_display_capabilities_obj, zephyr_display_capabilities);

static mp_obj_t zephyr_display_format(size_t n_args, const mp_obj_t *args_in) {
    zephyr_display_obj_t *self = MP_OBJ_TO_PTR(args_in[0]);
    struct display_capabilities capabilities;
    int ret;

    if (n_args > 1) {
        ret = display_set_pixel_format(self->dev, mp_obj_get_int(args_in[1]));
        if (ret < 0) {
            mp_raise_ValueError(MP_ERROR_TEXT("Invalid pixel format"));
        }
    }

    display_get_capabilities(self->dev, &capabilities);

    return mp_obj_new_int(capabilities.current_pixel_format);
}

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(zephyr_display_format_obj, 1, 2, zephyr_display_format);

static mp_obj_t zephyr_display_blanking(const mp_obj_t self_in, const mp_obj_t onoff) {
    zephyr_display_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int ret;

    if (mp_obj_is_true(onoff)) {
        ret = display_blanking_on(self->dev);
    } else {
        ret = display_blanking_off(self->dev);
    }

    if (ret < 0) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Couldn't set blanking"));
    }

    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_2(zephyr_display_blanking_obj, zephyr_display_blanking);

static mp_obj_t zephyr_display_clear(const mp_obj_t self_in) {
    zephyr_display_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int ret;

    ret = display_clear(self->dev);
    if (ret == -ENOSYS) {
        mp_raise_msg(&mp_type_RuntimeError,
            MP_ERROR_TEXT("Clearing is not supported by the display"));
    } else if (ret < 0) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Couldn't clear display"));
    }

    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_1(zephyr_display_clear_obj, zephyr_display_clear);

static mp_obj_t zephyr_display_set_brightness(const mp_obj_t self_in, mp_obj_t arg) {
    zephyr_display_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int ret;

    ret = display_set_brightness(self->dev, CLAMP(mp_obj_get_int(arg), 0, 0xff));
    if (ret == -ENOSYS) {
        mp_raise_msg(&mp_type_RuntimeError,
            MP_ERROR_TEXT("Setting brightness is not supported by the display"));
    } else if (ret < 0) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Couldn't set brightness"));
    }

    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_2(zephyr_display_set_brightness_obj, zephyr_display_set_brightness);

static mp_obj_t zephyr_display_set_contrast(const mp_obj_t self_in, mp_obj_t arg) {
    zephyr_display_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int ret;

    ret = display_set_contrast(self->dev, CLAMP(mp_obj_get_int(arg), 0, 0xff));
    if (ret == -ENOSYS) {
        mp_raise_msg(&mp_type_RuntimeError,
            MP_ERROR_TEXT("Setting contrast is not supported by the display"));
    } else if (ret < 0) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Couldn't set contrast"));
    }

    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_2(zephyr_display_set_contrast_obj, zephyr_display_set_contrast);

static mp_obj_t zephyr_display_orientation(size_t n_args, const mp_obj_t *args_in) {
    zephyr_display_obj_t *self = MP_OBJ_TO_PTR(args_in[0]);
    struct display_capabilities capabilities;
    int ret;

    if (n_args > 1) {
        int arg = mp_obj_get_int(args_in[1]);

        if (arg > DISPLAY_ORIENTATION_ROTATED_270 || arg < 0) {
            mp_raise_ValueError(MP_ERROR_TEXT("Invalid orientation"));
        }

        ret = display_set_orientation(self->dev, arg);
        if (ret == -ENOSYS) {
            mp_raise_msg(&mp_type_RuntimeError,
                MP_ERROR_TEXT("Setting orientation is not supported by the display"));
        } else if (ret < 0) {
            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Couldn't set orientation"));
        }
    }

    display_get_capabilities(self->dev, &capabilities);

    return mp_obj_new_int(capabilities.current_orientation);
}

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(zephyr_display_orientation_obj, 1, 2, zephyr_display_orientation);

#if MICROPY_PY_FRAMEBUF

extern const mp_obj_module_t mp_module_framebuf;

static mp_obj_t zephyr_display_as_framebuf_show(const mp_obj_t self_in) {
    mp_obj_t display = mp_load_attr(self_in, MP_QSTR_display);
    mp_obj_t write = mp_load_attr(display, MP_QSTR_write);

    mp_call_function_1(write, self_in);

    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_1(zephyr_display_as_framebuf_show_obj, zephyr_display_as_framebuf_show);

/* Return a anonymous class type instance allowing to use framebuf with a show function
 * keyed to this display.
 */
static mp_obj_t zephyr_display_as_framebuf(const mp_obj_t self_in) {
    zephyr_display_obj_t *self = MP_OBJ_TO_PTR(self_in);
    struct display_capabilities capabilities;
    mp_obj_t base[1];
    mp_obj_t base_tuple;
    mp_obj_t args[4];
    mp_obj_t locals;
    mp_obj_t fb_type;
    uint8_t *buf;
    size_t buf_size;
    mp_obj_t instance;
    mp_obj_t bound_show;

    base[0] = mp_load_attr(MP_OBJ_FROM_PTR(&mp_module_framebuf), MP_QSTR_FrameBuffer);

    base_tuple = mp_obj_new_tuple(1, base);

    /* Store Display in type*/
    locals = mp_obj_new_dict(0);
    mp_obj_dict_store(locals, MP_OBJ_NEW_QSTR(MP_QSTR_display), self_in);

    args[0] = MP_OBJ_NEW_QSTR(MP_QSTR_framebuf_display);
    args[1] = base_tuple;
    args[2] = locals;

    /* Create unique type */
    fb_type = mp_call_function_n_kw(MP_OBJ_FROM_PTR(&mp_type_type), 3, 0, args);

    display_get_capabilities(self->dev, &capabilities);

    /* Allocate a bytearray buffer */
    buf_size = (capabilities.x_resolution * capabilities.y_resolution
        * DISPLAY_BITS_PER_PIXEL(capabilities.current_pixel_format)) / 8;
    buf = m_new(uint8_t, buf_size);
    memset(buf, 0, buf_size);
    args[0] = mp_obj_new_bytearray_by_ref(buf_size, buf);
    /* FrameBuffer size */
    args[1] = mp_obj_new_int(capabilities.x_resolution);
    args[2] = mp_obj_new_int(capabilities.y_resolution);
    /* FrameBuffer format */
    args[3] = mp_obj_new_int(zephyr_display_framebuf_current_format_helper(self));

    /* Instantiate unique type */
    instance = mp_call_function_n_kw(fb_type, 4, 0, args);

    /* Bind the instance to the type's show method as the self doesn't resolve otherwise,
     * This is fine because the type is unique.
     */
    bound_show = mp_obj_new_bound_meth(MP_OBJ_FROM_PTR(&zephyr_display_as_framebuf_show_obj),
        instance);

    /* Locals object was ingested as a reference */
    mp_obj_dict_store(locals,
        MP_OBJ_NEW_QSTR(MP_QSTR_show),
        bound_show);

    return instance;
}

static MP_DEFINE_CONST_FUN_OBJ_1(zephyr_display_as_framebuf_obj, zephyr_display_as_framebuf);

#endif

static const mp_rom_map_elem_t zephyr_display_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&zephyr_display_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_rgb), MP_ROM_PTR(&zephyr_display_rgb_obj) },
    { MP_ROM_QSTR(MP_QSTR_capabilities), MP_ROM_PTR(&zephyr_display_capabilities_obj) },
    { MP_ROM_QSTR(MP_QSTR_format), MP_ROM_PTR(&zephyr_display_format_obj) },
    { MP_ROM_QSTR(MP_QSTR_blanking), MP_ROM_PTR(&zephyr_display_blanking_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&zephyr_display_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_brightness), MP_ROM_PTR(&zephyr_display_set_brightness_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_contrast), MP_ROM_PTR(&zephyr_display_set_contrast_obj) },
    { MP_ROM_QSTR(MP_QSTR_orientation), MP_ROM_PTR(&zephyr_display_orientation_obj) },
    { MP_ROM_QSTR(MP_QSTR_display_count), MP_ROM_INT(DT_ZEPHYR_DISPLAYS_COUNT) },
    #if MICROPY_PY_FRAMEBUF
    { MP_ROM_QSTR(MP_QSTR_as_framebuf), MP_ROM_PTR(&zephyr_display_as_framebuf_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_ORIENTATION_NORMAL), MP_ROM_INT(DISPLAY_ORIENTATION_NORMAL) },
    { MP_ROM_QSTR(MP_QSTR_ORIENTATION_90), MP_ROM_INT(DISPLAY_ORIENTATION_ROTATED_90) },
    { MP_ROM_QSTR(MP_QSTR_ORIENTATION_180), MP_ROM_INT(DISPLAY_ORIENTATION_ROTATED_180) },
    { MP_ROM_QSTR(MP_QSTR_ORIENTATION_270), MP_ROM_INT(DISPLAY_ORIENTATION_ROTATED_270) },
    { MP_ROM_QSTR(MP_QSTR_FORMAT_RGB_888), MP_ROM_INT(PIXEL_FORMAT_RGB_888) },
    { MP_ROM_QSTR(MP_QSTR_FORMAT_MONO01), MP_ROM_INT(PIXEL_FORMAT_MONO01) },
    { MP_ROM_QSTR(MP_QSTR_FORMAT_MONO10), MP_ROM_INT(PIXEL_FORMAT_MONO10) },
    { MP_ROM_QSTR(MP_QSTR_FORMAT_ARGB_8888), MP_ROM_INT(PIXEL_FORMAT_ARGB_8888) },
    { MP_ROM_QSTR(MP_QSTR_FORMAT_RGB_565), MP_ROM_INT(PIXEL_FORMAT_RGB_565) },
    { MP_ROM_QSTR(MP_QSTR_FORMAT_RGB_565X), MP_ROM_INT(PIXEL_FORMAT_RGB_565X) },
    { MP_ROM_QSTR(MP_QSTR_FORMAT_L_8), MP_ROM_INT(PIXEL_FORMAT_L_8) },
    #if PANEL_PIXEL_FORMAT_AL_88 /* will be in Zephyr 4.3 */
    { MP_ROM_QSTR(MP_QSTR_FORMAT_AL_88), MP_ROM_INT(PIXEL_FORMAT_AL_88) },
    #endif
    /* Zephyr 4.4+ */
    #if PANEL_PIXEL_FORMAT_XRGB_8888
    { MP_ROM_QSTR(MP_QSTR_FORMAT_XRGB_8888), MP_ROM_INT(PIXEL_FORMAT_XRGB_8888) },
    #endif
    #if PANEL_PIXEL_FORMAT_BGR_888
    { MP_ROM_QSTR(MP_QSTR_FORMAT_BGR_888), MP_ROM_INT(PIXEL_FORMAT_BGR_888) },
    #endif
    #if PANEL_PIXEL_FORMAT_ABGR_8888
    { MP_ROM_QSTR(MP_QSTR_FORMAT_ABGR_8888), MP_ROM_INT(PIXEL_FORMAT_ABGR_8888) },
    #endif
    #if PANEL_PIXEL_FORMAT_RGBA_8888
    { MP_ROM_QSTR(MP_QSTR_FORMAT_RGBA_8888), MP_ROM_INT(PIXEL_FORMAT_RGBA_8888) },
    #endif
    #if PANEL_PIXEL_FORMAT_BGRA_8888
    { MP_ROM_QSTR(MP_QSTR_FORMAT_BGRA_8888), MP_ROM_INT(PIXEL_FORMAT_BGRA_8888) },
    #endif
    #if PANEL_PIXEL_FORMAT_I_4
    { MP_ROM_QSTR(MP_QSTR_FORMAT_I_4), MP_ROM_INT(PIXEL_FORMAT_I_4) },
    #endif
    #if PANEL_PIXEL_FORMAT_L_4
    { MP_ROM_QSTR(MP_QSTR_FORMAT_L_4), MP_ROM_INT(PIXEL_FORMAT_L_4) },
    #endif
};
static MP_DEFINE_CONST_DICT(zephyr_display_locals_dict, zephyr_display_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    zephyr_display_type,
    MP_QSTR_Display,
    MP_TYPE_FLAG_NONE,
    make_new, zephyr_display_make_new,
    print, zephyr_display_print,
    locals_dict, &zephyr_display_locals_dict
    );

#endif //  defined(CONFIG_DISPLAY)
