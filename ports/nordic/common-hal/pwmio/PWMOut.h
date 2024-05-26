// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "nrfx_pwm.h"
#include "py/obj.h"
#include "shared-bindings/microcontroller/Pin.h"

typedef struct {
    mp_obj_base_t base;
    NRF_PWM_Type *pwm;
    uint8_t channel : 7;
    bool variable_frequency : 1;
    const mcu_pin_obj_t *pin;
    uint16_t duty_cycle;
    uint32_t frequency;
} pwmio_pwmout_obj_t;

NRF_PWM_Type *pwmout_allocate(uint16_t countertop, nrf_pwm_clk_t base_clock,
    bool variable_frequency, int8_t *channel_out, bool *pwm_already_in_use_out,
    IRQn_Type *irq);
void pwmout_free_channel(NRF_PWM_Type *pwm, int8_t channel);
