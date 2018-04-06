/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_ATMEL_SAMD_PERIPHERALS_H
#define MICROPY_INCLUDED_ATMEL_SAMD_PERIPHERALS_H

#include <stdint.h>

#include "mpconfigport.h"

// Routines common across chip families.
uint8_t samd_peripherals_spi_baudrate_to_baud_reg_value(const uint32_t baudrate);
uint32_t samd_peripherals_spi_baud_reg_value_to_baudrate(const uint8_t baud_reg_value);

Sercom* sercom_insts[SERCOM_INST_NUM];

#ifdef SAMD21
#include "samd21_peripherals.h"
#endif
#ifdef SAMD51
#include "samd51_peripherals.h"
#endif

#endif // MICROPY_INCLUDED_ATMEL_SAMD_PINS_H
