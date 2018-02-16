/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016, 2017 Scott Shawcroft for Adafruit Industries
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

#include "spi_flash_api.h"

#include <stdint.h>

#include "atmel_start_pins.h"
#include "hal_gpio.h"

bool spi_flash_command(uint8_t* request, uint8_t* response, uint32_t length) {
    return true;
}

bool spi_flash_sector_command(uint8_t command, uint32_t address) {
    return true;
}

bool spi_flash_write_data(uint32_t address, uint8_t* data, uint32_t data_length) {
    return true;
}

bool spi_flash_read_data(uint32_t address, uint8_t* data, uint32_t data_length) {
    return true;
}


void spi_flash_init(void) {
    MCLK->APBCMASK.bit.QSPI_ = true;
    MCLK->AHBMASK.bit.QSPI_ = true;
    MCLK->AHBMASK.bit.QSPI_2X_ = false; // Only true if we are doing DDR.

    QSPI->CTRLA.reg = QSPI_CTRLA_SWRST;
    // We don't need to wait because we're running as fast as the CPU.

    QSPI->BAUD.bit.BAUD = 2;
    QSPI->CTRLB.reg = QSPI_CTRLB_MODE_MEMORY |
                      QSPI_CTRLB_CSMODE_NORELOAD |
                      QSPI_CTRLB_DATALEN_8BITS |
                      QSPI_CTRLB_CSMODE_LASTXFER;

    QSPI->CTRLA.bit.ENABLE = 1;

    // The QSPI is only connected to one set of pins in the SAMD51 so we can hard code it.
    uint32_t pins[6] = {PIN_PA08, PIN_PA09, PIN_PA10, PIN_PA11, PIN_PB10, PIN_PB11};
    for (uint8_t i = 0; i < 6; i++) {
        gpio_set_pin_direction(pins[i], GPIO_DIRECTION_IN);
        gpio_set_pin_pull_mode(pins[i], GPIO_PULL_OFF);
        gpio_set_pin_function(pins[i], GPIO_PIN_FUNCTION_H);
    }
}
