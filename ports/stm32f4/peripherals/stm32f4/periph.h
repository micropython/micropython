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

#ifndef __MICROPY_INCLUDED_STM32F4_PERIPHERALS_PERIPH_H__
#define __MICROPY_INCLUDED_STM32F4_PERIPHERALS_PERIPH_H__

#include <stdint.h>
#include <stdbool.h>

#include "stm32f4xx_hal.h"
#include "stm32f4/pins.h"

// #define PA  0
// #define PB  1
// #define PC  2
// #define PD  3
// #define PE  4
// #define PF  5
// #define PG  6
// #define PH  7
// #define PI  8
// #define PJ  9
// #define PK  10

// I2C

// Numerical Version

/*
typedef struct {
    mp_obj_base_t base;
    uint8_t i2c_index:3; // Index of the I2C unit
    uint8_t alt_index:4; // Alt index is arbitrary, it just lists additional pin options
    uint8_t sda_pin_port:4;
    uint8_t sda_pin_number:4;
    uint8_t scl_pin_port:4;
    uint8_t scl_pin_number:4;
} mcu_i2c_periph_obj_t;

#define SDA(port, number) \
    .sda_pin_port = port, \
    .sda_pin_number = number, 

#define SCL(port, number) \
    .scl_pin_port = port, \
    .scl_pin_number = number 

#define I2C(index, alt, sda, scl)       \
{ \
    { &prefix_fields }, \
    .i2c_index = index, \
    .alt_index = alt, \
    sda \
    scl \
}
*/

// Address Version
typedef struct {
    uint8_t i2c_index:4; // Index of the I2C unit (1 to 3)
    uint8_t altfn_index:4; //Index of the altfn for this pin (0 to 15)
    const mcu_pin_obj_t * pin;
} mcu_i2c_sda_obj_t;

// Address Version
typedef struct {
    uint8_t i2c_index:4; // Index of the I2C unit (1 to 3)
    uint8_t altfn_index:4; //Index of the altfn for this pin (0 to 15)
    const mcu_pin_obj_t * pin;
} mcu_i2c_scl_obj_t;


#define I2C_SDA(index, alt, sda_pin)       \
{ \
    .i2c_index = index, \
    .altfn_index = alt, \
    .pin = sda_pin, \
}

#define I2C_SCL(index, alt, scl_pin)       \
{ \
    .i2c_index = index, \
    .altfn_index = alt, \
    .pin = scl_pin, \
}

// TODO: SPI, UART, etc

// Choose based on chip
#ifdef STM32F412Zx
#include "stm32f412zx/periph.h"
#endif
#ifdef STM32F411xE
#include "stm32f411xe/periph.h"
#endif
#endif // __MICROPY_INCLUDED_STM32F4_PERIPHERALS_PERIPH_H__
