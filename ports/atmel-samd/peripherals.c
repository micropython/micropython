/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Dan Halbert for Adafruit Industries
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

#include "peripherals.h"

#include "hpl_sercom_config.h"

// Routines that are the same across all samd variants.


// Convert frequency to clock-speed-dependent value. Return 255 if > 255.
uint8_t samd_peripherals_spi_baudrate_to_baud_reg_value(const uint32_t baudrate) {
    uint32_t baud_reg_value = (uint32_t) (((float) PROTOTYPE_SERCOM_SPI_M_SYNC_CLOCK_FREQUENCY /
                                           (2 * baudrate)) - 0.5f);
    return (uint8_t) (baud_reg_value > 255 ? 255 : baud_reg_value);
}

// Convert BAUD reg value back to a frequency.
uint32_t samd_peripherals_spi_baud_reg_value_to_baudrate(const uint8_t baud_reg_value) {
    return PROTOTYPE_SERCOM_SPI_M_SYNC_CLOCK_FREQUENCY / (2 * (baud_reg_value + 1));
}

