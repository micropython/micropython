/*
 * This file is part of Adafruit for EFR32 project
 *
 * The MIT License (MIT)
 *
 * Copyright 2023 Silicon Laboratories Inc. www.silabs.com
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

#ifndef __MICROPY_INCLUDED_EFR32_PERIPHERALS_PERIPH_H__
#define __MICROPY_INCLUDED_EFR32_PERIPHERALS_PERIPH_H__

#include <stdint.h>
#include <stdbool.h>

#include "pins.h"
#include "uartdrv.h"
#include "sl_pwm.h"
#include "em_vdac.h"
#define TIM_BANK_ARRAY_LEN  5


// Timer Peripheral
typedef struct {
    TIMER_TypeDef *timer;
    uint8_t channel;
    uint8_t fn_index;
    const mcu_pin_obj_t *pin;
} mcu_tim_pin_obj_t;

#define TIM(timer_index, channel_index, pin_fun, pin_num) \
    {                                                     \
        .timer = timer_index,                             \
        .channel = channel_index,                         \
        .fn_index = pin_fun,                              \
        .pin = pin_num                                    \
    }

extern mcu_tim_pin_obj_t mcu_tim_list[TIM_BANK_ARRAY_LEN];

#define DAC_BANK_ARRAY_LEN 4

typedef struct {
    VDAC_TypeDef *vdac;
    uint8_t channel;
    uint8_t fn_index;
    bool is_used;
    uint16_t value;
    const mcu_pin_obj_t *pin;
} mcu_dac_pin_obj_t;

#define DAC(vdac_index, channel_index, pin_fun, used, dac_value, pin_num) \
    {                                                                     \
        .vdac = vdac_index,                                               \
        .channel = channel_index,                                         \
        .fn_index = pin_fun,                                              \
        .is_used = used,                                                  \
        .value = dac_value,                                               \
        .pin = pin_num                                                    \
    }

extern mcu_dac_pin_obj_t mcu_dac_list[DAC_BANK_ARRAY_LEN];

#endif  // __MICROPY_INCLUDED_EFR32_PERIPHERALS_PERIPH_H__
