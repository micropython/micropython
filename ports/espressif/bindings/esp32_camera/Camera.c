/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Jeff Epler for Adafruit Industries
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

#include <math.h>

#include "py/mphal.h"
#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"

#include "bindings/esp32_camera/__init__.h"
#include "bindings/esp32_camera/Camera.h"
#include "common-hal/esp32_camera/Camera.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"
#include "esp_camera.h"
#include "sensor.h"

//| class Camera:
//|     def __init__(
//|         self,
//|         *,
//|         data_pins: List[microcontroller.Pin],
//|         pixel_clock: microcontroller.Pin,
//|         vsync: microcontroller.Pin,
//|         href: microcontroller.Pin,
//|         i2c: busio.I2C,
//|         external_clock_pin: microcontroller.Pin,
//|         external_clock_frequency: int,
//|         powerdown_pin: Optional[microcontroller.Pin] = None,
//|         reset_pin: Optional[microcontroller.Pin] = None,
//|         pixel_format: PixelFormat=PixelFormat.RGB565,
//|         frame_size: FrameSize=FrameSize.QQVGA,
//|         jpeg_quality: int=15,
//|         double_buffered: bool = True,
//|         grab_mode: GrabMode = GrabMode.WhenEmpty,
//|     ):
//|     """
//|     Configure and initialize a camera with the given properties
//|
//|     This driver requires that the ``CIRCUITPY_RESERVED_PSRAM`` in ``/.env`` be large enough to hold the camera frambuffer(s). Generally, boards with built-in cameras will have a default setting that is large enough. If the constructor raises a MemoryError, this probably indicates the setting is too small and should be increased.
//|
//|     :param data_pins: The 8 data data_pins used for image data transfer from the camera module, least significant bit first
//|     :param pixel_clock: The pixel clock output from the camera module
//|     :param vsync: The vertical sync pulse output from the camera module
//|     :param href: The horizontal reference output from the camera module
//|     :param i2c: The I2C bus connected to the camera module
//|     :param external_clock_frequency: The frequency generated on the external clock pin
//|     :param external_clock_pin: The pin on which to generate the external clock
//|     :param powerdown_pin: The powerdown input to the camera module
//|     :param reset_pin: The reset input to the camera module
//|     :param pixel_format: The pixel format of the captured image
//|     :param frame_size: The size of captured image
//|     :param jpeg_quality: For `PixelFormat.JPEG`, the quality. Higher numbers increase quality. If the quality is too high, the JPEG data will be larger than the availalble buffer size and the image will be unusable or truncated. The exact range of appropriate values depends on the sensor and must be determined empirically.
//|     :param framebuffer_count: The number of framebuffers
//|     :param grab_mode: When to grab a new frame
//|     """
//|
STATIC mp_obj_t esp32_camera_camera_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_data_pins, ARG_pixel_clock_pin, ARG_vsync_pin, ARG_href_pin, ARG_i2c, ARG_external_clock_pin, ARG_external_clock_frequency, ARG_powerdown_pin, ARG_reset_pin, ARG_pixel_format, ARG_frame_size, ARG_jpeg_quality, ARG_framebuffer_count, ARG_grab_mode, NUM_ARGS };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_data_pins, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_pixel_clock_pin, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_vsync_pin, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_href_pin, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_i2c, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_external_clock_pin, MP_ARG_OBJ | MP_ARG_KW_ONLY },
        { MP_QSTR_external_clock_frequency, MP_ARG_INT | MP_ARG_REQUIRED },
        { MP_QSTR_powerdown_pin, MP_ARG_OBJ | MP_ARG_KW_ONLY, { .u_obj = MP_ROM_NONE } },
        { MP_QSTR_reset_pin, MP_ARG_OBJ | MP_ARG_KW_ONLY, { .u_obj = MP_ROM_NONE } },
        { MP_QSTR_pixel_format, MP_ARG_OBJ | MP_ARG_KW_ONLY, { .u_obj = MP_ROM_PTR((void *)&pixel_format_RGB565_obj) } },
        { MP_QSTR_frame_size, MP_ARG_OBJ | MP_ARG_KW_ONLY, { .u_obj = MP_ROM_PTR((void *)&frame_size_QQVGA_obj) } },
        { MP_QSTR_jpeg_quality, MP_ARG_INT | MP_ARG_KW_ONLY, { .u_int = 15 } },
        { MP_QSTR_framebuffer_count, MP_ARG_INT | MP_ARG_KW_ONLY, { .u_int = 1 } },
        { MP_QSTR_grab_mode, MP_ARG_OBJ | MP_ARG_KW_ONLY, { .u_obj = MP_ROM_PTR((void *)&grab_mode_WHEN_EMPTY_obj) } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    MP_STATIC_ASSERT(MP_ARRAY_SIZE(allowed_args) == NUM_ARGS);
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint8_t data_pins[8];
    uint8_t data_pin_count;
    validate_pins(MP_QSTR_data_pins, data_pins, MP_ARRAY_SIZE(data_pins), args[ARG_data_pins].u_obj, &data_pin_count);
    mp_arg_validate_length(data_pin_count, 8, MP_QSTR_data_pins);

    const mcu_pin_obj_t *pixel_clock_pin = validate_obj_is_free_pin(args[ARG_pixel_clock_pin].u_obj);
    const mcu_pin_obj_t *vsync_pin = validate_obj_is_free_pin(args[ARG_vsync_pin].u_obj);
    const mcu_pin_obj_t *href_pin = validate_obj_is_free_pin(args[ARG_href_pin].u_obj);
    const busio_i2c_obj_t *i2c = MP_OBJ_TO_PTR(mp_arg_validate_type(args[ARG_i2c].u_obj, &busio_i2c_type, MP_QSTR_i2c));
    const mcu_pin_obj_t *external_clock_pin = validate_obj_is_free_pin(args[ARG_external_clock_pin].u_obj);
    const mcu_pin_obj_t *powerdown_pin = validate_obj_is_free_pin_or_none(args[ARG_powerdown_pin].u_obj);
    const mcu_pin_obj_t *reset_pin = validate_obj_is_free_pin_or_none(args[ARG_reset_pin].u_obj);
    const mp_int_t external_clock_frequency = mp_arg_validate_int_range(args[ARG_external_clock_frequency].u_int, 0, 40000000, MP_QSTR_clock_frequency);

    camera_grab_mode_t grab_mode = validate_grab_mode(args[ARG_grab_mode].u_obj, MP_QSTR_grab_mode);
    framesize_t frame_size = validate_frame_size(args[ARG_frame_size].u_obj, MP_QSTR_frame_size);
    pixformat_t pixel_format = validate_pixel_format(args[ARG_pixel_format].u_obj, MP_QSTR_pixel_format);
    mp_int_t jpeg_quality = mp_arg_validate_int_range(args[ARG_jpeg_quality].u_int, 2, 55, MP_QSTR_jpeg_quality);
    mp_int_t framebuffer_count = mp_arg_validate_int_range(args[ARG_framebuffer_count].u_int, 1, 2, MP_QSTR_framebuffer_count);

    esp32_camera_camera_obj_t *self = m_new_obj(esp32_camera_camera_obj_t);
    self->base.type = &esp32_camera_camera_type;
    common_hal_esp32_camera_camera_construct(
        self,
        data_pins,
        external_clock_pin,
        pixel_clock_pin,
        vsync_pin,
        href_pin,
        powerdown_pin,
        reset_pin,
        i2c,
        external_clock_frequency,
        pixel_format,
        frame_size,
        jpeg_quality,
        framebuffer_count,
        grab_mode);
    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Deinitialises the camera and releases all memory resources for reuse."""
//|         ...
//|
STATIC mp_obj_t esp32_camera_camera_deinit(mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_esp32_camera_camera_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_deinit_obj, esp32_camera_camera_deinit);

STATIC void check_for_deinit(esp32_camera_camera_obj_t *self) {
    if (common_hal_esp32_camera_camera_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def __enter__(self) -> Camera:
//|         """No-op used by Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
STATIC mp_obj_t esp32_camera_camera_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_esp32_camera_camera_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp32_camera_camera___exit___obj, 4, 4, esp32_camera_camera_obj___exit__);

//|     frame_available: bool
//|     """True if a frame is available, False otherwise"""

STATIC mp_obj_t esp32_camera_camera_frame_available_get(const mp_obj_t self_in) {
    return mp_obj_new_bool(false);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_frame_available_get_obj, esp32_camera_camera_frame_available_get);

MP_PROPERTY_GETTER(esp32_camera_camera_frame_available_obj,
    (mp_obj_t)&esp32_camera_camera_frame_available_get_obj);

//|     def take(timeout: Optional[float]=0.25) -> Optional[ReadableBuffer]:
//|         """Record a frame. Wait up to 'timeout' seconds for a frame to be captured."""
//|
STATIC mp_obj_t esp32_camera_camera_take(size_t n_args, const mp_obj_t *args) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_float_t timeout = n_args < 2 ? MICROPY_FLOAT_CONST(0.25) : mp_obj_get_float(args[1]);
    check_for_deinit(self);
    camera_fb_t *result = common_hal_esp32_camera_camera_take(self, (int)MICROPY_FLOAT_C_FUN(round)(timeout * 1000));
    if (!result) {
        return mp_const_none;
    }
    return mp_obj_new_memoryview('b', result->len, result->buf);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp32_camera_camera_take_obj, 1, 2, esp32_camera_camera_take);

//|     pixel_format: PixelFormat
//|     """The pixel format of the camera"""
//|
//|     frame_size: FrameSize
//|     """The size of the captured image"""
//|
//|     contrast: int
//|     """The contrast of the sensor"""

STATIC const mp_rom_map_elem_t esp32_camera_camera_locals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_take), MP_ROM_PTR(&esp32_camera_camera_take_obj) },
    { MP_ROM_QSTR(MP_QSTR_frame_available), MP_ROM_PTR(&esp32_camera_camera_frame_available_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&esp32_camera_camera_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&esp32_camera_camera___exit___obj) },
};

STATIC MP_DEFINE_CONST_DICT(esp32_camera_camera_locals_dict, esp32_camera_camera_locals_table);

const mp_obj_type_t esp32_camera_camera_type = {
    .base = { &mp_type_type },
    .name = MP_QSTR_Camera,
    .make_new = esp32_camera_camera_make_new,
    .locals_dict = (mp_obj_t)&esp32_camera_camera_locals_dict,
};
