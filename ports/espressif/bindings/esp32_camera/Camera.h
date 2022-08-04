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

extern const mp_obj_type_t esp32_camera_camera_type;
typedef struct esp32_camera_camera_obj esp32_camera_camera_obj_t;

extern void common_hal_esp32_camera_camera_construct(
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
    camera_grab_mode_t grab_mode);

extern void common_hal_esp32_camera_camera_deinit(esp32_camera_camera_obj_t *self);
extern bool common_hal_esp32_camera_camera_deinited(esp32_camera_camera_obj_t *self);
extern bool common_hal_esp32_camera_camera_available(esp32_camera_camera_obj_t *self);
extern camera_fb_t *common_hal_esp32_camera_camera_take(esp32_camera_camera_obj_t *self, int timeout_ms);
