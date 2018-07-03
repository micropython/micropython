/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2018 Damien P. George
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

#include <stdbool.h>

#define mp_hal_delay_us_fast(us) mp_hal_delay_us(us)

#define MP_HAL_PIN_MODE_INPUT           (0)
#define MP_HAL_PIN_MODE_OUTPUT          (1)
#define MP_HAL_PIN_MODE_ALT             (2)
#define MP_HAL_PIN_MODE_ANALOG          (3)
#define MP_HAL_PIN_MODE_OPEN_DRAIN      (5)
#define MP_HAL_PIN_MODE_ALT_OPEN_DRAIN  (6)
#define MP_HAL_PIN_PULL_NONE            (GPIO_NOPULL)
#define MP_HAL_PIN_PULL_UP              (GPIO_PULLUP)
#define MP_HAL_PIN_PULL_DOWN            (GPIO_PULLDOWN)

#define mp_hal_pin_obj_t        uint32_t
#define mp_hal_pin_input(p)     mp_hal_pin_config((p), MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_NONE, 0)
#define mp_hal_pin_output(p)    mp_hal_pin_config((p), MP_HAL_PIN_MODE_OUTPUT, MP_HAL_PIN_PULL_NONE, 0)
#define mp_hal_pin_open_drain(p) mp_hal_pin_config((p), MP_HAL_PIN_MODE_OPEN_DRAIN, MP_HAL_PIN_PULL_NONE, 0)
#define mp_hal_pin_low(p)       (((GPIO_TypeDef*)((p) & ~0xf))->BSRR = 0x10000 << ((p) & 0xf))
#define mp_hal_pin_high(p)      (((GPIO_TypeDef*)((p) & ~0xf))->BSRR = 1 << ((p) & 0xf))
#define mp_hal_pin_od_low(p)    mp_hal_pin_low(p)
#define mp_hal_pin_od_high(p)   mp_hal_pin_high(p)
#define mp_hal_pin_read(p)      ((((GPIO_TypeDef*)((p) & ~0xf))->IDR >> ((p) & 0xf)) & 1)
#define mp_hal_pin_write(p, v)  do { if (v) { mp_hal_pin_high(p); } else { mp_hal_pin_low(p); } } while (0)

void mp_hal_pin_config(uint32_t port_pin, uint32_t mode, uint32_t pull, uint32_t alt);
void mp_hal_pin_config_speed(uint32_t port_pin, uint32_t speed);

#define pin_A0  (GPIOA_BASE |  0)
#define pin_A1  (GPIOA_BASE |  1)
#define pin_A2  (GPIOA_BASE |  2)
#define pin_A3  (GPIOA_BASE |  3)
#define pin_A4  (GPIOA_BASE |  4)
#define pin_A5  (GPIOA_BASE |  5)
#define pin_A6  (GPIOA_BASE |  6)
#define pin_A7  (GPIOA_BASE |  7)
#define pin_A8  (GPIOA_BASE |  8)
#define pin_A9  (GPIOA_BASE |  9)
#define pin_A10 (GPIOA_BASE | 10)
#define pin_A11 (GPIOA_BASE | 11)
#define pin_A12 (GPIOA_BASE | 12)
#define pin_A13 (GPIOA_BASE | 13)
#define pin_A14 (GPIOA_BASE | 14)
#define pin_A15 (GPIOA_BASE | 15)

#define pin_B0  (GPIOB_BASE |  0)
#define pin_B1  (GPIOB_BASE |  1)
#define pin_B2  (GPIOB_BASE |  2)
#define pin_B3  (GPIOB_BASE |  3)
#define pin_B4  (GPIOB_BASE |  4)
#define pin_B5  (GPIOB_BASE |  5)
#define pin_B6  (GPIOB_BASE |  6)
#define pin_B7  (GPIOB_BASE |  7)
#define pin_B8  (GPIOB_BASE |  8)
#define pin_B9  (GPIOB_BASE |  9)
#define pin_B10 (GPIOB_BASE | 10)
#define pin_B11 (GPIOB_BASE | 11)
#define pin_B12 (GPIOB_BASE | 12)
#define pin_B13 (GPIOB_BASE | 13)
#define pin_B14 (GPIOB_BASE | 14)
#define pin_B15 (GPIOB_BASE | 15)

#define pin_C0  (GPIOC_BASE |  0)
#define pin_C1  (GPIOC_BASE |  1)
#define pin_C2  (GPIOC_BASE |  2)
#define pin_C3  (GPIOC_BASE |  3)
#define pin_C4  (GPIOC_BASE |  4)
#define pin_C5  (GPIOC_BASE |  5)
#define pin_C6  (GPIOC_BASE |  6)
#define pin_C7  (GPIOC_BASE |  7)
#define pin_C8  (GPIOC_BASE |  8)
#define pin_C9  (GPIOC_BASE |  9)
#define pin_C10 (GPIOC_BASE | 10)
#define pin_C11 (GPIOC_BASE | 11)
#define pin_C12 (GPIOC_BASE | 12)
#define pin_C13 (GPIOC_BASE | 13)
#define pin_C14 (GPIOC_BASE | 14)
#define pin_C15 (GPIOC_BASE | 15)

#define pin_D0  (GPIOD_BASE |  0)
#define pin_D1  (GPIOD_BASE |  1)
#define pin_D2  (GPIOD_BASE |  2)
#define pin_D3  (GPIOD_BASE |  3)
#define pin_D4  (GPIOD_BASE |  4)
#define pin_D5  (GPIOD_BASE |  5)
#define pin_D6  (GPIOD_BASE |  6)
#define pin_D7  (GPIOD_BASE |  7)
#define pin_D8  (GPIOD_BASE |  8)
#define pin_D9  (GPIOD_BASE |  9)
#define pin_D10 (GPIOD_BASE | 10)
#define pin_D11 (GPIOD_BASE | 11)
#define pin_D12 (GPIOD_BASE | 12)
#define pin_D13 (GPIOD_BASE | 13)
#define pin_D14 (GPIOD_BASE | 14)
#define pin_D15 (GPIOD_BASE | 15)

#define pin_E0  (GPIOE_BASE |  0)
#define pin_E1  (GPIOE_BASE |  1)
#define pin_E2  (GPIOE_BASE |  2)
#define pin_E3  (GPIOE_BASE |  3)
#define pin_E4  (GPIOE_BASE |  4)
#define pin_E5  (GPIOE_BASE |  5)
#define pin_E6  (GPIOE_BASE |  6)
#define pin_E7  (GPIOE_BASE |  7)
#define pin_E8  (GPIOE_BASE |  8)
#define pin_E9  (GPIOE_BASE |  9)
#define pin_E10 (GPIOE_BASE | 10)
#define pin_E11 (GPIOE_BASE | 11)
#define pin_E12 (GPIOE_BASE | 12)
#define pin_E13 (GPIOE_BASE | 13)
#define pin_E14 (GPIOE_BASE | 14)
#define pin_E15 (GPIOE_BASE | 15)

#define pin_F0  (GPIOF_BASE |  0)
#define pin_F1  (GPIOF_BASE |  1)
#define pin_F2  (GPIOF_BASE |  2)
#define pin_F3  (GPIOF_BASE |  3)
#define pin_F4  (GPIOF_BASE |  4)
#define pin_F5  (GPIOF_BASE |  5)
#define pin_F6  (GPIOF_BASE |  6)
#define pin_F7  (GPIOF_BASE |  7)
#define pin_F8  (GPIOF_BASE |  8)
#define pin_F9  (GPIOF_BASE |  9)
#define pin_F10 (GPIOF_BASE | 10)
#define pin_F11 (GPIOF_BASE | 11)
#define pin_F12 (GPIOF_BASE | 12)
#define pin_F13 (GPIOF_BASE | 13)
#define pin_F14 (GPIOF_BASE | 14)
#define pin_F15 (GPIOF_BASE | 15)
