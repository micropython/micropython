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

#ifndef __MICROPY_INCLUDED_STM32_PERIPHERALS_PERIPH_H__
#define __MICROPY_INCLUDED_STM32_PERIPHERALS_PERIPH_H__

#include <stdint.h>
#include <stdbool.h>

#include STM32_HAL_H
#include "pins.h"

// Comm Peripherals

typedef struct {
    uint8_t periph_index : 4;     // Index of the peripheral instance
    uint8_t altfn_index : 4;      // Index of the altfn for this pin (0 to 15)
    const mcu_pin_obj_t *pin;   // Pin Object
} mcu_periph_obj_t;

#define PERIPH(index, alt, p_pin)       \
    { \
        .periph_index = index, \
        .altfn_index = alt, \
        .pin = p_pin, \
    }

// Timer Peripheral

typedef struct {
    uint8_t tim_index : 4;
    uint8_t altfn_index : 4;
    uint8_t channel_index : 4;
    const mcu_pin_obj_t *pin;
} mcu_tim_pin_obj_t;

#define TIM(index, alt, channel, tim_pin)       \
    { \
        .tim_index = index, \
        .altfn_index = alt, \
        .channel_index = channel, \
        .pin = tim_pin, \
    }

// F4 Series
// Access Lines

#ifdef STM32F401xE
#define HAS_DAC 0
#define HAS_TRNG 0
#define HAS_BASIC_TIM 0
#include "stm32f4/stm32f401xe/periph.h"
#endif

#ifdef STM32F411xE
#define HAS_DAC 0
#define HAS_TRNG 0
#define HAS_BASIC_TIM 0
#include "stm32f4/stm32f411xe/periph.h"
#endif

#ifdef STM32F412Zx
#define HAS_DAC 0
#define HAS_TRNG 1
#define HAS_BASIC_TIM 1
#include "stm32f4/stm32f412zx/periph.h"
#endif

// Foundation Lines

#ifdef STM32F405xx
#define HAS_DAC 1
#define HAS_TRNG 1
#define HAS_BASIC_TIM 1
#include "stm32f4/stm32f405xx/periph.h"
#endif

#ifdef STM32F407xx
#define HAS_DAC 1
#define HAS_TRNG 1
#define HAS_BASIC_TIM 1
#include "stm32f4/stm32f407xx/periph.h"
#endif

// F7 Series

#ifdef STM32F746xx
#define HAS_DAC 0
#define HAS_TRNG 0
#define HAS_BASIC_TIM 0
#include "stm32f7/stm32f746xx/periph.h"
#endif

#ifdef STM32F767xx
#define HAS_DAC 0
#define HAS_TRNG 0
#define HAS_BASIC_TIM 0
#include "stm32f7/stm32f767xx/periph.h"
#endif

// H7 Series
// Single Core

#ifdef STM32H743xx
#define HAS_DAC 0
#define HAS_TRNG 0
#define HAS_BASIC_TIM 0
#include "stm32h7/stm32h743xx/periph.h"
#endif

#endif // __MICROPY_INCLUDED_STM32_PERIPHERALS_PERIPH_H__
