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

#include "shared-bindings/displayio/Bitmap.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"
#include "esp_camera.h"
#include "sensor.h"

//| class Camera:
//|     def __init__(
//|         self,
//|         *,
//|         data_pins: List[microcontroller.Pin],
//|         pixel_clock_pin: microcontroller.Pin,
//|         vsync_pin: microcontroller.Pin,
//|         href_pin: microcontroller.Pin,
//|         i2c: busio.I2C,
//|         external_clock_pin: Optional[microcontroller.Pin] = None,
//|         external_clock_frequency: int = 20_000_000,
//|         powerdown_pin: Optional[microcontroller.Pin] = None,
//|         reset_pin: Optional[microcontroller.Pin] = None,
//|         pixel_format: PixelFormat = PixelFormat.RGB565,
//|         frame_size: FrameSize = FrameSize.QQVGA,
//|         jpeg_quality: int = 15,
//|         framebuffer_count: int = 1,
//|         grab_mode: GrabMode = GrabMode.WHEN_EMPTY,
//|     ) -> None:
//|         """
//|         Configure and initialize a camera with the given properties
//|
//|         This driver requires that the ``CIRCUITPY_RESERVED_PSRAM`` in ``settings.toml`` be large enough to hold the camera frambuffer(s). Generally, boards with built-in cameras will have a default setting that is large enough. If the constructor raises a MemoryError or an IDFError, this probably indicates the setting is too small and should be increased.
//|
//|
//|         .. important::
//|
//|             Not all supported sensors have all
//|             of the properties listed below. For instance, the
//|             OV5640 supports `denoise`, but the
//|             OV2640 does not. The underlying esp32-camera
//|             library does not provide a reliable API to check
//|             which settings are supported. CircuitPython makes
//|             a best effort to determine when an unsupported
//|             property is set and will raise an exception in
//|             that case.
//|
//|         :param data_pins: The 8 data data_pins used for image data transfer from the camera module, least significant bit first
//|         :param pixel_clock_pin: The pixel clock output from the camera module
//|         :param vsync_pin: The vertical sync pulse output from the camera module
//|         :param href_pin: The horizontal reference output from the camera module
//|         :param i2c: The I2C bus connected to the camera module
//|         :param external_clock_pin: The pin on which to generate the external clock
//|         :param external_clock_frequency: The frequency generated on the external clock pin
//|         :param powerdown_pin: The powerdown input to the camera module
//|         :param reset_pin: The reset input to the camera module
//|         :param pixel_format: The pixel format of the captured image
//|         :param frame_size: The size of captured image
//|         :param jpeg_quality: For `PixelFormat.JPEG`, the quality. Higher numbers increase quality. If the quality is too high, the JPEG data will be larger than the availalble buffer size and the image will be unusable or truncated. The exact range of appropriate values depends on the sensor and must be determined empirically.
//|         :param framebuffer_count: The number of framebuffers (1 for single-buffered and 2 for double-buffered)
//|         :param grab_mode: When to grab a new frame
//|         """
STATIC mp_obj_t esp32_camera_camera_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_data_pins, ARG_pixel_clock_pin, ARG_vsync_pin, ARG_href_pin, ARG_i2c, ARG_external_clock_pin, ARG_external_clock_frequency, ARG_powerdown_pin, ARG_reset_pin, ARG_pixel_format, ARG_frame_size, ARG_jpeg_quality, ARG_framebuffer_count, ARG_grab_mode, NUM_ARGS };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_data_pins, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_pixel_clock_pin, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_vsync_pin, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_href_pin, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_i2c, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_external_clock_pin, MP_ARG_OBJ | MP_ARG_KW_ONLY, { .u_obj = MP_ROM_NONE } },
        { MP_QSTR_external_clock_frequency, MP_ARG_INT | MP_ARG_KW_ONLY, { .u_int = 20000000L } },
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

    const mcu_pin_obj_t *pixel_clock_pin =
        validate_obj_is_free_pin(args[ARG_pixel_clock_pin].u_obj, MP_QSTR_pixel_clock_pin);
    const mcu_pin_obj_t *vsync_pin =
        validate_obj_is_free_pin(args[ARG_vsync_pin].u_obj, MP_QSTR_vsync_pin);
    const mcu_pin_obj_t *href_pin =
        validate_obj_is_free_pin(args[ARG_href_pin].u_obj, MP_QSTR_href_pin);
    busio_i2c_obj_t *i2c = MP_OBJ_TO_PTR(mp_arg_validate_type(args[ARG_i2c].u_obj, &busio_i2c_type, MP_QSTR_i2c));
    const mcu_pin_obj_t *external_clock_pin =
        validate_obj_is_free_pin_or_none(args[ARG_external_clock_pin].u_obj, MP_QSTR_external_clock_pin);
    const mcu_pin_obj_t *powerdown_pin =
        validate_obj_is_free_pin_or_none(args[ARG_powerdown_pin].u_obj, MP_QSTR_powerdown_pin);
    const mcu_pin_obj_t *reset_pin =
        validate_obj_is_free_pin_or_none(args[ARG_reset_pin].u_obj, MP_QSTR_reset_pin);
    const mp_int_t external_clock_frequency =
        mp_arg_validate_int_range(args[ARG_external_clock_frequency].u_int, 0, 40000000, MP_QSTR_external_clock_frequency);

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
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
STATIC mp_obj_t esp32_camera_camera_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    return esp32_camera_camera_deinit(args[0]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp32_camera_camera___exit___obj, 4, 4, esp32_camera_camera_obj___exit__);

//|     frame_available: bool
//|     """True if a frame is available, False otherwise"""

STATIC mp_obj_t esp32_camera_camera_frame_available_get(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(esp_camera_fb_available());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_frame_available_get_obj, esp32_camera_camera_frame_available_get);

MP_PROPERTY_GETTER(esp32_camera_camera_frame_available_obj,
    (mp_obj_t)&esp32_camera_camera_frame_available_get_obj);

//|     def take(
//|         self, timeout: Optional[float] = 0.25
//|     ) -> Optional[displayio.Bitmap | ReadableBuffer]:
//|         """Record a frame. Wait up to 'timeout' seconds for a frame to be captured.
//|
//|         In the case of timeout, `None` is returned.
//|         If `pixel_format` is `PixelFormat.JPEG`, the returned value is a read-only `memoryview`.
//|         Otherwise, the returned value is a read-only `displayio.Bitmap`.
//|         """
STATIC mp_obj_t esp32_camera_camera_take(size_t n_args, const mp_obj_t *args) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_float_t timeout = n_args < 2 ? MICROPY_FLOAT_CONST(0.25) : mp_obj_get_float(args[1]);
    check_for_deinit(self);
    camera_fb_t *result = common_hal_esp32_camera_camera_take(self, (int)MICROPY_FLOAT_C_FUN(round)(timeout * 1000));
    if (!result) {
        return mp_const_none;
    }
    pixformat_t format = common_hal_esp32_camera_camera_get_pixel_format(self);
    if (format == PIXFORMAT_JPEG) {
        return mp_obj_new_memoryview('b', result->len, result->buf);
    } else {
        int width = common_hal_esp32_camera_camera_get_width(self);
        int height = common_hal_esp32_camera_camera_get_height(self);
        displayio_bitmap_t *bitmap = m_new_obj(displayio_bitmap_t);
        bitmap->base.type = &displayio_bitmap_type;
        common_hal_displayio_bitmap_construct_from_buffer(bitmap, width, height, (format == PIXFORMAT_RGB565) ? 16 : 8, (uint32_t *)(void *)result->buf, true);
        return bitmap;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp32_camera_camera_take_obj, 1, 2, esp32_camera_camera_take);


//|     def reconfigure(
//|         self,
//|         frame_size: Optional[FrameSize] = None,
//|         pixel_format: Optional[PixelFormat] = None,
//|         grab_mode: Optional[GrabMode] = None,
//|         framebuffer_count: Optional[int] = None,
//|     ) -> None:
//|         """Change multiple related camera settings simultaneously
//|
//|         Because these settings interact in complex ways, and take longer than
//|         the other properties to set, they are set together in a single function call.
//|
//|         If an argument is unspecified or None, then the setting is unchanged."""

STATIC mp_obj_t esp32_camera_camera_reconfigure(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);

    enum { ARG_frame_size, ARG_pixel_format, ARG_grab_mode, ARG_framebuffer_count };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_frame_size, MP_ARG_OBJ, {.u_obj = MP_ROM_NONE} },
        { MP_QSTR_pixel_format, MP_ARG_OBJ, {.u_obj = MP_ROM_NONE} },
        { MP_QSTR_grab_mode, MP_ARG_OBJ, {.u_obj = MP_ROM_NONE} },
        { MP_QSTR_framebuffer_count, MP_ARG_OBJ, {.u_obj = MP_ROM_NONE} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    framesize_t frame_size =
        args[ARG_frame_size].u_obj != MP_ROM_NONE
        ?  validate_frame_size(args[ARG_frame_size].u_obj, MP_QSTR_frame_size)
        : common_hal_esp32_camera_camera_get_frame_size(self);
    pixformat_t pixel_format =
        args[ARG_pixel_format].u_obj != MP_ROM_NONE
        ?  validate_pixel_format(args[ARG_pixel_format].u_obj, MP_QSTR_pixel_format)
        : common_hal_esp32_camera_camera_get_pixel_format(self);
    camera_grab_mode_t grab_mode =
        args[ARG_grab_mode].u_obj != MP_ROM_NONE
        ?  validate_grab_mode(args[ARG_grab_mode].u_obj, MP_QSTR_grab_mode)
        : common_hal_esp32_camera_camera_get_grab_mode(self);
    bool framebuffer_count =
        args[ARG_framebuffer_count].u_obj != MP_ROM_NONE
        ?  mp_obj_get_int(args[ARG_framebuffer_count].u_obj)
        : common_hal_esp32_camera_camera_get_framebuffer_count(self);

    common_hal_esp32_camera_camera_reconfigure(self, frame_size, pixel_format, grab_mode, framebuffer_count);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(esp32_camera_camera_reconfigure_obj, 1, esp32_camera_camera_reconfigure);

//|     pixel_format: PixelFormat
//|     """The pixel format of captured frames"""

STATIC mp_obj_t esp32_camera_camera_get_pixel_format(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return cp_enum_find(&esp32_camera_pixel_format_type, common_hal_esp32_camera_camera_get_pixel_format(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_pixel_format_obj, esp32_camera_camera_get_pixel_format);

MP_PROPERTY_GETTER(esp32_camera_camera_pixel_format_obj,
    (mp_obj_t)&esp32_camera_camera_get_pixel_format_obj);


//|     frame_size: FrameSize
//|     """The size of captured frames"""

STATIC mp_obj_t esp32_camera_camera_get_frame_size(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return cp_enum_find(&esp32_camera_frame_size_type, common_hal_esp32_camera_camera_get_frame_size(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_frame_size_obj, esp32_camera_camera_get_frame_size);

MP_PROPERTY_GETTER(esp32_camera_camera_frame_size_obj,
    (mp_obj_t)&esp32_camera_camera_get_frame_size_obj);

//|     contrast: int
//|     """The sensor contrast.  Positive values increase contrast, negative values lower it. The total range is device-specific but is often from -2 to +2 inclusive."""

STATIC mp_obj_t esp32_camera_camera_get_contrast(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_esp32_camera_camera_get_contrast(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_contrast_obj, esp32_camera_camera_get_contrast);

STATIC mp_obj_t esp32_camera_camera_set_contrast(const mp_obj_t self_in, const mp_obj_t arg) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_esp32_camera_camera_set_contrast(self, mp_obj_get_int(arg));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32_camera_camera_set_contrast_obj, esp32_camera_camera_set_contrast);
MP_PROPERTY_GETSET(esp32_camera_camera_contrast_obj,
    (mp_obj_t)&esp32_camera_camera_get_contrast_obj,
    (mp_obj_t)&esp32_camera_camera_set_contrast_obj);

//|     brightness: int
//|     """The sensor brightness.  Positive values increase brightness, negative values lower it. The total range is device-specific but is often from -2 to +2 inclusive."""

STATIC mp_obj_t esp32_camera_camera_get_brightness(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_esp32_camera_camera_get_brightness(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_brightness_obj, esp32_camera_camera_get_brightness);

STATIC mp_obj_t esp32_camera_camera_set_brightness(const mp_obj_t self_in, const mp_obj_t arg) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_esp32_camera_camera_set_brightness(self, mp_obj_get_int(arg));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32_camera_camera_set_brightness_obj, esp32_camera_camera_set_brightness);
MP_PROPERTY_GETSET(esp32_camera_camera_brightness_obj,
    (mp_obj_t)&esp32_camera_camera_get_brightness_obj,
    (mp_obj_t)&esp32_camera_camera_set_brightness_obj);

//|     saturation: int
//|     """The sensor saturation.  Positive values increase saturation (more vibrant colors), negative values lower it (more muted colors).  The total range is device-specific but the value is often from -2 to +2 inclusive."""

STATIC mp_obj_t esp32_camera_camera_get_saturation(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_esp32_camera_camera_get_saturation(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_saturation_obj, esp32_camera_camera_get_saturation);

STATIC mp_obj_t esp32_camera_camera_set_saturation(const mp_obj_t self_in, const mp_obj_t arg) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_esp32_camera_camera_set_saturation(self, mp_obj_get_int(arg));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32_camera_camera_set_saturation_obj, esp32_camera_camera_set_saturation);
MP_PROPERTY_GETSET(esp32_camera_camera_saturation_obj,
    (mp_obj_t)&esp32_camera_camera_get_saturation_obj,
    (mp_obj_t)&esp32_camera_camera_set_saturation_obj);

//|     sharpness: int
//|     """The sensor sharpness.  Positive values increase sharpness (more defined edges), negative values lower it (softer edges).  The total range is device-specific but the value is often from -2 to +2 inclusive."""

STATIC mp_obj_t esp32_camera_camera_get_sharpness(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_esp32_camera_camera_get_sharpness(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_sharpness_obj, esp32_camera_camera_get_sharpness);

STATIC mp_obj_t esp32_camera_camera_set_sharpness(const mp_obj_t self_in, const mp_obj_t arg) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_esp32_camera_camera_set_sharpness(self, mp_obj_get_int(arg));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32_camera_camera_set_sharpness_obj, esp32_camera_camera_set_sharpness);
MP_PROPERTY_GETSET(esp32_camera_camera_sharpness_obj,
    (mp_obj_t)&esp32_camera_camera_get_sharpness_obj,
    (mp_obj_t)&esp32_camera_camera_set_sharpness_obj);

//|     denoise: int
//|     """The sensor 'denoise' setting.  Any camera sensor has inherent 'noise', especially in low brightness environments. Software algorithms can decrease noise at the expense of fine detail.  A larger value increases the amount of software noise removal.  The total range is device-specific but the value is often from 0 to 10."""

STATIC mp_obj_t esp32_camera_camera_get_denoise(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_esp32_camera_camera_get_denoise(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_denoise_obj, esp32_camera_camera_get_denoise);

STATIC mp_obj_t esp32_camera_camera_set_denoise(const mp_obj_t self_in, const mp_obj_t arg) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_esp32_camera_camera_set_denoise(self, mp_obj_get_int(arg));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32_camera_camera_set_denoise_obj, esp32_camera_camera_set_denoise);
MP_PROPERTY_GETSET(esp32_camera_camera_denoise_obj,
    (mp_obj_t)&esp32_camera_camera_get_denoise_obj,
    (mp_obj_t)&esp32_camera_camera_set_denoise_obj);

//|     gain_ceiling: GainCeiling
//|     """The sensor 'gain ceiling' setting. "Gain" is an analog multiplier applied to the raw sensor data.  The 'ceiling' is the maximum gain value that the sensor will use. A higher gain means that the sensor has a greater response to light, but also makes sensor noise more visible."""

STATIC mp_obj_t esp32_camera_camera_get_gain_ceiling(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return cp_enum_find(&esp32_camera_gain_ceiling_type, common_hal_esp32_camera_camera_get_gainceiling(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_gain_ceiling_obj, esp32_camera_camera_get_gain_ceiling);

STATIC mp_obj_t esp32_camera_camera_set_gain_ceiling(const mp_obj_t self_in, const mp_obj_t arg) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_esp32_camera_camera_set_gainceiling(self, validate_gain_ceiling(arg, MP_QSTR_gain_ceiling));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32_camera_camera_set_gain_ceiling_obj, esp32_camera_camera_set_gain_ceiling);
MP_PROPERTY_GETSET(esp32_camera_camera_gain_ceiling_obj,
    (mp_obj_t)&esp32_camera_camera_get_gain_ceiling_obj,
    (mp_obj_t)&esp32_camera_camera_set_gain_ceiling_obj);

//|     quality: int
//|     """The 'quality' setting when capturing JPEG images.  This is similar to the quality setting when exporting a jpeg image from photo editing software.  Typical values range from 5 to 40, with higher numbers leading to larger image sizes and better overall image quality. However, when the quality is set to a high number, the total size of the JPEG data can exceed the size of an internal buffer, causing image capture to fail."""

STATIC mp_obj_t esp32_camera_camera_get_quality(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_esp32_camera_camera_get_quality(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_quality_obj, esp32_camera_camera_get_quality);

STATIC mp_obj_t esp32_camera_camera_set_quality(const mp_obj_t self_in, const mp_obj_t arg) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_esp32_camera_camera_set_quality(self, mp_obj_get_int(arg));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32_camera_camera_set_quality_obj, esp32_camera_camera_set_quality);
MP_PROPERTY_GETSET(esp32_camera_camera_quality_obj,
    (mp_obj_t)&esp32_camera_camera_get_quality_obj,
    (mp_obj_t)&esp32_camera_camera_set_quality_obj);

//|     colorbar: bool
//|     """When `True`, a test pattern image is captured and the real sensor data is not used."""

STATIC mp_obj_t esp32_camera_camera_get_colorbar(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_esp32_camera_camera_get_colorbar(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_colorbar_obj, esp32_camera_camera_get_colorbar);

STATIC mp_obj_t esp32_camera_camera_set_colorbar(const mp_obj_t self_in, const mp_obj_t arg) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_esp32_camera_camera_set_colorbar(self, mp_obj_is_true(arg));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32_camera_camera_set_colorbar_obj, esp32_camera_camera_set_colorbar);
MP_PROPERTY_GETSET(esp32_camera_camera_colorbar_obj,
    (mp_obj_t)&esp32_camera_camera_get_colorbar_obj,
    (mp_obj_t)&esp32_camera_camera_set_colorbar_obj);

//|     whitebal: bool
//|     """When `True`, the camera attempts to automatically control white balance.  When `False`, the `wb_mode` setting is used instead."""

STATIC mp_obj_t esp32_camera_camera_get_whitebal(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_esp32_camera_camera_get_whitebal(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_whitebal_obj, esp32_camera_camera_get_whitebal);

STATIC mp_obj_t esp32_camera_camera_set_whitebal(const mp_obj_t self_in, const mp_obj_t arg) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_esp32_camera_camera_set_whitebal(self, mp_obj_is_true(arg));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32_camera_camera_set_whitebal_obj, esp32_camera_camera_set_whitebal);
MP_PROPERTY_GETSET(esp32_camera_camera_whitebal_obj,
    (mp_obj_t)&esp32_camera_camera_get_whitebal_obj,
    (mp_obj_t)&esp32_camera_camera_set_whitebal_obj);

//|     gain_ctrl: bool
//|     """When `True`, the camera attempts to automatically control the sensor gain, up to the value in the `gain_ceiling` property.  When `False`, the `agc_gain` setting is used instead."""

STATIC mp_obj_t esp32_camera_camera_get_gain_ctrl(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_esp32_camera_camera_get_gain_ctrl(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_gain_ctrl_obj, esp32_camera_camera_get_gain_ctrl);

STATIC mp_obj_t esp32_camera_camera_set_gain_ctrl(const mp_obj_t self_in, const mp_obj_t arg) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_esp32_camera_camera_set_gain_ctrl(self, mp_obj_is_true(arg));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32_camera_camera_set_gain_ctrl_obj, esp32_camera_camera_set_gain_ctrl);
MP_PROPERTY_GETSET(esp32_camera_camera_gain_ctrl_obj,
    (mp_obj_t)&esp32_camera_camera_get_gain_ctrl_obj,
    (mp_obj_t)&esp32_camera_camera_set_gain_ctrl_obj);

//|     exposure_ctrl: bool
//|     """When `True` the camera attempts to automatically control the exposure. When `False`, the `aec_value` setting is used instead."""

STATIC mp_obj_t esp32_camera_camera_get_exposure_ctrl(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_esp32_camera_camera_get_exposure_ctrl(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_exposure_ctrl_obj, esp32_camera_camera_get_exposure_ctrl);

STATIC mp_obj_t esp32_camera_camera_set_exposure_ctrl(const mp_obj_t self_in, const mp_obj_t arg) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_esp32_camera_camera_set_exposure_ctrl(self, mp_obj_is_true(arg));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32_camera_camera_set_exposure_ctrl_obj, esp32_camera_camera_set_exposure_ctrl);
MP_PROPERTY_GETSET(esp32_camera_camera_exposure_ctrl_obj,
    (mp_obj_t)&esp32_camera_camera_get_exposure_ctrl_obj,
    (mp_obj_t)&esp32_camera_camera_set_exposure_ctrl_obj);

//|     hmirror: bool
//|     """When `True` the camera image is mirrored left-to-right"""

STATIC mp_obj_t esp32_camera_camera_get_hmirror(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_esp32_camera_camera_get_hmirror(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_hmirror_obj, esp32_camera_camera_get_hmirror);

STATIC mp_obj_t esp32_camera_camera_set_hmirror(const mp_obj_t self_in, const mp_obj_t arg) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_esp32_camera_camera_set_hmirror(self, mp_obj_is_true(arg));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32_camera_camera_set_hmirror_obj, esp32_camera_camera_set_hmirror);
MP_PROPERTY_GETSET(esp32_camera_camera_hmirror_obj,
    (mp_obj_t)&esp32_camera_camera_get_hmirror_obj,
    (mp_obj_t)&esp32_camera_camera_set_hmirror_obj);

//|     vflip: bool
//|     """When `True` the camera image is flipped top-to-bottom"""

STATIC mp_obj_t esp32_camera_camera_get_vflip(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_esp32_camera_camera_get_vflip(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_vflip_obj, esp32_camera_camera_get_vflip);

STATIC mp_obj_t esp32_camera_camera_set_vflip(const mp_obj_t self_in, const mp_obj_t arg) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_esp32_camera_camera_set_vflip(self, mp_obj_is_true(arg));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32_camera_camera_set_vflip_obj, esp32_camera_camera_set_vflip);
MP_PROPERTY_GETSET(esp32_camera_camera_vflip_obj,
    (mp_obj_t)&esp32_camera_camera_get_vflip_obj,
    (mp_obj_t)&esp32_camera_camera_set_vflip_obj);

//|     aec2: bool
//|     """When `True` the sensor's "night mode" is enabled, extending the range of automatic gain control."""

STATIC mp_obj_t esp32_camera_camera_get_aec2(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_esp32_camera_camera_get_aec2(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_aec2_obj, esp32_camera_camera_get_aec2);

STATIC mp_obj_t esp32_camera_camera_set_aec2(const mp_obj_t self_in, const mp_obj_t arg) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_esp32_camera_camera_set_aec2(self, mp_obj_is_true(arg));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32_camera_camera_set_aec2_obj, esp32_camera_camera_set_aec2);
MP_PROPERTY_GETSET(esp32_camera_camera_aec2_obj,
    (mp_obj_t)&esp32_camera_camera_get_aec2_obj,
    (mp_obj_t)&esp32_camera_camera_set_aec2_obj);

//|     awb_gain: bool
//|     """Access the awb_gain property of the camera sensor"""

STATIC mp_obj_t esp32_camera_camera_get_awb_gain(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_esp32_camera_camera_get_awb_gain(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_awb_gain_obj, esp32_camera_camera_get_awb_gain);

STATIC mp_obj_t esp32_camera_camera_set_awb_gain(const mp_obj_t self_in, const mp_obj_t arg) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_esp32_camera_camera_set_awb_gain(self, mp_obj_is_true(arg));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32_camera_camera_set_awb_gain_obj, esp32_camera_camera_set_awb_gain);
MP_PROPERTY_GETSET(esp32_camera_camera_awb_gain_obj,
    (mp_obj_t)&esp32_camera_camera_get_awb_gain_obj,
    (mp_obj_t)&esp32_camera_camera_set_awb_gain_obj);

//|     agc_gain: int
//|     """Access the gain level of the sensor. Higher values produce brighter images.  Typical settings range from 0 to 30. """

STATIC mp_obj_t esp32_camera_camera_get_agc_gain(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_esp32_camera_camera_get_agc_gain(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_agc_gain_obj, esp32_camera_camera_get_agc_gain);

STATIC mp_obj_t esp32_camera_camera_set_agc_gain(const mp_obj_t self_in, const mp_obj_t arg) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_esp32_camera_camera_set_agc_gain(self, mp_obj_get_int(arg));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32_camera_camera_set_agc_gain_obj, esp32_camera_camera_set_agc_gain);
MP_PROPERTY_GETSET(esp32_camera_camera_agc_gain_obj,
    (mp_obj_t)&esp32_camera_camera_get_agc_gain_obj,
    (mp_obj_t)&esp32_camera_camera_set_agc_gain_obj);

//|     aec_value: int
//|     """Access the exposure value of the camera.  Higher values produce brighter images.  Typical settings range from 0 to 1200."""

STATIC mp_obj_t esp32_camera_camera_get_aec_value(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_esp32_camera_camera_get_aec_value(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_aec_value_obj, esp32_camera_camera_get_aec_value);

STATIC mp_obj_t esp32_camera_camera_set_aec_value(const mp_obj_t self_in, const mp_obj_t arg) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_esp32_camera_camera_set_aec_value(self, mp_obj_get_int(arg));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32_camera_camera_set_aec_value_obj, esp32_camera_camera_set_aec_value);
MP_PROPERTY_GETSET(esp32_camera_camera_aec_value_obj,
    (mp_obj_t)&esp32_camera_camera_get_aec_value_obj,
    (mp_obj_t)&esp32_camera_camera_set_aec_value_obj);

//|     special_effect: int
//|     """Enable a "special effect". Zero is no special effect.  On OV5640, special effects range from 0 to 6 inclusive and select various color modes."""

STATIC mp_obj_t esp32_camera_camera_get_special_effect(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_esp32_camera_camera_get_special_effect(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_special_effect_obj, esp32_camera_camera_get_special_effect);

STATIC mp_obj_t esp32_camera_camera_set_special_effect(const mp_obj_t self_in, const mp_obj_t arg) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_esp32_camera_camera_set_special_effect(self, mp_obj_get_int(arg));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32_camera_camera_set_special_effect_obj, esp32_camera_camera_set_special_effect);
MP_PROPERTY_GETSET(esp32_camera_camera_special_effect_obj,
    (mp_obj_t)&esp32_camera_camera_get_special_effect_obj,
    (mp_obj_t)&esp32_camera_camera_set_special_effect_obj);

//|     wb_mode: int
//|     """The white balance mode.  0 is automatic white balance.  Typical values range from 0 to 4 inclusive."""

STATIC mp_obj_t esp32_camera_camera_get_wb_mode(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_esp32_camera_camera_get_wb_mode(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_wb_mode_obj, esp32_camera_camera_get_wb_mode);

STATIC mp_obj_t esp32_camera_camera_set_wb_mode(const mp_obj_t self_in, const mp_obj_t arg) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_esp32_camera_camera_set_wb_mode(self, mp_obj_get_int(arg));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32_camera_camera_set_wb_mode_obj, esp32_camera_camera_set_wb_mode);
MP_PROPERTY_GETSET(esp32_camera_camera_wb_mode_obj,
    (mp_obj_t)&esp32_camera_camera_get_wb_mode_obj,
    (mp_obj_t)&esp32_camera_camera_set_wb_mode_obj);

//|     ae_level: int
//|     """The exposure offset for automatic exposure.  Typical values range from -2 to +2."""

STATIC mp_obj_t esp32_camera_camera_get_ae_level(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_esp32_camera_camera_get_ae_level(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_ae_level_obj, esp32_camera_camera_get_ae_level);

STATIC mp_obj_t esp32_camera_camera_set_ae_level(const mp_obj_t self_in, const mp_obj_t arg) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_esp32_camera_camera_set_ae_level(self, mp_obj_get_int(arg));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32_camera_camera_set_ae_level_obj, esp32_camera_camera_set_ae_level);
MP_PROPERTY_GETSET(esp32_camera_camera_ae_level_obj,
    (mp_obj_t)&esp32_camera_camera_get_ae_level_obj,
    (mp_obj_t)&esp32_camera_camera_set_ae_level_obj);

//|     dcw: bool
//|     """When `True` an advanced white balance mode is selected."""

STATIC mp_obj_t esp32_camera_camera_get_dcw(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_esp32_camera_camera_get_dcw(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_dcw_obj, esp32_camera_camera_get_dcw);

STATIC mp_obj_t esp32_camera_camera_set_dcw(const mp_obj_t self_in, const mp_obj_t arg) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_esp32_camera_camera_set_dcw(self, mp_obj_is_true(arg));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32_camera_camera_set_dcw_obj, esp32_camera_camera_set_dcw);
MP_PROPERTY_GETSET(esp32_camera_camera_dcw_obj,
    (mp_obj_t)&esp32_camera_camera_get_dcw_obj,
    (mp_obj_t)&esp32_camera_camera_set_dcw_obj);

//|     bpc: bool
//|     """When `True`, "black point compensation" is enabled. This can make black parts of the image darker."""

STATIC mp_obj_t esp32_camera_camera_get_bpc(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_esp32_camera_camera_get_bpc(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_bpc_obj, esp32_camera_camera_get_bpc);

STATIC mp_obj_t esp32_camera_camera_set_bpc(const mp_obj_t self_in, const mp_obj_t arg) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_esp32_camera_camera_set_bpc(self, mp_obj_is_true(arg));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32_camera_camera_set_bpc_obj, esp32_camera_camera_set_bpc);
MP_PROPERTY_GETSET(esp32_camera_camera_bpc_obj,
    (mp_obj_t)&esp32_camera_camera_get_bpc_obj,
    (mp_obj_t)&esp32_camera_camera_set_bpc_obj);

//|     wpc: bool
//|     """When `True`, "white point compensation" is enabled.  This can make white parts of the image whiter."""

STATIC mp_obj_t esp32_camera_camera_get_wpc(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_esp32_camera_camera_get_wpc(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_wpc_obj, esp32_camera_camera_get_wpc);

STATIC mp_obj_t esp32_camera_camera_set_wpc(const mp_obj_t self_in, const mp_obj_t arg) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_esp32_camera_camera_set_wpc(self, mp_obj_is_true(arg));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32_camera_camera_set_wpc_obj, esp32_camera_camera_set_wpc);
MP_PROPERTY_GETSET(esp32_camera_camera_wpc_obj,
    (mp_obj_t)&esp32_camera_camera_get_wpc_obj,
    (mp_obj_t)&esp32_camera_camera_set_wpc_obj);

//|     raw_gma: bool
//|     """When `True`, raw gamma mode is enabled."""

STATIC mp_obj_t esp32_camera_camera_get_raw_gma(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_esp32_camera_camera_get_raw_gma(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_raw_gma_obj, esp32_camera_camera_get_raw_gma);

STATIC mp_obj_t esp32_camera_camera_set_raw_gma(const mp_obj_t self_in, const mp_obj_t arg) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_esp32_camera_camera_set_raw_gma(self, mp_obj_is_true(arg));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32_camera_camera_set_raw_gma_obj, esp32_camera_camera_set_raw_gma);
MP_PROPERTY_GETSET(esp32_camera_camera_raw_gma_obj,
    (mp_obj_t)&esp32_camera_camera_get_raw_gma_obj,
    (mp_obj_t)&esp32_camera_camera_set_raw_gma_obj);

//|     lenc: bool
//|     """Enable "lens correction". This can help compensate for light fall-off at the edge of the sensor area."""

STATIC mp_obj_t esp32_camera_camera_get_lenc(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_esp32_camera_camera_get_lenc(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_lenc_obj, esp32_camera_camera_get_lenc);

STATIC mp_obj_t esp32_camera_camera_set_lenc(const mp_obj_t self_in, const mp_obj_t arg) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_esp32_camera_camera_set_lenc(self, mp_obj_is_true(arg));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32_camera_camera_set_lenc_obj, esp32_camera_camera_set_lenc);
MP_PROPERTY_GETSET(esp32_camera_camera_lenc_obj,
    (mp_obj_t)&esp32_camera_camera_get_lenc_obj,
    (mp_obj_t)&esp32_camera_camera_set_lenc_obj);

//|     max_frame_size: FrameSize
//|     """The maximum frame size that can be captured"""
STATIC mp_obj_t esp32_camera_camera_get_max_frame_size(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return cp_enum_find(&esp32_camera_frame_size_type, common_hal_esp32_camera_camera_get_max_frame_size(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_max_frame_size_obj, esp32_camera_camera_get_max_frame_size);

MP_PROPERTY_GETTER(esp32_camera_camera_max_frame_size_obj,
    (mp_obj_t)&esp32_camera_camera_get_max_frame_size_obj);


//|     address: int
//|     """The I2C (SCCB) address of the sensor"""
STATIC mp_obj_t esp32_camera_camera_get_address(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_esp32_camera_camera_get_address(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_address_obj, esp32_camera_camera_get_address);

MP_PROPERTY_GETTER(esp32_camera_camera_address_obj,
    (mp_obj_t)&esp32_camera_camera_get_address_obj);


//|     sensor_name: str
//|     """The name of the sensor"""
STATIC mp_obj_t esp32_camera_camera_get_sensor_name(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    const char *sensor_name = common_hal_esp32_camera_camera_get_sensor_name(self);
    return mp_obj_new_str(sensor_name, strlen(sensor_name));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_sensor_name_obj, esp32_camera_camera_get_sensor_name);

MP_PROPERTY_GETTER(esp32_camera_camera_sensor_name_obj,
    (mp_obj_t)&esp32_camera_camera_get_sensor_name_obj);


//|     supports_jpeg: bool
//|     """True if the sensor can capture images in JPEG format"""
STATIC mp_obj_t esp32_camera_camera_get_supports_jpeg(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_esp32_camera_camera_get_supports_jpeg(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_supports_jpeg_obj, esp32_camera_camera_get_supports_jpeg);

MP_PROPERTY_GETTER(esp32_camera_camera_supports_jpeg_obj,
    (mp_obj_t)&esp32_camera_camera_get_supports_jpeg_obj);

//|     height: int
//|     """The height of the image being captured"""
STATIC mp_obj_t esp32_camera_camera_get_height(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_esp32_camera_camera_get_height(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_height_obj, esp32_camera_camera_get_height);

MP_PROPERTY_GETTER(esp32_camera_camera_height_obj,
    (mp_obj_t)&esp32_camera_camera_get_height_obj);

//|     width: int
//|     """The width of the image being captured"""
STATIC mp_obj_t esp32_camera_camera_get_width(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_esp32_camera_camera_get_width(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_width_obj, esp32_camera_camera_get_width);

MP_PROPERTY_GETTER(esp32_camera_camera_width_obj,
    (mp_obj_t)&esp32_camera_camera_get_width_obj);

//|     grab_mode: GrabMode
//|     """The grab mode of the camera"""
STATIC mp_obj_t esp32_camera_camera_get_grab_mode(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return cp_enum_find(&esp32_camera_grab_mode_type, common_hal_esp32_camera_camera_get_grab_mode(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_grab_mode_obj, esp32_camera_camera_get_grab_mode);

MP_PROPERTY_GETTER(esp32_camera_camera_grab_mode_obj,
    (mp_obj_t)&esp32_camera_camera_get_grab_mode_obj);


//|     framebuffer_count: int
//|     """True if double buffering is used"""
//|
STATIC mp_obj_t esp32_camera_camera_get_framebuffer_count(const mp_obj_t self_in) {
    esp32_camera_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_esp32_camera_camera_get_framebuffer_count(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_camera_camera_get_framebuffer_count_obj, esp32_camera_camera_get_framebuffer_count);

MP_PROPERTY_GETTER(esp32_camera_camera_framebuffer_count_obj,
    (mp_obj_t)&esp32_camera_camera_get_framebuffer_count_obj);


STATIC const mp_rom_map_elem_t esp32_camera_camera_locals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_address), MP_ROM_PTR(&esp32_camera_camera_address_obj) },
    { MP_ROM_QSTR(MP_QSTR_aec2), MP_ROM_PTR(&esp32_camera_camera_aec2_obj) },
    { MP_ROM_QSTR(MP_QSTR_aec_value), MP_ROM_PTR(&esp32_camera_camera_aec_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_ae_level), MP_ROM_PTR(&esp32_camera_camera_ae_level_obj) },
    { MP_ROM_QSTR(MP_QSTR_agc_gain), MP_ROM_PTR(&esp32_camera_camera_agc_gain_obj) },
    { MP_ROM_QSTR(MP_QSTR_awb_gain), MP_ROM_PTR(&esp32_camera_camera_awb_gain_obj) },
    { MP_ROM_QSTR(MP_QSTR_bpc), MP_ROM_PTR(&esp32_camera_camera_bpc_obj) },
    { MP_ROM_QSTR(MP_QSTR_brightness), MP_ROM_PTR(&esp32_camera_camera_brightness_obj) },
    { MP_ROM_QSTR(MP_QSTR_colorbar), MP_ROM_PTR(&esp32_camera_camera_colorbar_obj) },
    { MP_ROM_QSTR(MP_QSTR_contrast), MP_ROM_PTR(&esp32_camera_camera_contrast_obj) },
    { MP_ROM_QSTR(MP_QSTR_dcw), MP_ROM_PTR(&esp32_camera_camera_dcw_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&esp32_camera_camera_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_denoise), MP_ROM_PTR(&esp32_camera_camera_denoise_obj) },
    { MP_ROM_QSTR(MP_QSTR_framebuffer_count), MP_ROM_PTR(&esp32_camera_camera_framebuffer_count_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&esp32_camera_camera___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_exposure_ctrl), MP_ROM_PTR(&esp32_camera_camera_exposure_ctrl_obj) },
    { MP_ROM_QSTR(MP_QSTR_frame_available), MP_ROM_PTR(&esp32_camera_camera_frame_available_obj) },
    { MP_ROM_QSTR(MP_QSTR_frame_size), MP_ROM_PTR(&esp32_camera_camera_frame_size_obj) },
    { MP_ROM_QSTR(MP_QSTR_gain_ceiling), MP_ROM_PTR(&esp32_camera_camera_gain_ceiling_obj) },
    { MP_ROM_QSTR(MP_QSTR_gain_ctrl), MP_ROM_PTR(&esp32_camera_camera_gain_ctrl_obj) },
    { MP_ROM_QSTR(MP_QSTR_grab_mode), MP_ROM_PTR(&esp32_camera_camera_grab_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_PTR(&esp32_camera_camera_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_hmirror), MP_ROM_PTR(&esp32_camera_camera_hmirror_obj) },
    { MP_ROM_QSTR(MP_QSTR_lenc), MP_ROM_PTR(&esp32_camera_camera_lenc_obj) },
    { MP_ROM_QSTR(MP_QSTR_max_frame_size), MP_ROM_PTR(&esp32_camera_camera_max_frame_size_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel_format), MP_ROM_PTR(&esp32_camera_camera_pixel_format_obj) },
    { MP_ROM_QSTR(MP_QSTR_quality), MP_ROM_PTR(&esp32_camera_camera_quality_obj) },
    { MP_ROM_QSTR(MP_QSTR_raw_gma), MP_ROM_PTR(&esp32_camera_camera_raw_gma_obj) },
    { MP_ROM_QSTR(MP_QSTR_reconfigure), MP_ROM_PTR(&esp32_camera_camera_reconfigure_obj) },
    { MP_ROM_QSTR(MP_QSTR_saturation), MP_ROM_PTR(&esp32_camera_camera_saturation_obj) },
    { MP_ROM_QSTR(MP_QSTR_sensor_name), MP_ROM_PTR(&esp32_camera_camera_sensor_name_obj) },
    { MP_ROM_QSTR(MP_QSTR_sharpness), MP_ROM_PTR(&esp32_camera_camera_sharpness_obj) },
    { MP_ROM_QSTR(MP_QSTR_special_effect), MP_ROM_PTR(&esp32_camera_camera_special_effect_obj) },
    { MP_ROM_QSTR(MP_QSTR_supports_jpeg), MP_ROM_PTR(&esp32_camera_camera_supports_jpeg_obj) },
    { MP_ROM_QSTR(MP_QSTR_take), MP_ROM_PTR(&esp32_camera_camera_take_obj) },
    { MP_ROM_QSTR(MP_QSTR_vflip), MP_ROM_PTR(&esp32_camera_camera_vflip_obj) },
    { MP_ROM_QSTR(MP_QSTR_wb_mode), MP_ROM_PTR(&esp32_camera_camera_wb_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_whitebal), MP_ROM_PTR(&esp32_camera_camera_whitebal_obj) },
    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&esp32_camera_camera_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_wpc), MP_ROM_PTR(&esp32_camera_camera_wpc_obj) },
};

STATIC MP_DEFINE_CONST_DICT(esp32_camera_camera_locals_dict, esp32_camera_camera_locals_table);

const mp_obj_type_t esp32_camera_camera_type = {
    .base = { &mp_type_type },
    .name = MP_QSTR_Camera,
    .make_new = esp32_camera_camera_make_new,
    .locals_dict = (mp_obj_t)&esp32_camera_camera_locals_dict,
};
