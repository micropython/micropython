/*
 * This file is part of the Circuit Python project, https://github.com/adafruit/circuitpython
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Roy Hooper
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
#include "py/objarray.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/gc.h"

#include <string.h>

#include "PixelBuf.h"
#include "shared-bindings/_pixelbuf/types.h"
#include "../../shared-module/_pixelbuf/PixelBuf.h"
#include "shared-bindings/digitalio/DigitalInOut.h"

extern const pixelbuf_byteorder_obj_t byteorder_BGR;
extern const mp_obj_type_t pixelbuf_byteorder_type;
extern const int32_t colorwheel(float pos);

//| .. currentmodule:: pixelbuf
//|
//| :class:`PixelBuf` -- A fast RGB[W] pixel buffer for LED and similar devices
//| ===========================================================================
//|
//| :class:`~_pixelbuf.PixelBuf` implements an RGB[W] bytearray abstraction.
//|
//| .. class:: PixelBuf(size, buf, byteorder=BGR, bpp=3)
//|
//|   Create a PixelBuf object of the specified size, byteorder, and bits per pixel.
//|
//|   When given a second bytearray (``rawbuf``), changing brightness adjusts the
//|   brightness of all members of ``buf``.
//|
//|   When only given ``buf``, ``brightness`` applies to the next pixel assignment.
//|
//|   When ``dotstar`` is True, and ``bpp`` is 4, the 4th value in a tuple/list
//|   is the individual pixel brightness (0-1).  Not compatible with RGBW Byteorders.
//|   Compatible `ByteOrder` classes are bpp=3, or bpp=4 and has_luminosity=True (g LBGR).
//|
//|   :param ~int size: Number of pixelsx
//|   :param ~bytearray buf: Bytearray to store pixel data in
//|   :param ~_pixelbuf.ByteOrder byteorder: Byte order constant from `_pixelbuf` (also sets the bpp)
//|   :param ~float brightness: Brightness (0 to 1.0, default 1.0)
//|   :param ~bytearray rawbuf: Bytearray to store raw pixel colors in
//|   :param ~int offset: Offset from start of buffer (default 0)
//|   :param ~bool dotstar: Dotstar mode (default False)
//|   :param ~bool auto_write: Whether to automatically write pixels (Default False)
//|   :param ~callable write_function: (optional) Callable to use to send pixels
//|   :param ~list write_args: (optional) Tuple or list of args to pass to ``write_function``.  The 
//|          PixelBuf instance is appended after these args.
//|
STATIC mp_obj_t pixelbuf_pixelbuf_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_arg_check_num(n_args, kw_args, 2, MP_OBJ_FUN_ARGS_MAX, true);
    enum { ARG_size, ARG_buf, ARG_byteorder, ARG_brightness, ARG_rawbuf, ARG_offset, ARG_dotstar,
           ARG_auto_write, ARG_write_function, ARG_write_args };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_size, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_buf, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_byteorder, MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_brightness, MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_rawbuf, MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_offset, MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_dotstar, MP_ARG_BOOL, { .u_bool = false } },
        { MP_QSTR_auto_write, MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_write_function, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_write_args, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (mp_obj_is_subclass_fast(args[ARG_byteorder].u_obj, &pixelbuf_byteorder_type)) 
        mp_raise_TypeError_varg(translate("byteorder is not an instance of ByteOrder (got a %s)"), mp_obj_get_type_str(args[ARG_byteorder].u_obj));

    pixelbuf_byteorder_obj_t *byteorder = (args[ARG_byteorder].u_obj == mp_const_none) ? MP_OBJ_FROM_PTR(&byteorder_BGR) : args[ARG_byteorder].u_obj;

    if (byteorder->has_white && args[ARG_dotstar].u_bool)
        mp_raise_ValueError_varg(translate("Can not use dotstar with %s"), mp_obj_get_type_str(byteorder));

    size_t effective_bpp = args[ARG_dotstar].u_bool ? 4 : byteorder->bpp; // Always 4 for DotStar
    size_t bytes = args[ARG_size].u_int * effective_bpp;
    size_t offset = args[ARG_offset].u_int;
    mp_buffer_info_t bufinfo, rawbufinfo;

    mp_get_buffer_raise(args[ARG_buf].u_obj, &bufinfo, MP_BUFFER_READ | MP_BUFFER_WRITE);
    bool two_buffers = args[ARG_rawbuf].u_obj != mp_const_none;
    if (two_buffers) {
        mp_get_buffer_raise(args[ARG_rawbuf].u_obj, &rawbufinfo, MP_BUFFER_READ | MP_BUFFER_WRITE);
        if (rawbufinfo.len != bufinfo.len) {
            mp_raise_ValueError(translate("rawbuf is not the same size as buf"));
        }
    }

    if (bytes + offset > bufinfo.len)
        mp_raise_ValueError_varg(translate("buf is too small. need %d bytes"), bytes + offset);

    if (!MP_OBJ_IS_TYPE(args[ARG_write_args].u_obj, &mp_type_list) &&
        !MP_OBJ_IS_TYPE(args[ARG_write_args].u_obj, &mp_type_tuple) &&
        args[ARG_write_args].u_obj != mp_const_none) 
    {
        mp_raise_ValueError(translate("write_args must be a list, tuple, or None"));
    }

    // Validation complete, allocate and populate object.
    pixelbuf_pixelbuf_obj_t *self = m_new_obj(pixelbuf_pixelbuf_obj_t);

    self->base.type = &pixelbuf_pixelbuf_type;
    self->pixels = args[ARG_size].u_int;
    self->bytes = bytes;
    self->byteorder = *byteorder;  // Copied because we modify for dotstar
    self->bytearray = args[ARG_buf].u_obj;
    self->two_buffers = two_buffers;
    self->rawbytearray = two_buffers ? args[ARG_rawbuf].u_obj : NULL;
    self->offset = offset;
    self->dotstar_mode = args[ARG_dotstar].u_bool;
    self->buf = (uint8_t *)bufinfo.buf + offset;
    self->rawbuf = two_buffers ? (uint8_t *)rawbufinfo.buf + offset : NULL;
    self->pixel_step = effective_bpp;
    self->auto_write = args[ARG_auto_write].u_bool;

    if (self->dotstar_mode) {
        // Ensure sane configuration
        if (!self->byteorder.has_luminosity) {
            self->byteorder.has_luminosity = true;
            self->byteorder.byteorder.b += 1;
            self->byteorder.byteorder.g += 1;
            self->byteorder.byteorder.r += 1;
        }
        self->byteorder.byteorder.w = 0;
    }

    // Show/auto-write callbacks
    self->write_function = args[ARG_write_function].u_obj;
    mp_obj_t function_args = args[ARG_write_args].u_obj;
    mp_obj_t *src_objs = (mp_obj_t *)&mp_const_none_obj;
    size_t num_items = 0;
    if (function_args != mp_const_none) {
        if (MP_OBJ_IS_TYPE(function_args, &mp_type_list)) {
            mp_obj_list_t *t = MP_OBJ_TO_PTR(function_args);
            num_items = t->len;
            src_objs = t->items;
        } else {
            mp_obj_tuple_t *l = MP_OBJ_TO_PTR(function_args);
            num_items = l->len;
            src_objs = l->items;
        }
    }
    self->write_function_args = mp_obj_new_tuple(num_items + 1, NULL);
    for (size_t i = 0; i < num_items; i++) {
        self->write_function_args->items[i] = src_objs[i];
    }
    self->write_function_args->items[num_items] = self;

    if (args[ARG_brightness].u_obj == mp_const_none) {
        self->brightness = 1.0;
    } else {
        self->brightness = mp_obj_get_float(args[ARG_brightness].u_obj);
        if (self->brightness < 0)
            self->brightness = 0;
        else if (self->brightness > 1)
            self->brightness = 1;
    }
    
    if (self->dotstar_mode) { 
        // Initialize the buffer with the dotstar start bytes.
        // Header and end must be setup by caller
        for (uint i = 0; i < self->pixels * 4; i += 4) {
            self->buf[i] = DOTSTAR_LED_START_FULL_BRIGHT;
            if (two_buffers) {
                self->rawbuf[i] = DOTSTAR_LED_START_FULL_BRIGHT;
            }
        }
    }
    
    return MP_OBJ_FROM_PTR(self);
}

//|   .. attribute:: bpp
//|
//|     The number of bytes per pixel in the buffer (read-only)
//|
STATIC mp_obj_t pixelbuf_pixelbuf_obj_get_bpp(mp_obj_t self_in) {
    mp_check_self(MP_OBJ_IS_TYPE(self_in, &pixelbuf_pixelbuf_type));
    pixelbuf_pixelbuf_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int_from_uint(self->byteorder.bpp);
}
MP_DEFINE_CONST_FUN_OBJ_1(pixelbuf_pixelbuf_get_bpp_obj, pixelbuf_pixelbuf_obj_get_bpp);

const mp_obj_property_t pixelbuf_pixelbuf_bpp_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&pixelbuf_pixelbuf_get_bpp_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};


//|   .. attribute:: brightness
//|
//|     Float value between 0 and 1.  Output brightness.
//|     If the PixelBuf was allocated with two both a buf and a rawbuf,
//|     setting this value causes a recomputation of the values in buf.
//|     If only a buf was provided, then the brightness only applies to
//|     future pixel changes.
//|     In DotStar mode 
//|
STATIC mp_obj_t pixelbuf_pixelbuf_obj_get_brightness(mp_obj_t self_in) {
    mp_check_self(MP_OBJ_IS_TYPE(self_in, &pixelbuf_pixelbuf_type));
    pixelbuf_pixelbuf_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_float(self->brightness);
}
MP_DEFINE_CONST_FUN_OBJ_1(pixelbuf_pixelbuf_get_brightness_obj, pixelbuf_pixelbuf_obj_get_brightness);


STATIC mp_obj_t pixelbuf_pixelbuf_obj_set_brightness(mp_obj_t self_in, mp_obj_t value) {
    mp_check_self(MP_OBJ_IS_TYPE(self_in, &pixelbuf_pixelbuf_type));
    pixelbuf_pixelbuf_obj_t *self = MP_OBJ_TO_PTR(self_in);
    self->brightness = mp_obj_float_get(value);
    if (self->brightness > 1)
        self->brightness = 1;
    else if (self->brightness < 0)
        self->brightness = 0;
    if (self->two_buffers)
        pixelbuf_recalculate_brightness(self);
    if (self->auto_write)
        call_write_function(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(pixelbuf_pixelbuf_set_brightness_obj, pixelbuf_pixelbuf_obj_set_brightness);

const mp_obj_property_t pixelbuf_pixelbuf_brightness_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&pixelbuf_pixelbuf_get_brightness_obj,
              (mp_obj_t)&pixelbuf_pixelbuf_set_brightness_obj,
              (mp_obj_t)&mp_const_none_obj},
};

void pixelbuf_recalculate_brightness(pixelbuf_pixelbuf_obj_t *self) {
    uint8_t *buf = (uint8_t *)self->buf;
    uint8_t *rawbuf = (uint8_t *)self->rawbuf;
    // Compensate for shifted buffer (bpp=3 dotstar)
    for (uint i = 0; i < self->bytes; i++) {
        // Don't adjust per-pixel luminance bytes in dotstar mode
        if (!self->dotstar_mode || (i % 4 != 0)) 
            buf[i] = rawbuf[i] * self->brightness;
    }
}

//|   .. attribute:: auto_write
//|
//|     Whether to automatically write the pixels after each update.
//|
STATIC mp_obj_t pixelbuf_pixelbuf_obj_get_auto_write(mp_obj_t self_in) {
    mp_check_self(MP_OBJ_IS_TYPE(self_in, &pixelbuf_pixelbuf_type));
    pixelbuf_pixelbuf_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(self->auto_write);
}
MP_DEFINE_CONST_FUN_OBJ_1(pixelbuf_pixelbuf_get_auto_write_obj, pixelbuf_pixelbuf_obj_get_auto_write);


STATIC mp_obj_t pixelbuf_pixelbuf_obj_set_auto_write(mp_obj_t self_in, mp_obj_t value) {
    mp_check_self(MP_OBJ_IS_TYPE(self_in, &pixelbuf_pixelbuf_type));
    pixelbuf_pixelbuf_obj_t *self = MP_OBJ_TO_PTR(self_in);
    self->auto_write = mp_obj_is_true(value);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(pixelbuf_pixelbuf_set_auto_write_obj, pixelbuf_pixelbuf_obj_set_auto_write);

const mp_obj_property_t pixelbuf_pixelbuf_auto_write_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&pixelbuf_pixelbuf_get_auto_write_obj,
              (mp_obj_t)&pixelbuf_pixelbuf_set_auto_write_obj,
              (mp_obj_t)&mp_const_none_obj},
};


//|   .. attribute:: buf
//|
//|     (read-only) bytearray of pixel data after brightness adjustment.  If an offset was provided
//|     then this bytearray is the subset of the bytearray passed in that represents the
//|     actual pixels.
//|
STATIC mp_obj_t pixelbuf_pixelbuf_obj_get_buf(mp_obj_t self_in) {
    mp_check_self(MP_OBJ_IS_TYPE(self_in, &pixelbuf_pixelbuf_type));
    pixelbuf_pixelbuf_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bytearray_by_ref(self->bytes, self->buf);
}
MP_DEFINE_CONST_FUN_OBJ_1(pixelbuf_pixelbuf_get_buf_obj, pixelbuf_pixelbuf_obj_get_buf);

const mp_obj_property_t pixelbuf_pixelbuf_buf_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&pixelbuf_pixelbuf_get_buf_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|   .. attribute:: byteorder
//|
//|     `ByteOrder` class for the buffer (read-only)
//|
STATIC mp_obj_t pixelbuf_pixelbuf_obj_get_byteorder(mp_obj_t self_in) {
    mp_check_self(MP_OBJ_IS_TYPE(self_in, &pixelbuf_pixelbuf_type));
    pixelbuf_pixelbuf_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return &self->byteorder;
}
MP_DEFINE_CONST_FUN_OBJ_1(pixelbuf_pixelbuf_get_byteorder_obj, pixelbuf_pixelbuf_obj_get_byteorder);

const mp_obj_property_t pixelbuf_pixelbuf_byteorder_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&pixelbuf_pixelbuf_get_byteorder_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC mp_obj_t pixelbuf_pixelbuf_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    mp_check_self(MP_OBJ_IS_TYPE(self_in, &pixelbuf_pixelbuf_type));
    pixelbuf_pixelbuf_obj_t *self = MP_OBJ_TO_PTR(self_in);
    switch (op) {
        case MP_UNARY_OP_BOOL: return mp_const_true;
        case MP_UNARY_OP_LEN: return MP_OBJ_NEW_SMALL_INT(self->pixels);
        default: return MP_OBJ_NULL; // op not supported
    }
}

//|   .. method:: show()
//|
//|     Call the associated write function to display the pixels.
//|

STATIC mp_obj_t pixelbuf_pixelbuf_show(mp_obj_t self_in) {
    mp_check_self(MP_OBJ_IS_TYPE(self_in, &pixelbuf_pixelbuf_type));
    pixelbuf_pixelbuf_obj_t *self = MP_OBJ_TO_PTR(self_in);
    call_write_function(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pixelbuf_pixelbuf_show_obj, pixelbuf_pixelbuf_show);

void call_write_function(pixelbuf_pixelbuf_obj_t *self) {
    // execute function if it's set
    if (self->write_function != mp_const_none) {
        mp_call_function_n_kw(self->write_function, self->write_function_args->len, 0, self->write_function_args->items);
    }
}



//|   .. method:: []
//|
//|     Get or set pixels.  Supports individual pixels and slices.
//|
STATIC mp_obj_t pixelbuf_pixelbuf_subscr(mp_obj_t self_in, mp_obj_t index_in, mp_obj_t value) {
    mp_check_self(MP_OBJ_IS_TYPE(self_in, &pixelbuf_pixelbuf_type));

    if (value == MP_OBJ_NULL) {
        // delete item
        // slice deletion
        return MP_OBJ_NULL; // op not supported
    } 

    pixelbuf_pixelbuf_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (0) {
#if MICROPY_PY_BUILTINS_SLICE
    } else if (MP_OBJ_IS_TYPE(index_in, &mp_type_slice)) {
        mp_bound_slice_t slice;

        if (!mp_seq_get_fast_slice_indexes(self->bytes, index_in, &slice))
            mp_raise_NotImplementedError(translate("Only slices with step=1 (aka None) are supported"));
        if ((slice.stop * self->pixel_step) > self->bytes) 
            mp_raise_IndexError(translate("Range out of bounds"));

        if (value == MP_OBJ_SENTINEL) { // Get
            size_t len = slice.stop - slice.start;
            return pixelbuf_get_pixel_array((uint8_t *) self->buf + slice.start, len, &self->byteorder, self->pixel_step, self->dotstar_mode);
        } else { // Set
            #if MICROPY_PY_ARRAY_SLICE_ASSIGN

            if (!(MP_OBJ_IS_TYPE(value, &mp_type_list) || MP_OBJ_IS_TYPE(value, &mp_type_tuple)))
                mp_raise_ValueError(translate("tuple/list required on RHS"));

            size_t dst_len = slice.stop - slice.start;

            mp_obj_t *src_objs;
            size_t num_items;
            if (MP_OBJ_IS_TYPE(value, &mp_type_list)) {
                mp_obj_list_t *t = MP_OBJ_TO_PTR(value);
                num_items = t->len;
                src_objs = t->items;
            } else {
                mp_obj_tuple_t *l = MP_OBJ_TO_PTR(value);
                num_items = l->len;
                src_objs = l->items;
            }
            if (num_items != dst_len)
                mp_raise_ValueError_varg(translate("Unmatched number of items on RHS (expected %d, got %d)."),
                                                   dst_len, num_items);

            for (size_t i = slice.start; i < slice.stop; i++) {
                mp_obj_t *item = src_objs[i-slice.start];
                if (MP_OBJ_IS_TYPE(value, &mp_type_list) || MP_OBJ_IS_TYPE(value, &mp_type_tuple) || MP_OBJ_IS_INT(value)) {
                    pixelbuf_set_pixel(self->buf + (i * self->pixel_step), 
                        self->two_buffers ? self->rawbuf + (i * self->pixel_step) : NULL, 
                        self->brightness, item, &self->byteorder, self->dotstar_mode);
                }
            }
            if (self->auto_write)
                call_write_function(self);
            return mp_const_none;
            #else
            return MP_OBJ_NULL; // op not supported
            #endif
        }
#endif
    } else { // Single index rather than slice.
        size_t index = mp_get_index(self->base.type, self->pixels, index_in, false);
        size_t offset = (index * self->pixel_step);
        if (offset > self->bytes) 
            mp_raise_IndexError(translate("Pixel beyond bounds of buffer"));

        if (value == MP_OBJ_SENTINEL) { // Get
            uint8_t *pixelstart = (uint8_t *)(self->two_buffers ? self->rawbuf : self->buf) + offset;
            return pixelbuf_get_pixel(pixelstart, &self->byteorder, self->dotstar_mode);
        } else { // Store
            pixelbuf_set_pixel(self->buf + offset, self->two_buffers ? self->rawbuf + offset : NULL, 
                self->brightness, value, &self->byteorder, self->dotstar_mode);
            if (self->auto_write)
                call_write_function(self);
            return mp_const_none;
        }
    }
}

STATIC const mp_rom_map_elem_t pixelbuf_pixelbuf_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_auto_write), MP_ROM_PTR(&pixelbuf_pixelbuf_auto_write_obj)},
    { MP_ROM_QSTR(MP_QSTR_bpp), MP_ROM_PTR(&pixelbuf_pixelbuf_bpp_obj)},
    { MP_ROM_QSTR(MP_QSTR_brightness), MP_ROM_PTR(&pixelbuf_pixelbuf_brightness_obj)},
    { MP_ROM_QSTR(MP_QSTR_buf), MP_ROM_PTR(&pixelbuf_pixelbuf_buf_obj)},
    { MP_ROM_QSTR(MP_QSTR_byteorder), MP_ROM_PTR(&pixelbuf_pixelbuf_byteorder_obj)},
    { MP_ROM_QSTR(MP_QSTR_show), MP_ROM_PTR(&pixelbuf_pixelbuf_show_obj)},
};

STATIC MP_DEFINE_CONST_DICT(pixelbuf_pixelbuf_locals_dict, pixelbuf_pixelbuf_locals_dict_table);


const mp_obj_type_t pixelbuf_pixelbuf_type = {
        { &mp_type_type },
        .name = MP_QSTR_PixelBuf,
        .subscr = pixelbuf_pixelbuf_subscr,
        .make_new = pixelbuf_pixelbuf_make_new,
        .unary_op = pixelbuf_pixelbuf_unary_op,
        .print = NULL,
        .locals_dict = (mp_obj_t)&pixelbuf_pixelbuf_locals_dict,
};
