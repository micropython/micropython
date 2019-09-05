/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Dan Halbert for Adafruit Industries
 * Copyright (c) 2019 Lucian Copeland for Adafruit Industries
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

#ifndef __MICROPY_INCLUDED_STM32F4_PERIPHERALS_PINS_H__
#define __MICROPY_INCLUDED_STM32F4_PERIPHERALS_PINS_H__

#include <stdint.h>
#include <stdbool.h>

#include "stm32f4xx_hal.h"

typedef struct {
    mp_obj_base_t base;
    //uint8_t number; //(3)port,(5)pin 
    uint8_t port_number;
    GPIO_TypeDef * port;
    uint8_t number;
} mcu_pin_obj_t;

// extern GPIO_TypeDef *port_lookup_table[];

// static inline GPIO_TypeDef * get_GPIO_ptr(const mcu_pin_obj_t *p)
// {
// 	return port_lookup_table[0x7&( (p->number) >> 5)];
// }

extern const mp_obj_type_t mcu_pin_type;

// Used in device-specific pins.c
#define PIN(p_port_num, p_port, p_number)       \
{ \
    { &mcu_pin_type }, \
    .port_number = (p_port_num), \
    .port = (p_port),      \
    .number = (p_number), \
}

// Use illegal pin value to mark unassigned pins.
#define NO_PIN 0xff

// Choose based on chip
#ifdef STM32F412Zx
#include "stm32f412zx/pins.h"
#endif
#ifdef STM32F411xE
#include "stm32f411xe/pins.h"
#endif
#endif // __MICROPY_INCLUDED_STM32F4_PERIPHERALS_PINS_H__
