/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Lucian Copeland for Adafruit Industries
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

#ifndef __MICROPY_INCLUDED_STM32_PERIPHERALS_PINS_H__
#define __MICROPY_INCLUDED_STM32_PERIPHERALS_PINS_H__

#include <stdint.h>
#include <stdbool.h>

#include STM32_HAL_H

typedef struct {
    mp_obj_base_t base;
    uint8_t port : 4;
    uint8_t number : 4;
    uint8_t adc_unit : 3;
    uint8_t adc_channel : 5;
} mcu_pin_obj_t;

// Standard stm32 adc unit combinations
#define ADC_1   1
#define ADC_12  3
#define ADC_123 7
#define ADC_3   4

// STM32 ADC pins can have a combination of 1, 2 or all 3 ADCs on a single pin,
// but all 3 ADCs will share the same input number per pin.
// F4 family has 3 ADC max, 24 channels max.
#define ADC_INPUT(mask, number) \
    .adc_unit = mask, \
    .adc_channel = number,

#define NO_ADC \
    .adc_unit = 0x00, \
    .adc_channel = 0x1f

extern const mp_obj_type_t mcu_pin_type;

// STM32 can have up to 9 ports, each restricted to 16 pins
// We split the pin/port evenly, in contrast to nrf.
#define PIN(p_port, p_number, p_adc)       \
    { \
        { &mcu_pin_type }, \
        .port = p_port, \
        .number = p_number, \
        p_adc \
    }

// Use illegal pin value to mark unassigned pins.
#define NO_PIN 0xff

// F4 Series
#ifdef STM32F401xE
#include "stm32f4/stm32f401xe/pins.h"
#endif
#ifdef STM32F411xE
#include "stm32f4/stm32f411xe/pins.h"
#endif
#ifdef STM32F412Zx
#include "stm32f4/stm32f412zx/pins.h"
#endif
#ifdef STM32F405xx
#include "stm32f4/stm32f405xx/pins.h"
#endif
#ifdef STM32F407xx
#include "stm32f4/stm32f407xx/pins.h"
#endif

// F7 Series
#ifdef STM32F746xx
#include "stm32f7/stm32f746xx/pins.h"
#endif
#ifdef STM32F767xx
#include "stm32f7/stm32f767xx/pins.h"
#endif

// H7 Series
#ifdef STM32H743xx
#include "stm32h7/stm32h743xx/pins.h"
#endif

#endif // __MICROPY_INCLUDED_STM32_PERIPHERALS_PINS_H__
