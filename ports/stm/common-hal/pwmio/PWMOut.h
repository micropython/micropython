// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include STM32_HAL_H
#include "peripherals/periph.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    TIM_HandleTypeDef handle;
    TIM_OC_InitTypeDef chan_handle;
    const mcu_tim_pin_obj_t *tim;
    uint32_t frequency;
    uint32_t period;
    const mcu_pin_obj_t *pin;
    uint16_t duty_cycle;
    uint8_t channel;
    bool variable_frequency;
} pwmio_pwmout_obj_t;
