/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2019 Artur Pacholec
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

// DO NOT include this file directly. Use shared-bindings/microcontroller/Pin.h instead to ensure
// that all necessary includes are already included.

#ifndef MICROPY_INCLUDED_MIMXRT10XX_PERIPHERALS_PINS_H
#define MICROPY_INCLUDED_MIMXRT10XX_PERIPHERALS_PINS_H

#include <stdint.h>
#include <stdbool.h>

#include "fsl_iomuxc.h"
#include "fsl_pwm.h"
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

#ifdef MIMXRT1011_SERIES
#include "MIMXRT1011/pins.h"
#elif defined(MIMXRT1021_SERIES)
#include "MIMXRT1021/pins.h"
#elif defined(MIMXRT1062_SERIES)
#include "MIMXRT1062/pins.h"
#endif

#endif // MICROPY_INCLUDED_MIMXRT10XX_PERIPHERALS_PINS_H
