/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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

#ifndef __MICROPY_INCLUDED_STM32F4_PERIPHERALS_PERIPH_H__
#define __MICROPY_INCLUDED_STM32F4_PERIPHERALS_PERIPH_H__

#include <stdint.h>
#include <stdbool.h>

#include "stm32f4xx_hal.h"
#include "stm32f4/pins.h"

#define GPIO_A  0
#define GPIO_B  1
#define GPIO_C  2
#define GPIO_D  3
#define GPIO_E  4
#define GPIO_F  5
#define GPIO_G  6
#define GPIO_H  7
#define GPIO_I  8
#define GPIO_J  9
#define GPIO_K  10

#define NO_ALT  0x0F

extern const mp_obj_type_t prefix_fields;

// I2C

typedef struct {
    mp_obj_base_t base;
    uint8_t i2c_index:3; // Index of the I2C unit
    uint8_t alt_index:4; // Alt index is arbitrary, it just lists additional pin options
    uint8_t sda_pin_port:4;
    uint8_t sda_pin_number:4;
    uint8_t scl_pin_port:4;
    uint8_t scl_pin_number:4;
} mcu_i2c_periph_obj_t;

#define I2C(i2c_index, alt_index, sda_port, sda_number, scl_port, scl_number)       \
{ \
    { &prefix_fields }, \
    .i2c_index = i2c_index, \
    .sda_pin_port = sda_port, \
    .sda_pin_number = sda_number, \
    .scl_pin_port = scl_port, \
    .scl_pin_number = scl_number \
}

//ALT
// typedef struct {
//     mp_obj_base_t base;
//     uint8_t i2c_index:4; // Index of the I2C unit
//     uint8_t alt_index:4; // Alt index is arbitrary, it just lists additional pin options
//     mcu_pin_obj_t * sda;
//     mcu_pin_obj_t * scl;
// } mcu_i2c_periph_obj_t;

// #define I2C(i2c_index, alt_index, sda, scl)       \
// { \
//     { &prefix_fields }, \
//     .i2c_index = i2c_index, \
//     .sda = sda, \
//     .scl = scl, \
// }

// TODO: SPI, UART, etc

// Choose based on chip
#ifdef STM32F412Zx
#include "stm32f412zx/periph.h"
#endif
#ifdef STM32F411xE
#include "stm32f411xe/periph.h"
#endif
#endif // __MICROPY_INCLUDED_STM32F4_PERIPHERALS_PERIPH_H__
