// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "driver/ledc.h"

typedef struct {
    mp_obj_base_t base;
    ledc_timer_config_t tim_handle;
    ledc_channel_config_t chan_handle;
    const mcu_pin_obj_t *pin;
    uint8_t duty_resolution;
    bool variable_frequency : 1;
    bool deinited : 1;
} pwmio_pwmout_obj_t;
