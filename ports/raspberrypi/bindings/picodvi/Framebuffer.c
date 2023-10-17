/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Scott Shawcroft for Adafruit Industries
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
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/objarray.h"

#include "bindings/picodvi/Framebuffer.h"
#include "shared-bindings/util.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/framebufferio/__init__.h"
#include "shared-module/framebufferio/FramebufferDisplay.h"

//| class Framebuffer:
//|     def __init__(
//|         self,
//|         width: int,
//|         height: int,
//|         *,
//|         clk_dp: microcontroller.Pin,
//|         clk_dn: microcontroller.Pin,
//|         red_dp: microcontroller.Pin,
//|         red_dn: microcontroller.Pin,
//|         green_dp: microcontroller.Pin,
//|         green_dn: microcontroller.Pin,
//|         blue_dp: microcontroller.Pin,
//|         blue_dn: microcontroller.Pin,
//|         color_depth: int = 8,
//|     ) -> None:
//|         """Create a Framebuffer object with the given dimensions. Memory is
//|         allocated outside of onto the heap and then moved outside on VM end.
//|
//|         .. warning:: This will change the system clock speed to match the DVI signal.
//|            Make sure to initialize other objects after this one so they account
//|            for the changed clock.
//|
//|         This allocates a very large framebuffer and is most likely to succeed
//|         the earlier it is attempted.
//|
//|         Each dp and dn pair of pins must be neighboring, such as 19 and 20.
//|         They must also be ordered the same way. In other words, dp must be
//|         less than dn for all pairs or dp must be greater than dn for all pairs.
//|
//|         The framebuffer pixel format varies depending on color_depth:
//|
//|         * 1 - Each bit is a pixel. Either white (1) or black (0).
//|         * 2 - Each 2 bits is a pixels. Grayscale between white (0x3) and black (0x0).
//|         * 8 - Each byte is a pixels in RGB332 format.
//|         * 16 - Each two bytes are a pixel in RGB565 format.
//|
//|         Two output resolutions are currently supported, 640x480 and 800x480.
//|         Monochrome framebuffers (color_depth=1 or 2) must be full resolution.
//|         Color framebuffers must be half resolution (320x240 or 400x240) and
//|         pixels will be duplicated to create the signal.
//|
//|         A Framebuffer is often used in conjunction with a
//|         `framebufferio.FramebufferDisplay`.
//|
//|         :param int width: the width of the target display signal. Only 320, 400, 640 or 800 is currently supported depending on color_depth.
//|         :param int height: the height of the target display signal. Only 240 or 480 is currently supported depending on color_depth.
//|         :param ~microcontroller.Pin clk_dp: the positive clock signal pin
//|         :param ~microcontroller.Pin clk_dn: the negative clock signal pin
//|         :param ~microcontroller.Pin red_dp: the positive red signal pin
//|         :param ~microcontroller.Pin red_dn: the negative red signal pin
//|         :param ~microcontroller.Pin green_dp: the positive green signal pin
//|         :param ~microcontroller.Pin green_dn: the negative green signal pin
//|         :param ~microcontroller.Pin blue_dp: the positive blue signal pin
//|         :param ~microcontroller.Pin blue_dn: the negative blue signal pin
//|         :param int color_depth: the color depth of the framebuffer in bits. 1, 2 for grayscale
//|           and 8 or 16 for color
//|         """

STATIC mp_obj_t picodvi_framebuffer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_width, ARG_height, ARG_clk_dp, ARG_clk_dn, ARG_red_dp, ARG_red_dn, ARG_green_dp,
           ARG_green_dn, ARG_blue_dp, ARG_blue_dn, ARG_color_depth };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_width, MP_ARG_INT | MP_ARG_REQUIRED },
        { MP_QSTR_height, MP_ARG_INT | MP_ARG_REQUIRED },

        { MP_QSTR_clk_dp, MP_ARG_KW_ONLY | MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_clk_dn, MP_ARG_KW_ONLY | MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_red_dp, MP_ARG_KW_ONLY | MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_red_dn, MP_ARG_KW_ONLY | MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_green_dp, MP_ARG_KW_ONLY | MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_green_dn, MP_ARG_KW_ONLY | MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_blue_dp, MP_ARG_KW_ONLY | MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_blue_dn, MP_ARG_KW_ONLY | MP_ARG_OBJ | MP_ARG_REQUIRED },

        { MP_QSTR_color_depth, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    picodvi_framebuffer_obj_t *self = &allocate_display_bus_or_raise()->picodvi;
    self->base.type = &picodvi_framebuffer_type;

    mp_uint_t width = (mp_uint_t)mp_arg_validate_int_min(args[ARG_width].u_int, 0, MP_QSTR_width);
    mp_uint_t height = (mp_uint_t)mp_arg_validate_int_min(args[ARG_height].u_int, 0, MP_QSTR_height);
    mp_uint_t color_depth = args[ARG_color_depth].u_int;
    if (color_depth != 1 && color_depth != 2 && color_depth != 8 && color_depth != 16) {
        mp_raise_ValueError_varg(translate("Invalid %q"), MP_QSTR_color_depth);
    }
    common_hal_picodvi_framebuffer_construct(self,
        width, height,
        validate_obj_is_free_pin(args[ARG_clk_dp].u_obj, MP_QSTR_clk_dp),
        validate_obj_is_free_pin(args[ARG_clk_dn].u_obj, MP_QSTR_clk_dn),
        validate_obj_is_free_pin(args[ARG_red_dp].u_obj, MP_QSTR_red_dp),
        validate_obj_is_free_pin(args[ARG_red_dn].u_obj, MP_QSTR_red_dn),
        validate_obj_is_free_pin(args[ARG_green_dp].u_obj, MP_QSTR_green_dp),
        validate_obj_is_free_pin(args[ARG_green_dn].u_obj, MP_QSTR_green_dn),
        validate_obj_is_free_pin(args[ARG_blue_dp].u_obj, MP_QSTR_blue_dp),
        validate_obj_is_free_pin(args[ARG_blue_dn].u_obj, MP_QSTR_blue_dn),
        color_depth);

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Free the resources (pins, timers, etc.) associated with this
//|         `picodvi.Framebuffer` instance.  After deinitialization, no further operations
//|         may be performed."""
//|         ...
STATIC mp_obj_t picodvi_framebuffer_deinit(mp_obj_t self_in) {
    picodvi_framebuffer_obj_t *self = (picodvi_framebuffer_obj_t *)self_in;
    common_hal_picodvi_framebuffer_deinit(self);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(picodvi_framebuffer_deinit_obj, picodvi_framebuffer_deinit);

static void check_for_deinit(picodvi_framebuffer_obj_t *self) {
    if (common_hal_picodvi_framebuffer_deinited(self)) {
        raise_deinited_error();
    }
}

//|     width: int
//|     """The width of the framebuffer, in pixels. It may be doubled for output."""
STATIC mp_obj_t picodvi_framebuffer_get_width(mp_obj_t self_in) {
    picodvi_framebuffer_obj_t *self = (picodvi_framebuffer_obj_t *)self_in;
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_picodvi_framebuffer_get_width(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(picodvi_framebuffer_get_width_obj, picodvi_framebuffer_get_width);
MP_PROPERTY_GETTER(picodvi_framebuffer_width_obj,
    (mp_obj_t)&picodvi_framebuffer_get_width_obj);

//|     height: int
//|     """The width of the framebuffer, in pixels. It may be doubled for output."""
//|
STATIC mp_obj_t picodvi_framebuffer_get_height(mp_obj_t self_in) {
    picodvi_framebuffer_obj_t *self = (picodvi_framebuffer_obj_t *)self_in;
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_picodvi_framebuffer_get_height(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(picodvi_framebuffer_get_height_obj, picodvi_framebuffer_get_height);

MP_PROPERTY_GETTER(picodvi_framebuffer_height_obj,
    (mp_obj_t)&picodvi_framebuffer_get_height_obj);

STATIC const mp_rom_map_elem_t picodvi_framebuffer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&picodvi_framebuffer_deinit_obj) },

    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&picodvi_framebuffer_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_PTR(&picodvi_framebuffer_height_obj) },
};
STATIC MP_DEFINE_CONST_DICT(picodvi_framebuffer_locals_dict, picodvi_framebuffer_locals_dict_table);

STATIC void picodvi_framebuffer_get_bufinfo(mp_obj_t self_in, mp_buffer_info_t *bufinfo) {
    common_hal_picodvi_framebuffer_get_buffer(self_in, bufinfo, 0);
}

// These versions exist so that the prototype matches the protocol,
// avoiding a type cast that can hide errors
STATIC void picodvi_framebuffer_swapbuffers(mp_obj_t self_in, uint8_t *dirty_row_bitmap) {
    (void)dirty_row_bitmap;
    common_hal_picodvi_framebuffer_refresh(self_in);
}

STATIC void picodvi_framebuffer_deinit_proto(mp_obj_t self_in) {
    common_hal_picodvi_framebuffer_deinit(self_in);
}

STATIC int picodvi_framebuffer_get_width_proto(mp_obj_t self_in) {
    return common_hal_picodvi_framebuffer_get_width(self_in);
}

STATIC int picodvi_framebuffer_get_height_proto(mp_obj_t self_in) {
    return common_hal_picodvi_framebuffer_get_height(self_in);
}

STATIC int picodvi_framebuffer_get_color_depth_proto(mp_obj_t self_in) {
    return common_hal_picodvi_framebuffer_get_color_depth(self_in);
    ;
}

STATIC int picodvi_framebuffer_get_bytes_per_cell_proto(mp_obj_t self_in) {
    return 1;
}

STATIC int picodvi_framebuffer_get_native_frames_per_second_proto(mp_obj_t self_in) {
    return 60;
}

STATIC bool picodvi_framebuffer_get_pixels_in_byte_share_row_proto(mp_obj_t self_in) {
    return true;
}

STATIC int picodvi_framebuffer_get_row_stride_proto(mp_obj_t self_in) {
    return common_hal_picodvi_framebuffer_get_row_stride(self_in);
}

STATIC const framebuffer_p_t picodvi_framebuffer_proto = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_framebuffer)
    .get_bufinfo = picodvi_framebuffer_get_bufinfo,
    .get_width = picodvi_framebuffer_get_width_proto,
    .get_height = picodvi_framebuffer_get_height_proto,
    .get_color_depth = picodvi_framebuffer_get_color_depth_proto,
    .get_row_stride = picodvi_framebuffer_get_row_stride_proto,
    .get_bytes_per_cell = picodvi_framebuffer_get_bytes_per_cell_proto,
    .get_native_frames_per_second = picodvi_framebuffer_get_native_frames_per_second_proto,
    .get_pixels_in_byte_share_row = picodvi_framebuffer_get_pixels_in_byte_share_row_proto,
    .swapbuffers = picodvi_framebuffer_swapbuffers,
    .deinit = picodvi_framebuffer_deinit_proto,
};

MP_DEFINE_CONST_OBJ_TYPE(
    picodvi_framebuffer_type,
    MP_QSTR_Framebuffer,
    MP_TYPE_FLAG_NONE,
    locals_dict, &picodvi_framebuffer_locals_dict,
    make_new, picodvi_framebuffer_make_new,
    buffer, common_hal_picodvi_framebuffer_get_buffer,
    protocol, &picodvi_framebuffer_proto
    );
