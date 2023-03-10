/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#pragma once

#include "py/enum.h"
#include "py/obj.h"

#include "esp_camera.h"

#include "shared-bindings/busio/I2C.h"

extern const mp_obj_type_t espcamera_camera_type;
typedef struct espcamera_camera_obj espcamera_camera_obj_t;

extern void common_hal_espcamera_camera_construct(
    espcamera_camera_obj_t *self,
    uint8_t data_pins[8],
    const mcu_pin_obj_t *external_clock_pin,
    const mcu_pin_obj_t *pixel_clock_pin,
    const mcu_pin_obj_t *vsync_pin,
    const mcu_pin_obj_t *href_pin,
    const mcu_pin_obj_t *powerdown_pin,
    const mcu_pin_obj_t *reset_pin,
    busio_i2c_obj_t *i2c,
    mp_int_t external_clock_frequency,
    pixformat_t pixel_format,
    framesize_t frame_size,
    mp_int_t jpeg_quality,
    mp_int_t framebuffer_count,
    camera_grab_mode_t grab_mode);

extern void common_hal_espcamera_camera_deinit(espcamera_camera_obj_t *self);
extern bool common_hal_espcamera_camera_deinited(espcamera_camera_obj_t *self);
extern bool common_hal_espcamera_camera_available(espcamera_camera_obj_t *self);
extern camera_fb_t *common_hal_espcamera_camera_take(espcamera_camera_obj_t *self, int timeout_ms);
extern void common_hal_espcamera_camera_reconfigure(espcamera_camera_obj_t *self, framesize_t frame_size, pixformat_t pixel_format, camera_grab_mode_t grab_mode, mp_int_t framebuffer_count);

#define DECLARE_SENSOR_GETSET(type, name, field_name, setter_function_name) \
    DECLARE_SENSOR_GET(type, name, field_name, setter_function_name) \
    DECLARE_SENSOR_SET(type, name, setter_function_name)

#define DECLARE_SENSOR_STATUS_GETSET(type, name, status_field_name, setter_function_name) \
    DECLARE_SENSOR_GETSET(type, name, status.status_field_name, setter_function_name)

#define DECLARE_SENSOR_STATUS_GET(type, name, status_field_name, setter_function_name) \
    DECLARE_SENSOR_GET(type, name, status.status_field_name, setter_function_name)

#define DECLARE_SENSOR_GET(type, name, status_field_name, setter_function_name) \
    extern type common_hal_espcamera_camera_get_##name(espcamera_camera_obj_t * self);

#define DECLARE_SENSOR_SET(type, name, setter_function_name) \
    extern void common_hal_espcamera_camera_set_##name(espcamera_camera_obj_t * self, type value);

DECLARE_SENSOR_GET(pixformat_t, pixel_format, pixformat, set_pixformat)
DECLARE_SENSOR_STATUS_GET(framesize_t, frame_size, framesize, set_framesize)
DECLARE_SENSOR_STATUS_GETSET(int, contrast, contrast, set_contrast);
DECLARE_SENSOR_STATUS_GETSET(int, brightness, brightness, set_brightness);
DECLARE_SENSOR_STATUS_GETSET(int, saturation, saturation, set_saturation);
DECLARE_SENSOR_STATUS_GETSET(int, sharpness, sharpness, set_sharpness);
DECLARE_SENSOR_STATUS_GETSET(int, denoise, denoise, set_denoise);
DECLARE_SENSOR_STATUS_GETSET(gainceiling_t, gainceiling, gainceiling, set_gainceiling);
DECLARE_SENSOR_STATUS_GETSET(int, quality, quality, set_quality);
DECLARE_SENSOR_STATUS_GETSET(bool, colorbar, colorbar, set_colorbar);
DECLARE_SENSOR_STATUS_GETSET(bool, whitebal, whitebal, set_whitebal);
DECLARE_SENSOR_STATUS_GETSET(bool, gain_ctrl, gain_ctrl, set_gain_ctrl);
DECLARE_SENSOR_STATUS_GETSET(bool, exposure_ctrl, exposure_ctrl, set_exposure_ctrl);
DECLARE_SENSOR_STATUS_GETSET(bool, hmirror, hmirror, set_hmirror);
DECLARE_SENSOR_STATUS_GETSET(bool, vflip, vflip, set_vflip);
DECLARE_SENSOR_STATUS_GETSET(bool, aec2, aec2, set_aec2);
DECLARE_SENSOR_STATUS_GETSET(bool, awb_gain, awb_gain, set_awb_gain);
DECLARE_SENSOR_STATUS_GETSET(int, agc_gain, agc_gain, set_agc_gain);
DECLARE_SENSOR_STATUS_GETSET(int, aec_value, aec_value, set_aec_value);
DECLARE_SENSOR_STATUS_GETSET(int, special_effect, special_effect, set_special_effect);
DECLARE_SENSOR_STATUS_GETSET(int, wb_mode, wb_mode, set_wb_mode);
DECLARE_SENSOR_STATUS_GETSET(int, ae_level, ae_level, set_ae_level);
DECLARE_SENSOR_STATUS_GETSET(bool, dcw, dcw, set_dcw);
DECLARE_SENSOR_STATUS_GETSET(bool, bpc, bpc, set_bpc);
DECLARE_SENSOR_STATUS_GETSET(bool, wpc, wpc, set_wpc);
DECLARE_SENSOR_STATUS_GETSET(bool, raw_gma, raw_gma, set_raw_gma);
DECLARE_SENSOR_STATUS_GETSET(bool, lenc, lenc, set_lenc);

// From settings
extern camera_grab_mode_t common_hal_espcamera_camera_get_grab_mode(espcamera_camera_obj_t *self);
extern int common_hal_espcamera_camera_get_framebuffer_count(espcamera_camera_obj_t *self);

// From camera_sensor_info_t
extern int common_hal_espcamera_camera_get_address(espcamera_camera_obj_t *self);
extern const char *common_hal_espcamera_camera_get_sensor_name(espcamera_camera_obj_t *self);
extern const bool common_hal_espcamera_camera_get_supports_jpeg(espcamera_camera_obj_t *self);
extern framesize_t common_hal_espcamera_camera_get_max_frame_size(espcamera_camera_obj_t *self);
extern int common_hal_espcamera_camera_get_width(espcamera_camera_obj_t *self);
extern int common_hal_espcamera_camera_get_height(espcamera_camera_obj_t *self);
