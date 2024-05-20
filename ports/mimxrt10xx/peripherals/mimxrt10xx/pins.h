// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Artur Pacholec
//
// SPDX-License-Identifier: MIT

// DO NOT include this file directly. Use shared-bindings/microcontroller/Pin.h instead to ensure
// that all necessary includes are already included.

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "fsl_iomuxc.h"
#include "sdk/drivers/pwm/fsl_pwm.h"
#include "py/obj.h"

extern const mp_obj_type_t mcu_pin_type;

// Use illegal pin value to mark unassigned pins.
#define NO_PIN 0xff
#define NO_ADC NULL
#define NO_PWM NULL

typedef struct {
    mp_obj_base_t base;
    GPIO_Type *gpio;
    uint8_t number;
    uint16_t mux_idx;
    uint32_t mux_reg;
    uint32_t cfg_reg;
    ADC_Type *adc;
    uint8_t adc_channel;
    uint32_t mux_reset;
    uint32_t pad_reset;
} mcu_pin_obj_t;

#define PIN(p_gpio, p_number, p_enum, p_adc, p_adc_channel, p_mux_reset, p_pad_reset) \
    { \
        { &mcu_pin_type }, \
        .gpio = p_gpio, \
        .number = p_number, \
        .mux_idx = kIOMUXC_SW_MUX_CTL_PAD_##p_enum, \
        .mux_reg = (uint32_t)&(IOMUXC->SW_MUX_CTL_PAD[kIOMUXC_SW_MUX_CTL_PAD_##p_enum]), \
        .cfg_reg = (uint32_t)&(IOMUXC->SW_PAD_CTL_PAD[kIOMUXC_SW_PAD_CTL_PAD_##p_enum]), \
        .adc = p_adc, \
        .adc_channel = p_adc_channel, \
        .mux_reset = p_mux_reset, \
        .pad_reset = p_pad_reset, \
    }

typedef void (gpio_change_interrupt_t)(void *data);
void disable_pin_change_interrupt(const mcu_pin_obj_t *pin);
void enable_pin_change_interrupt(const mcu_pin_obj_t *pin, gpio_change_interrupt_t func, void *data);

#ifdef MIMXRT1011_SERIES
#include "MIMXRT1011/pins.h"
#elif defined(MIMXRT1015_SERIES)
#include "MIMXRT1015/pins.h"
#elif defined(MIMXRT1021_SERIES)
#include "MIMXRT1021/pins.h"
#elif defined(MIMXRT1042_SERIES)
#include "MIMXRT1042/pins.h"
#elif defined(MIMXRT1052_SERIES)
#include "MIMXRT1052/pins.h"
#elif defined(MIMXRT1062_SERIES)
#include "MIMXRT1062/pins.h"
#elif defined(MIMXRT1176_cm7_SERIES)
#include "MIMXRT1176/pins.h"
#endif
