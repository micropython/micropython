// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "esp_camera.h"
#include "shared-bindings/pwmio/PWMOut.h"
#include "common-hal/busio/I2C.h"

typedef struct espcamera_camera_obj {
    mp_obj_base_t base;
    camera_config_t camera_config;
    camera_fb_t *buffer_to_return;
    pwmio_pwmout_obj_t pwm;
    busio_i2c_obj_t *i2c;
} espcamera_obj_t;
