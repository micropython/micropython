/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2014 Paul Sokolovsky
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

#ifndef MICROPY_INCLUDED_ATMEL_SAMD_RGB_LED_STATUS_H
#define MICROPY_INCLUDED_ATMEL_SAMD_RGB_LED_STATUS_H

#include <stdint.h>
#include <stdbool.h>

#include "mpconfigport.h"
#include "rgb_led_colors.h"

#if defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK) && !defined(CIRCUITPY_BITBANG_APA102)
#include "common-hal/busio/SPI.h"
extern busio_spi_obj_t status_apa102;
#endif

void rgb_led_status_init(void);
void reset_status_led(void);
void new_status_color(uint32_t rgb);
void temp_status_color(uint32_t rgb);
void clear_temp_status(void);

uint32_t color_brightness(uint32_t color, uint8_t brightness);

#endif  // MICROPY_INCLUDED_ATMEL_SAMD_RGB_LED_STATUS_H
