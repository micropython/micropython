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
#include "py/objtype.h"
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

extern const int32_t colorwheel(float pos);

//| .. currentmodule:: pixelbuf
//|
//| :class:`PixelBuf` -- A fast RGB[W] pixel buffer for LED and similar devices
//| ===========================================================================
//|
//| :class:`~_pixelbuf.PixelBuf` implements an RGB[W] bytearray abstraction.
//|
//| .. class:: PixelBuf(size, buf, byteorder="BGR", brightness=0, rawbuf=None, offset=0, auto_write=False)
//|
//|   Create a PixelBuf object of the specified size, byteorder, and bits per pixel.
//|
//|   When given a second bytearray (``rawbuf``), changing brightness adjusts the
//|   brightness of all members of ``buf``.
//|
//|   When only given ``buf``, ``brightness`` applies to the next pixel assignment.
//|
//|   When ``P`` (pwm duration) is present as the 4th character of the byteorder 
//|   string, the 4th value in the tuple/list for a pixel is the individual pixel 
//|   brightness (0.0-1.0) and will enable a Dotstar compatible 1st byte in the
//|   output buffer (``buf``).
//|
//|   :param ~int size: Number of pixelsx
//|   :param ~bytearray buf: Bytearray in which to store pixel data
//|   :param ~str byteorder: Byte order string (such as "BGR" or "PBGR")
//|   :param ~float brightness: Brightness (0 to 1.0, default 1.0)
//|   :param ~bytearray rawbuf: Bytearray in which to store raw pixel data (before brightness adjustment)
//|   :param ~int offset: Offset from start of buffer (default 0)
//|   :param ~bool auto_write: Whether to automatically write pixels (Default False)
//|
STATIC mp_obj_t pixelbuf_pixelbuf_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_arg_check_num(n_args, kw_args, 2, MP_OBJ_FUN_ARGS_MAX, true);
    enum { ARG_size, ARG_buf, ARG_byteorder, ARG_brightness, ARG_rawbuf, ARG_offset,
           ARG_auto_write };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_size, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_buf, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_byteorder, MP_ARG_OBJ, { .u_obj = MP_OBJ_NEW_QSTR(MP_QSTR_BGR) } },
        { MP_QSTR_brightness, MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_rawbuf, MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_offset, MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_auto_write, MP_ARG_BOOL, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    const char *byteorder = NULL;
    pixelbuf_byteorder_details_t byteorder_details;
    size_t bo_len;

    if (!MP_OBJ_IS_STR(args[ARG_byteorder].u_obj))
        mp_raise_TypeError(translate("byteorder is not a string"));

    byteorder = mp_obj_str_get_data(args[ARG_byteorder].u_obj, &bo_len);
    if (bo_len < 3 || bo_len > 4)
        mp_raise_ValueError(translate("Invalid byteorder string"));
    byteorder_details.order = args[ARG_byteorder].u_obj;

    byteorder_details.bpp = bo_len;
    char *dotstar = strchr(byteorder, 'P');
    char *r = strchr(byteorder, 'R');
    char *g = strchr(byteorder, 'G');
    char *b = strchr(byteorder, 'B');
    char *w = strchr(byteorder, 'W');
    int num_chars = (dotstar ? 1 : 0) + (w ? 1 : 0) + (r ? 1 : 0) + (g ? 1 : 0) + (b ? 1 : 0);
    if ((num_chars < byteorder_details.bpp) || !(r && b && g))
        mp_raise_ValueError(translate("Invalid byteorder string"));
    byteorder_details.is_dotstar = dotstar ? true : false;
    byteorder_details.has_white = w ? true : false;
    byteorder_details.byteorder.r = r - byteorder;
    byteorder_details.byteorder.g = g - byteorder;
    byteorder_details.byteorder.b = b - byteorder;
    byteorder_details.byteorder.w = w ? w - byteorder : 0;
    // The dotstar brightness byte is always first (as it goes with the pixel start bits)
    if (dotstar && byteorder[0] != 'P') {
        mp_raise_ValueError(translate("Invalid byteorder string"));
    }
    if (byteorder_details.has_white && byteorder_details.is_dotstar)
        mp_raise_ValueError(translate("Invalid byteorder string"));

    size_t effective_bpp = byteorder_details.is_dotstar ? 4 : byteorder_details.bpp; // Always 4 for DotStar
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

    // Validation complete, allocate and populate object.
    pixelbuf_pixelbuf_obj_t *self = m_new_obj(pixelbuf_pixelbuf_obj_t);

    self->base.type = &pixelbuf_pixelbuf_type;
    self->pixels = args[ARG_size].u_int;
    self->bytes = bytes;
    self->byteorder = byteorder_details;  // Copied because we modify for dotstar
    self->bytearray = args[ARG_buf].u_obj;
    self->two_buffers = two_buffers;
    self->rawbytearray = two_buffers ? args[ARG_rawbuf].u_obj : NULL;
    self->offset = offset;
    self->buf = (uint8_t *)bufinfo.buf + offset;
    self->rawbuf = two_buffers ? (uint8_t *)rawbufinfo.buf + offset : NULL;
    self->pixel_step = effective_bpp;
    self->auto_write = args[ARG_auto_write].u_bool;

    if (args[ARG_brightness].u_obj == mp_const_none) {
        self->brightness = 1.0;
    } else {
        self->brightness = mp_obj_get_float(args[ARG_brightness].u_obj);
        if (self->brightness < 0)
            self->brightness = 0;
        else if (self->brightness > 1)
            self->brightness = 1;
    }

    if (self->byteorder.is_dotstar) {
        // Initialize the buffer with the dotstar start bytes.
        // Note: Header and end must be setup by caller
        for (uint i = 0; i < self->pixels * 4; i += 4) {
            self->buf[i] = DOTSTAR_LED_START_FULL_BRIGHT;
            if (two_buffers) {
                self->rawbuf[i] = DOTSTAR_LED_START_FULL_BRIGHT;
            }
        }
    }

    return MP_OBJ_FROM_PTR(self);
}


// Helper to ensure we have the native super class instead of a subclass.
static pixelbuf_pixelbuf_obj_t* native_pixelbuf(mp_obj_t pixelbuf_obj) {
    mp_obj_t native_pixelbuf = mp_instance_cast_to_native_base(pixelbuf_obj, &pixelbuf_pixelbuf_type);
    mp_obj_assert_native_inited(native_pixelbuf);
    return MP_OBJ_TO_PTR(native_pixelbuf);
}

//|   .. attribute:: bpp
//|
//|     The number of bytes per pixel in the buffer (read-only)
//|
STATIC mp_obj_t pixelbuf_pixelbuf_obj_get_bpp(mp_obj_t self_in) {
    pixelbuf_pixelbuf_obj_t *self = native_pixelbuf(self_in);
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
    pixelbuf_pixelbuf_obj_t *self = native_pixelbuf(self_in);
    return mp_obj_new_float(self->brightness);
}
MP_DEFINE_CONST_FUN_OBJ_1(pixelbuf_pixelbuf_get_brightness_obj, pixelbuf_pixelbuf_obj_get_brightness);


STATIC mp_obj_t pixelbuf_pixelbuf_obj_set_brightness(mp_obj_t self_in, mp_obj_t value) {
    pixelbuf_pixelbuf_obj_t *self = native_pixelbuf(self_in);
    self->brightness = mp_obj_float_get(value);
    if (self->brightness > 1)
        self->brightness = 1;
    else if (self->brightness < 0)
        self->brightness = 0;
    if (self->two_buffers)
        pixelbuf_recalculate_brightness(self);
    if (self->auto_write)
        pixelbuf_call_show(self_in);
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
        if (!self->byteorder.is_dotstar || (i % 4 != 0))
            buf[i] = rawbuf[i] * self->brightness;
    }
}

mp_obj_t pixelbuf_call_show(mp_obj_t self_in) {
    mp_obj_t dest[2];
    mp_load_method(self_in, MP_QSTR_show, dest);
    return mp_call_method_n_kw(0, 0, dest);
}

//|   .. attribute:: auto_write
//|
//|     Whether to automatically write the pixels after each update.
//|
STATIC mp_obj_t pixelbuf_pixelbuf_obj_get_auto_write(mp_obj_t self_in) {
    pixelbuf_pixelbuf_obj_t *self = native_pixelbuf(self_in);
    return mp_obj_new_bool(self->auto_write);
}
MP_DEFINE_CONST_FUN_OBJ_1(pixelbuf_pixelbuf_get_auto_write_obj, pixelbuf_pixelbuf_obj_get_auto_write);


STATIC mp_obj_t pixelbuf_pixelbuf_obj_set_auto_write(mp_obj_t self_in, mp_obj_t value) {
    pixelbuf_pixelbuf_obj_t *self = native_pixelbuf(self_in);
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
    pixelbuf_pixelbuf_obj_t *self = native_pixelbuf(self_in);
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
//|     byteorder string for the buffer (read-only)
//|
STATIC mp_obj_t pixelbuf_pixelbuf_obj_get_byteorder(mp_obj_t self_in) {
    pixelbuf_pixelbuf_obj_t *self = native_pixelbuf(self_in);
    return self->byteorder.order;
}
MP_DEFINE_CONST_FUN_OBJ_1(pixelbuf_pixelbuf_get_byteorder_str, pixelbuf_pixelbuf_obj_get_byteorder);

const mp_obj_property_t pixelbuf_pixelbuf_byteorder_str = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&pixelbuf_pixelbuf_get_byteorder_str,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC mp_obj_t pixelbuf_pixelbuf_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    pixelbuf_pixelbuf_obj_t *self = native_pixelbuf(self_in);
    switch (op) {
        case MP_UNARY_OP_BOOL: return mp_const_true;
        case MP_UNARY_OP_LEN: return MP_OBJ_NEW_SMALL_INT(self->pixels);
        default: return MP_OBJ_NULL; // op not supported
    }
}

//|   .. method:: show()
//|
//|     Must be implemented in subclasses.
//|

STATIC mp_obj_t pixelbuf_pixelbuf_show(mp_obj_t self_in) {
    mp_raise_NotImplementedError(NULL);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pixelbuf_pixelbuf_show_obj, pixelbuf_pixelbuf_show);


//|   .. method:: __getitem__(index)
//|
//|     Returns the pixel value at the given index.
//|
//|   .. method:: __setitem__(index, value)
//|
//|     Sets the pixel value at the given index.
//|
STATIC mp_obj_t pixelbuf_pixelbuf_subscr(mp_obj_t self_in, mp_obj_t index_in, mp_obj_t value) {
    if (value == MP_OBJ_NULL) {
        // delete item
        // slice deletion
        return MP_OBJ_NULL; // op not supported
    }

    pixelbuf_pixelbuf_obj_t *self = native_pixelbuf(self_in);

    if (0) {
#if MICROPY_PY_BUILTINS_SLICE
    } else if (MP_OBJ_IS_TYPE(index_in, &mp_type_slice)) {
        mp_bound_slice_t slice;

        mp_seq_get_fast_slice_indexes(self->pixels, index_in, &slice);

        if ((slice.stop * self->pixel_step) > self->bytes)
            mp_raise_IndexError(translate("Range out of bounds"));
        if (slice.step < 0)
            mp_raise_IndexError(translate("Negative step not supported"));

        if (value == MP_OBJ_SENTINEL) { // Get
            size_t len = slice.stop - slice.start;
            if (slice.step > 1) {
                len = (len / slice.step) + (len % slice.step ? 1 : 0);
            }
            uint8_t *readbuf = self->two_buffers ? self->rawbuf : self->buf;
            return pixelbuf_get_pixel_array(readbuf + slice.start, len, &self->byteorder, self->pixel_step, slice.step, self->byteorder.is_dotstar);
        } else { // Set
            #if MICROPY_PY_ARRAY_SLICE_ASSIGN

            if (!(MP_OBJ_IS_TYPE(value, &mp_type_list) || MP_OBJ_IS_TYPE(value, &mp_type_tuple)))
                mp_raise_ValueError(translate("tuple/list required on RHS"));

            size_t dst_len = (slice.stop - slice.start);
            if (slice.step > 1) {
                dst_len = (dst_len / slice.step) + (dst_len % slice.step ? 1 : 0);
            }
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

            size_t target_i = slice.start;
            for (size_t i = slice.start; target_i < slice.stop; i++, target_i += slice.step) {
                mp_obj_t *item = src_objs[i-slice.start];
                if (MP_OBJ_IS_TYPE(value, &mp_type_list) || MP_OBJ_IS_TYPE(value, &mp_type_tuple) || MP_OBJ_IS_INT(value)) {
                    pixelbuf_set_pixel(self->buf + (target_i * self->pixel_step),
                        self->two_buffers ? self->rawbuf + (i * self->pixel_step) : NULL,
                        self->brightness, item, &self->byteorder, self->byteorder.is_dotstar);
                }
            }
            if (self->auto_write)
                pixelbuf_call_show(self_in);
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
            return pixelbuf_get_pixel(pixelstart, &self->byteorder, self->byteorder.is_dotstar);
        } else { // Store
            pixelbuf_set_pixel(self->buf + offset, self->two_buffers ? self->rawbuf + offset : NULL,
                self->brightness, value, &self->byteorder, self->byteorder.is_dotstar);
            if (self->auto_write)
                pixelbuf_call_show(self_in);
            return mp_const_none;
        }
    }
}

STATIC const mp_rom_map_elem_t pixelbuf_pixelbuf_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_auto_write), MP_ROM_PTR(&pixelbuf_pixelbuf_auto_write_obj)},
    { MP_ROM_QSTR(MP_QSTR_bpp), MP_ROM_PTR(&pixelbuf_pixelbuf_bpp_obj)},
    { MP_ROM_QSTR(MP_QSTR_brightness), MP_ROM_PTR(&pixelbuf_pixelbuf_brightness_obj)},
    { MP_ROM_QSTR(MP_QSTR_buf), MP_ROM_PTR(&pixelbuf_pixelbuf_buf_obj)},
    { MP_ROM_QSTR(MP_QSTR_byteorder), MP_ROM_PTR(&pixelbuf_pixelbuf_byteorder_str)},
    { MP_ROM_QSTR(MP_QSTR_show), MP_ROM_PTR(&pixelbuf_pixelbuf_show_obj)},
};

STATIC MP_DEFINE_CONST_DICT(pixelbuf_pixelbuf_locals_dict, pixelbuf_pixelbuf_locals_dict_table);


const mp_obj_type_t pixelbuf_pixelbuf_type = {
        { &mp_type_type },
        .name = MP_QSTR_PixelBuf,
        .subscr = pixelbuf_pixelbuf_subscr,
        .make_new = pixelbuf_pixelbuf_make_new,
        .unary_op = pixelbuf_pixelbuf_unary_op,
        .getiter = mp_obj_new_generic_iterator,
        .print = NULL,
        .locals_dict = (mp_obj_t)&pixelbuf_pixelbuf_locals_dict,
};
