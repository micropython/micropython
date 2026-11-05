// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/objtype.h"

#include "shared-bindings/mipidsi/Display.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/framebufferio/FramebufferDisplay.h"

//| class Display:
//|     def __init__(
//|         self,
//|         bus: Bus,
//|         init_sequence: ReadableBuffer,
//|         *,
//|         width: int,
//|         height: int,
//|         hsync_pulse_width: int,
//|         hsync_back_porch: int,
//|         hsync_front_porch: int,
//|         vsync_pulse_width: int,
//|         vsync_back_porch: int,
//|         vsync_front_porch: int,
//|         pixel_clock_frequency: int,
//|         virtual_channel: int = 0,
//|         rotation: int = 0,
//|         color_depth: int = 16,
//|         backlight_pin: Optional[microcontroller.Pin] = None,
//|         brightness: float = 1.0,
//|         native_frames_per_second: int = 60,
//|         backlight_on_high: bool = True,
//|         low_power_blanking: bool = True,
//|     ) -> None:
//|         """Create a MIPI DSI Display object connected to the given bus.
//|
//|         This allocates a framebuffer and configures the DSI display to use the
//|         specified virtual channel for communication.
//|
//|         The framebuffer pixel format varies depending on color_depth:
//|
//|         * 16 - Each two bytes is a pixel in RGB565 format.
//|         * 24 - Each three bytes is a pixel in RGB888 format.
//|
//|         A Display is often used in conjunction with a
//|         `framebufferio.FramebufferDisplay`.
//|
//|         Drivers for DSI bridges can subclass Display. Set up the bridge before
//|         calling ``super().__init__()``, which starts DSI video.
//|
//|         :param Bus bus: the DSI bus to use
//|         :param ~circuitpython_typing.ReadableBuffer init_sequence: Byte-packed initialization sequence for the display
//|         :param int width: the width of the framebuffer in pixels
//|         :param int height: the height of the framebuffer in pixels
//|         :param int hsync_pulse_width: horizontal sync pulse width in pixel clocks
//|         :param int hsync_back_porch: horizontal back porch in pixel clocks
//|         :param int hsync_front_porch: horizontal front porch in pixel clocks
//|         :param int vsync_pulse_width: vertical sync pulse width in lines
//|         :param int vsync_back_porch: vertical back porch in lines
//|         :param int vsync_front_porch: vertical front porch in lines
//|         :param int pixel_clock_frequency: pixel clock frequency in Hz. The clock actually
//|             generated can differ; read it back from `pixel_clock_frequency`.
//|         :param int virtual_channel: the DSI virtual channel (0-3)
//|         :param int rotation: the rotation of the display in degrees clockwise (0, 90, 180, 270)
//|         :param int color_depth: the color depth of the framebuffer in bits (16 or 24)
//|         :param microcontroller.Pin backlight_pin: Pin connected to the display's backlight
//|         :param float brightness: Initial display brightness (0.0 to 1.0)
//|         :param int native_frames_per_second: Number of display refreshes per second
//|         :param bool backlight_on_high: If True, pulling the backlight pin high turns the backlight on
//|         :param bool low_power_blanking: If True, the DSI lanes may drop to low power mode
//|             during blanking. Set False to keep them in high speed mode, which bridges
//|             that regenerate the pixel clock from the DSI clock (such as the LT8912B) need.
//|         """
//|

static mp_obj_t mipidsi_display_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_bus, ARG_init_sequence, ARG_width, ARG_height, ARG_hsync_pulse_width, ARG_hsync_back_porch,
           ARG_hsync_front_porch, ARG_vsync_pulse_width, ARG_vsync_back_porch, ARG_vsync_front_porch,
           ARG_pixel_clock_frequency, ARG_virtual_channel, ARG_rotation,
           ARG_color_depth, ARG_backlight_pin, ARG_brightness, ARG_native_frames_per_second,
           ARG_backlight_on_high, ARG_low_power_blanking };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bus, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_init_sequence, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_width, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_height, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_hsync_pulse_width, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_hsync_back_porch, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_hsync_front_porch, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_vsync_pulse_width, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_vsync_back_porch, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_vsync_front_porch, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_pixel_clock_frequency, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_virtual_channel, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_rotation, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_color_depth, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 16} },
        { MP_QSTR_backlight_pin, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_none} },
        { MP_QSTR_brightness, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NEW_SMALL_INT(1)} },
        { MP_QSTR_native_frames_per_second, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 60} },
        { MP_QSTR_backlight_on_high, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = true} },
        { MP_QSTR_low_power_blanking, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = true} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mipidsi_display_obj_t *self = &allocate_display_bus_or_raise()->mipidsi;
    self->base.type = &mipidsi_display_type;

    mipidsi_bus_obj_t *bus = mp_arg_validate_type(args[ARG_bus].u_obj, &mipidsi_bus_type, MP_QSTR_bus);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_init_sequence].u_obj, &bufinfo, MP_BUFFER_READ);

    const mcu_pin_obj_t *backlight_pin =
        validate_obj_is_free_pin_or_none(args[ARG_backlight_pin].u_obj, MP_QSTR_backlight_pin);

    mp_float_t brightness = mp_obj_get_float(args[ARG_brightness].u_obj);

    mp_int_t rotation = args[ARG_rotation].u_int;
    if (rotation % 90 != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("Display rotation must be in 90 degree increments"));
    }

    mp_uint_t virtual_channel = (mp_uint_t)mp_arg_validate_int_range(args[ARG_virtual_channel].u_int, 0, 3, MP_QSTR_virtual_channel);
    mp_uint_t width = (mp_uint_t)mp_arg_validate_int_min(args[ARG_width].u_int, 0, MP_QSTR_width);
    mp_uint_t height = (mp_uint_t)mp_arg_validate_int_min(args[ARG_height].u_int, 0, MP_QSTR_height);
    mp_uint_t color_depth = args[ARG_color_depth].u_int;

    if (color_depth != 16 && color_depth != 24) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q"), MP_QSTR_color_depth);
    }

    common_hal_mipidsi_display_construct(self, bus, bufinfo.buf, bufinfo.len, virtual_channel, width, height,
        rotation, color_depth, MP_OBJ_TO_PTR(backlight_pin), brightness,
        args[ARG_native_frames_per_second].u_int,
        args[ARG_backlight_on_high].u_bool,
        args[ARG_hsync_pulse_width].u_int,
        args[ARG_hsync_back_porch].u_int,
        args[ARG_hsync_front_porch].u_int,
        args[ARG_vsync_pulse_width].u_int,
        args[ARG_vsync_back_porch].u_int,
        args[ARG_vsync_front_porch].u_int,
        args[ARG_pixel_clock_frequency].u_int,
        args[ARG_low_power_blanking].u_bool);

    return MP_OBJ_FROM_PTR(self);
}

// Helper to ensure we have the native super class instead of a subclass.
static mipidsi_display_obj_t *native_display(mp_obj_t display_obj) {
    mp_obj_t native_display = mp_obj_cast_to_native_base(display_obj, &mipidsi_display_type);
    mp_obj_assert_native_inited(native_display);
    return MP_OBJ_TO_PTR(native_display);
}

//|     def deinit(self) -> None:
//|         """Free the resources (pins, timers, etc.) associated with this
//|         `mipidsi.Display` instance.  After deinitialization, no further operations
//|         may be performed."""
//|         ...
//|
static mp_obj_t mipidsi_display_deinit(mp_obj_t self_in) {
    mipidsi_display_obj_t *self = native_display(self_in);
    common_hal_mipidsi_display_deinit(self);
    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_1(mipidsi_display_deinit_obj, mipidsi_display_deinit);

static void check_for_deinit(mipidsi_display_obj_t *self) {
    if (common_hal_mipidsi_display_deinited(self)) {
        raise_deinited_error();
    }
}

//|     width: int
//|     """The width of the framebuffer, in pixels."""
static mp_obj_t mipidsi_display_get_width(mp_obj_t self_in) {
    mipidsi_display_obj_t *self = native_display(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_mipidsi_display_get_width(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(mipidsi_display_get_width_obj, mipidsi_display_get_width);
MP_PROPERTY_GETTER(mipidsi_display_width_obj,
    (mp_obj_t)&mipidsi_display_get_width_obj);

//|     height: int
//|     """The height of the framebuffer, in pixels."""
//|
//|
static mp_obj_t mipidsi_display_get_height(mp_obj_t self_in) {
    mipidsi_display_obj_t *self = native_display(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_mipidsi_display_get_height(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(mipidsi_display_get_height_obj, mipidsi_display_get_height);

MP_PROPERTY_GETTER(mipidsi_display_height_obj,
    (mp_obj_t)&mipidsi_display_get_height_obj);

//|     color_depth: int
//|     """The color depth of the framebuffer."""
static mp_obj_t mipidsi_display_get_color_depth(mp_obj_t self_in) {
    mipidsi_display_obj_t *self = native_display(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_mipidsi_display_get_color_depth(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(mipidsi_display_get_color_depth_obj, mipidsi_display_get_color_depth);
MP_PROPERTY_GETTER(mipidsi_display_color_depth_obj,
    (mp_obj_t)&mipidsi_display_get_color_depth_obj);

//|     pixel_clock_frequency: int
//|     """The pixel clock frequency the DSI host actually generates, in Hz. This
//|     can differ from the requested frequency when the clock source cannot divide
//|     down to it exactly."""
static mp_obj_t mipidsi_display_get_pixel_clock_frequency(mp_obj_t self_in) {
    mipidsi_display_obj_t *self = native_display(self_in);
    check_for_deinit(self);
    return mp_obj_new_int_from_uint(common_hal_mipidsi_display_get_pixel_clock_frequency(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(mipidsi_display_get_pixel_clock_frequency_obj, mipidsi_display_get_pixel_clock_frequency);
MP_PROPERTY_GETTER(mipidsi_display_pixel_clock_frequency_obj,
    (mp_obj_t)&mipidsi_display_get_pixel_clock_frequency_obj);


static const mp_rom_map_elem_t mipidsi_display_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&mipidsi_display_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mipidsi_display_deinit_obj) },

    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&mipidsi_display_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_PTR(&mipidsi_display_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_color_depth), MP_ROM_PTR(&mipidsi_display_color_depth_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel_clock_frequency), MP_ROM_PTR(&mipidsi_display_pixel_clock_frequency_obj) },
};
static MP_DEFINE_CONST_DICT(mipidsi_display_locals_dict, mipidsi_display_locals_dict_table);

static void mipidsi_display_get_bufinfo(mp_obj_t self_in, mp_buffer_info_t *bufinfo) {
    common_hal_mipidsi_display_get_buffer(self_in, bufinfo, 0);
}

static float mipidsi_display_get_brightness_proto(mp_obj_t self_in) {
    return common_hal_mipidsi_display_get_brightness(self_in);
}

static bool mipidsi_display_set_brightness_proto(mp_obj_t self_in, mp_float_t value) {
    common_hal_mipidsi_display_set_brightness(self_in, value);
    return true;
}

// These versions exist so that the prototype matches the protocol,
// avoiding a type cast that can hide errors
static void mipidsi_display_swapbuffers(mp_obj_t self_in, uint8_t *dirty_row_bitmap) {
    common_hal_mipidsi_display_refresh_rows(self_in, dirty_row_bitmap);
}

static void mipidsi_display_deinit_proto(mp_obj_t self_in) {
    common_hal_mipidsi_display_deinit(self_in);
}

static int mipidsi_display_get_width_proto(mp_obj_t self_in) {
    return common_hal_mipidsi_display_get_width(self_in);
}

static int mipidsi_display_get_height_proto(mp_obj_t self_in) {
    return common_hal_mipidsi_display_get_height(self_in);
}

static int mipidsi_display_get_color_depth_proto(mp_obj_t self_in) {
    return common_hal_mipidsi_display_get_color_depth(self_in);
}

static bool mipidsi_display_get_grayscale_proto(mp_obj_t self_in) {
    return common_hal_mipidsi_display_get_grayscale(self_in);
}

static int mipidsi_display_get_bytes_per_cell_proto(mp_obj_t self_in) {
    return 1;
}

static int mipidsi_display_get_native_frames_per_second_proto(mp_obj_t self_in) {
    return common_hal_mipidsi_display_get_native_frames_per_second(self_in);
}

static bool mipidsi_display_get_pixels_in_byte_share_row_proto(mp_obj_t self_in) {
    return true;
}

static int mipidsi_display_get_row_stride_proto(mp_obj_t self_in) {
    return common_hal_mipidsi_display_get_row_stride(self_in);
}

static const framebuffer_p_t mipidsi_display_proto = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_framebuffer)
    .get_bufinfo = mipidsi_display_get_bufinfo,
    .set_brightness = mipidsi_display_set_brightness_proto,
    .get_brightness = mipidsi_display_get_brightness_proto,
    .get_width = mipidsi_display_get_width_proto,
    .get_height = mipidsi_display_get_height_proto,
    .get_color_depth = mipidsi_display_get_color_depth_proto,
    .get_grayscale = mipidsi_display_get_grayscale_proto,
    .get_row_stride = mipidsi_display_get_row_stride_proto,
    .get_bytes_per_cell = mipidsi_display_get_bytes_per_cell_proto,
    .get_native_frames_per_second = mipidsi_display_get_native_frames_per_second_proto,
    .get_pixels_in_byte_share_row = mipidsi_display_get_pixels_in_byte_share_row_proto,
    .swapbuffers = mipidsi_display_swapbuffers,
    .deinit = mipidsi_display_deinit_proto,
};

MP_DEFINE_CONST_OBJ_TYPE(
    mipidsi_display_type,
    MP_QSTR_Display,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    locals_dict, &mipidsi_display_locals_dict,
    make_new, mipidsi_display_make_new,
    buffer, common_hal_mipidsi_display_get_buffer,
    protocol, &mipidsi_display_proto
    );
