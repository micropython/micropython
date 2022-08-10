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

#include "py/runtime.h"

#include "bindings/esp32_camera/Camera.h"
#include "bindings/espidf/__init__.h"
#include "common-hal/esp32_camera/Camera.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "common-hal/microcontroller/Pin.h"

#include "esp32-camera/driver/private_include/cam_hal.h"

static void maybe_claim_pin(const mcu_pin_obj_t *pin) {
    if (pin) {
        claim_pin(pin);
    }
}

void common_hal_esp32_camera_camera_construct(
    esp32_camera_camera_obj_t *self,
    uint8_t data_pins[8],
    const mcu_pin_obj_t *external_clock_pin,
    const mcu_pin_obj_t *pixel_clock_pin,
    const mcu_pin_obj_t *vsync_pin,
    const mcu_pin_obj_t *href_pin,
    const mcu_pin_obj_t *powerdown_pin,
    const mcu_pin_obj_t *reset_pin,
    const busio_i2c_obj_t *i2c,
    mp_int_t external_clock_frequency,
    pixformat_t pixel_format,
    framesize_t frame_size,
    mp_int_t jpeg_quality,
    mp_int_t framebuffer_count,
    camera_grab_mode_t grab_mode) {

    for (int i = 0; i < 8; i++) {
        claim_pin_number(data_pins[i]);
    }
    claim_pin(external_clock_pin);
    claim_pin(pixel_clock_pin);
    claim_pin(vsync_pin);
    claim_pin(href_pin);
    maybe_claim_pin(powerdown_pin);
    maybe_claim_pin(reset_pin);

    common_hal_pwmio_pwmout_construct(&self->pwm, external_clock_pin, 1, external_clock_frequency, true);

    self->camera_config.pin_pwdn = common_hal_mcu_pin_number(powerdown_pin);
    self->camera_config.pin_reset = common_hal_mcu_pin_number(reset_pin);
    self->camera_config.pin_xclk = common_hal_mcu_pin_number(external_clock_pin);

    self->camera_config.pin_sccb_sda = NO_PIN;
    self->camera_config.pin_sccb_scl = NO_PIN;
    /* sccb i2c port set below */

    self->camera_config.pin_d7 = data_pins[7];
    self->camera_config.pin_d6 = data_pins[6];
    self->camera_config.pin_d5 = data_pins[5];
    self->camera_config.pin_d4 = data_pins[4];
    self->camera_config.pin_d3 = data_pins[3];
    self->camera_config.pin_d2 = data_pins[2];
    self->camera_config.pin_d1 = data_pins[1];
    self->camera_config.pin_d0 = data_pins[0];

    self->camera_config.pin_vsync = common_hal_mcu_pin_number(vsync_pin);
    self->camera_config.pin_href = common_hal_mcu_pin_number(href_pin);
    self->camera_config.pin_pclk = common_hal_mcu_pin_number(pixel_clock_pin);

    self->camera_config.xclk_freq_hz = external_clock_frequency;

    self->camera_config.ledc_timer = self->pwm.tim_handle.timer_num;
    self->camera_config.ledc_channel = self->pwm.chan_handle.channel;

    self->camera_config.pixel_format = pixel_format;
    self->camera_config.frame_size = frame_size;
    self->camera_config.jpeg_quality = jpeg_quality;
    self->camera_config.fb_count = framebuffer_count;
    self->camera_config.grab_mode = grab_mode;

    self->camera_config.sccb_i2c_port = i2c->i2c_num;

    CHECK_ESP_RESULT(esp_camera_init(&self->camera_config));
}

extern void common_hal_esp32_camera_camera_deinit(esp32_camera_camera_obj_t *self) {
    if (common_hal_esp32_camera_camera_deinited(self)) {
        return;
    }

    common_hal_pwmio_pwmout_deinit(&self->pwm);

    reset_pin_number(self->camera_config.pin_pwdn);
    reset_pin_number(self->camera_config.pin_reset);
    reset_pin_number(self->camera_config.pin_xclk);

    reset_pin_number(self->camera_config.pin_d7);
    reset_pin_number(self->camera_config.pin_d6);
    reset_pin_number(self->camera_config.pin_d5);
    reset_pin_number(self->camera_config.pin_d4);
    reset_pin_number(self->camera_config.pin_d3);
    reset_pin_number(self->camera_config.pin_d2);
    reset_pin_number(self->camera_config.pin_d1);
    reset_pin_number(self->camera_config.pin_d0);

    esp_camera_deinit();

    self->camera_config.xclk_freq_hz = 0;
}

bool common_hal_esp32_camera_camera_deinited(esp32_camera_camera_obj_t *self) {
    return !self->camera_config.xclk_freq_hz;
}

bool common_hal_esp32_camera_camera_available(esp32_camera_camera_obj_t *self) {
    return esp_camera_fb_available();
}

camera_fb_t *common_hal_esp32_camera_camera_take(esp32_camera_camera_obj_t *self, int timeout_ms) {
    if (self->buffer_to_return) {
        esp_camera_fb_return(self->buffer_to_return);
        self->buffer_to_return = NULL;
    }
    return self->buffer_to_return = esp_camera_fb_get_timeout(timeout_ms);
}

#define SENSOR_GETSET(type, name, field_name, setter_function_name) \
    SENSOR_GET(type, name, field_name, setter_function_name) \
    SENSOR_SET(type, name, setter_function_name)

#define SENSOR_STATUS_GETSET(type, name, status_field_name, setter_function_name) \
    SENSOR_GETSET(type, name, status.status_field_name, setter_function_name)

#define SENSOR_GET(type, name, status_field_name, setter_function_name) \
    type common_hal_esp32_camera_camera_get_##name(esp32_camera_camera_obj_t * self) { \
        sensor_t *sensor = esp_camera_sensor_get(); \
        if (!sensor->setter_function_name) { \
            mp_raise_AttributeError(translate("no such attribute")); \
        } \
        return sensor->status_field_name; \
    }

#define SENSOR_SET(type, name, setter_function_name) \
    void common_hal_esp32_camera_camera_set_##name(esp32_camera_camera_obj_t * self, type value) { \
        sensor_t *sensor = esp_camera_sensor_get(); \
        if (!sensor->setter_function_name) { \
            mp_raise_AttributeError(translate("no such attribute")); \
        } \
        if (sensor->setter_function_name(sensor, value) < 0) { \
            mp_raise_ValueError(translate("invalid setting")); \
        } \
    }

pixformat_t common_hal_esp32_camera_camera_get_pixel_format(esp32_camera_camera_obj_t *self) {
    return self->camera_config.pixel_format;
}

framesize_t common_hal_esp32_camera_camera_get_frame_size(esp32_camera_camera_obj_t *self) {
    return self->camera_config.frame_size;
}

#include "esp_log.h"

void common_hal_esp32_camera_camera_reconfigure(esp32_camera_camera_obj_t *self, framesize_t frame_size, pixformat_t pixel_format, camera_grab_mode_t grab_mode, mp_int_t framebuffer_count) {
    sensor_t *sensor = esp_camera_sensor_get();
    camera_sensor_info_t *sensor_info = esp_camera_sensor_get_info(&sensor->id);

    if (PIXFORMAT_JPEG == pixel_format && (!sensor_info->support_jpeg)) {
        raise_esp_error(ESP_ERR_NOT_SUPPORTED);
    }

    if (frame_size > sensor_info->max_size) {
        frame_size = sensor_info->max_size;
    }

    cam_deinit();
    self->camera_config.pixel_format = pixel_format;
    self->camera_config.frame_size = frame_size;
    self->camera_config.grab_mode = grab_mode;
    self->camera_config.fb_count = framebuffer_count;
    sensor->set_pixformat(sensor, self->camera_config.pixel_format);
    sensor->set_framesize(sensor, self->camera_config.frame_size);
    cam_init(&self->camera_config);
    cam_config(&self->camera_config, frame_size, sensor_info->pid);
    cam_start();
}

SENSOR_STATUS_GETSET(int, contrast, contrast, set_contrast);
SENSOR_STATUS_GETSET(int, brightness, brightness, set_brightness);
SENSOR_STATUS_GETSET(int, saturation, saturation, set_saturation);
SENSOR_STATUS_GETSET(int, sharpness, sharpness, set_sharpness);
SENSOR_STATUS_GETSET(int, denoise, denoise, set_denoise);
SENSOR_STATUS_GETSET(gainceiling_t, gainceiling, gainceiling, set_gainceiling);
SENSOR_STATUS_GETSET(int, quality, quality, set_quality);
SENSOR_STATUS_GETSET(bool, colorbar, colorbar, set_colorbar);
SENSOR_STATUS_GETSET(bool, whitebal, awb, set_whitebal);
SENSOR_STATUS_GETSET(bool, gain_ctrl, agc, set_gain_ctrl);
SENSOR_STATUS_GETSET(bool, exposure_ctrl, aec, set_exposure_ctrl);
SENSOR_STATUS_GETSET(bool, hmirror, hmirror, set_hmirror);
SENSOR_STATUS_GETSET(bool, vflip, vflip, set_vflip);
SENSOR_STATUS_GETSET(bool, aec2, aec2, set_aec2);
SENSOR_STATUS_GETSET(bool, awb_gain, awb_gain, set_awb_gain);
SENSOR_STATUS_GETSET(int, agc_gain, agc_gain, set_agc_gain);
SENSOR_STATUS_GETSET(int, aec_value, aec_value, set_aec_value);
SENSOR_STATUS_GETSET(int, special_effect, special_effect, set_special_effect);
SENSOR_STATUS_GETSET(int, wb_mode, wb_mode, set_wb_mode);
SENSOR_STATUS_GETSET(int, ae_level, ae_level, set_ae_level);
SENSOR_STATUS_GETSET(bool, dcw, dcw, set_dcw);
SENSOR_STATUS_GETSET(bool, bpc, bpc, set_bpc);
SENSOR_STATUS_GETSET(bool, wpc, wpc, set_wpc);
SENSOR_STATUS_GETSET(bool, raw_gma, raw_gma, set_raw_gma);
SENSOR_STATUS_GETSET(bool, lenc, lenc, set_lenc);

const char *common_hal_esp32_camera_camera_get_sensor_name(esp32_camera_camera_obj_t *self) {
    sensor_t *sensor = esp_camera_sensor_get();
    camera_sensor_info_t *sensor_info = esp_camera_sensor_get_info(&sensor->id);
    return sensor_info->name;
}

const bool common_hal_esp32_camera_camera_get_supports_jpeg(esp32_camera_camera_obj_t *self) {
    sensor_t *sensor = esp_camera_sensor_get();
    camera_sensor_info_t *sensor_info = esp_camera_sensor_get_info(&sensor->id);
    return sensor_info->support_jpeg;
}

const framesize_t common_hal_esp32_camera_camera_get_max_frame_size(esp32_camera_camera_obj_t *self) {
    sensor_t *sensor = esp_camera_sensor_get();
    camera_sensor_info_t *sensor_info = esp_camera_sensor_get_info(&sensor->id);
    return sensor_info->max_size;
}

const int common_hal_esp32_camera_camera_get_address(esp32_camera_camera_obj_t *self) {
    sensor_t *sensor = esp_camera_sensor_get();
    camera_sensor_info_t *sensor_info = esp_camera_sensor_get_info(&sensor->id);
    return sensor_info->sccb_addr;
}

const int common_hal_esp32_camera_camera_get_width(esp32_camera_camera_obj_t *self) {
    sensor_t *sensor = esp_camera_sensor_get();
    framesize_t framesize = sensor->status.framesize;
    return resolution[framesize].width;
}

const int common_hal_esp32_camera_camera_get_height(esp32_camera_camera_obj_t *self) {
    sensor_t *sensor = esp_camera_sensor_get();
    framesize_t framesize = sensor->status.framesize;
    return resolution[framesize].height;
}

const camera_grab_mode_t common_hal_esp32_camera_camera_get_grab_mode(esp32_camera_camera_obj_t *self) {
    return self->camera_config.grab_mode;
}

const int common_hal_esp32_camera_camera_get_framebuffer_count(esp32_camera_camera_obj_t *self) {
    return self->camera_config.fb_count;
}
