// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright 2019 Sony Semiconductor Solutions Corporation
//
// SPDX-License-Identifier: MIT

#pragma once

#include <nuttx/timers/pwm.h>

#include "common-hal/microcontroller/Pin.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *pin;
    struct pwm_info_s info;
    bool variable_frequency;
    int8_t number;
} pwmio_pwmout_obj_t;

void pwmout_start(uint8_t pwm_num);
void pwmout_stop(uint8_t pwm_num);
