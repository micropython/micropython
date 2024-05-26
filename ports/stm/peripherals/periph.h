// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

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
        .tim_index = index - 1, \
        .altfn_index = alt, \
        .channel_index = channel - 1, \
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

#ifdef STM32F412Cx
#define HAS_DAC 0
#define HAS_TRNG 1
#define HAS_BASIC_TIM 1
#include "stm32f4/stm32f412cx/periph.h"
#endif

#ifdef STM32F412Zx
#define HAS_DAC 0
#define HAS_TRNG 1
#define HAS_BASIC_TIM 1
#include "stm32f4/stm32f412zx/periph.h"
#endif

// Foundation Lines

#ifdef STM32L4R5xx
#define HAS_DAC 1
#define HAS_TRNG 1
#define HAS_BASIC_TIM 1
#include "stm32l4/stm32l4r5xx/periph.h"
#endif

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

#ifdef STM32F446xx
#define HAS_DAC 0
#define HAS_TRNG 0
#define HAS_BASIC_TIM 0
#include "stm32f4/stm32f446xx/periph.h"
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
